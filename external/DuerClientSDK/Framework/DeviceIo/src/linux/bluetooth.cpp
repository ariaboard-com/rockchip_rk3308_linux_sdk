/*
 * Copyright (c) 2018 Baidu, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "bluetooth.h"
#include "LoggerUtils/DcsSdkLogger.h"
#include "socket_app.h"

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <unistd.h>
#include <signal.h>

using duerOSDcsApp::framework::DeviceIo;
using duerOSDcsApp::framework::DeviceInput;
using duerOSDcsApp::framework::tSOCKET_APP;
using duerOSDcsApp::framework::socket_send;
using duerOSDcsApp::framework::socket_recieve;
using duerOSDcsApp::framework::BtControlCmd;

#define dbg(fmt, ...) APP_DEBUG("[duer bt debug ]" fmt, ##__VA_ARGS__)
#define err(fmt, ...) APP_ERROR("[duer bt error ]" fmt, ##__VA_ARGS__)

/* as same as APP_BLE_WIFI_INTRODUCER_GATT_ATTRIBUTE_SIZE */
#define BLE_SOCKET_RECV_LEN 22

#ifdef BLUEZ5_UTILS
static char sock_path[] = "/data/bluez5_utils/socket_dueros";
#else
static char sock_path[] = "/data/bsa/config/socket_dueros";
#endif

#define WIFI_STATUS_PATH "/tmp/wifi_status.txt"
#define BT_STATUS_PATH "/data/bsa/config/bt_config.xml"

enum class BtControlType {
    BT_NONE = 0,
    BT_AUDIO_PLAY,
    BLE_WIFI_INTRODUCER
};

typedef struct {
    pthread_t tid;
    int socket_recv_done;
    bool is_bt_open;
    int is_ble_open;
    int is_a2dp_open;
    BtControlType type;
    tSOCKET_APP socket_app;
} bt_control_t;

static bt_control_t bt_control = {0, 0, false, 0, 0, BtControlType::BT_NONE};


static void bt_a2dp_sink_cmd_process(char *data);
static void bt_socket_thread_delete(void);

static bool bt_server_is_open()
{
    return bt_control.is_bt_open;
}

static bool ble_is_open()
{
    bool ret = false;

    if (bt_control.is_ble_open) {
        ret = true;
    }

    return ret;
}

static bool bt_is_a2dp_open()
{
    bool ret = false;

    if (bt_control.is_a2dp_open) {
        ret = true;
    }

    return ret;
}

static int bt_socket_send(void *data, int len)
{
    /*
        printf("bt_socket_send, len: %d\n", len);
        for(int i = 0; i < len; i++)
            printf("%02x ", ((char*)data)[i]);
        printf("\n\n");
    */
    return socket_send(bt_control.socket_app.client_sockfd, (char *) data, len);
}

static int bt_control_cmd_send(enum BtControlCmd bt_ctrl_cmd)
{
    char cmd[10];
    memset(cmd, 0, 10);
    sprintf(cmd, "%d", bt_ctrl_cmd);

    if (bt_control.type != BtControlType::BT_AUDIO_PLAY) {
        APP_DEBUG("Not bluetooth play mode, don`t send bluetooth control commands\n");
        return 0;
    }

    APP_DEBUG("bt_control_cmd_send, cmd: %s, len: %d\n", cmd, strlen(cmd));

    if (bt_socket_send(cmd, strlen(cmd)) < 0) {
        APP_ERROR("Bt control cmd send failed\n");
        return -1;
    }

    return 0;
}

static void *bt_socket_recieve(void *arg)
{
    char data[BLE_SOCKET_RECV_LEN];
    int bytes = 0;

    APP_DEBUG("bt_socket_recieve\n");

#ifdef BLUEZ5_UTILS

    if (-1 == system("mkdir -p /data/bluez5_utils")) {
        APP_ERROR("mkdir /data/bluez5_utils failed, errno : %d\n", errno);
        goto exit;
    }

#else

    if (-1 == system("mkdir -p /data/bsa/config")) {
        APP_ERROR("mkdir /data/bsa/config failed\n");
        goto exit;
    }

#endif

    strcpy(bt_control.socket_app.sock_path, sock_path);

    if ((setup_socket_server(&bt_control.socket_app)) < 0) {
        goto exit;
    }

    if (accpet_client(&bt_control.socket_app) < 0) {
        goto exit;
    }

    APP_DEBUG("Client connected\n");

    if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_REGISTER) < 0) {
        APP_ERROR("Bt socket send register failed\n");
        goto exit;
    }

    while (bt_control.socket_recv_done) {
        memset(data, 0, sizeof(data));
        bytes = socket_recieve(bt_control.socket_app.client_sockfd, data, sizeof(data));

        if (bytes <= 0) {
            APP_DEBUG("Client leaved, break\n");
            break;
        }

        /*
                printf("bt_socket_recieve, bytes: %d\n", bytes);
                for(int i = 0; i < bytes; i++)
                    printf("%02x ", (data)[i]);
                printf("\n\n");
        */
        if (bt_control.type == BtControlType::BT_AUDIO_PLAY)
            bt_a2dp_sink_cmd_process(data);
        else if (bt_control.type == BtControlType::BLE_WIFI_INTRODUCER)
            DeviceIo::getInstance()->getNotify()->callback(DeviceInput::BLE_SERVER_RECV, data, bytes);
    }

exit:
    APP_DEBUG("Exit socket recv thread\n");
    return NULL;
}

void socket_recieve_handle(int signal, siginfo_t *siginfo, void *u_contxt)
{
    APP_DEBUG("socket_recieve_handle exec for kill\n");
    pthread_exit(NULL);
    return;
}

static int bt_socket_thread_create(void)
{
    struct sigaction sigact;

    APP_DEBUG("bt_socket_thread_create\n");

    if (!bt_control.socket_recv_done) {
        bt_control.socket_recv_done = 1;

        sigact.sa_sigaction = socket_recieve_handle;
        sigact.sa_flags = SA_SIGINFO;
        sigemptyset(&sigact.sa_mask);
        sigaction(SIGUSR2, &sigact, NULL);

        if (pthread_create(&bt_control.tid, NULL, bt_socket_recieve, NULL)) {
            APP_ERROR("Create ble pthread failed\n");
            return -1;
        }
    }

    return 0;
}

static void bt_socket_thread_delete(void)
{
    int ret;
    APP_DEBUG("bt_socket_thread_delete\n");

    if (bt_control.socket_recv_done) {
        bt_control.socket_recv_done = 0;
        teardown_socket_server(&bt_control.socket_app);

        if (bt_control.tid) {
            ret = pthread_kill(bt_control.tid, SIGUSR2);

            if (ret == 0) {
                APP_DEBUG("pthread_kill success\n");
            } else if (ret == ESRCH) {
                APP_DEBUG("The id = %lu thread has exited or does not exist\n", bt_control.tid);
            } else {
                APP_DEBUG("pthread_kill error, ret: %d\n", ret);
                return;
            }

            pthread_join(bt_control.tid, NULL);
            bt_control.tid = 0;
        }
    }
}

static int bt_bsa_server_open(void)
{
#ifndef BLUEZ5_UTILS

    if (bt_server_is_open())
        return 0;

    if (-1 == system("bsa_server.sh start")) {
        APP_ERROR("Start bsa server failed\n");
        return -1;
    }

#endif
    bt_control.is_bt_open = true;

    return 0;
}

static int bt_bsa_server_close(void)
{
#ifndef BLUEZ5_UTILS

    if (!bt_server_is_open())
        return 0;

    if (-1 == system("bsa_server.sh stop")) {
        APP_ERROR("Stop bsa server failed\n");
        return -1;
    }

#endif
    bt_control.is_bt_open = false;

    return 0;
}

static int bt_a2dp_sink_server_open(void)
{
    APP_DEBUG("bt_a2dp_sink_server_open\n");

#ifndef BLUEZ5_UTILS

    if (-1 == system("bsa_bt_sink.sh start")) {
        APP_ERROR("Start a2dp sink failed\n");
        return -1;
    }

#endif
    bt_control.is_a2dp_open = 1;
    return 0;
}

static int bt_a2dp_sink_server_close(void)
{
    bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_QUIT);

#ifndef BLUEZ5_UTILS

    if (-1 == system("bsa_bt_sink.sh stop")) {
        APP_DEBUG("Stop a2pd sink failed\n");
        return -1;
    }

#endif

    bt_control.is_a2dp_open = 0;
    APP_DEBUG("bt_a2dp_sink_server_close\n");
    return 0;
}

static void bt_a2dp_sink_cmd_process(char *data)
{
    int cmd = atoi(data);

    APP_DEBUG("bt_a2dp_sink_cmd_process, data: %s\n", data);

    switch (cmd) {
    case BtControlCmd::APP_AVK_BT_CONNECT:
        APP_DEBUG("APP_AVK_BT_CONNECT\n");

        if (DeviceIo::getInstance()->getNotify()) {
            DeviceIo::getInstance()->getNotify()->callback(DeviceInput::BT_PAIR_SUCCESS, NULL, 0);
        }

        break;

    case BtControlCmd::APP_AVK_BT_DISCONNECT:
        APP_DEBUG("APP_AVK_BT_DISCONNECT\n");
        bt_a2dp_sink_server_close();
        break;

    case BtControlCmd::APP_AVK_BT_PLAY:
        APP_DEBUG("APP_AVK_BT_PLAY\n");

        if (DeviceIo::getInstance()->getNotify()) {
            DeviceIo::getInstance()->getNotify()->callback(DeviceInput::BT_START_PLAY, NULL, 0);
        }

        break;

    case BtControlCmd::APP_AVK_BT_STOP:
        APP_DEBUG("APP_AVK_BT_STOP\n");

        if (DeviceIo::getInstance()->getNotify()) {
            DeviceIo::getInstance()->getNotify()->callback(DeviceInput::BT_STOP_PLAY, NULL, 0);
        }

        break;

    case BtControlCmd::APP_AVK_BT_WAIT_PAIR:
        APP_DEBUG("APP_AVK_BT_WAIT_PAIR\n");

        if (DeviceIo::getInstance()->getNotify()) {
            DeviceIo::getInstance()->getNotify()->callback(DeviceInput::BT_DO_PAIR, NULL, 0);
        }

        break;

    case BtControlCmd::APP_AVK_BT_PAIR_SUCCESS:
        APP_DEBUG("APP_AVK_BT_PAIR_SUCCESS\n");
        break;

    case BtControlCmd::APP_AVK_BT_PAIR_FAILED_OTHER:
        APP_DEBUG("APP_AVK_BT_PAIR_FAILED_OTHER\n");
        break;
    }
}

static void bt_a2dp_sink_close_wait(void)
{
    while (1) {
        if (!bt_control.is_a2dp_open) {
            break;
        } else {
            APP_DEBUG("wait a2dp sink server close\n");
            usleep(300000);
        }
    }
}

static int ble_wifi_introducer_server_open(void)
{
#ifdef BLUEZ5_UTILS

    if (-1 == system("/usr/bin/bluez5_utils_wifi_config.sh start")) {
        APP_ERROR("Start bluez5 utils bt failed, errno: %d\n", errno);
        return -1;
    }

#else

    if (-1 == system("/usr/bin/bsa_ble_wifi_introducer.sh start")) {
        APP_ERROR("Start bsa ble failed, errno: %d\n", errno);
        return -1;
    }

#endif
    bt_control.is_ble_open = 1;
    APP_DEBUG("ble_wifi_introducer_server_open\n");
    return 0;
}

static int ble_wifi_introducer_server_close(void)
{
#ifdef BLUEZ5_UTILS

    if (-1 == system("/usr/bin/bluez5_utils_wifi_config.sh stop")) {
        APP_ERROR("Stop bluez5 utils bt failed, errno: %d\n", errno);
        return -1;
    }

#else

    if (-1 == system("/usr/bin/bsa_ble_wifi_introducer.sh stop")) {
        APP_DEBUG("Stop bsa ble failed, errno: %d\n", errno);
        return -1;
    }

#endif
    bt_control.is_ble_open = 0;
    APP_DEBUG("ble_wifi_introducer_server_close\n");

    return 0;
}

static int ble_open_server()
{
    int ret = 0;

    if (bt_control.is_ble_open) {
        return ret;
    }

    bt_control.type = BtControlType::BLE_WIFI_INTRODUCER;

    if (bt_socket_thread_create() < 0) {
        goto error;
    }

    if (ble_wifi_introducer_server_open() < 0) {
        goto error;
    }

    return ret;

error:
    bt_control.type = BtControlType::BT_NONE;
    return -1;
}

static int ble_close_server()
{
    int ret = 0;

    if (!bt_control.is_ble_open) {
        return ret;
    }

    if (ble_wifi_introducer_server_close() < 0) {
        ret = -1;
    }

    bt_socket_thread_delete();

    bt_control.type = BtControlType::BT_NONE;

    return ret;
}

static int bt_open_a2dp_server()
{
    int ret = 0;

    if (bt_control.is_a2dp_open) {
        return ret;
    }

    bt_control.type = BtControlType::BT_AUDIO_PLAY;

    if (bt_socket_thread_create() < 0) {
        goto error;
    }

    if (bt_a2dp_sink_server_open() < 0) {
        goto error;
    }

    return ret;
error:
    bt_control.type = BtControlType::BT_NONE;
    return -1;
}

