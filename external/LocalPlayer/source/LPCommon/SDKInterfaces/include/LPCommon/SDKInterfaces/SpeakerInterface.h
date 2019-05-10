/*
 * SpeakerInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_SPEAKERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_SPEAKERINTERFACE_H_

#include <ostream>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * The SpeakerInterface is concerned with the control of volume and mute settings of a speaker.
 * The two settings are independent of each other, and the respective APIs shall not affect
 * the other setting in any way. Compound behaviors (such as unmuting when volume is adjusted) will
 * be handled at a layer above this interface.
 */
class SpeakerInterface {
public:
    /**
     * This enum provides the type of the @c SpeakerInterface.
     */
    enum class Type {
        /// Speaker source that should be synced with RKP.
        RKP_SYNCED,
        /// Speaker source that will not be synced with RKP.
        LOCAL
    };

    /**
     * This contains the current settings of the @c SpeakerInterface.
     * The minimum volume level should correspond to no volume output, but this setting should still be independent
     * from mute. For speakers which do not have independent mute and volume settings, the interface must be implemented
     * as if they did so. For example, when unmuting a speaker, the volume setting should correspond to the level it was
     * at before muting.
     */
    struct SpeakerSettings {
        /// Must be within [RKP_SET_VOLUME_MIN, RKP_SET_VOLUME_MAX].
        int8_t volume;
        /// True means muted, false means unmuted.
        bool mute;
        /// Operator overload to compare two SpeakerSettings objects.
        bool operator==(const SpeakerSettings& rhs) const {
            return volume == rhs.volume && mute == rhs.mute;
        }
    };

    /**
     * Set the absolute volume of the speaker. @c volume
     * will be [RKP_SET_VOLUME_MIN, RKP_SET_VOLUME_MAX], and implementers of the interface must normalize
     * the volume to fit the needs of their drivers.
     *
     * @param volume A volume to set the speaker to.
     * @return Whether the operation was successful.
     */
    virtual bool setVolume(int8_t volume) = 0;

    /**
     * Set a relative change for the volume of the speaker. @c volume
     * will be [RKP_ADJUST_VOLUME_MIN, RKP_ADJUST_VOLUME_MAX], and implementers of the interface must normalize
     * the volume to fit the needs of their drivers.
     *
     * @param delta The delta to apply to the volume.
     * @return Whether the operation was successful.
     */
    virtual bool adjustVolume(int8_t delta) = 0;

    /**
     * Set the mute of the speaker.
     *
     * @param mute Represents whether the speaker should be muted (true) or unmuted (false).
     * @return Whether the operation was successful.
     */
    virtual bool setMute(bool mute) = 0;

    /**
     * Return a @c SpeakerSettings object to indicate the current settings of the speaker.
     *
     * @param settings A @c SpeakerSettings object if successful.
     * @return Whether the operation was successful.
     */
    virtual bool getSpeakerSettings(SpeakerSettings* settings) = 0;

    /**
     * Get the @c Type. The @c Type should remain static and not change with the lifetime of the speaker.
     *
     * @return The @c Type.
     */
    virtual Type getSpeakerType() = 0;
};

/**
 * Write a @c Type value to an @c ostream as a string.
 *
 * @param stream The stream to write the value to.
 * @param type The type value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, SpeakerInterface::Type type) {
    switch (type) {
        case SpeakerInterface::Type::RKP_SYNCED:
            stream << "RKP_SYNCED";
            return stream;
        case SpeakerInterface::Type::LOCAL:
            stream << "LOCAL";
            return stream;
    }
    stream << "UNKNOWN";
    return stream;
}

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_SPEAKERINTERFACE_H_
