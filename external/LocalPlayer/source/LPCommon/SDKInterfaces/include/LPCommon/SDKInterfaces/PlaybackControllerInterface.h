/*
 * PlaybackControllerInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_PLAYBACKCONTROLLERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_PLAYBACKCONTROLLERINTERFACE_H_

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * The PlaybackController Interface can be used to send events for navigating a playback queue with
 * an on-client button press or GUI affordance, rather than through a speech request.
 */
class PlaybackControllerInterface {
public:
    /**
     * Destructor
     */
    virtual ~PlaybackControllerInterface() = default;

    /**
     * This method can be called by the client when "Play" is pressed on a physical button or on the GUI.
     * A PlayCommandIssued event message will be sent to RKP.
     */
    virtual void playButtonPressed() = 0;

    /**
     * This method can be called by the client when "Pause" is pressed on a physical button or on the GUI.
     * A PauseCommandIssued event message will be sent to RKP.
     */
    virtual void pauseButtonPressed() = 0;

    /**
     * This method can be called by the client when "Next" is pressed on a physical button or on the GUI.
     * A NextCommandIssued event message will be sent to RKP.
     */
    virtual void nextButtonPressed() = 0;

    /**
     * This method can be called by the client when "Previous" is pressed on a physical button or on the GUI.
     * A PreviousCommandIssued event message will be sent to RKP.
     */
    virtual void previousButtonPressed() = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_PLAYBACKCONTROLLERINTERFACE_H_
