//
// Created by yuyaohui on 18-03-08
//

#ifndef DUER_LINK_DLP_CLOUD_RECEIVED_OBSERVER_H
#define DUER_LINK_DLP_CLOUD_RECEIVED_OBSERVER_H

class DuerLinkMessageObserverInterface {
public:

    /**
     * A function that a client must implement to receive Messages from DLP.
     *
     * @param message The DLP message that has been received.
     */
    virtual void receive(const std::string& message) = 0;
};

#endif  //DUER_LINK_DLP_CLOUD_RECEIVED_OBSERVER_H
