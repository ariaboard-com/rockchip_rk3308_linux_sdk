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

#ifndef DCS_DUERLINKMTKINSTANCE_H
#define DCS_DUERLINKMTKINSTANCE_H

#include <string>
#include <vector>
#include <netinet/in.h>
#include "duer_link/network_define_public.h"
#include "network_observer/network_status_observer.h"
#include "device_ctrl_observer/duer_link_received_data_observer.h"

#define NETWORK_SSID_CONFIG_HEAD1       "#Basic configuration\n\ninterface="
#define NETWORK_SSID_CONFIG_HEAD2       "\nssid="
#define NETWORK_SSID_CONFIG_MIDDLE      "channel=6\nctrl_interface=/var/run/hostapd\n\n"\
                                        "#WPA and WPA2 configuration\n"\
                                        "macaddr_acl=0\nauth_algs=1\nignore_broadcast_ssid=0\n#wpa=2\n"\
                                        "#wpa_key_mgmt=WPA-PSK\n#wpa_passphrase="
#define NETWORK_SSID_CONFIG_END         "#rsn_pairwise=CCMP\n#wpa_pairwise=TKIP\n\n\n"\
                                        "#Hardware configuration\n\ndriver=nl80211\n"\
                                        "hw_mode=g\nieee80211n=1\n\n"

#define NETWORK_WPA_CONF_HEAD           "\nnetwork={\n\tssid=\""
#define NETWORK_WPA_CONF_MIDDLE         "\"\n\tpsk=\""
#define NETWORK_WPA_CONF_END            "\"\n\tkey_mgmt=WPA-PSK\n}\n"
#define DUERLINK_WPA_INSERT_FLAG        "ap_scan=1"

#define DUERLINK_NETWORK_DEVICE_MTK_FOR_AP DuerLinkMtkInstance::get_hostap_interface()
#define DUERLINK_NETWORK_DEVICE_MTK_FOR_WORK "wlan0"

#define DUERLINK_WPA_HOSTAPD_CONFIG_FILE_PATH_MTK "/etc/hostapd_mtk.conf"
#define DUERLINK_WPA_CONFIG_FILE_MTK "/data/cfg/wpa_supplicant.conf"

#define SSID_PREFIX_HODOR       "Hodor"
#define SSID_PREFIX_STANDARD    "Standard"
#define SSID_PREFIX_INDIVIDUAL  "Individual"

#define DUERLINK_AUTO_CONFIG_TIMEOUT 10*60

#define PACKET_SIZE         4096
#define MAX_WAIT_TIME       1;//5
#define MAX_PACKETS_COUNT   4
#define MAX_PING_INTERVAL   300
#define PING_DEST_HOST1       "114.114.114.114"
#define PING_DEST_HOST2       "8.8.8.8"
#define DUERLINK_NETWORK_CONFIGURE_PING_COUNT 18

namespace duerOSDcsApp {
namespace application {

struct IcmpEchoReply {
    int icmpSeq;
    int icmpLen;
    int ipTtl;
    double rtt;
    std::string fromAddr;
    bool isReply;
};

struct PingResult {
    int dataLen;
    int nsend;
    int nreceived;
    char ip[32];
    std::string error;
    std::vector<IcmpEchoReply> icmpEchoReplys;
};

//network status
enum InternetConnectivity {
    UNAVAILABLE = 0,
    AVAILABLE,
    UNKNOW
};

//operation type
enum operation_type {
    EOperationStart = 10,
    EAutoEnd,
    EAutoConfig,
    EManualConfig
};

class NetWorkPingStatusObserver {
public:
    virtual void network_status_changed(InternetConnectivity status, bool wakeupTrigger) = 0;
};

class DuerLinkMtkInstance {
public:
    static DuerLinkMtkInstance* get_instance();

    static void destroy();

    void init_duer_link_log(duerLink::duer_link_log_cb_t duer_link_log);

#ifdef DUERLINK_V2
    void init_config_network_parameter(duerLink::platform_type speaker_type,
                                       duerLink::auto_config_network_type autoType,
                                       std::string devicedID,
                                       std::string interface);

