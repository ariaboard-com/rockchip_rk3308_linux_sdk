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

#include "DCSApp/ApplicationManager.h"
#include "DCSApp/DuerLinkWrapper.h"
#include "DCSApp/DeviceIoWrapper.h"
#include "DCSApp/SoundController.h"
#include "DCSApp/Configuration.h"
#include "LoggerUtils/DcsSdkLogger.h"
#include <DeviceTools/StringUtils.h>

namespace duerOSDcsApp {
namespace application {

static const std::string QUERY_CURRENT_VERSION = "当前版本号为";
static const std::string POWER_SLEEP = "好的,已休眠";
static const std::string POWER_SHUTDOWN = "拔掉电源就可以关机了";
static const std::string POWER_REBOOT = "拔掉电源再插上就可以重启了";

void ApplicationManager::onDialogUXStateChanged(DialogUXState state) {
    if (state == m_dialogState) {
        return;
    }
    m_dialogState = state;
    onStateChanged();
    if (m_dcsSdk) {
        switch (state) {
            case DialogUXState::MEDIA_PLAYING:
#ifdef KITTAI_KEY_WORD_DETECTOR
                m_dcsSdk->enterPlayMusicScene();
#endif
                break;
            case DialogUXState::MEDIA_STOPPED:
            case DialogUXState::MEDIA_FINISHED:
                if (!(DeviceIoWrapper::getInstance()->isBtPlaying()
                || DeviceIoWrapper::getInstance()->isDlnaPlaying())) {
#ifdef KITTAI_KEY_WORD_DETECTOR
                    m_dcsSdk->exitPlayMusicScene();
#endif
                }
                break;
            default:
                break;
        }
    }
}

void ApplicationManager::onMessageSendComplete(bool success) {
    DeviceIoWrapper::getInstance()->setRecognizing(false);
    if (success) {
        APP_INFO("ApplicationManager::onMessageSendComplete(true)!");
    } else {
        APP_INFO("ApplicationManager::onMessageSendComplete(false)!");
    }
    if (!m_isFromSpeaking) {
        DeviceIoWrapper::getInstance()->ledSpeechOff();
    }
}

void ApplicationManager::onReceivedDirective(const std::string &contextId, const std::string &message) {
    if (message.empty()) {
        return;
    }
    //APP_INFO("ApplicationManager onReceivedDirective:%s", message.c_str());
    std::string playbackctl_namespace = "ai.dueros.device_interface.speaker_controller";
    std::vector<std::string> filter_namespace;
    filter_namespace.emplace_back("ai.dueros.device_interface.voice_output");
    filter_namespace.emplace_back("ai.dueros.device_interface.audio_player");
    rapidjson::Document root;
    root.Parse<0>(message.c_str());
    if (!root.HasParseError()) {
        if (root.HasMember("directive") &&
            root["directive"].HasMember("header") &&
            root["directive"]["header"].HasMember("namespace")) {
            std::string headerNamespace = root["directive"]["header"]["namespace"].GetString();
            for (size_t i = 0; i < filter_namespace.size(); ++i) {
                if (filter_namespace[i].compare(headerNamespace) == 0) {
                    if (!m_isFirstReceiveMsg) {
                        APP_INFO("receive new audio directive, exit mute.");
                        DeviceIoWrapper::getInstance()->exitMute();
                        DeviceIoWrapper::getInstance()->ledSpeechOff();
                        m_isFirstReceiveMsg = true;
                    }
                    break;
                }
            }
            if (headerNamespace == playbackctl_namespace) {
                if (!m_isFirstReceiveMsg) {
                    m_isFirstReceiveMsg = true;
                }
            }
        }
    }
}

void ApplicationManager::onAlertStateChange(const std::string &alertToken,
                                            DialogUXStateObserverInterface::AlertState state,
                                            const std::string &reason) {
    if (DialogUXStateObserverInterface::AlertState::STARTED == state) {
        APP_INFO("ApplicationManager onAlertStateChange STARTED");
        DeviceIoWrapper::getInstance()->setAlertRing(true);
        DeviceIoWrapper::getInstance()->ledAlarm();
    } else if (DialogUXStateObserverInterface::AlertState::STOPPED == state
               || DialogUXStateObserverInterface::AlertState::COMPLETED == state) {
        APP_INFO("ApplicationManager onAlertStateChange STOPPED or COMPLETED");
        DeviceIoWrapper::getInstance()->setAlertRing(false);
        DeviceIoWrapper::getInstance()->ledAlarmOff();
    }
}

void ApplicationManager::onExpectSpeech(bool success) {
    APP_INFO("ApplicationManager onExpectSpeech");
    DeviceIoWrapper::getInstance()->ledWakeUp(DeviceIoWrapper::getInstance()->getDirection());
}

void ApplicationManager::onSpeechFinished() {
    if (m_isFromSpeaking) {
        DeviceIoWrapper::getInstance()->ledSpeechOff();
    }
}

void ApplicationManager::onConnectionStatusChanged(const Status status, const ChangedReason reason) {
    if (m_connectionStatus == status) {
        return;
    }
    m_connectionStatus = status;
    onStateChanged();
}

void ApplicationManager::onSpeechAsrCanceled() {
    DeviceIoWrapper::getInstance()->ledSpeechOff();
}

void ApplicationManager::setSpeakerVolume(int64_t volume) {
    APP_INFO("ApplicationManager setSpeakerVolume: %lld", volume);
    DeviceIoWrapper::getInstance()->setCurrentVolume((unsigned int)volume);
    Configuration::getInstance()->setCommVol((unsigned int)volume);
}

void ApplicationManager::setSpeakerMute(bool isMute) {
    DeviceIoWrapper::getInstance()->setMute(isMute);
    if (isMute) {
        APP_INFO("ApplicationManager setSpeakerMute:Mute");
        DeviceIoWrapper::getInstance()->ledMute();
    } else {
        APP_INFO("ApplicationManager setSpeakerMute:unMute");
        DeviceIoWrapper::getInstance()->ledVolume();
    }
    DeviceIoWrapper::getInstance()->muteChanged();
}

int ApplicationManager::getSpeakerVolume() {
    return DeviceIoWrapper::getInstance()->getCurrentVolume();
}

bool ApplicationManager::getSpeakerMuteStatus() {
    return DeviceIoWrapper::getInstance()->isMute();
}

#ifdef SUPPORT_DEBUGGER
bool ApplicationManager::setStartDebugMode() {
    APP_INFO("ApplicationManager setStartDebug");
    LOGGER_ENABLE(true);
    // 当前版本暂时不禁止adb
//    if (Configuration::getInstance()->getDebug()) {
//        APP_INFO("ApplicationManager setStartDebug in debug mode");
//        system("systemctl start android-tools-adbd.service");
//    } else {
//        APP_INFO("ApplicationManager setStartDebug in release mode");
//        system("/data/usr/bin/android-gadget-setup adb && ( /data/usr/bin/adbd & )");
//    }

    debugStarted();
    return true;
}

bool ApplicationManager::setStopDebugMode() {
    APP_INFO("ApplicationManager setStopDebug");
    LOGGER_ENABLE(false);
    // 当前版本暂时不禁止adb
//    if (Configuration::getInstance()->getDebug()) {
//        system("systemctl stop android-tools-adbd.service");
//    } else {
//        system("killall -9 adbd");
//    }

    debugStoped();
    return true;
}

void ApplicationManager::debugStarted() {
    if (m_dcsSdk) {
        m_dcsSdk->debugStarted();
    }
}

void ApplicationManager::debugStoped() {
    if (m_dcsSdk) {
        m_dcsSdk->debugStoped();
    }
}
#endif

void ApplicationManager::setBluetoothStatus(bool status) {
    if (status) {
        APP_INFO("ApplicationManager setBluetoothStatus:Open");
        DeviceIoWrapper::getInstance()->openBluetooth();
    } else {
        APP_INFO("ApplicationManager setBluetoothStatus:Close");
        DeviceIoWrapper::getInstance()->closeBluetooth();
    }
}

void ApplicationManager::setMicrophoneStatus(bool status) {
    if (!status) {
	    if (m_dcsSdk) {
		    m_dcsSdk->enterSleepMode();
            DeviceIoWrapper::getInstance()->setSleepMode(true);
		}
	} else {
        if (m_dcsSdk) {
            m_dcsSdk->enterWakeupMode();
            DeviceIoWrapper::getInstance()->setSleepMode(false);
        }
    }
#if (defined Hodor) || (defined Kuke) || (defined Dot) || (defined Box86)
    if (!status) {
        APP_INFO("ApplicationManager setMicrophoneStatus: enterSleepMode");
        DeviceIoWrapper::getInstance()->enterSleepMode(true);
    }
#endif
}

void ApplicationManager::setBluetoothConnectionStatus(bool status) {
    if (status) {
        APP_INFO("UIManager setBluetoothConnectionStatus:true");
    } else {
        APP_INFO("UIManager setBluetoothConnectionStatus:false");
        if (DeviceIoWrapper::getInstance()->isBluetoothConnected()) {
            DeviceIoWrapper::getInstance()->unpairBluetooth();
        }
    }
}

void ApplicationManager::queryCurrentVersion() {
    APP_INFO("UIManager queryCurrentVersion");
    std::string version_code = DeviceIoWrapper::getInstance()->getVersion();
    std::string version_str = QUERY_CURRENT_VERSION + version_code;
#ifdef LOCALTTS
#ifndef Sengled
    DeviceIoWrapper::getInstance()->ledTts();
    SoundController::getInstance()->playTts(version_str, true, ledTtsOffCallback);
#endif
#endif
}

void ApplicationManager::powerSleep() {
#if (defined Hodor) || (defined Kuke) || (defined Dot) || (defined Box86)
    DeviceIoWrapper::getInstance()->ledTts();
    SoundController::getInstance()->playTts(POWER_SLEEP, true, ledTtsOffSleepCallback);
#endif
}

void ApplicationManager::powerShutdown() {
#ifdef LOCALTTS
#ifndef Sengled
    DeviceIoWrapper::getInstance()->ledTts();
    SoundController::getInstance()->playTts(POWER_SHUTDOWN, true, ledTtsOffCallback);
#endif
#endif
}

void ApplicationManager::powerReboot() {
#ifdef LOCALTTS
#ifndef Sengled
    DeviceIoWrapper::getInstance()->ledTts();
    SoundController::getInstance()->playTts(POWER_REBOOT, true, ledTtsOffCallback);
#endif
#endif
}

void ApplicationManager::informSdkConnectionStatus(duerOSDcsSDK::sdkInterfaces::SdkConnectionState sdkConnectionStatus) {
    switch (sdkConnectionStatus) {
    case duerOSDcsSDK::sdkInterfaces::SdkConnectionState::SDK_AUTH_FAILED:
        APP_INFO("ApplicationManager informSdkConnectionStatus: SDK_AUTH_FAILED");
#ifdef Box86
        if (m_dcsSdk && m_dcsSdk->isOAuthByPassPair()) {
            DeviceIoWrapper::getInstance()->ledNetLoginFailed();
            SoundController::getInstance()->accountUnbound(ApplicationManager::loginFailed);
        }
#else
        DeviceIoWrapper::getInstance()->ledNetConnectFailed();
        SoundController::getInstance()->accountUnbound(nullptr);
#endif
        break;

    case duerOSDcsSDK::sdkInterfaces::SdkConnectionState::SDK_AUTH_SUCCEED:
        APP_INFO("ApplicationManager informSdkConnectionStatus: SDK_AUTH_SUCCEED");
        break;

    case duerOSDcsSDK::sdkInterfaces::SdkConnectionState::SDK_CONNECT_FAILED:
        APP_INFO("ApplicationManager informSdkConnectionStatus: SDK_CONNECT_FAILED");
        break;

    case duerOSDcsSDK::sdkInterfaces::SdkConnectionState::SDK_CONNECT_SUCCEED:
        APP_INFO("ApplicationManager informSdkConnectionStatus: SDK_CONNECT_SUCCEED");
        if (DuerLinkWrapper::getInstance()->isFirstNetworkReady() || DuerLinkWrapper::getInstance()->isFromConfigNetwork()) {
            APP_INFO("ApplicationManager informSdkConnectionStatus: SDK_CONNECT_SUCCEED networkLinkOrRecoverySuccess");
            DuerLinkWrapper::getInstance()->networkLinkOrRecoverySuccess();
            /// because app is working no notify network link
            DuerLinkWrapper::getInstance()->setFirstNetworkReady(false);
            /// because app is working no notify network link, but after config network must notify
            DuerLinkWrapper::getInstance()->setFromConfigNetwork(false);
        }

        break;

    default:
        break;
    }
}

void ApplicationManager::informCustomizeDirective(const std::string &directive) {
    APP_DEBUG("ApplicationManager informCustomizeDirective: directive = [%s]", directive.c_str());
}

bool ApplicationManager::linkThirdDevice(
        int deviceType, const std::string &clientId, const std::string& message) {
    return DuerLinkWrapper::getInstance()->startLoudspeakerCtrlDevicesService(
            static_cast<duerLink::device_type>(deviceType), clientId, message);
}

bool ApplicationManager::unlinkThirdDevice(int deviceType, const std::string& message) {
    return DuerLinkWrapper::getInstance()->disconnectDevicesConnectionsBySpeType(
            static_cast<duerLink::device_type>(deviceType), message);
}

bool ApplicationManager::onThirdDlpMessage(int deviceType, const std::string &message) {
    return DuerLinkWrapper::getInstance()->sendMsgToDevicesBySpecType(message,
                                                                      static_cast<duerLink::device_type>(deviceType));
}

void ApplicationManager::notifySdkContext(const std::string &context, int deviceType) {
    DuerLinkWrapper::getInstance()->sendDlpMsgToAllClients(context);
}

void ApplicationManager::notifySdkContextBySessionId(
        const std::string& context, unsigned short sessionId, int deviceType) {
    DuerLinkWrapper::getInstance()->sendDlpMessageBySessionId(context, sessionId);
}

bool ApplicationManager::systemInformationGetStatus(duerOSDcsSDK::sdkInterfaces::SystemInformation &systemInformation) {
    APP_INFO("ApplicationManager systemInformationGetStatus");
    systemInformation.ssid = DeviceIoWrapper::getInstance()->getWifiSsid();
    systemInformation.mac = DeviceIoWrapper::getInstance()->getWifiMac();
    systemInformation.ip = DeviceIoWrapper::getInstance()->getWifiIp();
    systemInformation.bluetoothMac = DeviceIoWrapper::getInstance()->getBtMac();
    systemInformation.deviceId = DeviceIoWrapper::getInstance()->getDeviceId();
    systemInformation.sn = DeviceIoWrapper::getInstance()->getDeviceId();
    systemInformation.softwareVersion = DeviceIoWrapper::getInstance()->getVersion();
    systemInformation.onlineStatus = DuerLinkWrapper::getInstance()->isNetworkOnline();
    systemInformation.deviceVersion = DeviceIoWrapper::getInstance()->getDeviceVersion();

    return true;
}

bool ApplicationManager::systemInformationHardReset() {
    APP_INFO("ApplicationManager systemInformationHardReset");
#ifdef Box86
    DuerLinkWrapper::getInstance()->waitLogin();
    DuerLinkWrapper::getInstance()->setFromConfigNetwork(true);
#endif

    return true;
}

bool ApplicationManager::systemUpdateGetStatus(duerOSDcsSDK::sdkInterfaces::SystemUpdateInformation &systemUpdateInformation) {
    APP_INFO("ApplicationManager systemUpdateGetStatus");
    system("upgrade_silent_client 2 GetStatus");

    return true;
}

bool ApplicationManager::systemUpdateUpdate() {
    APP_INFO("ApplicationManager systemUpdateUpdate");
    system("upgrade_silent_client 3 Update");

    return true;
}

bool ApplicationManager::getCurrentConnectedDeviceInfo(int deviceType,
                                   std::string& clientId, std::string& deviceId) {
    return DuerLinkWrapper::getInstance()->getCurrentConnectedDeviceInfo(
            static_cast<duerLink::device_type>(deviceType), clientId, deviceId);
}

std::string ApplicationManager::getWifiBssid() {
    return DeviceIoWrapper::getInstance()->getWifiBssid();
}

#ifdef SUPPORT_INFRARED
bool ApplicationManager::sendInfraredRayCodeRequest(int carrierFrequency, const std::string &pattern) {
    std::string carrierFrequencyStr = deviceCommonLib::deviceTools::convertToString(carrierFrequency);
    carrierFrequencyStr += ",";
    carrierFrequencyStr += pattern;
    APP_INFO("ApplicationManager send Infrared Ray Code Request: %s", carrierFrequencyStr.c_str());
    return 0 == DeviceIoWrapper::getInstance()->transmitInfrared(carrierFrequencyStr);
}
#endif

void ApplicationManager::setDcsSdk(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) {
    m_dcsSdk = dcsSdk;
}

void ApplicationManager::setMicrophoneWrapper(std::shared_ptr<AudioMicrophoneInterface> micWrapper) {
    m_micWrapper = micWrapper;
}

void ApplicationManager::microphoneOn() {
    if (m_micWrapper) {
        m_micWrapper->startStreamingMicrophoneData();
    }
    APP_INFO("ApplicationManager Microphone On!");
}

void ApplicationManager::microphoneOff() {
    if (m_micWrapper) {
        m_micWrapper->stopStreamingMicrophoneData();
    }
    APP_INFO("ApplicationManager Microphone Off!");
}

void ApplicationManager::stopMusicPlay() {
    if (m_dcsSdk) {
        m_dcsSdk->cancelMusicPlay();
        m_dcsSdk->cancelBluetoothPlay();
    }
}

void ApplicationManager::forceHoldFocus(bool holdFlag) {
    if (m_dcsSdk) {
        m_dcsSdk->forceHoldFocus(holdFlag);
    }
}

void ApplicationManager::cancelMusicPlay() {
    if (m_dcsSdk) {
        m_dcsSdk->cancelMusicPlay();
    }
}

void ApplicationManager::muteChanged(int volume, bool muted) {
    if (m_dcsSdk) {
        m_dcsSdk->muteChanged(volume, muted);
    }
}

void ApplicationManager::volumeChanged(int volume, bool muted) {
    if (m_dcsSdk) {
        m_dcsSdk->volumeChanged(volume, muted);
    }
}

void ApplicationManager::closeLocalActiveAlert() {
    if (m_dcsSdk) {
        m_dcsSdk->closeLocalActiveAlert();
    }
}

void ApplicationManager::ledTtsOffCallback() {
    DeviceIoWrapper::getInstance()->ledSpeechOff();
}

void ApplicationManager::ledTtsOffSleepCallback() {
    DeviceIoWrapper::getInstance()->ledSpeechOff();
    DeviceIoWrapper::getInstance()->enterSleepMode(true);
}

void ApplicationManager::onStateChanged() {
    if (m_connectionStatus == duerOSDcsSDK::sdkInterfaces::ConnectionStatusObserverInterface::Status::DISCONNECTED) {
        APP_INFO("ApplicationManager onStateChanged: Client not connected!");
    } else if (m_connectionStatus == duerOSDcsSDK::sdkInterfaces::ConnectionStatusObserverInterface::Status::PENDING) {
        APP_INFO("ApplicationManager onStateChanged: Connecting...");
    } else if (m_connectionStatus == duerOSDcsSDK::sdkInterfaces::ConnectionStatusObserverInterface::Status::CONNECTED) {
        switch (m_dialogState) {
        case DialogUXState::IDLE:
            APP_INFO("ApplicationManager onStateChanged: DuerOS is currently idle!");
            return;

        case DialogUXState::LISTENING:
            m_isFromSpeaking = false;
            m_isFirstReceiveMsg = false;
            APP_INFO("ApplicationManager onStateChanged: Listening...");
            return;

        case DialogUXState::THINKING:
            DeviceIoWrapper::getInstance()->ledAsr();
            APP_INFO("ApplicationManager onStateChanged: Thinking...");
            return;;

        case DialogUXState::SPEAKING:
            m_isFromSpeaking = true;
            DeviceIoWrapper::getInstance()->ledTts();
            APP_INFO("ApplicationManager onStateChanged: Speaking...");
            return;

        case DialogUXState::FINISHED:
            APP_INFO("ApplicationManager onStateChanged: SpeakFinished...");
            return;

        case DialogUXState::MEDIA_PLAYING:
            APP_INFO("ApplicationManager onStateChanged: Media Playing...");
            return;

        case DialogUXState::MEDIA_STOPPED:
            APP_INFO("ApplicationManager onStateChanged: Media Play Stopped...");
            return;

        case DialogUXState::MEDIA_FINISHED:
            APP_INFO("ApplicationManager onStateChanged: Media Play Finished...");
            return;
        }
    }
}

void ApplicationManager::loginFailed() {
    DeviceIoWrapper::getInstance()->ledNetOff();
}

}  // namespace application
}  // namespace duerOSDcsApp
