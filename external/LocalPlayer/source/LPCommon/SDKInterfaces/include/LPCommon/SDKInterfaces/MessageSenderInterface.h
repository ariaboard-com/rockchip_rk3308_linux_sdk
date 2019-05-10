/*
 * MessageSenderInterface.h
 *
 * Copyright 2018 Rockchip.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.rock-chips.com/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_MESSAGESENDERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_MESSAGESENDERINTERFACE_H_

#include "LPCommon/RKP/MessageRequest.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/// This specifies an interface to send a message.
class MessageSenderInterface {
public:
    /// Destructor.
    virtual ~MessageSenderInterface() = default;

    /**
     * Send a message.  This function must operate asynchronously, internally storing the message to be sent
     * when the connection is able to process it.
     * The @c onSendCompleted callback method of the @c MessageRequest object is invoked as follows:
     *
     * @li If the @c MessageSenderInterface object is not connected, the callback value is NOT_CONNECTED.
     * @li Otherwise, the object attempts to send the message, and once this has either succeeded or failed,
     *     the callback value is set appropriately.
     *
     * @param request The @c MessageRequest to send.
     */
    virtual void sendMessage(std::shared_ptr<rkp::MessageRequest> request) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_MESSAGESENDERINTERFACE_H_
