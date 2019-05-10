//
// Created by yuyaohui on 18-04-22.
//

#ifndef DUER_LINK_TV_CON_STATUS_CHANGE_OBSERVER_H
#define DUER_LINK_TV_CON_STATUS_CHANGE_OBSERVER_H

#include <string>

class DuerLinkTVConStatusChangeObserver {
public:
    
    /**
     * create http2 connection
     */
    virtual void notify_dlp_network_status_change(int status) = 0;
};

#endif //DUER_LINK_TV_CON_STATUS_CHANGE_OBSERVER_H
