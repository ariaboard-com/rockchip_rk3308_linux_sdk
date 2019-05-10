/*
 * AudioPlayerObserverInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIOPLAYEROBSERVERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIOPLAYEROBSERVERINTERFACE_H_

#include <chrono>
#include <string>

#include <LPCommon/RKP/PlayerActivity.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * This class allows any observers of the @c AudioPlayer to be notified of changes in the @c AudioPlayer audio state.
 */
class AudioPlayerObserverInterface {
public:
    /**
     * Destructor
     */
    virtual ~AudioPlayerObserverInterface() = default;

    /// The context of the AudioPlayer when the observer is notified of the @c PlayerActivity state change.
    struct Context {
        /// The ID of the @c AudioItem that the @ AudioPlayer is handling.
        std::string audioItemId;

        /// The offset in millisecond from the start of the @c AudioItem.
        std::chrono::milliseconds offset;
    };

    /**
     * Used to notify the observer when the @c AudioPlayer has a change in @c PlayerActivity.
     *
     * @param state The @c PlayerActivity of the @c AudioPlayer.
     * @param context The @c Context of the @c AudioPlayer.
     */
    virtual void onPlayerActivityChanged(rkpCommon::rkp::PlayerActivity state, const Context& context) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_AUDIOPLAYEROBSERVERINTERFACE_H_
