//
// Created by yuyaohui on 17-09-20.
//

#ifndef NETWORK_DEFINE__PUBLIC_H
#define NETWORK_DEFINE__PUBLIC_H

namespace duerLink {

    #define Duer_Link_Version "0.1.8"

    //config status
    enum notify_network_status_type {
        ENetworkNone = 11,
        ENetworkConfigExited,
        ENetworkConfigStarted,
        ENetworkDeviceConnected,
        ENetworkConfigIng,
        ENetworkConfigRouteFailed,
        ENetworkLinkSucceed,
        ENetworkLinkFailed,
        ENetworkRecoveryStart,
        ENetworkRecoverySucceed,
        ENetworkRecoveryFailed
    };

    //device type
    enum platform_type {
        ERaspberryPi=31,
        EMtk,
        EHodor,
        EOther
    };

    //config type
    enum auto_config_network_type {
        ESoftAP=41,
        EBle,
        EAll
    };

    enum third_party_ID {
        EThird_Party_SuNing = 61
    };

    //device type
    enum device_type {
        EDeviceAll = 11,
        EDeviceTV,
        EDeviceSuningTV,
        EDeviceHodor
    };

    //device connection status
    enum device_connection_status {
        ENotFoundDevice = 13,
        EDeviceFound,
        EDeviceConnectionFailed,
        EDeviceConnectionSuccess,
        EDeviceDisconnectionFailed,
        EDeviceDisconnectionSuccess,
        EDeviceSendMsgFailed,
        EDeviceSendMsgSuccess
    };

    //SoftAP environment prepare and destroy
    typedef bool (*softap_prepare_env_cb_t)();
    typedef bool (*softap_destroy_env_cb_t)();

    //BLE environment prepare and destroy
    typedef bool (*ble_prepare_env_cb_t)();
    typedef bool (*ble_destroy_env_cb_t)();
    typedef int (*ble_send_data_cb_t)(char *data, unsigned short data_length);

    //connect wifi
    typedef bool (*platform_connect_wifi_cb_t)(const char *ssid,
                                               const int ssid_len,
                                               const char *pwd,
                                               const int pwd_len);

    //logger interface
    typedef void (*duer_link_log_cb_t)(const char* msg, ...);
}

#endif //NETWORK_DEFINE__PUBLIC_H
