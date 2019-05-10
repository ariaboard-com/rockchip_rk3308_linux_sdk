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

#include "DCSApp/DeviceIoWrapper.h"
#include "DCSApp/SoundController.h"
#include "DCSApp/Configuration.h"
#include "DCSApp/DuerLinkWrapper.h"
#include <LoggerUtils/DcsSdkLogger.h>

namespace duerOSDcsApp {
namespace application {

static const unsigned int SLEEP_MODE_MINUTES = 10;

using duerOSDcsApp::framework::DeviceIo;
using duerOSDcsApp::framework::DeviceInput;
using duerOSDcsApp::framework::LedState;
using duerOSDcsApp::framework::BtControl;

#define BT_TRANS_CHUNK_SIZE 320

static const int MAX_USER_VOLUME = 100;
static const int MIN_USER_VOLUME = 0;

static const int MUSIC_PLAYBACK_TRACK_ID = 0x0001 << 0; //track:0 1<<0
static const int INFO_PLAYBACK_TRACK_ID = 0x0001 << 1;
static const int TTS_PLAYBACK_TRACK_ID = 0x0001 << 2;
static const int ALERT_PLAYBACK_TRACK_ID = 0x0001 << 3;
static const int RAPID_PLAYBACK_TRACK_ID = 0x0001 << 7;
static const int NATTS_PLAYBACK_TRACK_ID = 0x0001 << 8;

DeviceIoWrapper *DeviceIoWrapper::s_deviceIoWrapper = NULL;
pthread_once_t  DeviceIoWrapper::s_initOnce = PTHREAD_ONCE_INIT;
pthread_once_t  DeviceIoWrapper::s_destroyOnce = PTHREAD_ONCE_INIT;

DeviceIoWrapper::DeviceIoWrapper() : m_btPhoneConnectFlag{false},
                                     m_isTouchStartNetworkConfig{false},
                                     m_isBtPlaying{false},
                                     m_isDlnaPlaying{false},
                                     m_direction{-1},
                                     m_btTransBuffer{NULL},
                                     m_btBufferPos(0),
                                     m_isAlertRing{false},
                                     m_isRecognizing{false},
                                     m_isRecoveryingNetwork{false},
                                     m_sleepMode{false},
                                     m_pauseBtnClicked{false},
                                     m_lowVolumeFlag(true),
                                     m_eqInitFlag(false) {
    DeviceIo::getInstance()->setNotify(this);
    m_btTransBuffer = (char *)malloc(sizeof(char) * BT_TRANS_CHUNK_SIZE);
    s_destroyOnce = PTHREAD_ONCE_INIT;
    m_vloumeWhenPressed = getCurrentVolume();
}

DeviceIoWrapper::~DeviceIoWrapper() {
    if (m_btTransBuffer) {
        free(m_btTransBuffer);
    }
    s_initOnce = PTHREAD_ONCE_INIT;
    m_observers.clear();
}

DeviceIoWrapper* DeviceIoWrapper::getInstance() {
    pthread_once(&s_initOnce, &DeviceIoWrapper::init);
    return s_deviceIoWrapper;
}

void DeviceIoWrapper::init() {
    if (s_deviceIoWrapper == NULL) {
        s_deviceIoWrapper = new DeviceIoWrapper();
    }
}

void DeviceIoWrapper::destroy() {
    DeviceIo::getInstance()->releaseInstance();
    if (s_deviceIoWrapper) {
        delete s_deviceIoWrapper;
        s_deviceIoWrapper = NULL;
    }
}

void DeviceIoWrapper::release() {
    pthread_once(&s_destroyOnce, DeviceIoWrapper::destroy);
}

void DeviceIoWrapper::callback(DeviceInput event, void *data, int len) {
    APP_INFO("hardware  event:%d", static_cast<int>(event));

    switch (event) {
        case DeviceInput::KEY_VOLUME_MUTE: {
            handleVolumeMute();
            break;
        }
        case DeviceInput::KEY_VOLUME_DOWN: {
            handleVolumeDown(*(int *)data);
            break;
        }
        case DeviceInput::KEY_VOLUME_UP: {
            handleVolumeUp(*(int*)data);
            break;
        }
        case DeviceInput::KEY_WAKE_UP: {
            handleWakeUp();
            break;
        }
        case DeviceInput::KEY_BLUETOOTH_SHORT: {
            if (isBleOpened()) {
                exitMute();
                SoundController::getInstance()->bleNetworkConfig();
            } else {
                handleBluetooth();
            }
            break;
        }
        case DeviceInput::KEY_BLUETOOTH_LONG: {
            if (isBleOpened()) {
                SoundController::getInstance()->bleNetworkConfig();
            } else {
                if (isBluetoothConnected()) {
                    unpairBluetooth();
                }
            }
            break;
        }
        case DeviceInput::KEY_MIC_MUTE: {
            handleSleepMode();
            break;
        }
	    case DeviceInput::KEY_SLEEP_MODE: {
            handleSleepMode();
            break;
        }
        case DeviceInput::KEY_ONE_SHORT: {
            handleWakeUp();
            break;
        }
        case DeviceInput::SLIDER_PRESSED: {
            handleTouchEvent(TouchEvent::TOUCH_PRESSED);
            break;
        }
        case DeviceInput::SLIDER_RELEASE: {
            handleTouchEvent(TouchEvent::TOUCH_RELEASED);
            break;
        }
        case DeviceInput::VOLUME_CHANGED: {
            if (len == sizeof(int)) {
                int value = *((int*)data);
                handleSliderSlide(value);
            }
            break;
        }
        case DeviceInput::BT_WAIT_PAIR: {

            break;
        }
        case DeviceInput::BT_DO_PAIR: {
            DeviceIo::getInstance()->controlLed(LedState::LED_BT_DO_PAIR);
            break;
        }
        case DeviceInput::BT_PAIR_FAILED_PAIRED: {
            ledBtWaitPair();
            SoundController::getInstance()->btPairFailedPaired();
            break;
        }
        case DeviceInput::BT_PAIR_FAILED_OTHER: {
            DeviceIo::getInstance()->controlLed(LedState::LED_BT_PAIR_FAILED);
            SoundController::getInstance()->btPairFailedOther();
            break;
        }
        case DeviceInput::BT_PAIR_SUCCESS: {
            DeviceIo::getInstance()->controlLed(LedState::LED_BT_PAIR_SUCCESS);
            SoundController::getInstance()->btPairSuccess(deviceioWrapperBtSoundPlayFinished);
            break;
        }
        case DeviceInput::BT_DISCONNECT: {
            SoundController::getInstance()->btDisconnect(deviceioWrapperBtDisconnectSoundPlayFinished);
            APP_INFO("DeviceIoWrapper callback BT_DISCONNECT");
            m_isBtPlaying = false;
            for (auto observer : m_observers) {
                if (observer) {
                    observer->btDisconnect();
                }
            }
            break;
        }
        case DeviceInput::BT_START_PLAY: {
            APP_INFO("DeviceIoWrapper callback BT_START_PLAY");
            m_isBtPlaying = true;
            for (auto observer : m_observers) {
                if (observer) {
                    observer->btStartPlay();
                }
            }
            break;
        }
        case DeviceInput::BT_STOP_PLAY: {
            APP_INFO("DeviceIoWrapper callback BT_STOP_PLAY");
            m_isBtPlaying = false;
            for (auto observer : m_observers) {
                if (observer) {
                    observer->btStopPlay();
                }
            }
            break;
        }
        case DeviceInput::BT_POWER_OFF: {
            APP_INFO("DeviceIoWrapper callback BT_POWER_OFF");
            m_isBtPlaying = false;
            for (auto observer : m_observers) {
                if (observer) {
                    observer->btPowerOff();
                }
            }
            break;
        }
        case DeviceInput::BT_HFP_AUDIO_CONNECT: {
            m_btPhoneConnectFlag = true;
            break;
        }
        case DeviceInput::BT_HFP_AUDIO_DISCONNECT: {
            m_btPhoneConnectFlag = false;
            break;
        }
        case DeviceInput::KEY_ONE_LONG:
        case DeviceInput::KEY_ENTER_AP:
            if (m_isRecoveryingNetwork) {
                setRecoveryingNetwork(false);
                DuerLinkWrapper::getInstance()->stopNetworkRecovery();
                break;
            }
            if (!m_isTouchStartNetworkConfig) {
                APP_INFO("DeviceIoWrapper callback start_network_config");
                setTouchStartNetworkConfig(true);
                DuerLinkWrapper::getInstance()->startNetworkConfig();
            } else {
                APP_INFO("DeviceIoWrapper callback stop_network_config");
                DuerLinkWrapper::getInstance()->stopNetworkConfig();
            }
            break;
        case DeviceInput::KEY_ONE_LONG_10S:
        case DeviceInput::KEY_EXIT_AP:
            if (m_isRecoveryingNetwork) {
                break;
            }
            DuerLinkWrapper::getInstance()->stopNetworkConfig();
            break;
        case DeviceInput::BLE_CLIENT_CONNECT:
            DuerLinkWrapper::getInstance()->bleClientConnected();
            break;
        case DeviceInput::BLE_CLIENT_DISCONNECT:
            DuerLinkWrapper::getInstance()->bleClientDisconnected();
            break;
        case DeviceInput::BLE_SERVER_RECV:
            DuerLinkWrapper::getInstance()->bleRecvData(data, len);
            break;
        case DeviceInput::KEY_CLOSE_TIMER_ALARM: {
            if (m_isAlertRing) {
                closeLocalActiveAlert();
                *(int*)data = 1;
                APP_INFO("CLOSE  TIMER ALARM!!!!!!!");

            } else {
                *(int*)data = 0;
            }
            break;
        }
        case DeviceInput::KEY_IS_SLEEP_STATE:{
            if (isSleepMode()) {
                *(int*)data = 1;
            } else {
                *(int*)data = 0;
            }
            break;
        }
        case DeviceInput::KEY_PLAY_PAUSE: {
            handlePlayAndPause();
            break;
        }
        case DeviceInput::DLNA_STOPPED: {
            m_isDlnaPlaying = false;
            APP_INFO("DeviceIoWrapper callback DLNA_STOPPED");
            for(auto observer : m_observers)
            {
                observer->dlnaStopPlay();
            }
            break;
        }
        case DeviceInput::DLNA_PLAYING: {
            m_isDlnaPlaying = true;
            APP_INFO("DeviceIoWrapper callback DLNA_PLAYING");
            for(auto observer : m_observers)
            {
                observer->dlnaStartPlay();
            }
            break;
        }
        case DeviceInput::DLNA_PAUSED: {
            m_isDlnaPlaying = false;
            for(auto observer : m_observers)
            {
                observer->dlnaPausePlay();
            }
            APP_INFO("DeviceIoWrapper callback DLNA_PAUSED");
            break;
        }
        default:
            break;
    }
}

void DeviceIoWrapper::handleVolumeMute() {
    if (isMute()) {
        setMute(false);
        ledVolume();
    } else {
        setMute(true);
        ledMute();
    }
    muteChanged();
}

void DeviceIoWrapper::handleVolumeDown(int dec) {
    SoundController::getInstance()->volume();
    int currentVolume = getCurrentVolume();
    setCurrentVolume(currentVolume - dec);
    Configuration::getInstance()->setCommVol(currentVolume - dec);
}

void DeviceIoWrapper::handleVolumeUp(int inc) {
    SoundController::getInstance()->volume();
    int currentVolume = getCurrentVolume();
    setCurrentVolume(currentVolume + inc);
    Configuration::getInstance()->setCommVol(currentVolume + inc);
}

void DeviceIoWrapper::handleWakeUp() {
    for (auto observer : m_observers) {
        if (observer) {
            observer->touchStartAsr();
        }
    }
}

void DeviceIoWrapper::handleBluetooth() {
    if (!isBluetoothOpen()) {
        SoundController::getInstance()->openBluetooth(staticFunc, this);
        openBluetooth();
    } else {
        SoundController::getInstance()->closeBluetooth(staticFunc, this);
        closeBluetooth();
    }
}

bool DeviceIoWrapper::isBtPlaying() {
    return m_isBtPlaying;
}

bool DeviceIoWrapper::isDlnaPlaying() {
    return m_isDlnaPlaying;
}

void DeviceIoWrapper::handleTouchEvent(TouchEvent event) {
    if (event == TouchEvent::TOUCH_PRESSED) {
        m_vloumeWhenPressed = getCurrentVolume();
        DeviceIo::getInstance()->controlLed(LedState::LED_VOLUME, &m_vloumeWhenPressed, sizeof(int));
    }

    if (event == TouchEvent::TOUCH_RELEASED) {
        m_vloumeWhenPressed = 0;
    }
}

void DeviceIoWrapper::handleSliderSlide(int incre) {
    setCurrentVolume(m_vloumeWhenPressed + incre);
    Configuration::getInstance()->setCommVol(m_vloumeWhenPressed + incre);
}

int DeviceIoWrapper::fetchWakeupDirection() {
    return DeviceIo::getInstance()->getAngle();
}

void DeviceIoWrapper::ledWakeUp(int angle) {
    DeviceIo::getInstance()->controlLed(LedState::LED_WAKE_UP_DOA, &angle, sizeof(angle));
}

void DeviceIoWrapper::ledTts() {
    DeviceIo::getInstance()->controlLed(LedState::LED_PLAY_TTS);
}

void DeviceIoWrapper::ledAsr() {
    DeviceIo::getInstance()->controlLed(LedState::LED_SPEECH_PARSE);
}

void DeviceIoWrapper::ledMute() {
    DeviceIo::getInstance()->controlLed(LedState::LED_MUTE);
}

void DeviceIoWrapper::ledBtOff() {
    LedState layer = LedState::LED_BT_WAIT_PAIR;
    DeviceIo::getInstance()->controlLed(LedState::LED_CLOSE_A_LAYER, &layer, sizeof(int));
}

void DeviceIoWrapper::ledBtClose() {
    DeviceIo::getInstance()->controlLed(LedState::LED_BT_CLOSE);
}

void DeviceIoWrapper::ledSpeechOff() {
    LedState layer = LedState::LED_SPEECH_PARSE;
    DeviceIo::getInstance()->controlLed(LedState::LED_CLOSE_A_LAYER, &layer, sizeof(int));
}

void DeviceIoWrapper::ledMuteOff() {
    LedState layer = LedState::LED_MUTE;
    DeviceIo::getInstance()->controlLed(LedState::LED_CLOSE_A_LAYER, &layer, sizeof(int));
}

void DeviceIoWrapper::ledNetOff() {
    LedState layer = LedState::LED_NET_WAIT_CONNECT;
    DeviceIo::getInstance()->controlLed(LedState::LED_CLOSE_A_LAYER, &layer, sizeof(int));
}

void DeviceIoWrapper::ledNetWaitConnect() {
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_WAIT_CONNECT);
}

void DeviceIoWrapper::ledNetDoConnect() {
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_DO_CONNECT);
}

