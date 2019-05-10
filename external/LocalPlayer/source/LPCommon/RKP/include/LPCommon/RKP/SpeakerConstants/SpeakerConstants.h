/*
 * SpeakerConstants.h
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

/**
 * @file
 */
#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_SPEAKERCONSTANTS_SPEAKERCONSTANTS_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_SPEAKERCONSTANTS_SPEAKERCONSTANTS_H_

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace speakerConstants {

/// RKP setVolume Minimum.
const int8_t RKP_SET_VOLUME_MIN = 0;

/// RKP setVolume Maximum.
const int8_t RKP_SET_VOLUME_MAX = 100;

/// RKP adjustVolume Minimum.
const int8_t RKP_ADJUST_VOLUME_MIN = -100;

/// RKP adjustVolume Maximum.
const int8_t RKP_ADJUST_VOLUME_MAX = 100;

}  // namespace speakerConstants
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_SPEAKERCONSTANTS_SPEAKERCONSTANTS_H_
