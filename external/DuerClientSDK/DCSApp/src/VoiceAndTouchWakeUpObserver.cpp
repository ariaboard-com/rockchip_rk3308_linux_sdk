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

#include <Dlna/DlnaDmrOutput.h>
#include "DCSApp/VoiceAndTouchWakeUpObserver.h"
#include "DCSApp/DuerLinkWrapper.h"
#include "DCSApp/SoundController.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace application {

static const std::string KEYWORD_XIAODUXIAODU = "小度小度";
static const std::string KEYWORD_VOLUME_UP = "大声一点";
static const std::string KEYWORD_PREVIOUS = "上一个";
static const std::string KEYWORD_LAST = "上一首";
static const std::string KEYWORD_VOLUME_DOWN = "小声一点";
static const std::string KEYWORD_NEXT = "下一个";
static const std::string KEYWORD_NEXT_SONG = "下一首";
static const std::string KEYWORD_PAUSE = "暂停";

VoiceAndTouchWakeUpObserver::VoiceAndTouchWakeUpObserver() {

}

void VoiceAndTouchWakeUpObserver::onKeyWordDetected(std::string keyword,
                                                    uint64_t beginIndex,
                                                    uint64_t endIndex) {
    APP_INFO("VoiceAndTouchWakeUpObserver onKeyWordDetected:%s", keyword.c_str());
    printf("VoiceAndTouchWakeUpObserver onKeyWordDetected:%s\n", keyword.c_str());
    DeviceIoWrapper::getInstance()->setRecognizing(true);
    wakeupTriggered(true, beginIndex, endIndex, keyword);

#if 0 
    if (endIndex != sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX &&
        beginIndex == sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX) {
        if (KEYWORD_XIAODUXIAODU == keyword) {
            APP_INFO("VoiceAndTouchWakeUpObserver onKeyWordDetected:%s", keyword.c_str());
            DeviceIoWrapper::getInstance()->setRecognizing(true);
            wakeupTriggered(true, beginIndex, endIndex, keyword);
        } else {
            if (!DeviceIoWrapper::getInstance()->isRecognizing()) {
                APP_INFO("VoiceAndTouchWakeUpObserver onSceneKeyWordDetected:%s", keyword.c_str());
                if (KEYWORD_VOLUME_UP == keyword) {
                    DeviceIoWrapper::getInstance()->wakeupVolumeUp();
                } else if (KEYWORD_VOLUME_DOWN == keyword) {
                    DeviceIoWrapper::getInstance()->wakeupVolumeDown();
                } else if ((KEYWORD_PREVIOUS == keyword) || (KEYWORD_LAST == keyword)) {
                    if (DeviceIoWrapper::getInstance()->isBtPlaying()) {
                        DeviceIoWrapper::getInstance()->btPlayPrevious();
                    } else {
                        if (m_dcsSdk) {
                            m_dcsSdk->issuePlaybackPreviousCommand();
                        }
                    }
                } else if ((KEYWORD_NEXT == keyword) || (KEYWORD_NEXT_SONG == keyword)) {
                    if (DeviceIoWrapper::getInstance()->isBtPlaying()) {
                        DeviceIoWrapper::getInstance()->btPlayNext();
                    } else {
                        if (m_dcsSdk) {
                            m_dcsSdk->issuePlaybackNextCommand();
                        }
                    }
                } else if (KEYWORD_PAUSE == keyword) {
                    if (DeviceIoWrapper::getInstance()->isBtPlaying()) {
                        DeviceIoWrapper::getInstance()->btPausePlay();
                    }
#ifdef DUEROS_DLNA
                    else if(DeviceIoWrapper::getInstance()->isDlnaPlaying()) {
                        duerOSDcsApp::dueros_dlna::Output::get_instance().output_pause();
                    }
#endif
                    else {
                        if (m_dcsSdk) {
                            m_dcsSdk->issuePlaybackPauseCommand();
                        }
                    }
                }
            }
        }
    }
#endif
}

void VoiceAndTouchWakeUpObserver::onLocationDetected(int location) {
    APP_INFO("VoiceAndTouchWakeUpObserver onLocationDetected:%d\n", location);
    DeviceIoWrapper::getInstance()->ledNetOff();
    DeviceIoWrapper::getInstance()->setDirection(location);
    DeviceIoWrapper::getInstance()->ledWakeUp(DeviceIoWrapper::getInstance()->getDirection());
}


