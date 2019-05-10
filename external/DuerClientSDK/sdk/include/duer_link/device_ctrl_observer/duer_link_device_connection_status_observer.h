//
// Created by yuyaohui on 17-10-22.
//

#ifndef DUER_LINK_DEVICE_CONNECTION_STATUS_OBSERVER_H
#define DUER_LINK_DEVICE_CONNECTION_STATUS_OBSERVER_H

#include <string>
#include "network_define_public.h"

class DuerLinkDeviceConnectionStatusObserver {
public:
    virtual void notify_connection_status_change(
            duerLink::device_connection_status status,duerLink::device_type type) = 0;
};

#endif //DUER_LINK_DEVICE_CONNECTION_STATUS_OBSERVER_H
