/*
 * Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef DUEROS_DCS_APP_SAMPLEAPP_APPLICATIONMANAGER_H_
#define DUEROS_DCS_APP_SAMPLEAPP_APPLICATIONMANAGER_H_

#include <DcsSdk/DcsSdk.h>
#include "DCSApp/Audio/AudioMicrophoneInterface.h"

namespace duerOSDcsApp {
namespace application {

using duerOSDcsSDK::sdkInterfaces::DialogUXStateObserverInterface;

class ApplicationManager
        : public DialogUXStateObserverInterface
        , public duerOSDcsSDK::sdkInterfaces::ConnectionStatusObserverInterface
        , public duerOSDcsSDK::sdkInterfaces::ApplicationImplementation {
public:
    explicit ApplicationManager() = default;
    /**
     * DCS dialog state changed
     * @param state
     */
    void onDialogUXStateChanged(DialogUXState state) override;

    /**
     * DCS message send complete
     * @param success
     */
    void onMessageSendComplete(bool success) override;

    /**
     * DCS received directive
     * @param contextId
     * @param message
     */
    void onReceivedDirective(const std::string& contextId, const std::string& message) override;

    /**
     * DCS alert state change
     * @param alertToken
     * @param state
     * @param reason
     */
    void onAlertStateChange(const std::string & alertToken,
                            DialogUXStateObserverInterface::AlertState state,
                            const std::string &reason="") override;

    /**
     * DCS expect speech
     * @param success
     */
    void onExpectSpeech(bool success) override;

    /**
     * DCS speech finished
     */
    void onSpeechFinished() override;

    /**
     * DCS connection status changed
     * @param status
     * @param reason
     */
    void onConnectionStatusChanged(const Status status, const ChangedReason reason) override;

    /**
     * Inform that the SDK had canceled the ASR request.
     */
    void onSpeechAsrCanceled() override;

    /**
     * DCS set speaker volume
     * @param volume
     */
    void setSpeakerVolume(int64_t volume) override;

    /**
     * DCS set speaker mute
     * @param isMute
     */
    void setSpeakerMute(bool isMute) override;

    /**
     * DCS get the volume of speaker
     * @return speaker volume
     */
    int getSpeakerVolume() override;

    /**
     * DCS get the mute status of speaker.
     * @return speaker mute status.
     */
    bool getSpeakerMuteStatus() override;

    /**
     * DCS set status of bluetooth
     * @param status
     */
    void setBluetoothStatus(bool status) override;

    /**
     * DCS set status of microphone
     * @param status
     */
    void setMicrophoneStatus(bool status) override;

    /**
     * DCS set connection status of bluetooth
     * @param status
     */
    void setBluetoothConnectionStatus(bool status) override;

    /**
     * DCS query current version
     */
    void queryCurrentVersion() override;

    /**
     * DCS power sleep
     */
    void powerSleep() override;

    /**
     * DCS power shutdown
     */
    void powerShutdown() override;

    /**
     * DCS power reboot
     */
    void powerReboot() override;

    /**
     * DCS show the sdk status
     * @param message
     */
    void informSdkConnectionStatus(duerOSDcsSDK::sdkInterfaces::SdkConnectionState sdkConnectionStatus) override;

    /**
     * DCS show the customize directive
     * @param directive
     */
    void informCustomizeDirective(const std::string& directive) override;

    /**
     * DLP link the third device
     * @param deviceType device type.
     * @param clientId client id to connect default blank string.
     * @param message message send to third device default blank string.
     * @return
     */
    bool linkThirdDevice(int deviceType,
                         const std::string& clientId,
                         const std::string& messsage) override;

    /**
     * DLP unlink the third device
     * @param deviceType device type.
     * @param message message send to third device default blank string.
     * @return
     */
    bool unlinkThirdDevice(int deviceType, const std::string& message) override;

    /**
     * DLP the third dlp message callback
     * @param deviceType
     * @param message
     * @return
     */
    bool onThirdDlpMessage(int deviceType, const std::string& message) override;

    /**
     * DLP notify the sdk context
     * @param context
     * @param deviceType
     */
    void notifySdkContext(const std::string& context, int deviceType) override;

    /**
     * DLP notify the sdk context by session id.
     * @param context message.
     * @param sessionId session id.
     * @param deviceType device type.
     */
    void notifySdkContextBySessionId(const std::string& context, unsigned short sessionId,
                                     int deviceType) override;

    /**
     * DLP get the system information
     * @param systemInformation
     * @return
     */
    bool systemInformationGetStatus(duerOSDcsSDK::sdkInterfaces::SystemInformation& systemInformation) override;

    /**
     * DLP reset the device
     * @return
     */
    bool systemInformationHardReset() override;

    /**
     * DLP get the status of system update
     * @return
     */
    bool systemUpdateGetStatus(duerOSDcsSDK::sdkInterfaces::SystemUpdateInformation &systemUpdateInformation) override;

    /**
     * DLP notify update system
     * @return
     */
    bool systemUpdateUpdate() override;

    /**
     * DLP: Get current connected device info by device type.
     * @param[in] deviceType Device type defined by DuerLink.
     * @param[out] clientId Store client id from DuerLink.
     * @param[out] deviceId Store device id from DuerLink.
     * @return True/False: Operation succeed/failed.
     */
    bool getCurrentConnectedDeviceInfo(int deviceType,
                                       std::string& clientId, std::string& deviceId) override;

    /**
     * get wifi bssid
     * @param[in] void
     * @return string
     */
    std::string getWifiBssid();