void DeviceIoWrapper::ledNetConnectSuccess() {
#ifdef Box86
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_LOGIN_SUCCESS);
#else
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_CONNECT_SUCCESS);
#endif
}

void DeviceIoWrapper::ledNetConnectFailed() {
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_CONNECT_FAILED);
}

void DeviceIoWrapper::ledNetRecoveryConnect() {
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_RECOVERY);
}

void DeviceIoWrapper::ledNetWaitLogin() {
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_WAIT_LOGIN);
}

void DeviceIoWrapper::ledNetLoginFailed() {
    DeviceIo::getInstance()->controlLed(LedState::LED_NET_LOGIN_FAILED);
}

void DeviceIoWrapper::ledBtWaitPair() {
    DeviceIo::getInstance()->controlLed(LedState::LED_BT_WAIT_PAIR);
}

int DeviceIoWrapper::getCurrentVolume() const {
    return DeviceIo::getInstance()->getVolume(0);
}

void DeviceIoWrapper::setCurrentVolume(int current_volume) {
    initializeEQ();
    current_volume = std::max(current_volume, MIN_USER_VOLUME);
    current_volume = std::min(current_volume, MAX_USER_VOLUME);
    APP_INFO("%s, current_volume: %d\n", __func__, current_volume);

    bool currIsLow = current_volume >= 90 ? false : true;
    if (currIsLow != m_lowVolumeFlag) {
        if (m_lowVolumeFlag && !currIsLow) {
            system("sh /oem/scripts/rk3308_ti5733_config_high_vol_pa.sh");
        } else {
            system("sh /oem/scripts/rk3308_ti5733_config_low_vol_pa.sh");
        }
    }
    m_lowVolumeFlag = currIsLow;

    if(isBluetoothOpen())
        btSetVolume(current_volume);

    DeviceIo::getInstance()->setVolume(current_volume,
						(MUSIC_PLAYBACK_TRACK_ID |
						INFO_PLAYBACK_TRACK_ID |
						TTS_PLAYBACK_TRACK_ID |
						RAPID_PLAYBACK_TRACK_ID |
						NATTS_PLAYBACK_TRACK_ID));
    ledVolume();
    volumeChanged();
}

