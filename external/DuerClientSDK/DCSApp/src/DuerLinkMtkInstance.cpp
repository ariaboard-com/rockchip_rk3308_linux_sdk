/*
 * Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
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

#include <string>
#include <fstream>
#include <unistd.h>
#include <cstring>
#include <algorithm>
#include <netdb.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <csignal>
#include <errno.h>
#include <paths.h>
#include <sys/wait.h>

#include "LoggerUtils/DcsSdkLogger.h"
#include "DCSApp/DuerLinkMtkInstance.h"
#include "DCSApp/Configuration.h"
#include "DCSApp/DeviceIoWrapper.h"
#include "duer_link/duer_link_sdk.h"

#include <DeviceTools/PrintTickCount.h>

namespace duerOSDcsApp {
namespace application {

using namespace duerLink;

using std::string;
using std::vector;
using std::ifstream;

static string m_target_ssid;
static string m_target_pwd;
static string m_target_ssid_prefix;
static int m_ping_interval = 1;
static int m_network_status = 0;
static bool m_pinging = false;

DuerLinkMtkInstance* DuerLinkMtkInstance::m_duerLink_mtk_instance = nullptr;

int system_fd_closexec(const char* command) {
    int wait_val = 0;
    pid_t pid = -1;

    if (!command)
        return 1;

    if ((pid = vfork()) < 0)
        return -1;

    if (pid == 0) {
        int i = 0;
        int stdin_fd = fileno(stdin);
        int stdout_fd = fileno(stdout);
        int stderr_fd = fileno(stderr);
        long sc_open_max = sysconf(_SC_OPEN_MAX);
        if (sc_open_max < 0) {
            APP_ERROR("Warning, sc_open_max is unlimited!\n");
            sc_open_max = 20000; /* enough? */
        }
        /* close all descriptors in child sysconf(_SC_OPEN_MAX) */
        for (; i < sc_open_max; i++) {
            if (i == stdin_fd || i == stdout_fd || i == stderr_fd)
                continue;
            close(i);
        }

        execl(_PATH_BSHELL, "sh", "-c", command, (char*)0);
        APP_DEBUG("%s, %d\n", __func__, __LINE__);
        _exit(127);
    }

    while (waitpid(pid, &wait_val, 0) < 0) {
        APP_INFO("%s, errno: %d. This is fine.\n", __func__, errno);
        if (errno != EINTR) {
            wait_val = -1;
            break;
        }
    }

    return wait_val;
}

bool system_command(const char* cmd) {
    pid_t status = 0;
    bool ret_value = false;

    APP_INFO("System [%s]", cmd);

    status = system_fd_closexec(cmd);

    if (-1 == status) {
        APP_ERROR("system failed!");
    } else {
        if (WIFEXITED(status)) {
            if (0 == WEXITSTATUS(status)) {
                ret_value = true;
            } else {
                APP_ERROR("System shell script failed:[%d].", WEXITSTATUS(status));
            }
        } else {
            APP_INFO("System status = [%d]", WEXITSTATUS(status));
        }
    }

    return ret_value;
}

