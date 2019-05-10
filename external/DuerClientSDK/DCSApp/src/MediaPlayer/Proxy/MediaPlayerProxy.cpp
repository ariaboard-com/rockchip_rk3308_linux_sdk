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

#include <DeviceTools/TimeUtils.h>
#include "DCSApp/DeviceIoWrapper.h"
#include "DCSApp/SoundController.h"
#include "DCSApp/DuerLinkWrapper.h"
#include "DCSApp/ActivityMonitorSingleton.h"
#include "MediaPlayerProxy.h"
#include "LoggerUtils/DcsSdkLogger.h"
#include <malloc.h>

namespace duerOSDcsApp {
namespace mediaPlayer {

#define FORMAT_MP3 "AUDIO_MPEG"
#define NETWORK_SLOW_REPORT_INTERVAL_MS 3600000L

using namespace duerOSDcsSDK::sdkInterfaces;
using namespace duerOSDcsApp::application;

std::shared_ptr<MediaPlayerProxy> MediaPlayerProxy::create(const std::string& audio_device) {
    APP_INFO("createCalled");
    std::shared_ptr<MediaPlayerProxy> mediaPlayer(new MediaPlayerProxy(audio_device));

    if (mediaPlayer->init()) {
        return mediaPlayer;
    } else {
        return nullptr;
    }
}

bool MediaPlayerProxy::init() {
    return true;
}

void MediaPlayerProxy::setImpl(std::shared_ptr<Mp3UrlPlayerInterface> impl) {
    m_impl = impl;
}

MediaPlayerProxy::MediaPlayerProxy(const std::string& audio_device) : m_offset_ms(0L),
    m_latestNetworkStatusTimestamp(0L) {

}

MediaPlayerProxy::~MediaPlayerProxy() {
    APP_INFO("~MediaPlayerProxy");
}

void MediaPlayerProxy::setStreamFormat(const std::string& streamFormat) {
}

MediaPlayerStatus MediaPlayerProxy::setSource(
    std::shared_ptr<AttachmentReader> attachmentReader) {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus MediaPlayerProxy::setSource(const std::string& url) {
    APP_INFO("setSourceCalled");

    if (url.empty()) {
        APP_ERROR("setSource url is empty.");
        return MediaPlayerStatus::FAILURE;
    } else {
        m_resource_url = url;
        return MediaPlayerStatus::SUCCESS;
    }
}

MediaPlayerStatus MediaPlayerProxy::setSource(const std::string& audio_file_path,
        bool repeat) {
    return MediaPlayerStatus::FAILURE;
}

void MediaPlayerProxy::setObserver(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> playerObserver) {
    if (playerObserver) {
        m_playerObserver = playerObserver;
    }
}

MediaPlayerStatus MediaPlayerProxy::setOffset(std::chrono::milliseconds offset) {
    m_offset_ms = offset.count();
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus MediaPlayerProxy::play() {
    APP_INFO("playCalled");
    if (m_impl) {
        APP_INFO("mp3 player");
        m_impl->audioPlay(m_resource_url,
                          m_offset_ms,
                          15000);
        return MediaPlayerStatus::SUCCESS;
    } else {
        APP_ERROR("play null ptr.");
        return MediaPlayerStatus::FAILURE;
    }
}

MediaPlayerStatus MediaPlayerProxy::stop() {
    APP_INFO("stopCalled");
    if (m_impl) {
        m_impl->audioStop();
    }

    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus MediaPlayerProxy::pause() {
    APP_INFO("pauseCalled");
    if (m_impl) {
        m_impl->audioPause();
    }

    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus MediaPlayerProxy::resume() {
    APP_INFO("resumeCalled");
    if (m_impl) {
        m_impl->audioResume();
    }

    return MediaPlayerStatus::SUCCESS;
}

std::chrono::milliseconds MediaPlayerProxy::getOffset() {
    long offset_ms = 0L;

    if (m_impl) {
        offset_ms = m_impl->getProgress();
    }

    return std::chrono::milliseconds(offset_ms);
}

MediaPlayerStatus MediaPlayerProxy::getPosition(long* track_duration,
        long* track_pos) {
    if (m_impl) {
        *track_pos = m_impl->getProgress() * 1000000;
        *track_duration = m_impl->getDuration() * 1000000;
    }

    //APP_INFO("duration:%ld, position:%ld", *track_duration, *track_pos);
    return MediaPlayerStatus::SUCCESS;
}

float MediaPlayerProxy::getFactor() {
    APP_INFO("MediaPlayerProxy::getFactor");

    if (m_impl) {
        return m_impl->getFactor();
    }

    return 0;
}
MediaPlayerStatus MediaPlayerProxy::setFactor(float factor) {
    if (m_impl) {
        m_impl->setFactor(factor);
    }

    return MediaPlayerStatus::SUCCESS;
}

void MediaPlayerProxy::playbackStarted(int offset_ms) {
    m_offset_ms = 0L;
    APP_INFO("playback_startedCalled");
    executePlaybackStart();
}

void MediaPlayerProxy::playbackBufferunderrun(PlayProgressInfo playProgressInfo) {
    APP_INFO("playback buffer underrun");
    executePlaybackOnBufferUnderrun(playProgressInfo);
}

void MediaPlayerProxy::playbackBufferefilled() {
    APP_INFO("playback buffer refilled");
    executePlaybackOnBufferRefilled();
}

void MediaPlayerProxy::playbackStopped(int offset_ms) {
    APP_INFO("playback_stoppedCalled");
    executePlaybackStop();
}

void MediaPlayerProxy::playbackPaused(int offset_ms) {
    APP_INFO("playback_pausedCalled");
    executePlaybackPause();
}

void MediaPlayerProxy::playbackResumed(int offset_ms) {
    APP_INFO("playback_resumeCalled");
    executePlaybackResume();
}

void MediaPlayerProxy::playbackNearlyFinished(int offset_ms) {
    APP_INFO("playback_nearfinishedCalled");
    executePlaybackNearlyFinish();
}

void MediaPlayerProxy::playbackFinished(int offset_ms, AudioPlayerFinishStatus status) {
    m_offset_ms = 0L;
    APP_INFO("playback_finishedCalled");
    executePlaybackFinish();
}

void MediaPlayerProxy::playbackProgress(int offset_ms) {
}

void MediaPlayerProxy::playbackError() {
    APP_INFO("playback_errorCalled");
    executePlaybackError();
}

void MediaPlayerProxy::reportBufferTime(long time) {
    APP_INFO("playback_bufferCalled");
}

void MediaPlayerProxy::playbackStreamUnreach() {
    SoundController::getInstance()->networkConnectFailed();
    DeviceIoWrapper::getInstance()->ledNetConnectFailed();
}

bool MediaPlayerProxy::queryIsSeekable(bool* isSeekable) {
    if (m_impl) {
        return (m_impl->getDuration() > 0);
    } else {
        return false;
    }
}

bool MediaPlayerProxy::seek(unsigned int positionMs) {
    if (m_impl) {
        if (m_impl->seekTo(positionMs)) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

void MediaPlayerProxy::executePlaybackStart() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackStarted();
    }
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_ON);
}

void MediaPlayerProxy::executeUnderBuffer() {
    if (m_playerObserver) {
        m_playerObserver->onBufferUnderrun();
    }
}

void MediaPlayerProxy::executePlaybackStop() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackStopped();
    }
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_OFF);
}

void MediaPlayerProxy::executePlaybackPause() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackPaused();
    }

