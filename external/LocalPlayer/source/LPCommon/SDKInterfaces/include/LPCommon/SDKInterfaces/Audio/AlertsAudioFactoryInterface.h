/*
 * AlertsAudioFactoryInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIO_ALERTSAUDIOFACTORYINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIO_ALERTSAUDIOFACTORYINTERFACE_H_

#include <istream>
#include <memory>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {
namespace audio {

/**
 * This is an interface to produce functions that provide streams to the alert audio resources.
 */
class AlertsAudioFactoryInterface {
public:
    virtual ~AlertsAudioFactoryInterface() = default;

    virtual std::function<std::unique_ptr<std::istream>()> alarmDefault() const = 0;
    virtual std::function<std::unique_ptr<std::istream>()> alarmShort() const = 0;
    virtual std::function<std::unique_ptr<std::istream>()> timerDefault() const = 0;
    virtual std::function<std::unique_ptr<std::istream>()> timerShort() const = 0;
    virtual std::function<std::unique_ptr<std::istream>()> reminderDefault() const = 0;
    virtual std::function<std::unique_ptr<std::istream>()> reminderShort() const = 0;
};

}  // namespace audio
}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIO_ALERTSAUDIOFACTORYINTERFACE_H_