int DeviceIoWrapper::getAlertVolume() const {
    return DeviceIo::getInstance()->getVolume(3);
}

void DeviceIoWrapper::setAlertVolume(int volume) {
#if PLAYBACK_DEVICE_NUM <= 1
    APP_INFO("%s, volume: %d; Not Implemented\n", __func__, volume);
#else
    DeviceIo::getInstance()->setVolume(volume, ALERT_PLAYBACK_TRACK_ID);
    ledVolume();
    volumeChanged();
#endif
}

void DeviceIoWrapper::initCommonVolume(int volume) {
    APP_INFO("%s, volume: %d\n", __func__, volume);
    DeviceIo::getInstance()->setVolume(volume,
						(MUSIC_PLAYBACK_TRACK_ID |
						INFO_PLAYBACK_TRACK_ID |
						TTS_PLAYBACK_TRACK_ID |
						RAPID_PLAYBACK_TRACK_ID |
						NATTS_PLAYBACK_TRACK_ID));
    volumeChanged();
}

void DeviceIoWrapper::initAlertVolume(int volume) {
#if PLAYBACK_DEVICE_NUM <= 1
    APP_INFO("%s, volume: %d; Not Implemented\n", __func__, volume);
#else
    DeviceIo::getInstance()->setVolume(volume, ALERT_PLAYBACK_TRACK_ID);
    volumeChanged();
#endif
}

