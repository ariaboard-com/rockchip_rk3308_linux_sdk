/*
 * ErrorTypeConversion.h
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

#ifndef LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_ERRORTYPECONVERSION_H_
#define LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_ERRORTYPECONVERSION_H_

#include <gst/gst.h>

#include <LPCommon/Utils/MediaPlayer/ErrorTypes.h>

namespace localPlayerSDK {
namespace mediaPlayer {

/**
 * Convert a GStreamer @c GError to an @c ErrorType.
 *
 * @param error The @c GError to convert.
 * @param remoteResource Indicates whether it should be for a resource that is remote.
 * @return The converted @c ErrorType.
 */
rkpCommon::utils::mediaPlayer::ErrorType gerrorToErrorType(const GError* error, bool remoteResource);

}  // namespace mediaPlayer
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_MEDIAPLAYER_INCLUDE_MEDIAPLAYER_ERRORTYPECONVERSION_H_
