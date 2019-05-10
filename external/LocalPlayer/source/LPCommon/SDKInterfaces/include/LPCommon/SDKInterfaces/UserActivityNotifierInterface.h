/*
 * UserActivityNotifierInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_USERACTIVITYNOTIFIERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_USERACTIVITYNOTIFIERINTERFACE_H_

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * This interface is used to notify an implementation of the user activity. Any component that interacts with the user
 * (e.g. AudioInputProcessor) should register an instance of this interface to signal when user interaction is detected
 * (e.g. SpeechStarted).
 */
class UserActivityNotifierInterface {
public:
    /// Destructor.
    virtual ~UserActivityNotifierInterface() = default;

    /// The function to be called when the user has become active.
    virtual void onUserActive() = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_USERACTIVITYNOTIFIERINTERFACE_H_