bool DeviceIoWrapper::isMute() const {
    return DeviceIo::getInstance()->isMute();
}

int DeviceIoWrapper::setMute(bool mute) {
    return DeviceIo::getInstance()->setMute(mute);
}

void DeviceIoWrapper::ledVolume() {
    int current_volume = getCurrentVolume();
    DeviceIo::getInstance()->controlLed(LedState::LED_VOLUME, &current_volume, sizeof(int));
}

bool DeviceIoWrapper::isBluetoothOpen() {
    return DeviceIo::getInstance()->controlBt(BtControl::BT_IS_OPENED);
}

bool DeviceIoWrapper::isBluetoothConnected() {
    return DeviceIo::getInstance()->controlBt(BtControl::BT_IS_CONNECTED);
}

void DeviceIoWrapper::openBluetooth() {
    ledBtWaitPair();
    DeviceIo::getInstance()->controlBt(BtControl::BT_OPEN);
}

void DeviceIoWrapper::closeBluetooth() {
#ifdef Box86
    ledBtClose();
#else
    ledBtOff();
#endif
    DeviceIo::getInstance()->controlBt(BtControl::BT_CLOSE);
}

void DeviceIoWrapper::unpairBluetooth() {
    DeviceIo::getInstance()->controlBt(BtControl::BT_UNPAIR);
}

