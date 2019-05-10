//
// Created by yuyaohui on 17-08-20.
//

#ifndef DUER_LINK_HTTP2_CONNECTION_OBSERVER_H
#define DUER_LINK_HTTP2_CONNECTION_OBSERVER_H

#include <string>
#include "duer_link_dlp_cloud_received_observer.h"
#include "duer_link_tv_con_status_change_observer.h"

class DuerLinkHttp2ConnectionObserver{
public:
    virtual std::string NotifyReceivedData(const std::string& jsonPackageData,int iSessionID = -1) = 0;
    //virtual std::string NotifyReceivedDataFromCloud(const std::string& jsonPackageData,std::string uuid = "") = 0;

    virtual std::string notify_received_bduss(const std::string& bdussValue) = 0;

    /*tvlink op func*/
    /**
     * init dlp network module
     */
    virtual void init_dlp_network_module(std::string server_address,
            std::string long_connection_method,
            std::string short_connection_method,
            std::string ping_method,
            std::string logout_method) = 0;
    /**
     * To make connections to DLP.  Once enabled, the object will attempt to
     * create a connection to DLP.  If the object is already connected, this function will do nothing.
     */
    virtual void enable() = 0;

    /**
     * To disable connections to DLP.
     */
    virtual void disable() = 0;

    /**
     * Send Message to Dlp server
     */
    virtual void send_message(const std::string& sendBuffer) = 0;

    /**
     * Set Dlp Message Observer
     */
    virtual void set_dlp_message_observer(DuerLinkMessageObserverInterface* observer) = 0;

    /**
     * Set Dlp Message Observer
     */
    virtual void set_dlp_network_con_status_change_observer(
          DuerLinkTVConStatusChangeObserver* network_status_change_observer) = 0;

    /**
     * set client id
     */
    virtual void set_client_id(std::string client_id) = 0;

    /**
     * set device id
     */
    virtual void set_device_id(std::string device_id) = 0;

    /**
     * set access_id
     */
    virtual void set_access_token(std::string access_token) = 0;

    /**
     * set dueros device id
     */
    virtual void set_dueros_device_id(std::string dueros_device_id) = 0;

    /**
     * get dueros device id
     */
    virtual std::string dueros_device_id() = 0;
};

#endif //DUER_LINK_HTTP2_CONNECTION_OBSERVER_H