    void init_discovery_parameter(std::string devicedID, std::string appId, std::string interface);
#else

    void init_config_network_parameter(duerLink::platform_type speaker_type,
                                       duerLink::auto_config_network_type autoType,
                                       const std::string& devicedID,
                                       const std::string& interface,
                                       const std::string& bdussfile,
                                       const std::string& clientId);

    void init_discovery_parameter(const std::string& devicedID,
                                  const std::string& appId,
                                  const std::string& interface,
                                  const std::string& bdussfile);
#endif
    void init_softAp_env_cb();
    void init_ble_env_cb();
    void init_wifi_connect_cb();

    bool set_wpa_conf(bool change_file);

    void set_network_observer(NetWorkPingStatusObserver *observer);
    bool ping_network(bool wakeupTrigger);
    void start_network_monitor();
    void start_verify_network();

    void set_networkConfig_observer(duerLink::NetworkConfigStatusObserver* config_listener);
    void set_monitor_observer(NetWorkPingStatusObserver *ping_listener);

    void start_network_recovery();
    void stop_network_recovery();
    void start_discover_and_bound();

    void set_dlp_data_observer(DuerLinkReceivedDataObserver* observer);

    void ble_client_connected();
    void ble_client_disconnected();
    bool ble_recv_data(void *data, int len);

    bool start_network_config(int timeout);
    void stop_network_config();

    bool send_dlp_msg_to_all_clients(const std::string& sendBuffer);

    bool send_dlp_msg_by_specific_session_id(const std::string& message, unsigned short sessionId);

    bool start_loudspeaker_ctrl_devices_service(duerLink::device_type device_type_value,
                                                const std::string& client_id,
                                                const std::string& message);

    bool start_loudspeaker_ctrl_devices_service_by_device_id(const std::string& device_id);

    bool send_msg_to_devices_by_spec_type(const std::string& msg_buf,
                                          duerLink::device_type device_type_value);
    bool disconnect_devices_connections_by_spe_type(duerLink::device_type device_type_value,
                                                    const std::string& message);

    bool get_curret_connect_device_info(std::string& client_id,
                                        std::string& device_id,
                                        duerLink::device_type device_type_value);

    void init_network_config_timeout_alarm();
    void start_network_config_timeout_alarm(int timeout);
    void stop_network_config_timeout_alarm();

    inline operation_type get_operation_type() {return m_operation_type;};
    inline void set_operation_type(operation_type type) {m_operation_type = type;};

    void OnNetworkReady();
    static const char* get_hostap_interface();

private:
    DuerLinkMtkInstance();
    virtual ~DuerLinkMtkInstance();

    bool is_first_network_config(std::string path);

    static void sigalrm_fn(int sig);

    unsigned short getChksum(unsigned short *addr,int len);
    int packIcmp(int pack_no, struct icmp* icmp);
    bool unpackIcmp(char *buf,int len, struct IcmpEchoReply *icmpEchoReply);
    struct timeval timevalSub(struct timeval timeval1,struct timeval timval2);
    bool getsockaddr(const char * hostOrIp, sockaddr_in* sockaddr);
    bool sendPacket();
    bool recvPacket(PingResult &pingResult);
    bool ping(std::string host, int count, PingResult& pingResult);
    static void *monitor_work_routine(void *arg);
    static void *verify_work_routine(void *arg);
    bool check_recovery_network_status();

private:
    static DuerLinkMtkInstance* m_duerLink_mtk_instance;

    char m_sendpacket[PACKET_SIZE];
    char m_recvpacket[PACKET_SIZE];
    int m_maxPacketSize;
    int m_sockfd;
    int m_datalen;
    int m_nsend;
    int m_nreceived;
    int m_icmp_seq;
    struct sockaddr_in m_dest_addr;
    struct sockaddr_in m_from_addr;
    pid_t m_pid;
    pthread_mutex_t m_ping_lock;

    NetWorkPingStatusObserver *m_pObserver;
    duerLink::NetworkConfigStatusObserver *m_pMtkConfigObserver;

    operation_type m_operation_type;
    int m_stop_network_recovery;
};

}
}

#endif //DCS_DUERLINKMTKINSTANCE_H