void DeviceIoWrapper::transmitAudioToBluetooth(const char *audioData,
                                               unsigned int len) {
    if (!m_btPhoneConnectFlag) {
        return;
    }
    while (true) {
        void *p = m_btTransBuffer + m_btBufferPos;
        if (m_btBufferPos + len > BT_TRANS_CHUNK_SIZE) {
            memcpy(p, audioData, BT_TRANS_CHUNK_SIZE - m_btBufferPos);
            int cp_len = BT_TRANS_CHUNK_SIZE - m_btBufferPos;
            m_btBufferPos = BT_TRANS_CHUNK_SIZE;
            DeviceIo::getInstance()->controlBt(BtControl::BT_HFP_RECORD, m_btTransBuffer, BT_TRANS_CHUNK_SIZE);
            memset(m_btTransBuffer, 0, BT_TRANS_CHUNK_SIZE);
            audioData += cp_len;
            len -= cp_len;
            m_btBufferPos = 0;
        } else {
            memcpy(p, audioData, len);
            m_btBufferPos += len;
            break;
        }
    }
}

void DeviceIoWrapper::btResumePlay() {
    DeviceIo::getInstance()->controlBt(BtControl::BT_RESUME_PLAY);
}

void DeviceIoWrapper::btPausePlay() {
    DeviceIo::getInstance()->controlBt(BtControl::BT_PAUSE_PLAY);
}