static int bt_close_a2dp_server()
{
    int ret = 0;

    if (!bt_control.is_a2dp_open) {
        return ret;
    }

    if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_CLOSE) < 0) {
        ret = -1;
        return ret;
    }

    bt_a2dp_sink_close_wait();
    bt_socket_thread_delete();

    bt_control.type = BtControlType::BT_NONE;
    return ret;
}

static int bt_open()
{
    int ret = 0;

    if (ble_is_open()) {
        APP_DEBUG("Close ble wifi config server.");

        if (ble_close_server() < 0) {
            ret = -1;
            return ret;
        }
    }

    APP_DEBUG("Open a2dp sink.");

    if (bt_open_a2dp_server() < 0) {
        ret = -1;
        return ret;
    }

    return ret;
}

static int ble_open()
{
    int ret = 0;

    if (bt_is_a2dp_open()) {
        APP_DEBUG("Close a2dp sink.");
        bt_close_a2dp_server();
    }

    if (ble_open_server() < 0) {
        ret = -1;
        return ret;
    }

    return ret;
}

int get_bt_mac(char *bt_mac)
{
    char reply[128] = {"\0"};
    FILE *fp = NULL;
    char *p = NULL;

    fp = fopen(BT_STATUS_PATH, "r");

    if (!fp) {
        printf("open get bt_mac file error!");
        return -1;
    }

    memset(reply, 0, sizeof(reply));

    while (fgets(reply, sizeof(reply), fp)) {
        p = strstr(reply, "<bd_addr>");

        if (!p) {
            continue;
        }

        p = p + strlen("<bd_addr>");
        memcpy(bt_mac, p, 17);
        bt_mac[17] = 0;
        p = NULL;
        break;
    }

    fclose(fp);

    APP_DEBUG("the bt mac : %s, len %d\n", bt_mac, strlen(bt_mac));

    return 0;
}

int get_wifi_mac(char *wifi_mac)
{
    int sock_mac;
    struct ifreq ifr_mac;
    char mac_addr[18] = {0};

    sock_mac = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_mac == -1) {
        APP_ERROR("create mac socket failed.");
        return -1;
    }

    memset(&ifr_mac, 0, sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, "wlan0", sizeof(ifr_mac.ifr_name) - 1);

    if ((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0) {
        APP_ERROR("Mac socket ioctl failed.");
        return -1;
    }

    sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);

    close(sock_mac);
    strncpy(wifi_mac, mac_addr, 18);
    APP_DEBUG("the wifi mac : %s\r\n", wifi_mac);

    return 0;
}

static int get_wifi_ip(char *wifi_ip)
{
    char system_cmd[512] = {0};
    char reply[128] = {"\0"};
    FILE *fp = NULL;
    char *p = NULL;

    snprintf(system_cmd, 512 - 1,
             "wpa_cli -iwlan0 -p/var/run/wpa_supplicant status > %s", WIFI_STATUS_PATH);
    system(system_cmd);

    fp = fopen(WIFI_STATUS_PATH, "r");

    if (!fp) {
        printf("open get wifi_status file error!");
        return -1;
    }

    memset(reply, 0, sizeof(reply));

    while (fgets(reply, sizeof(reply), fp)) {
        p = strstr(reply, "ip_address=");

        if (!p) {
            continue;
        }

        p = p + strlen("ip_address=");
        strncpy(wifi_ip, p, strlen(p) - 1);
        p = NULL;
        break;
    }

    fclose(fp);

    APP_DEBUG("the ip address : %s, len : %d \n", wifi_ip, strlen(wifi_ip));
    return 0;

}

static int get_wifi_ssid(char *wifi_ssid)
{
    char system_cmd[512] = {0};
    char reply[128] = {"\0"};
    FILE *fp = NULL;
    char *p = NULL;

    snprintf(system_cmd, 512 - 1,
             "wpa_cli -iwlan0 -p/var/run/wpa_supplicant status > %s", WIFI_STATUS_PATH);
    system(system_cmd);

    fp = fopen(WIFI_STATUS_PATH, "r");

    if (!fp) {
        printf("open get wifi_status file error!");
        return -1;
    }

    memset(reply, 0, sizeof(reply));

    while (fgets(reply, sizeof(reply), fp)) {
        if (strstr(reply, "bssid=")) {
            continue;
        }

        p = strstr(reply, "ssid=");

        if (!p) {
            continue;
        }

        p = p + strlen("ssid=");
        strncpy(wifi_ssid, p, strlen(p) - 1);
        p = NULL;
        break;
    }


    fclose(fp);

    APP_DEBUG("the wifi ssid : %s, len : %d \n", wifi_ssid, strlen(wifi_ssid));

    return 0;

}

