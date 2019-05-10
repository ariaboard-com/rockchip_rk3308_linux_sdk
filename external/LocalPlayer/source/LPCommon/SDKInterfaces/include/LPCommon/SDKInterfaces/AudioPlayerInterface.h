/*
 * AudioPlayerInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIOPLAYERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIOPLAYERINTERFACE_H_

#include <chrono>
#include <memory>

#include <LPCommon/SDKInterfaces/AudioPlayerObserverInterface.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * This class provides an interface to the @c AudioPlayer.
 */
class AudioPlayerInterface {
public:
    /**
     * Destructor
     */
    virtual ~AudioPlayerInterface() = default;

    /**
     * This function adds an observer to @c AudioPlayer so that it will get notified for AudioPlayer state changes.
     *
     * @param observer The @c AudioPlayerObserverInterface
     */
    virtual void addObserver(std::shared_ptr<rkpCommon::sdkInterfaces::AudioPlayerObserverInterface> observer) = 0;

    /**
     * This function removes an observer from @c AudioPlayer so that it will no longer be notified of
     * AudioPlayer state changes.
     *
     * @param observer The @c AudioPlayerObserverInterface
     */
    virtual void removeObserver(std::shared_ptr<rkpCommon::sdkInterfaces::AudioPlayerObserverInterface> observer) = 0;

    /**
     * This function retreive the offset of the current AudioItem the @c AudioPlayer is handling.
     * @note This function is blocking.
     *
     * @return This returns the offset in millisecond.
     */
    virtual std::chrono::milliseconds getAudioItemOffset() = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIOPLAYERINTERFACE_H_
