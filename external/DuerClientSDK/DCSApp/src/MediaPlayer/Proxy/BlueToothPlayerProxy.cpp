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

#include "BlueToothPlayerProxy.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

std::shared_ptr<BlueToothPlayerProxy> BlueToothPlayerProxy::create() {
    APP_INFO("BlueToothPlayerProxy createCalled");
    std::shared_ptr<BlueToothPlayerProxy> bluetoothPlayer(new BlueToothPlayerProxy());

    if (!bluetoothPlayer) {
        APP_ERROR("BlueToothPlayerProxy Create Failed");
        return nullptr;
    } else {
        return bluetoothPlayer;
    }
}

BlueToothPlayerProxy::BlueToothPlayerProxy() : m_localMediaPlayerPlayActivity {STOPPED},
                     m_bluetoothImpl {nullptr},
m_dlnaImpl {nullptr} {
    APP_ERROR("BlueToothPlayerProxy Constructor");
}

BlueToothPlayerProxy::~BlueToothPlayerProxy() {
    APP_ERROR("BlueToothPlayerProxy Destructor");
}

void BlueToothPlayerProxy::setObserver(
    std::shared_ptr<LocalMediaPlayerObserverInterface> playerObserver) {
    if (playerObserver) {
        m_playerObserver = playerObserver;
    }
}

LocalMediaPlayerStatus BlueToothPlayerProxy::play(const LocalMediaPlayerName& name) {
    APP_INFO("BlueToothPlayerProxy play");
    m_localMediaPlayerPlayActivity = PLAYING;
    if (BLUETOOTH == name) {
        if (m_bluetoothImpl) {
            m_bluetoothImpl->play();
        }
    } else if (DLNA == name) {
        if (m_dlnaImpl) {
            m_dlnaImpl->play();
        }
    }
    return LocalMediaPlayerStatus::LOCAL_MEDIA_PLAYER_SUCCESS;
}

LocalMediaPlayerStatus BlueToothPlayerProxy::stop(const LocalMediaPlayerName& name) {
    APP_INFO("BlueToothPlayerProxy stop");
    m_localMediaPlayerPlayActivity = STOPPED;
    if (name == BLUETOOTH) {
        if (m_bluetoothImpl) {
            m_bluetoothImpl->stop();
        }
    } else if (DLNA == name) {
        APP_INFO("BlueToothPlayerProxy dlnastop");

        if (m_dlnaImpl) {
            m_dlnaImpl->stop();
        }
    }

    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;
        playInfo.m_playerActivity = m_localMediaPlayerPlayActivity;
        playInfo.m_status = BACKGROUND;
        playInfo.m_playerName = name;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    return LocalMediaPlayerStatus::LOCAL_MEDIA_PLAYER_SUCCESS;
}

LocalMediaPlayerStatus BlueToothPlayerProxy::pause(const LocalMediaPlayerName& name) {
    APP_INFO("BlueToothPlayerProxy pause");
    m_localMediaPlayerPlayActivity = PAUSED;
    if (BLUETOOTH == name) {
        if (m_bluetoothImpl) {
            m_bluetoothImpl->pause();
        }
    } else if (DLNA == name) {
        if (m_dlnaImpl) {
            m_dlnaImpl->pause();
        }
    }

    return LocalMediaPlayerStatus::LOCAL_MEDIA_PLAYER_SUCCESS;
}

LocalMediaPlayerStatus BlueToothPlayerProxy::resume(const LocalMediaPlayerName& name) {
    APP_INFO("BlueToothPlayerProxy resume");
    m_localMediaPlayerPlayActivity = PLAYING;
    if (BLUETOOTH == name) {
        if (m_bluetoothImpl) {
            m_bluetoothImpl->resume();
        }
    } else if (DLNA == name) {
        if (m_dlnaImpl) {
            m_dlnaImpl->resume();
        }
    }

    return LocalMediaPlayerStatus::LOCAL_MEDIA_PLAYER_SUCCESS;
}

LocalMediaPlayerStatus BlueToothPlayerProxy::playNext(const LocalMediaPlayerName& name) {
    APP_INFO("BlueToothPlayerProxy playNext");
    m_localMediaPlayerPlayActivity = PLAYING;
    if (BLUETOOTH == name) {
        if (m_bluetoothImpl) {
            m_bluetoothImpl->playNext();
        }
    } else if (DLNA == name) {
        if (m_dlnaImpl) {
            m_dlnaImpl->playNext();
        }
    }

    return LocalMediaPlayerStatus::LOCAL_MEDIA_PLAYER_SUCCESS;
}

LocalMediaPlayerStatus BlueToothPlayerProxy::playPrevious(const LocalMediaPlayerName& name) {
    APP_INFO("BlueToothPlayerProxy playPrevious");
    m_localMediaPlayerPlayActivity = PLAYING;
    if (BLUETOOTH == name) {
        if (m_bluetoothImpl) {
            m_bluetoothImpl->playPrevious();
        }
    } else if (DLNA == name) {
        if (m_dlnaImpl) {
            m_dlnaImpl->playPrevious();
        }
    }

    return LocalMediaPlayerStatus::LOCAL_MEDIA_PLAYER_SUCCESS;
}

