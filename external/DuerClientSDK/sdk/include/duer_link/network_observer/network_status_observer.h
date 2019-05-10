//
// Created by yuyaohui on 17-7-5.
//

#ifndef DUERLINK_NETWORK_STATUS_OBSERVER_H
#define DUERLINK_NETWORK_STATUS_OBSERVER_H

#include <string>
#include "network_define_public.h"

namespace duerLink {

    //network config Observer
    class NetworkConfigStatusObserver {
    public:
        virtual void notify_network_config_status(duerLink::notify_network_status_type notify_type) = 0;
    };

    //access token Observer
    class AccessTokenObserver {
    public:
        virtual void access_token_status(int status, const std::string &info) = 0;
        virtual void userinfo_status(int status, const std::string &info) = 0;
    };
}

#endif 