void VoiceAndTouchWakeUpObserver::touchStartAsr() {
    APP_INFO("VoiceAndTouchWakeUpObserver touchStartAsr");
    wakeupTriggered(false);
    APP_INFO("VoiceAndTouchWakeUpObserver touchStartAsr end");
}

void VoiceAndTouchWakeUpObserver::wakeupTriggered(bool is_voice_wakeup,
                                                  uint64_t beginIndex,
                                                  uint64_t endIndex,
                                                  std::string keyword) {
	// SoundController::getInstance()->wakeUp();
	APP_INFO("VoiceAndTouchWakeUpObserver wakeupTriggered");
    DeviceIoWrapper::getInstance()->initializeEQ();
    if ((DUERLINK_NETWORK_SUCCEEDED == DuerLinkWrapper::getInstance()->getNetworkStatus()) ||
        (DUERLINK_NETWORK_CONFIG_SUCCEEDED == DuerLinkWrapper::getInstance()->getNetworkStatus()) ||
        (DUERLINK_NETWORK_RECOVERY_SUCCEEDED == DuerLinkWrapper::getInstance()->getNetworkStatus())) {
        if (!m_dcsSdk) {
            return;
        }
        sdkInterfaces::SdkConnectionState m_sdkConnectionStates = m_dcsSdk->getSdkConnectionStates();
        APP_INFO("VoiceAndTouchWakeUpObserver  connectionState networkStatus:%d", m_sdkConnectionStates);
        if (m_sdkConnectionStates == sdkInterfaces::SdkConnectionState::SDK_CONNECT_SUCCEED) {
            SoundController::getInstance()->wakeUp();
            DeviceIoWrapper::getInstance()->ledNetOff();
            if (is_voice_wakeup) {
                //int target_dir = DeviceIoWrapper::getInstance()->fetchWakeupDirection();
                //DeviceIoWrapper::getInstance()->setDirection(target_dir);
                //DeviceIoWrapper::getInstance()->ledWakeUp(DeviceIoWrapper::getInstance()->getDirection());
                DeviceIoWrapper::getInstance()->ledWakeUp(DeviceIoWrapper::getInstance()->getDirection());
                if (m_dcsSdk) {
                    m_dcsSdk->notifyOfWakeWord(beginIndex, endIndex, keyword);
                }
            } /*else {
                DeviceIoWrapper::getInstance()->setDirection(-1);
                DeviceIoWrapper::getInstance()->ledWakeUp(DeviceIoWrapper::getInstance()->getDirection());
                if (m_dcsSdk) {
                    m_dcsSdk->notifyOfTapToTalk();
                }
            }*/
        } else {
            //网络故障状态下唤醒打点log
            if (is_voice_wakeup) {
                APP_INFO("Network unconnected when Voice wakeupd:%s", keyword.c_str());
            }

            if (m_sdkConnectionStates == sdkInterfaces::SdkConnectionState::SDK_AUTH_FAILED) {
                DeviceIoWrapper::getInstance()->setRecognizing(false);
                SoundController::getInstance()->accountUnbound(nullptr);
            } else if (m_sdkConnectionStates == sdkInterfaces::SdkConnectionState::SDK_CONNECT_FAILED) {
                DeviceIoWrapper::getInstance()->setRecognizing(false);
                SoundController::getInstance()->serverConnectFailed();
            } else if (m_sdkConnectionStates == sdkInterfaces::SdkConnectionState::SDK_CONNECTING) {
                DeviceIoWrapper::getInstance()->setRecognizing(false);
                SoundController::getInstance()->serverConnecting();
            }

        }

        DuerLinkWrapper::getInstance()->verifyNetworkStatus(true);
    } else {
        //网络故障状态下唤醒打点log
        if (is_voice_wakeup) {
            APP_INFO("Network unconnected when Voice wakeupd:%s", keyword.c_str());
        }

        DeviceIoWrapper::getInstance()->setRecognizing(false);
        if (DeviceIoWrapper::getInstance()->isAlertRing()) {
            DeviceIoWrapper::getInstance()->closeLocalActiveAlert();
        } else {
            DeviceIoWrapper::getInstance()->exitMute();
            DuerLinkWrapper::getInstance()->wakeupDuerLinkNetworkStatus();
        }
    }
}

void VoiceAndTouchWakeUpObserver::setDcsSdk(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) {
    m_dcsSdk = dcsSdk;
}

}  // namespace application
}  // namespace duerOSDcsApp