void BlueToothPlayerProxy::dlnaStartPlay() {
    APP_INFO("BlueToothPlayerProxy dlnaStartPlay11111");
    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;
        playInfo.m_playerActivity = PLAYING;
        playInfo.m_status = FOREGROUND;
        playInfo.m_playerName = DLNA;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    if (m_dcsSdk) {
#ifdef KITTAI_KEY_WORD_DETECTOR
        m_dcsSdk->enterPlayMusicScene();
#endif
    }
}

void BlueToothPlayerProxy::dlnaStopPlay() {
    APP_INFO("BlueToothPlayerProxy dlnaStopPlay");
    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;

        if (PAUSED != m_localMediaPlayerPlayActivity) {
            m_localMediaPlayerPlayActivity = STOPPED;
        }

        playInfo.m_playerActivity = m_localMediaPlayerPlayActivity;
        playInfo.m_status = BACKGROUND;
        playInfo.m_playerName = DLNA;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    if (m_dcsSdk) {
#ifdef KITTAI_KEY_WORD_DETECTOR
        m_dcsSdk->exitPlayMusicScene();
#endif
    }
}

void BlueToothPlayerProxy::dlnaPausePlay() {
    APP_INFO("BlueToothPlayerProxy dlnaPausePlay");
    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;

        if (PAUSED != m_localMediaPlayerPlayActivity) {
            m_localMediaPlayerPlayActivity = STOPPED;
        }

        playInfo.m_playerActivity = m_localMediaPlayerPlayActivity;
        playInfo.m_status = BACKGROUND;
        playInfo.m_playerName = DLNA;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    if (m_dcsSdk) {
#ifdef KITTAI_KEY_WORD_DETECTOR
        m_dcsSdk->exitPlayMusicScene();
#endif
    }
}

void BlueToothPlayerProxy::btStartPlay() {
    APP_INFO("BlueToothPlayerProxy btStartPlay");
    application::DeviceIoWrapper::deviceioWrapperBtSoundPlayFinished();
    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;
        playInfo.m_playerActivity = PLAYING;
        playInfo.m_status = FOREGROUND;
        playInfo.m_playerName = BLUETOOTH;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    if (m_dcsSdk) {
#ifdef KITTAI_KEY_WORD_DETECTOR
        m_dcsSdk->enterPlayMusicScene();
#endif
    }
}

void BlueToothPlayerProxy::btStopPlay() {
    APP_INFO("BlueToothPlayerProxy btStopPlay");
    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;

        if (PAUSED != m_localMediaPlayerPlayActivity) {
            m_localMediaPlayerPlayActivity = STOPPED;
        }

        playInfo.m_playerActivity = m_localMediaPlayerPlayActivity;
        playInfo.m_status = BACKGROUND;
        playInfo.m_playerName = BLUETOOTH;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    if (m_dcsSdk) {
#ifdef KITTAI_KEY_WORD_DETECTOR
        m_dcsSdk->exitPlayMusicScene();
#endif
    }
}

void BlueToothPlayerProxy::btDisconnect() {
    APP_INFO("BlueToothPlayerProxy btDisconnect");

    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;
        m_localMediaPlayerPlayActivity = STOPPED;
        playInfo.m_playerActivity = m_localMediaPlayerPlayActivity;
        playInfo.m_status = CLOSED;
        playInfo.m_playerName = BLUETOOTH;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    if (m_dcsSdk) {
#ifdef KITTAI_KEY_WORD_DETECTOR
        m_dcsSdk->exitPlayMusicScene();
#endif
    }
}

void BlueToothPlayerProxy::btPowerOff() {
    APP_INFO("BlueToothPlayerProxy btPowerOff");

    if (m_playerObserver) {
        LocalMediaPlayerPlayInfo playInfo;
        m_localMediaPlayerPlayActivity = STOPPED;
        playInfo.m_playerActivity = m_localMediaPlayerPlayActivity;
        playInfo.m_status = CLOSED;
        playInfo.m_playerName = BLUETOOTH;
        playInfo.m_audioId = "";
        playInfo.m_title = "";
        playInfo.m_artist = "";
        playInfo.m_album = "";
        playInfo.m_year = "";
        playInfo.m_genre = "";
        m_playerObserver->setLocalMediaPlayerPlayInfo(playInfo);
    }

    if (m_dcsSdk) {
#ifdef KITTAI_KEY_WORD_DETECTOR
        m_dcsSdk->exitPlayMusicScene();
#endif
    }
}
void BlueToothPlayerProxy::setDcsSdk(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) {
    m_dcsSdk = dcsSdk;
}

void BlueToothPlayerProxy::setBluetoothImpl(std::shared_ptr<BlueToothPlayerInterface> impl) {
    m_bluetoothImpl = impl;
}

void BlueToothPlayerProxy::setDlnaImpl(std::shared_ptr<DlnaPlayerInterface> impl) {
    m_dlnaImpl = impl;
}

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp
