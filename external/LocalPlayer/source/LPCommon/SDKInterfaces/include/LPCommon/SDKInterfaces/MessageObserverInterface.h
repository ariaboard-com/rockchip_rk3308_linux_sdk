/*
 * MessageObserverInterface.h
 *
 * Copyright 2016-2018 Rockchip.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_MESSAGEOBSERVERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_MESSAGEOBSERVERINTERFACE_H_

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * This class allows a client to receive messages from RKP.
 */
class MessageObserverInterface {
public:
    /**
     * Destructor.
     */
    virtual ~MessageObserverInterface() = default;

    /**
     * A function that a client must implement to receive Messages from RKP.
     *
     * @param contextId The context for the message, which in this case reflects the logical HTTP/2 stream the
     * message arrived on.
     * @param message The RKP message that has been received.
     */
    virtual void receive(const std::string& contextId, const std::string& message) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_MESSAGEOBSERVERINTERFACE_H_
