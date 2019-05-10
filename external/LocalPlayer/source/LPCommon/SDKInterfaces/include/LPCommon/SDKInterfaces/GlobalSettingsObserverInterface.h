/*
 * GlobalSettingsObserverInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_GLOBALSETTINGSOBSERVERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_GLOBALSETTINGSOBSERVERINTERFACE_H_

#include <unordered_map>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {
/**
 * The interface for observer of all the settings.
 */
class GlobalSettingsObserverInterface {
public:
    /**
     * Destructor
     */
    virtual ~GlobalSettingsObserverInterface() = default;

    /**
     * The callback executed when a setting is changed.
     * @param mapOfSettings The map which contains <key, value> pair for all settings.
     */
    virtual void onSettingChanged(const std::unordered_map<std::string, std::string>& mapOfSettings) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK
#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_GLOBALSETTINGSOBSERVERINTERFACE_H_
