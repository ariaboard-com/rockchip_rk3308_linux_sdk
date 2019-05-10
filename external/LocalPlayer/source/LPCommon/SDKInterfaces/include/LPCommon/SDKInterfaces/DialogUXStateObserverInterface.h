/*
 * DialogUXStateObserverInterface.h
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

/// @file DialogUXStateObserverInterface.h

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIALOGUXSTATEOBSERVERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIALOGUXSTATEOBSERVERINTERFACE_H_

#include <string>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/// A dialog specific UX state observer.
class DialogUXStateObserverInterface {
public:
    /// The different dialog specific RKP UX states.
    enum class DialogUXState {
        /// Nyx is idle and ready for an interaction.
        IDLE,

        /// Nyx is currently listening.
        LISTENING,

        /**
         * A customer request has been completed and no more input is accepted. In this state, Nyx is waiting for a
         * response from RKP.
         */
        THINKING,

        /// Nyx is responding to a request with speech.
        SPEAKING,

        /**
         * Nyx has finished processing a SPEAK directive. In this state there
         * are no notifications triggered. If the SPEAK directive is part of a
         * speech burst UX moves back to the SPEAKING state. If it was the last
         * SPEAK directive after timeout the UX state moves to the IDLE state.
         */
        FINISHED
    };

    /**
     * Destructor.
     */
    virtual ~DialogUXStateObserverInterface() = default;

    /**
     * This function is called whenever the RKP UX dialog state of the system changes. This function will block
     * processing of other state changes, so any implementation of this should return quickly.
     *
     * @param newState The new dialog specific RKP UX state.
     */
    virtual void onDialogUXStateChanged(DialogUXState newState) = 0;

    /**
     * This function converts the provided @c DialogUXState to a string.
     *
     * @param state The @c DialogUXState to convert to a string.
     * @return The string conversion of @c state.
     */
    static std::string stateToString(DialogUXState state);
};

inline std::string DialogUXStateObserverInterface::stateToString(DialogUXState state) {
    switch (state) {
        case DialogUXState::IDLE:
            return "IDLE";
        case DialogUXState::LISTENING:
            return "LISTENING";
        case DialogUXState::THINKING:
            return "THINKING";
        case DialogUXState::SPEAKING:
            return "SPEAKING";
        case DialogUXState::FINISHED:
            return "FINISHED";
    }
    return "Unknown State";
}

/**
 * Write a @c DialogUXStateObserverInterface::DialogUXState value to an @c ostream as a string.
 *
 * @param stream The stream to write the value to.
 * @param state The @c DialogUXStateObserverInterface::DialogUXState value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, const DialogUXStateObserverInterface::DialogUXState& state) {
    return stream << DialogUXStateObserverInterface::stateToString(state);
}

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIALOGUXSTATEOBSERVERINTERFACE_H_
