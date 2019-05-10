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

#include "LocalPlayerProxy.h"
#include "DCSApp/ActivityMonitorSingleton.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

using namespace duerOSDcsSDK::sdkInterfaces;
using namespace duerOSDcsApp::application;

std::shared_ptr<LocalPlayerProxy> LocalPlayerProxy::create(const std::string& audio_device) {
    APP_INFO("createCalled");
    std::shared_ptr<LocalPlayerProxy> mediaPlayer(new LocalPlayerProxy(audio_device));

    if (mediaPlayer->init()) {
        return mediaPlayer;
    } else {
        return nullptr;
    }
}

bool LocalPlayerProxy::init() {
    return true;
}

void LocalPlayerProxy::setImpl(std::shared_ptr<Mp3FilePlayerInterface> impl) {
    m_impl = impl;
}

LocalPlayerProxy::LocalPlayerProxy(const std::string& audio_device) : m_resPath(""),
    m_playerObserver {nullptr},
                 m_localPlayer {nullptr},
                 m_playStartFunc {nullptr},
                 m_playFinishFunc {nullptr},
                 m_argPtr {nullptr},
m_impl {nullptr} {

}

LocalPlayerProxy::~LocalPlayerProxy() {
    APP_INFO("~AlertsPlayerProxy");
}

void LocalPlayerProxy::playLocalSource(const std::string& source,
                                       bool needFocus,
                                       void (*start_callback)(void* arg),
                                       void* start_cb_arg,
                                       void (*finish_callback)()) {
    m_argPtr = start_cb_arg;
    m_playStartFunc = start_callback;
    m_playFinishFunc = finish_callback;

    if (m_localPlayer) {
        m_localPlayer->playLocalSource(source, needFocus, start_callback, start_cb_arg, finish_callback);
    }
}

void LocalPlayerProxy::playTts(const std::string& content, bool needFocus, void (*callback)()) {
}

MediaPlayerStatus LocalPlayerProxy::setSource(
    std::shared_ptr<AttachmentReader> attachmentReader) {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus LocalPlayerProxy::setSource(const std::string& url) {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus LocalPlayerProxy::setSource(const std::string& audio_source,
        bool repeat) {
    m_resPath = audio_source;
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus LocalPlayerProxy::setOffset(std::chrono::milliseconds offset) {
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus LocalPlayerProxy::play() {
    if (m_resPath.empty()) {
        return MediaPlayerStatus::FAILURE;
    } else {
        m_impl->audioPlay(m_resPath.c_str(), 0, 15000);
        return MediaPlayerStatus::SUCCESS;
    }
}

MediaPlayerStatus LocalPlayerProxy::stop() {
    m_impl->audioStop();
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus LocalPlayerProxy::pause() {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus LocalPlayerProxy::resume() {
    return MediaPlayerStatus::FAILURE;
}

std::chrono::milliseconds LocalPlayerProxy::getOffset() {
    return std::chrono::milliseconds(0);
}

void LocalPlayerProxy::setObserver(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> playerObserver) {
    if (playerObserver) {
        m_playerObserver = playerObserver;
    }
}

void LocalPlayerProxy::setLocalPlayObserver(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::LocalSourcePlayerInterface> playerObserver) {
    if (playerObserver) {
        m_localPlayer = playerObserver;
    }
}

void LocalPlayerProxy::playbackStarted(int offset_ms) {
    if (m_playStartFunc) {
        m_playStartFunc(m_argPtr);
    }

    if (m_playerObserver) {
        APP_INFO("LocalPlayerProxy playback_started");
        m_playerObserver->onPlaybackStarted();
    }
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_ON);
}

void LocalPlayerProxy::playbackStopped(int offset_ms) {
}

void LocalPlayerProxy::playbackPaused(int offset_ms) {
}

void LocalPlayerProxy::playbackResumed(int offset_ms) {
}

void LocalPlayerProxy::playbackNearlyFinished(int offset_ms) {
}

void LocalPlayerProxy::playbackFinished(int offset_ms, AudioPlayerFinishStatus status) {
    if (status == AUDIOPLAYER_FINISHSTATUS_END && m_playFinishFunc) {
        m_playFinishFunc();
    }

    if (m_playerObserver) {
        APP_INFO("LocalPlayerProxy playback_finished");
        m_playerObserver->onPlaybackFinished();
    }

    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_OFF);
}

void LocalPlayerProxy::playbackProgress(int offset_ms) {
}

void LocalPlayerProxy::playbackStreamUnreach() {
}

void LocalPlayerProxy::playbackError() {
}

void LocalPlayerProxy::reportBufferTime(long time) {
}

}  // mediaPlayer
}  // duerOSDcsApp


