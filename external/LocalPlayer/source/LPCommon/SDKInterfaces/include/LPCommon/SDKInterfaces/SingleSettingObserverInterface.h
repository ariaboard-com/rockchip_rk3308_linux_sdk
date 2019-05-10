/*
 * SingleSettingObserverInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_SINGLESETTINGOBSERVERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_SINGLESETTINGOBSERVERINTERFACE_H_

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {
/**
 * The interface for observer of a single setting.
 */
class SingleSettingObserverInterface {
public:
    /**
     * Destructor
     */
    virtual ~SingleSettingObserverInterface() = default;

    /**
     * The callback executed when a setting is changed.
     * @param key The name of the Setting.
     * @param value The value to which the setting has to be changed.
     */
    virtual void onSettingChanged(const std::string& key, const std::string& value) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK
#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_SINGLESETTINGOBSERVERINTERFACE_H_