bool check_ap0_interface_status(string ap) {
    int sockfd;
    bool ret = false;
    struct ifreq ifr_mac;

    if ((sockfd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        APP_ERROR("socket create failed.");
        return false;
    }

    memset(&ifr_mac,0,sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, ap.c_str(), sizeof(ifr_mac.ifr_name)-1);

    if ((ioctl(sockfd, SIOCGIFHWADDR, &ifr_mac)) < 0) {
        APP_ERROR("Mac ioctl failed.");
    } else {
        APP_INFO("Mac ioctl suceess.");
        ret = true;
    }
    close(sockfd);

    return ret;
}

bool tmp_wpa_conf(const string ssid, const string pwd, const string path) {
    string tmp_conf_path;
    string bk_conf_path;
    string file_conf_path;

    if (ssid.empty() || pwd.empty() || path.empty()){
        APP_ERROR("Input param is empty.");
        return false;
    }

    tmp_conf_path = path + ".tmp";
    bk_conf_path = path + ".bk";
    file_conf_path = path;

    //如果空密码,修改此处
    string insert_ct = NETWORK_WPA_CONF_HEAD;
    insert_ct += ssid;
    insert_ct += NETWORK_WPA_CONF_MIDDLE;
    insert_ct += pwd;
    insert_ct += NETWORK_WPA_CONF_END;

    APP_INFO("Wpa_supplicant.conf insert CT [%s]", insert_ct.c_str());

    deviceCommonLib::deviceTools::printTickCount("network_config cp wpa.conf begin.");

    string cmd_buff = "cp ";
    cmd_buff += file_conf_path + " " + bk_conf_path;
    cmd_buff += " &";

    if (!system_command(cmd_buff.c_str())) {
        APP_ERROR("Bake Wpa_supplicant.conf failed.");
        return false;
    }

    deviceCommonLib::deviceTools::printTickCount("network_config cp wpa.conf end.");

    sync();

    deviceCommonLib::deviceTools::printTickCount("network_config sync1 wpa.conf end.");

    APP_INFO("Bake Wpa_supplicant.conf successed.");

    string line;
    std::ifstream in(file_conf_path);
    std::ofstream out(tmp_conf_path);

    while (getline(in, line)) {
        if (strstr(line.c_str(), DUERLINK_WPA_INSERT_FLAG)){
            out << line << std::endl;
            out << insert_ct << std::endl;
            break;
        } else {
            out << line << std::endl;
        }
    }

    in.close();
    out.close();

    deviceCommonLib::deviceTools::printTickCount("network_config check wpa.conf end.");

    cmd_buff.clear();
    cmd_buff = "mv ";
    cmd_buff += tmp_conf_path + " " + file_conf_path;
    cmd_buff += " &";

    if (!system_command(cmd_buff.c_str())) {
        APP_ERROR("Update wpa_supplicant.conf failed.");
        return false;
    }

    deviceCommonLib::deviceTools::printTickCount("network_config update wpa.conf end.");

    sync();

    deviceCommonLib::deviceTools::printTickCount("network_config sync2 wpa.conf end.");

    APP_INFO("Update wpa_supplicant.conf successed.");

    return true;
}

void DuerLinkMtkInstance::OnNetworkReady() {
    system_command("/etc/init.d/S49ntp stop;"
                   "ntpdate cn.pool.ntp.org;"
                   "/etc/init.d/S49ntp start");
}

bool start_wpa_supplicant() {
    deviceCommonLib::deviceTools::printTickCount("network_config connect_ap begin.");
    system_command("killall wpa_supplicant; dhcpcd -k wlan0; killall dhcpcd;"
                   "ifconfig wlan0 0.0.0.0");
    bool ret = system_command("wpa_supplicant -Dnl80211 -i wlan0 "
                              "-c /data/cfg/wpa_supplicant.conf &");
    if (!ret) {
        APP_ERROR("start wpa_supplicant failed\n");
        deviceCommonLib::deviceTools::printTickCount("network_config connect_ap end.");
        return ret;
    }
    ret = system_command("dhcpcd wlan0 -t 0 &");
    if (!ret) {
        APP_ERROR("dhcpcd failed\n");
        deviceCommonLib::deviceTools::printTickCount("network_config connect_ap end.");
        return ret;
    }
    deviceCommonLib::deviceTools::printTickCount("network_config connect_ap end.");

    return true;
}

bool stop_wpa_supplicant() {
    return system_command("killall wpa_supplicant; dhcpcd -k wlan0;"
                          "killall dhcpcd; ifconfig wlan0 0.0.0.0");
}

bool set_ap_tmp_ipaddr() {
    if (!check_ap0_interface_status(DUERLINK_NETWORK_DEVICE_MTK_FOR_AP)) {
        APP_INFO("%s is down.", DUERLINK_NETWORK_DEVICE_MTK_FOR_AP);
        return false;
    }

    string cmd;
    cmd += "ifconfig ";
    cmd += DUERLINK_NETWORK_DEVICE_MTK_FOR_AP;
    cmd += " 192.168.1.1 netmask 255.255.255.0 &";

    return system_command(cmd.c_str());
}

bool delete_tmp_addr() {
    if (!check_ap0_interface_status(DUERLINK_NETWORK_DEVICE_MTK_FOR_AP)) {
        APP_INFO("%s is down.", DUERLINK_NETWORK_DEVICE_MTK_FOR_AP);
        return true;
    }

    string cmd;
    cmd += "ip addr delete 192.168.1.1 dev ";
    cmd += DUERLINK_NETWORK_DEVICE_MTK_FOR_AP;
    cmd += " &";

    return system_command(cmd.c_str());
}

bool echo_ap_to_devwifi() {
    return system_command("echo AP > /dev/wmtWifi &");
}

bool stop_ap_mode() {
    return system_command("killall hostapd &");
}

bool start_ap_mode() {
    string cmd = " ";
    cmd += DUERLINK_WPA_HOSTAPD_CONFIG_FILE_PATH_MTK;
    cmd += " &";

    if (stop_ap_mode()) {
        APP_INFO("[start_ap_mode] hostapd is ruuning.");
    }
    sleep(1);

    string tmp = "hostapd ";
    tmp += cmd;

    return system_command(tmp.c_str());
}

bool starup_ap_interface() {
    if (check_ap0_interface_status(DUERLINK_NETWORK_DEVICE_MTK_FOR_AP)) {
        APP_INFO("%s is up.", DUERLINK_NETWORK_DEVICE_MTK_FOR_AP);

        return true;
    }

    string cmd;
    cmd += "ifconfig ";
    cmd += DUERLINK_NETWORK_DEVICE_MTK_FOR_AP;
    cmd += " up &";

    return system_command(cmd.c_str());
}

bool down_ap_interface() {
    if (!check_ap0_interface_status(DUERLINK_NETWORK_DEVICE_MTK_FOR_AP)) {
        APP_INFO("%s is down.", DUERLINK_NETWORK_DEVICE_MTK_FOR_AP);
        return true;
    }

    string cmd;
    cmd += "ifconfig ";
    cmd += DUERLINK_NETWORK_DEVICE_MTK_FOR_AP;
    cmd += " down &";

    return system_command(cmd.c_str());
}

bool starup_wlan0_interface() {

    return system_command("ifconfig wlan0 up &");
}

bool down_wlan0_interface() {
    if (!check_ap0_interface_status(DUERLINK_NETWORK_DEVICE_MTK_FOR_AP)) {
        APP_INFO("%s is down.", DUERLINK_NETWORK_DEVICE_MTK_FOR_AP);
        return true;
    }

    system_command("ifconfig wlan0 0.0.0.0");

    return system_command("ifconfig wlan0 down &");
}

bool stop_dhcp_server() {
    return system_command("killall dnsmasq &");
}

bool start_dhcp_server() {
    if (stop_dhcp_server()) {
        APP_INFO("[Start_dhcp_server] dnsmasq is killed.");
    }
    sleep(1);

    return system_command("dnsmasq &");
}

bool change_hostapd_conf(const string ssid) {

    string conf_ct_all = NETWORK_SSID_CONFIG_HEAD1;
    conf_ct_all += DUERLINK_NETWORK_DEVICE_MTK_FOR_AP;
    conf_ct_all += NETWORK_SSID_CONFIG_HEAD2;
    conf_ct_all += ssid;
    conf_ct_all += "\n";
    conf_ct_all += NETWORK_SSID_CONFIG_MIDDLE;
    conf_ct_all += "\n";
    conf_ct_all += NETWORK_SSID_CONFIG_END;

    APP_INFO("Hostapd:\n%s", conf_ct_all.c_str());

    string ap_hstapd_temp_confile_path = DUERLINK_WPA_HOSTAPD_CONFIG_FILE_PATH_MTK;
    ap_hstapd_temp_confile_path += ".tmp";

    std::ofstream out(ap_hstapd_temp_confile_path.c_str());
    out << conf_ct_all << std::endl;
    out.close();

    string command_mv_string = ap_hstapd_temp_confile_path + " " + DUERLINK_WPA_HOSTAPD_CONFIG_FILE_PATH_MTK;
    string tmp = "mv ";
    tmp += command_mv_string;
    tmp += " &";

    return system_command(tmp.c_str());
}

bool get_device_interface_mac(string &mac_address) {
    int sock_mac;
    struct ifreq ifr_mac;
    char mac_addr[30] = {0};

    sock_mac = socket( AF_INET, SOCK_STREAM, 0 );
    if (sock_mac == -1) {
        APP_ERROR("create mac socket failed.");
        return false;
    }

    memset(&ifr_mac,0,sizeof(ifr_mac));
    strncpy(ifr_mac.ifr_name, DUERLINK_NETWORK_DEVICE_MTK_FOR_WORK, sizeof(ifr_mac.ifr_name)-1);

    if ((ioctl( sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0) {
        APP_ERROR("Mac socket ioctl failed.");
        close(sock_mac);
        return false;
    }

    sprintf(mac_addr,"%02X%02X",
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
            (unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]);

    APP_INFO("local mac:%s",mac_addr);

    close(sock_mac);

    mac_address = mac_addr;

    std::transform(mac_address.begin(), mac_address.end(), mac_address.begin(), toupper);

    return true;
}

void set_softAp_ssid_and_pwd(string hostapd_config_file_path) {
    // hostapd configuration file path
    string m_ap_hostapd_conffile_path = hostapd_config_file_path;

    string ssid;
    string suffix;
    string mac_address;

    m_target_ssid_prefix = Configuration::getInstance()->getSsidPrefix();

    APP_INFO("Configure SSID Prefix: [%s].", m_target_ssid_prefix.c_str());

    get_device_interface_mac(mac_address);
    suffix += mac_address;

    ssid = m_target_ssid_prefix + suffix;
    change_hostapd_conf(ssid);

    return ;
}

bool softap_prepare_env_cb() {
    bool ret_value = true;

    APP_INFO("prepare softAp environment resource.");

#ifdef Rk3308
    string cmd;
    string mac_address;
    stop_wpa_supplicant();
    usleep(20 * 1000); //20ms
    cmd.append("softapDemo ");
    cmd += Configuration::getInstance()->getSsidPrefix();
    get_device_interface_mac(mac_address);
    cmd += mac_address;
    system_command(cmd.c_str());
    int time = 100;
    while (time-- > 0 && access("/var/run/hostapd", F_OK)) {
        usleep(100 * 1000);
    }
    usleep(100 * 1000);
#else
    set_softAp_ssid_and_pwd(DUERLINK_WPA_HOSTAPD_CONFIG_FILE_PATH_MTK);

    //stop wpa supplicant.
    if (!stop_wpa_supplicant()) {
        APP_ERROR("stop supplicant failed.");
        ret_value = false;
    }

    //down and clear address.
//    if (!down_wlan0_interface()) {
//        APP_ERROR("down wlan0 failed.");
//        ret_value = false;
//    }

    //echo ap to /dev/wifi file.
    if (!echo_ap_to_devwifi()) {
        APP_ERROR("echo ap to wifi failed.");
        ret_value = false;
    }

    // start ap0 interface.
    if (!starup_ap_interface()) {
        APP_ERROR("starup ap interface failed.");
        ret_value = false;
    }

    //set ap ip address and network mask value.
    if (!set_ap_tmp_ipaddr()) {
        APP_ERROR("set ap tmp ipaddr failed.");
        ret_value = false;
    }

    //start hostapd service process.
    if (!start_ap_mode()) {
        APP_ERROR("start ap mode failed.");
        ret_value = false;
    }

    //startup dhcp server.
    if (!start_dhcp_server()) {
        APP_ERROR("start dhcp server failed.");
        ret_value = false;
    }
#endif
    APP_INFO("End prepare softAp environment resource.");

    return ret_value;
}

bool softap_destroy_env_cb() {
    bool ret_value = true;

    APP_INFO("destroy softAp environment resource.");
#ifdef Rk3308
    system_command("softapDemo stop");
#else
    if (!stop_dhcp_server()) {
        APP_ERROR("stop dhcp server failed.");
        ret_value = false;
    }

    //delete tmp addression.
    if (!delete_tmp_addr()) {
        APP_ERROR("delete tmp addr failed.");
        ret_value = false;
    }

    //stop ap mode.
    if (!stop_ap_mode()) {
        APP_ERROR("stop ap mode failed.");
        ret_value = false;
    }

    //shutdown ap0
    if (!down_ap_interface()) {
        APP_ERROR("down ap interface failed.");
        ret_value = false;
    }

    //up wlan0
//    if (!starup_wlan0_interface()) {
//        APP_ERROR("up wlan0 interface failed.");
//        ret_value = false;
//    }
#endif
    APP_INFO("End destroy softAp environment resource.");

    return ret_value;
}

bool platform_connect_wifi_cb(const char *ssid,
                              const int ssid_len,
                              const char *pwd,
                              const int pwd_len) {
    m_target_ssid.clear();
    m_target_pwd.clear();
    m_target_ssid = ssid;
    m_target_pwd = pwd;

    APP_INFO("Connect wifi, ssid and pwd [%s] [%s].",
             m_target_ssid.c_str(),
             m_target_pwd.c_str());

    tmp_wpa_conf(m_target_ssid, m_target_pwd, DUERLINK_WPA_CONFIG_FILE_MTK);

    return start_wpa_supplicant();
}

bool ble_prepare_env_cb() {
    APP_INFO("prepare ble environment resource.");
    framework::DeviceIo::getInstance()->controlBt(framework::BtControl::BLE_OPEN_SERVER);
    APP_INFO("End prepare ble environment resource.");

    return true;
}

bool ble_destroy_env_cb() {
    APP_INFO("destroy ble environment resource.");
    framework::DeviceIo::getInstance()->controlBt(framework::BtControl::BLE_CLOSE_SERVER);
    APP_INFO("End destroy ble environment resource.");

    return true;
}

int ble_send_data_cb(char *data, unsigned short data_length) {
    APP_INFO("ble send data.");

    framework::DeviceIo::getInstance()->controlBt(framework::BtControl::BLE_SERVER_SEND,
                                                  data,
                                                  data_length);

    APP_INFO("End ble send data.");

    return 0;
}

DuerLinkMtkInstance::DuerLinkMtkInstance() {
    m_maxPacketSize = MAX_PACKETS_COUNT;
    m_datalen = 56;
    m_nsend = 0;
    m_nreceived = 0;
    m_icmp_seq = 0;
    m_pMtkConfigObserver = NULL;
    m_stop_network_recovery = false;

    m_operation_type = operation_type::EOperationStart;

    pthread_mutex_init(&m_ping_lock, NULL);

    init_network_config_timeout_alarm();
}

DuerLinkMtkInstance::~DuerLinkMtkInstance() {
    pthread_mutex_destroy(&m_ping_lock);

    duerLinkSdk::get_instance()->destroy();
}

DuerLinkMtkInstance* DuerLinkMtkInstance::get_instance() {
    if (NULL == m_duerLink_mtk_instance) {
        m_duerLink_mtk_instance = new DuerLinkMtkInstance();
    }

    return m_duerLink_mtk_instance;
}

void DuerLinkMtkInstance::destroy() {
    if (NULL != m_duerLink_mtk_instance) {
        delete m_duerLink_mtk_instance;
        m_duerLink_mtk_instance = NULL;
    }
}

void DuerLinkMtkInstance::init_duer_link_log(duerLink::duer_link_log_cb_t duer_link_log) {
    duerLinkSdk::get_instance()->init_duer_link_log(duer_link_log);
}

bool DuerLinkMtkInstance::set_wpa_conf(bool change_file) {
    int skip_line = 0;
    string tmp_conf_path;
    string bk_conf_path;
    string file_conf_path;
    string cmd_buff;

    if (m_target_ssid.empty() || m_target_pwd.empty()){
        APP_ERROR("Input param is empty.");
        return false;
    }

    tmp_conf_path = DUERLINK_WPA_CONFIG_FILE_MTK;
    tmp_conf_path += ".tmp";
    bk_conf_path = DUERLINK_WPA_CONFIG_FILE_MTK;
    bk_conf_path += ".bk";
    file_conf_path = DUERLINK_WPA_CONFIG_FILE_MTK;

    if (!change_file) {
        cmd_buff.clear();
        cmd_buff = "mv ";
        cmd_buff += bk_conf_path + " " + file_conf_path;
        cmd_buff += " &";

        if (!system_command(cmd_buff.c_str())) {
            APP_ERROR("Reset wpa_supplicant.conf failed.");
            return false;
        }
        sync();

        APP_INFO("Reset wpa_supplicant.conf successed.");

        return false;
    }

    string insert_ct = NETWORK_WPA_CONF_HEAD;
    insert_ct += m_target_ssid;
    insert_ct += NETWORK_WPA_CONF_MIDDLE;
    insert_ct += m_target_pwd;
    insert_ct += NETWORK_WPA_CONF_END;

    string line;
    std::ifstream in(bk_conf_path);
    std::ofstream out(tmp_conf_path);

    //read file head
    while (getline (in, line)) {
        if (strstr(line.c_str(), DUERLINK_WPA_INSERT_FLAG)){
            out << line << std::endl;
            out << insert_ct << std::endl;
            break;
        } else {
            out << line << std::endl;
        }
    }

    //read file body
    vector<string> body;
    string tmp_ssid = "\"" + m_target_ssid + "\"";
    while (getline (in, line)) {
        if (strstr(line.c_str(), tmp_ssid.c_str())) {
            APP_ERROR("Set ssid is already exists.");
            body.pop_back();
            skip_line = 3;
            continue;
        }

        if (skip_line != 0) {
            skip_line --;
            continue;
        } else {
            body.push_back(line);
        }
    }

    for (unsigned int i = 0; i < body.size(); i++) {
        out << body[i] << std::endl;
    }

    in.close();
    out.close();

    cmd_buff.clear();
    cmd_buff = "mv ";
    cmd_buff += tmp_conf_path + " " + file_conf_path;
    cmd_buff += " &";

    if (!system_command(cmd_buff.c_str())) {
        APP_ERROR("Update Wpa_supplicant.conf failed.");
        return false;
    }
    sync();

    APP_INFO("Update Wpa_supplicant.conf successed.");

    return true;
}

bool DuerLinkMtkInstance::is_first_network_config(string path) {
    ifstream it_stream;
    int length = 0;
    string wpa_config_file = path;

    it_stream.open(wpa_config_file.c_str());
    if (!it_stream.is_open()) {
        APP_ERROR("wpa config file open error.");
        return false;
    }

    it_stream.seekg(0,std::ios::end);
    length = it_stream.tellg();
    it_stream.seekg(0,std::ios::beg);

    char *buffer = new char[length + 1];
    it_stream.read(buffer, length);
    it_stream.close();
    buffer[length] = 0;

    char * position = nullptr;
    position = strstr(buffer,"ssid");
    delete [] buffer;
    buffer = nullptr;

    if (nullptr == position) {
        APP_ERROR("First network config.");
        return true;
    }

    APP_INFO("Not first network config.");

    return false;
}

void DuerLinkMtkInstance::sigalrm_fn(int sig) {
    APP_INFO("alarm is run.");

    get_instance()->m_operation_type = operation_type::EAutoEnd;

    get_instance()->stop_network_config_timeout_alarm();
    duerLinkSdk::get_instance()->stop_network_config();
}

void DuerLinkMtkInstance::init_network_config_timeout_alarm() {
    APP_INFO("set alarm.");

    signal(SIGALRM, sigalrm_fn);
}

void DuerLinkMtkInstance::start_network_config_timeout_alarm(int timeout) {
    APP_INFO("start alarm.");

    alarm(timeout);
}

void DuerLinkMtkInstance::stop_network_config_timeout_alarm() {
    APP_INFO("stop alarm.");

    alarm(0);
}

void DuerLinkMtkInstance::init_softAp_env_cb() {
    duerLinkSdk::get_instance()->init_platform_softAp_control(softap_prepare_env_cb,
                                                              softap_destroy_env_cb);
}

void DuerLinkMtkInstance::init_ble_env_cb() {
    duerLinkSdk::get_instance()->init_platform_ble_control(ble_prepare_env_cb,
                                                           ble_destroy_env_cb,
                                                           ble_send_data_cb);
}

void DuerLinkMtkInstance::init_wifi_connect_cb() {
    duerLinkSdk::get_instance()->init_platform_connect_control(platform_connect_wifi_cb);
}

void DuerLinkMtkInstance::start_network_recovery() {
#ifdef Box86
    check_recovery_network_status();
#else
    if (is_first_network_config(DUERLINK_WPA_CONFIG_FILE_MTK)) {
        get_instance()->m_operation_type = operation_type::EAutoConfig;

        start_network_config_timeout_alarm(DUERLINK_AUTO_CONFIG_TIMEOUT);
        duerLinkSdk::get_instance()->start_network_config();
    } else {
        start_wpa_supplicant();
        check_recovery_network_status();
    }
#endif
}

void DuerLinkMtkInstance::stop_network_recovery() {
    m_stop_network_recovery = true;
}

void DuerLinkMtkInstance::start_discover_and_bound() {
    duerLinkSdk::get_instance()->start_device_discover_and_bound();
}

#ifdef DUERLINK_V2
void DuerLinkMtkInstance::init_config_network_parameter(platform_type speaker_type,
                                                        auto_config_network_type autoType,
                                                        string devicedID,
                                                        string interface) {
    duerLinkSdk::get_instance()->init_config_network_parameter(speaker_type,
                                                               autoType,
                                                               devicedID,
                                                               interface);
}

void DuerLinkMtkInstance::init_discovery_parameter(string devicedID,
                                                   string appId,
                                                   string interface) {
    //test cuid and access token for linkplay
//    duerLinkSdk::get_instance()->set_config_json_file("/data/duer/duerLink_config.json");

    duerLinkSdk::get_instance()->init_discovery_parameter(devicedID, appId, interface, "/data/duer/bduss.txt");

}
#else

void DuerLinkMtkInstance::init_config_network_parameter(platform_type speaker_type,
                                                        auto_config_network_type autoType,
                                                        const std::string& devicedID,
                                                        const std::string& interface,
                                                        const std::string& bdussfile,
                                                        const std::string& clientId) {
    duerLinkSdk::get_instance()->init_config_network_parameter(speaker_type,
                                                               autoType,
                                                               devicedID,
                                                               interface,
                                                               bdussfile,
                                                               clientId);
}

void DuerLinkMtkInstance::init_discovery_parameter(const std::string& devicedID,
                                                   const std::string& appId,
                                                   const std::string& interface,
                                                   const std::string& bdussfile) {
    //test cuid and access token for linkplay
//    duerLinkSdk::get_instance()->set_config_json_file("/data/duer/duerLink_config.json");

    duerLinkSdk::get_instance()->init_discovery_parameter(devicedID, appId, interface, bdussfile);

}
#endif

void DuerLinkMtkInstance::set_networkConfig_observer(NetworkConfigStatusObserver* config_listener) {
    if (config_listener) {
        m_pMtkConfigObserver = config_listener;
    }
    duerLinkSdk::get_instance()->set_networkConfig_observer(config_listener);
}

void DuerLinkMtkInstance::set_monitor_observer(NetWorkPingStatusObserver *ping_listener) {
    if (ping_listener) {
        m_pObserver = ping_listener;
    }

    APP_INFO("Set monitor observer.");
}

bool DuerLinkMtkInstance::check_recovery_network_status() {
#ifdef MTK8516
    deviceCommonLib::deviceTools::printTickCount("network_config main_thread recovery begin.");
#endif

    if (nullptr != m_pMtkConfigObserver) {
        (m_pMtkConfigObserver)->notify_network_config_status(duerLink::ENetworkRecoveryStart);
    }

    int network_check_count = 0;
    bool recovery = false;

    while(!(recovery = ping_network(false))) {
        if (m_stop_network_recovery || network_check_count == DUERLINK_NETWORK_CONFIGURE_PING_COUNT) {
            APP_ERROR("Network recovery ping failed.");

            break;
        }

        sleep(1);
        network_check_count++;
    }

    if (m_stop_network_recovery) {
        APP_INFO("Network recovery cancel.");
        start_network_config(DUERLINK_AUTO_CONFIG_TIMEOUT);
        return true;
    } else if (recovery) {
        if (nullptr != m_pMtkConfigObserver) {
            APP_INFO("Network recovery succed.");

            (m_pMtkConfigObserver)->notify_network_config_status(duerLink::ENetworkRecoverySucceed);
        }
        return true;
    } else {
        if (nullptr != m_pMtkConfigObserver) {
            APP_ERROR("Network recovery failed.");

            (m_pMtkConfigObserver)->notify_network_config_status(duerLink::ENetworkRecoveryFailed);
        }

        return false;
    }
}

void DuerLinkMtkInstance::ble_client_connected() {
    duerLinkSdk::get_instance()->ble_client_connected();
}

void DuerLinkMtkInstance::ble_client_disconnected() {
    duerLinkSdk::get_instance()->ble_client_disconnected();
}

bool DuerLinkMtkInstance::ble_recv_data(void *data, int len) {
    return duerLinkSdk::get_instance()->ble_recv_data(data, len);
}

bool DuerLinkMtkInstance::start_network_config(int timeout) {
    APP_INFO("start configing %d.", timeout);

    m_operation_type = operation_type::EManualConfig;

    start_network_config_timeout_alarm(timeout);
    return duerLinkSdk::get_instance()->start_network_config();
}

void DuerLinkMtkInstance::stop_network_config() {
    APP_INFO("start stopping.");

    m_operation_type = operation_type::EAutoEnd;

    stop_network_config_timeout_alarm();
    duerLinkSdk::get_instance()->stop_network_config();
}

void DuerLinkMtkInstance::set_dlp_data_observer(DuerLinkReceivedDataObserver* observer) {
    duerLinkSdk::get_instance()->set_dlp_data_observer(observer);
}

bool DuerLinkMtkInstance::send_dlp_msg_to_all_clients(const string& sendBuffer) {
    return duerLinkSdk::get_instance()->send_dlp_msg_to_all_clients(sendBuffer);
}

bool DuerLinkMtkInstance::send_dlp_msg_by_specific_session_id(
    const string& message, unsigned short sessionId) {
    return duerLinkSdk::get_instance()->send_dlp_msg_to_client_by_specific_id(message, sessionId);
}

bool DuerLinkMtkInstance::start_loudspeaker_ctrl_devices_service(
    device_type device_type_value, const string& client_id, const std::string& message) {
    return duerLinkSdk::get_instance()->start_loudspeaker_ctrl_devices_service(device_type_value,
                                                                               client_id,
                                                                               message);
}

bool DuerLinkMtkInstance::start_loudspeaker_ctrl_devices_service_by_device_id(const string& device_id) {
    return duerLinkSdk::get_instance()->start_loudspeaker_ctrl_devices_service_by_device_id(device_id);
}

bool DuerLinkMtkInstance::send_msg_to_devices_by_spec_type(const string& msg_buf,
                                                           device_type device_type_value) {
    return duerLinkSdk::get_instance()->send_msg_to_devices_by_spec_type(msg_buf,
                                                                         device_type_value);
}

bool DuerLinkMtkInstance::disconnect_devices_connections_by_spe_type(
        device_type device_type_value, const string& message) {
    APP_DEBUG("device_type_value = [%d], message = [%s]", device_type_value, message.c_str());
    return duerLinkSdk::get_instance()->disconnect_devices_connections_by_spe_type(device_type_value,
                                                                                   message);
}

bool DuerLinkMtkInstance::get_curret_connect_device_info(string& client_id,
                                                         string& device_id,
                                                         device_type device_type_value) {
    return duerLinkSdk::get_instance()->get_curret_connect_device_info(client_id,
                                                                       device_id,
                                                                       device_type_value);
}

void DuerLinkMtkInstance::set_network_observer(NetWorkPingStatusObserver *observer) {
    if (observer != nullptr){
        m_pObserver = observer;
    }
}

unsigned short DuerLinkMtkInstance::getChksum(unsigned short *addr,int len) {
    int nleft = len;
    int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    while (nleft > 1) {
        sum += *w++;
        nleft-= 2;
    }

    if (nleft == 1) {
        *(unsigned char *)(&answer) = *(unsigned char *)w;
        sum += answer;
    }

    sum = ((sum>>16) + (sum&0xffff));
    sum += (sum>>16);
    answer = ~sum;

    return answer;
}

int DuerLinkMtkInstance::packIcmp(int pack_no, struct icmp* icmp) {
    int packsize;
    struct icmp *picmp;
    struct timeval *tval;

    picmp = icmp;
    picmp->icmp_type = ICMP_ECHO;
    picmp->icmp_code = 0;
    picmp->icmp_cksum = 0;
    picmp->icmp_seq = pack_no;
    picmp->icmp_id = m_pid;
    packsize = (8 + m_datalen);
    tval= (struct timeval *)icmp->icmp_data;
    gettimeofday(tval, nullptr);
    picmp->icmp_cksum = getChksum((unsigned short *)icmp, packsize);

    return packsize;
}

bool DuerLinkMtkInstance::unpackIcmp(char *buf, int len, struct IcmpEchoReply *icmpEchoReply) {
    int iphdrlen;
    struct ip *ip;
    struct icmp *icmp;
    struct timeval *tvsend, tvrecv, tvresult;
    double rtt;

    ip = (struct ip *)buf;
    iphdrlen = ip->ip_hl << 2;
    icmp = (struct icmp *)(buf + iphdrlen);
    len -= iphdrlen;

    if (len < 8) {
        APP_ERROR("ICMP packets's length is less than 8.");
        return false;
    }

    if( (icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == m_pid) ) {
        tvsend = (struct timeval *)icmp->icmp_data;
        gettimeofday(&tvrecv, nullptr);
        tvresult = timevalSub(tvrecv, *tvsend);
        rtt = tvresult.tv_sec*1000 + tvresult.tv_usec/1000;  //ms
        icmpEchoReply->rtt = rtt;
        icmpEchoReply->icmpSeq = icmp->icmp_seq;
        icmpEchoReply->ipTtl = ip->ip_ttl;
        icmpEchoReply->icmpLen = len;

        return true;
    } else {
        return false;
    }
}

struct timeval DuerLinkMtkInstance::timevalSub(struct timeval timeval1, struct timeval timeval2) {
    struct timeval result;

    result = timeval1;

    if ((result.tv_usec < timeval2.tv_usec) && (timeval2.tv_usec < 0)) {
        -- result.tv_sec;
        result.tv_usec += 1000000;
    }

    result.tv_sec -= timeval2.tv_sec;

    return result;
}

bool DuerLinkMtkInstance::sendPacket() {
    size_t packetsize;
    while( m_nsend < m_maxPacketSize) {
        m_nsend ++;
        m_icmp_seq ++;
        packetsize = packIcmp(m_icmp_seq, (struct icmp*)m_sendpacket);

        if (sendto(m_sockfd,m_sendpacket, packetsize, 0, (struct sockaddr *) &m_dest_addr, sizeof(m_dest_addr)) < 0) {
            APP_ERROR("Ping sendto failed:%s.", strerror(errno));
            continue;
        }
    }

    return true;
}

bool DuerLinkMtkInstance::recvPacket(PingResult &pingResult) {
    int len = 0;
    struct IcmpEchoReply icmpEchoReply;
    int maxfds = m_sockfd + 1;
    int nfd  = 0;
    fd_set rset;
    struct timeval timeout;
    socklen_t fromlen = sizeof(m_from_addr);

    timeout.tv_sec = MAX_WAIT_TIME;
    timeout.tv_usec = 0;

    FD_ZERO(&rset);

    for (int recvCount = 0; recvCount < m_maxPacketSize; recvCount ++) {
        FD_SET(m_sockfd, &rset);
        if ((nfd = select(maxfds, &rset, nullptr, nullptr, &timeout)) == -1) {
            APP_ERROR("Ping recv select failed:%s.", strerror(errno));
            continue;
        }

        if (nfd == 0) {
            icmpEchoReply.isReply = false;
            pingResult.icmpEchoReplys.push_back(icmpEchoReply);
            continue;
        }

        if (FD_ISSET(m_sockfd, &rset)) {
            if ((len = recvfrom(m_sockfd,
                                m_recvpacket,
                                sizeof(m_recvpacket),
                                0,
                                (struct sockaddr *)&m_from_addr,
                                &fromlen)) <0) {
                if(errno == EINTR) {
                    continue;
                }
                APP_ERROR("Ping recvfrom failed: %s.", strerror(errno));
                continue;
            }

            icmpEchoReply.fromAddr = inet_ntoa(m_from_addr.sin_addr) ;
            if (strncmp(icmpEchoReply.fromAddr.c_str(), pingResult.ip, strlen(pingResult.ip)) != 0) {
                recvCount--;
                continue;
            }
        }

        if (!unpackIcmp(m_recvpacket, len, &icmpEchoReply)) {
            recvCount--;
            continue;
        }

        icmpEchoReply.isReply = true;
        pingResult.icmpEchoReplys.push_back(icmpEchoReply);
        m_nreceived ++;
    }

    return true;

}

bool DuerLinkMtkInstance::getsockaddr(const char * hostOrIp, struct sockaddr_in* sockaddr) {
    struct hostent *host;
    struct sockaddr_in dest_addr;
    unsigned long inaddr = 0l;

    bzero(&dest_addr,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;

    inaddr = inet_addr(hostOrIp);
    if (inaddr == INADDR_NONE) {
        host = gethostbyname(hostOrIp);
        if (host == nullptr) {
            return false;
        }
        memcpy( (char *)&dest_addr.sin_addr,host->h_addr, host->h_length);
    } else if (!inet_aton(hostOrIp, &dest_addr.sin_addr)) {
        return false;
    }

    *sockaddr = dest_addr;

    return true;
}

bool DuerLinkMtkInstance::ping(string host, int count, PingResult& pingResult) {
    int size = 50 * 1024;
    IcmpEchoReply icmpEchoReply;

    m_nsend = 0;
    m_nreceived = 0;
    pingResult.icmpEchoReplys.clear();
    m_maxPacketSize = count;
    m_pid = getpid();

    pingResult.dataLen = m_datalen;

    if ((m_sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) {
        APP_ERROR("Ping socket failed:%s.", strerror(errno));
        pingResult.error = strerror(errno);
        return false;
    }

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) != 0) {
        APP_ERROR("Setsockopt SO_RCVBUF failed:%s.", strerror(errno));
        close(m_sockfd);
        return false;
    }

    if (!getsockaddr(host.c_str(), &m_dest_addr)) {
        pingResult.error = "unknow host " + host;
        close(m_sockfd);
        return false;
    }

    strcpy(pingResult.ip, inet_ntoa(m_dest_addr.sin_addr));

    sendPacket();
    recvPacket(pingResult);

    pingResult.nsend = m_nsend;
    pingResult.nreceived = m_nreceived;

    close(m_sockfd);

    return true;
}

bool DuerLinkMtkInstance::ping_network(bool wakeupTrigger) {
    string hostOrIp = PING_DEST_HOST1;
    int nsend = 0, nreceived = 0;
    bool ret;
    PingResult pingResult;
    InternetConnectivity networkResult = UNAVAILABLE;

    pthread_mutex_lock(&m_ping_lock);

    for (int count = 1; count <= MAX_PACKETS_COUNT; count ++) {
        memset(&pingResult.ip, 0x0, 32);
        ret = ping(hostOrIp, 1, pingResult);

        if (!ret) {
            APP_ERROR("Ping error:%s", pingResult.error.c_str());
        } else {
            nsend += pingResult.nsend;
            nreceived += pingResult.nreceived;
            if (nreceived > 0)
                break;
        }

        if (count == 2) {
            hostOrIp = PING_DEST_HOST2;
        }
    }

    if (nreceived > 0) {
        ret = true;
        networkResult = AVAILABLE;
        if (m_network_status == (int)UNAVAILABLE) {
            m_ping_interval = 1;
        } else {
            if (m_ping_interval < MAX_PING_INTERVAL) {
                m_ping_interval = m_ping_interval * 2;
                if (m_ping_interval > MAX_PING_INTERVAL) {
                    m_ping_interval = MAX_PING_INTERVAL;
                }
            }
        }
        m_network_status = 1;
    } else {
        ret = false;
        networkResult = UNAVAILABLE;
        m_network_status = 0;
        m_ping_interval = 1;
    }

    if (m_pObserver) {
        m_pObserver->network_status_changed(networkResult, wakeupTrigger);
    }

    pthread_mutex_unlock(&m_ping_lock);

    return ret;
}

void *DuerLinkMtkInstance::monitor_work_routine(void *arg) {
    auto thread = static_cast<DuerLinkMtkInstance*>(arg);
    int time_interval = 1;
    int time_count = 1;
    while(1) {
        thread->ping_network(false);
        time_count = time_interval = m_ping_interval;
        APP_INFO("monitor_work_routine m_ping_interval:%d", m_ping_interval);

        while (time_count > 0) {
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            ::select(0, NULL, NULL, NULL, &tv);
            time_count--;
            if (time_interval != m_ping_interval) {
                APP_INFO("monitor_work_routine m_ping_interval:%d, time_interval:%d", m_ping_interval, time_interval);
                break;
            }
        }
    }

    return nullptr;
}

void DuerLinkMtkInstance::start_network_monitor() {
    pthread_t network_config_threadId;

    pthread_create(&network_config_threadId, nullptr, monitor_work_routine, this);
    pthread_detach(network_config_threadId);
}

void *DuerLinkMtkInstance::verify_work_routine(void *arg) {
    auto thread = static_cast<DuerLinkMtkInstance*>(arg);
    APP_INFO("verify_work_routine begin");
    m_pinging = true;
    thread->ping_network(true);
    m_pinging = false;
    APP_INFO("verify_work_routine end");

    return nullptr;
}

void DuerLinkMtkInstance::start_verify_network() {
    if (!m_pinging) {
        pthread_t network_verify_threadId;

        pthread_create(&network_verify_threadId, nullptr, verify_work_routine, this);
        pthread_detach(network_verify_threadId);
    } else {
        APP_INFO("start_verify_network m_pinging is TRUE, return");
    }
}

#include <errno.h>
#include <fcntl.h>
class HostAPHelper {
 public:
  static const char constexpr *WIFI_CHIP_TYPE_PATH = "/sys/class/rkwifi/chip";
  HostAPHelper() { memset(iname, 0, sizeof(iname)); };
  // copy from external/softapDemo/src/main.c
  int check_wifi_chip_type_string() {
    if (iname[0])
      return 0;

    int wififd = open(WIFI_CHIP_TYPE_PATH, O_RDONLY);

    if (wififd < 0) {
      APP_ERROR("Can't open %s, errno = %d", WIFI_CHIP_TYPE_PATH, errno);
      return -1;
    }
    if (0 >= read(wififd, iname, sizeof(iname))) {
      APP_ERROR("read %s failed", WIFI_CHIP_TYPE_PATH);
      close(wififd);
      return -1;
    }
    close(wififd);
    if (!strncmp(iname, "RTL", 3))
      strcpy(iname, "p2p0");
    else
      strcpy(iname, "wlan1");
    APP_INFO("%s: %s", __FUNCTION__, iname);

    return 0;
  }

  char* get_iname() {
    static std::mutex mtx;
    std::lock_guard<std::mutex> _lk(mtx);
    check_wifi_chip_type_string();
    if (iname[0])
      return iname;
    return nullptr;
  }

 private:
  char iname[32];
};
static HostAPHelper hostap_helper;

const char* DuerLinkMtkInstance::get_hostap_interface() {
    char *iname = hostap_helper.get_iname();
    return iname ? (const char*)iname : "wlan1";
}

}  // namespace application
}  // namespace duerOSDcsApp