void  DeviceIoWrapper::btSetVolume(int volume) {
    int current_volume = DeviceIoWrapper::getInstance()->getCurrentVolume();

    APP_INFO("current_volume: %d, volume: %d\n", current_volume, volume);

    if(current_volume < volume)
        btVolumeUp();
    else if(current_volume > volume)
        btVolumeDown();
}

void DeviceIoWrapper::btVolumeUp() {
    DeviceIo::getInstance()->controlBt(BtControl::BT_VOLUME_UP);
}

void DeviceIoWrapper::btVolumeDown() {
    DeviceIo::getInstance()->controlBt(BtControl::BT_VOLUME_DOWN);
}

void DeviceIoWrapper::btPlayPrevious() {
    DeviceIo::getInstance()->controlBt(BtControl::BT_AVRCP_BWD);
    DeviceIo::getInstance()->controlBt(BtControl::BT_RESUME_PLAY);
}

void DeviceIoWrapper::btPlayNext() {
    DeviceIo::getInstance()->controlBt(BtControl::BT_AVRCP_FWD);
    DeviceIo::getInstance()->controlBt(BtControl::BT_RESUME_PLAY);
}

std::string DeviceIoWrapper::getDeviceId() {
    char sn[32] = {0};

    if (DeviceIo::getInstance()->getSn(sn)) {
        return std::string(sn);
    } else {
        return Configuration::getInstance()->getDeviceId();
    }
}

std::string DeviceIoWrapper::getDeviceVersion() {
#ifdef MTK8516
    char deviceVersion[64] = { 0 };
    if (DeviceIo::getInstance()->getPCB(deviceVersion)) {
        APP_DEBUG("Device Version = [%s]", deviceVersion);
        return std::string(deviceVersion);
    } else {
        return "";
    }
#else
    return "";
#endif
}

std::string DeviceIoWrapper::getVersion() {
    return DeviceIo::getInstance()->getVersion();
}

void DeviceIoWrapper::setApplicationManager(std::shared_ptr<ApplicationManager> applicationManager) {
    m_applicationManager = applicationManager;
}

void DeviceIoWrapper::cancelMusic() {
    if (m_applicationManager) {
        m_applicationManager->cancelMusicPlay();
    }
}

void DeviceIoWrapper::exitMute() {
    if (!isSleepMode()) {
        if (isMute()) {
            ledMuteOff();
            setMute(false);
            muteChanged();
        }
    }
}

void DeviceIoWrapper::muteChanged() {
    int current_volume = getCurrentVolume();
    bool mute = isMute();
    if (m_applicationManager) {
        m_applicationManager->muteChanged(current_volume, mute);
    }
}

void DeviceIoWrapper::volumeChanged() {
    int current_volume = getCurrentVolume();
    bool mute = isMute();
    if (m_applicationManager) {
        APP_INFO("%s, current_volume: %d, mute: %d\n", __func__, current_volume,
                 mute);
        m_applicationManager->volumeChanged(current_volume, mute);
    }
}

void DeviceIoWrapper::deviceioWrapperBtSoundPlayFinished() {
    DeviceIoWrapper::getInstance()->ledBtOff();
}

void DeviceIoWrapper::deviceioWrapperBtDisconnectSoundPlayFinished() {
    DeviceIoWrapper::getInstance()->ledBtWaitPair();
}

int DeviceIoWrapper::getDirection() const {
    return m_direction;
}

void DeviceIoWrapper::setDirection(int direction) {
    DeviceIoWrapper::m_direction = direction;
}

void DeviceIoWrapper::addObserver(std::shared_ptr<DeviceIoWrapperObserverInterface> observer) {
    if (!observer) {
        APP_ERROR("DeviceIoWrapper addObserverFailed nullObserver");
        return;
    }
    m_observers.insert(observer);
}

