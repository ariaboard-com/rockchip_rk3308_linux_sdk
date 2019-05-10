//
// Created by yuyaohui on 18-04-12.
//

#ifndef DUER_LINK_HTTP2_CON_FACTORY_FACTORY_INTERFACE_H
#define DUER_LINK_HTTP2_CON_FACTORY_FACTORY_INTERFACE_H

#include <string>

#include "duer_link_http2_connection_observer.h"

class DuerLinkHttpConFactoryInterface {
public:
    
    /**
     * create http2 connection
     */
    virtual DuerLinkHttp2ConnectionObserver* create_http2_connection(
            std::string& client_id,
            std::string device_id,
            std::string access_token,
            std::string dueros_device_id) = 0;
};
#endif //DUER_LINK_HTTP2_CON_FACTORY_FACTORY_INTERFACE_H