#ifdef SUPPORT_INFRARED
    /**
     * send infrared ray code request
     * @param carrierFrequency
     * @param pattern
     */
    bool sendInfraredRayCodeRequest(int carrierFrequency,
                                    const std::string &pattern) override;
#endif

    /**
     * set DCS sdk handler
     * @param dcsSdk
     */
    void setDcsSdk(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk);

    /**
     * set microphone wrapper
     * @param micWrapper
     */
    void setMicrophoneWrapper(std::shared_ptr<AudioMicrophoneInterface> micWrapper);

    /**
     * DCS open microphone
     */
    void microphoneOn();

    /**
     * DCS close microphone
     */
    void microphoneOff();

    /**
     * DCS stop play wifi music and bluetooth music
     */
    void stopMusicPlay();

    /**
     * DCS force hold focus
     * @param holdFlag
     */
    void forceHoldFocus(bool holdFlag);

    /**
     * DCS cancel music play
     */
    void cancelMusicPlay();

    /**
     * notify DCS mute changed
     * @param volume
     * @param muted
     */
    void muteChanged(int volume, bool muted);

    /**
     * notify DCS volume changed
     * @param volume
     * @param muted
     */
    void volumeChanged(int volume, bool muted);

#ifdef SUPPORT_DEBUGGER
    bool setStartDebugMode();

    bool setStopDebugMode();

    /**
     * Inform the server the debugger mode has started.
     */
    void debugStarted();

    /**
    * Inform the server the debugger mode has stoped.
    */
    void debugStoped();
#endif
    /**
     * DCS perform close local active alert
     */
    void closeLocalActiveAlert();

    /**
     * notify close led tts
     */
    static void ledTtsOffCallback();

    /**
     * notify close led tts and enter sleep mode
     */
    static void ledTtsOffSleepCallback();

private:
    static void loginFailed();

    void onStateChanged();

    /// the current dialog state of DCS
    DialogUXState m_dialogState;

    /// the current connection state of DCS
    duerOSDcsSDK::sdkInterfaces::ConnectionStatusObserverInterface::Status m_connectionStatus;

    /// judge is from speaking, because speak finished led off
    bool m_isFromSpeaking;

    /// is first receive msg
    bool m_isFirstReceiveMsg;

    /// the DCS sdk handler
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> m_dcsSdk;

    /// the microphone wrapper
    std::shared_ptr<AudioMicrophoneInterface> m_micWrapper;
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif  //DUEROS_DCS_APP_SAMPLEAPP_APPLICATIONMANAGER_H_