void DeviceIoWrapper::removeObserver(std::shared_ptr<DeviceIoWrapperObserverInterface> observer) {
    if (!observer) {
        APP_ERROR("DeviceIoWrapper removeObserverFailed nullObserver");
        return;
    }
    m_observers.erase(observer);
}

void DeviceIoWrapper::ledAlarm() {
    DeviceIo::getInstance()->controlLed(LedState::LED_ALARM);
}

void DeviceIoWrapper::ledAlarmOff() {
    LedState layer = LedState::LED_ALARM;
    DeviceIo::getInstance()->controlLed(LedState::LED_CLOSE_A_LAYER, &layer, sizeof(int));
}

void DeviceIoWrapper::ledOtaDoing() {
    DeviceIo::getInstance()->controlLed(LedState::LED_ALL_OFF);
    DeviceIo::getInstance()->controlLed(LedState::LED_OTA_DOING);
}

void DeviceIoWrapper::setTouchStartNetworkConfig(bool isTouchStartNetworkConfig) {
    DeviceIoWrapper::m_isTouchStartNetworkConfig = isTouchStartNetworkConfig;
}

bool DeviceIoWrapper::isTouchStartNetworkConfig() const {
    return m_isTouchStartNetworkConfig;
}

void DeviceIoWrapper::closeLocalActiveAlert() {
    if (m_applicationManager) {
        m_applicationManager->closeLocalActiveAlert();
    }
}

void DeviceIoWrapper::staticFunc(void *arg) {
    if (arg == NULL) {
        return;
    }
    DeviceIoWrapper* deviceIoWrapper = static_cast<DeviceIoWrapper*>(arg);
    usleep(80000);
    deviceIoWrapper->exitMute();
}

void DeviceIoWrapper::setAlertRing(bool isAlertRing) {
    DeviceIoWrapper::m_isAlertRing = isAlertRing;
}

bool DeviceIoWrapper::isAlertRing() {
    return m_isAlertRing;
}

void DeviceIoWrapper::wakeupVolumeDown() {
    int currentVolume = getCurrentVolume();
    setCurrentVolume(currentVolume - 10);
    Configuration::getInstance()->setCommVol(currentVolume - 10);
}

void DeviceIoWrapper::wakeupVolumeUp() {
    int currentVolume = getCurrentVolume();
    setCurrentVolume(currentVolume + 10);
    Configuration::getInstance()->setCommVol(currentVolume + 10);
}

std::string DeviceIoWrapper::getBtMac() {
    char bt_mac[18] = {0};
    DeviceIo::getInstance()->controlBt(BtControl::GET_BT_MAC, bt_mac,
                                       sizeof(bt_mac));
    return std::string(bt_mac);
}

std::string DeviceIoWrapper::getWifiMac() {
    char wifi_mac[18] = {0};
    DeviceIo::getInstance()->controlBt(BtControl::GET_WIFI_MAC, wifi_mac,
                                       sizeof(wifi_mac));
    return std::string(wifi_mac);
}

std::string DeviceIoWrapper::getWifiIp() {
    char wifi_ip[40] = {0};
    DeviceIo::getInstance()->controlBt(BtControl::GET_WIFI_IP, wifi_ip,
                                       sizeof(wifi_ip));
    return std::string(wifi_ip);
}

std::string DeviceIoWrapper::getWifiSsid() {
    char wifi_ssid[128] = {0};
    DeviceIo::getInstance()->controlBt(BtControl::GET_WIFI_SSID, wifi_ssid,
                                       sizeof(wifi_ssid));
    return std::string(wifi_ssid);
}

std::string DeviceIoWrapper::getWifiBssid() {
    char wifi_bssid[128] = {0};
    DeviceIo::getInstance()->controlBt(BtControl::GET_WIFI_BSSID, wifi_bssid,
                                       sizeof(wifi_bssid));
    return std::string(wifi_bssid);
}

std::string DeviceIoWrapper::getlocalName() {
    char local_name[128] = {0};
    DeviceIo::getInstance()->controlBt(BtControl::GET_LOCAL_NAME, local_name,
                                       sizeof(local_name));
    return std::string(local_name);
}

void DeviceIoWrapper::OnNetworkReady() {
    return;
}

bool DeviceIoWrapper::isRecognizing() const {
    return m_isRecognizing;
}

void DeviceIoWrapper::setRecognizing(bool isRecognizing) {
    DeviceIoWrapper::m_isRecognizing = isRecognizing;
}