    ///@
    /// malloc_trim(0);
}

void MediaPlayerProxy::executePlaybackResume() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackResumed();
    }
}

void MediaPlayerProxy::executePlaybackNearlyFinish() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackNearlyfinished();
    }
}

void MediaPlayerProxy::executePlaybackFinish() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackFinished();
    }

    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_OFF);
    ///@
    ///malloc_trim(0);
}

void MediaPlayerProxy::executePlaybackError() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackError(
            duerOSDcsSDK::sdkInterfaces::ErrorType::MEDIA_ERROR_INVALID_REQUEST, "404");
    }

    ///@
    ///malloc_trim(0);
}

void MediaPlayerProxy::executePlaybackOnBufferUnderrun(PlayProgressInfo playProgressInfo) {
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_OFF);

    if (m_playerObserver) {
        m_playerObserver->onBufferUnderrun();
    }

#if defined (MTK8516) || defined (Rk3308)
    if (DuerLinkWrapper::getInstance()->getNetworkStatus() == DUERLINK_NETWORK_FAILED) {
        SoundController::getInstance()->networkConnectFailed();
        return;
    }

    if (playProgressInfo == PlayProgressInfo::DURING_PLAY) {
        int64_t currentTime = deviceCommonLib::deviceTools::currentTimeMs();

        if (currentTime - m_latestNetworkStatusTimestamp < NETWORK_SLOW_REPORT_INTERVAL_MS) {
            return;
        }

        m_latestNetworkStatusTimestamp = currentTime;
        SoundController::getInstance()->networkSlow();
    }

#endif

#if (defined Ingenic)

    if (playProgressInfo == PlayProgressInfo::DURING_PLAY) {
        SoundController::getInstance()->networkSlow();
    }

#endif
}

void MediaPlayerProxy::executePlaybackOnBufferRefilled() {
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_ON);

    if (m_playerObserver) {
        m_playerObserver->onBufferRefilled();
    }
}

}  // mediaPlayer
}  // duerOSDcsApp