static int get_wifi_bssid(char *wifi_bssid)
{
    char system_cmd[512] = {0};
    char reply[128] = {"\0"};
    FILE *fp = NULL;
    char *p = NULL;

    snprintf(system_cmd, 512 - 1,
             "wpa_cli -iwlan0 -p/var/run/wpa_supplicant status > %s", WIFI_STATUS_PATH);
    system(system_cmd);

    fp = fopen(WIFI_STATUS_PATH, "r");

    if (!fp) {
        printf("open get wifi_status file error!");
        return -1;
    }

    memset(reply, 0, sizeof(reply));

    while (fgets(reply, sizeof(reply), fp)) {
        p = strstr(reply, "bssid=");

        if (!p) {
            continue;
        }

        p = p + strlen("bssid=");
        strncpy(wifi_bssid, p, strlen(p) - 1);
        p = NULL;
        break;
    }


    fclose(fp);

    APP_DEBUG("the wifi bssid : %s\n", wifi_bssid);
    return 0;

}

int duer_bt_control(BtControl cmd, void *data, int len)
{
    using BtControl_rep_type = std::underlying_type<BtControl>::type;

    APP_DEBUG("controlBt, cmd: %d\n", cmd);

    int ret = 0;

    switch (cmd) {
    case BtControl::BT_IS_OPENED:
        ret = bt_is_a2dp_open();
        break;

    case BtControl::BLE_IS_OPENED:
        ret = ble_is_open();
        break;

    case BtControl::BT_OPEN:
        ret = bt_bsa_server_open();

        if (!ret)
            ret = bt_open();

        break;

    case BtControl::BT_CLOSE:
        ret = bt_close_a2dp_server();

        if (!ret)
            ret = bt_bsa_server_close();

        break;

    case BtControl::BLE_OPEN_SERVER:
        ret = bt_bsa_server_open();

        if (!ret)
            ret = ble_open();

        break;

    case BtControl::BLE_CLOSE_SERVER:
        ret = ble_close_server();

        if (!ret)
            ret = bt_bsa_server_close();

        break;

    case BtControl::GET_BT_MAC:
        if (get_bt_mac((char *)data) <= 0)
            ret = -1;

        break;

    case BtControl::GET_WIFI_MAC:
        if (get_wifi_mac((char *)data) <= 0)
            ret = -1;

        break;

    case BtControl::GET_WIFI_IP:
        if (get_wifi_ip((char *)data) <= 0)
            ret = -1;

        break;

    case BtControl::GET_WIFI_SSID:
        if (get_wifi_ssid((char *)data) <= 0)
            ret = -1;

        break;

    case BtControl::GET_WIFI_BSSID:
        if (get_wifi_bssid((char *)data) <= 0)
            ret = -1;

        break;

    case BtControl::BLE_SERVER_SEND:
        if (bt_socket_send(data, len) < 0) {
            APP_ERROR("Bt socket send data failed\n");
            ret = -1;
        }

        break;

    case BtControl::BT_VOLUME_UP:
        if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_VOLUME_UP) < 0) {
            APP_ERROR("Bt socket send volume up cmd failed\n");
            ret = -1;
        }

        break;

    case BtControl::BT_VOLUME_DOWN:
        if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_VOLUME_DOWN) < 0) {
            APP_ERROR("Bt socket send volume down cmd failed\n");
            ret = -1;
        }

        break;

    case BtControl::BT_RESUME_PLAY:
        if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_PLAY_START) < 0) {
            APP_ERROR("Bt socket send play cmd failed\n");
            ret = -1;
        }

        break;

    case BtControl::BT_PAUSE_PLAY:
        if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_PLAY_PAUSE) < 0) {
            APP_ERROR("Bt socket send pause cmd failed\n");
            ret = -1;
        }

        break;

    case BtControl::BT_AVRCP_FWD:
        if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_PLAY_PREVIOUS_TRACK) < 0) {
            APP_ERROR("Bt socket send previous track cmd failed\n");
            ret = -1;
        }

        break;

    case BtControl::BT_AVRCP_BWD:
        if (bt_control_cmd_send(BtControlCmd::APP_AVK_MENU_PLAY_NEXT_TRACK) < 0) {
            APP_ERROR("Bt socket send next track cmd failed\n");
            ret = -1;
        }

        break;

    default:
        APP_DEBUG("%s, cmd <%d> is not implemented.\n", __func__,
                  static_cast<BtControl_rep_type>(cmd));
    }

    return ret;
}