void DeviceIoWrapper::setRecoveryingNetwork(bool isRecoveryingNetwork) {
    DeviceIoWrapper::m_isRecoveryingNetwork = isRecoveryingNetwork;
}

bool DeviceIoWrapper::isBleOpened() {
    return DeviceIo::getInstance()->controlBt(BtControl::BLE_IS_OPENED);
}

bool DeviceIoWrapper::isSleepMode() const {
    return m_sleepMode;
}

void DeviceIoWrapper::setSleepMode(bool isSleepMode) {
    m_sleepMode = isSleepMode;
}

void DeviceIoWrapper::enterSleepMode(bool isLedLightOn) {
    APP_INFO("DeviceIoWrapper enterSleepMode");
    std::chrono::minutes minutesToWait(SLEEP_MODE_MINUTES);
    m_scheduledTimer.start(minutesToWait, std::bind(&DeviceIoWrapper::timeToTenMinutes, this));
    if (isLedLightOn) {
        ledSleepMode();
    }
    setSleepMode(true);
    if (m_applicationManager) {
	    m_applicationManager->setMicrophoneStatus(false);
        //m_applicationManager->microphoneOff();
    }
    if (!m_pauseBtnClicked) {
        m_applicationManager->forceHoldFocus(true);
    }
    setMute(true);
    if (isLedLightOn) {
        ledMute();
    }
    muteChanged();
}

void DeviceIoWrapper::exitSleepMode() {
    APP_INFO("DeviceIoWrapper exitSleepMode");
    m_scheduledTimer.stop();
    ledUnSleepMode();
    setSleepMode(false);
    if (m_applicationManager) {
	    m_applicationManager->setMicrophoneStatus(true);
        //m_applicationManager->microphoneOn();
    }
    if (!m_pauseBtnClicked) {
        m_applicationManager->forceHoldFocus(false);
    }
    setMute(false);
    muteChanged();
}

void DeviceIoWrapper::handleSleepMode() {
    if (!isSleepMode()) {
        enterSleepMode(true);
    } else {
        exitSleepMode();
    }
}

void DeviceIoWrapper::ledSleepMode() {
    DeviceIo::getInstance()->controlLed(LedState::LED_SLEEP_MODE);
}

void DeviceIoWrapper::ledUnSleepMode() {
    LedState layer = LedState::LED_SLEEP_MODE;
    DeviceIo::getInstance()->controlLed(LedState::LED_CLOSE_A_LAYER, &layer, sizeof(int));
}

bool DeviceIoWrapper::isPhoneConnected() const {
    return m_btPhoneConnectFlag;
}

void DeviceIoWrapper::timeToTenMinutes() {
    APP_INFO("time to ten minutes");
    if (m_applicationManager) {
        m_applicationManager->stopMusicPlay();
    }
}

void DeviceIoWrapper::initializeEQ() {
    if (!m_eqInitFlag) {
        int volume = getCurrentVolume();
        m_lowVolumeFlag = volume >= 90 ? false : true;
        if (m_lowVolumeFlag) {
            system("sh /oem/scripts/rk3308_ti5733_config_low_vol_pa.sh");
        } else {
            system("sh /oem/scripts/rk3308_ti5733_config_high_vol_pa.sh");
        }
        m_eqInitFlag = true;
    }
}

#ifdef SUPPORT_INFRARED
int DeviceIoWrapper::transmitInfrared(std::string &infraredCode) {
    return DeviceIo::getInstance()->transmitInfrared(infraredCode);
}
#endif

bool DeviceIoWrapper::isPauseBtnClicked() const {
    return m_pauseBtnClicked;
}

void DeviceIoWrapper::setPauseBtnFlag(bool flag) {
    m_pauseBtnClicked = flag;
}

void DeviceIoWrapper::handlePlayAndPause() {
    if (!isPauseBtnClicked()) {
        setPauseBtnFlag(true);
        if (m_applicationManager) {
            m_applicationManager->forceHoldFocus(true);
        }
    } else {
        setPauseBtnFlag(false);
        if (m_applicationManager) {
            m_applicationManager->forceHoldFocus(false);
        }
    }
}

}  // namespace application
}  // namespace duerOSDcsApp
