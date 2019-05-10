/*
 * AudioFactoryInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIO_AUDIOFACTORYINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIO_AUDIOFACTORYINTERFACE_H_

#include <memory>

#include "LPCommon/SDKInterfaces/Audio/AlertsAudioFactoryInterface.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {
namespace audio {

/**
 * This is the interface that distributes interfaces for various audio stream providers.
 */
class AudioFactoryInterface {
public:
    virtual ~AudioFactoryInterface() = default;

    /**
     * This shares a factory that produces audio streams for the alerts components.
     */
    virtual std::shared_ptr<AlertsAudioFactoryInterface> alerts() const = 0;
};

}  // namespace audio
}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIO_AUDIOFACTORYINTERFACE_H_
