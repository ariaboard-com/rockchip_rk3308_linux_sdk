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

#include "LocalTtsProxy.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

using namespace duerOSDcsSDK::sdkInterfaces;

std::shared_ptr<LocalTtsProxy> LocalTtsProxy::create(const std::string& audio_device) {
    APP_INFO("createCalled");
    std::shared_ptr<LocalTtsProxy> mediaPlayer(new LocalTtsProxy(audio_device));

    if (mediaPlayer->init()) {
        return mediaPlayer;
    } else {
        return nullptr;
    }
}

bool LocalTtsProxy::init() {
    return true;
}

void LocalTtsProxy::setImpl(std::shared_ptr<LocalTtsPlayerInterface> impl) {
    m_impl = impl;
}

LocalTtsProxy::LocalTtsProxy(const std::string& audio_device) : m_tts_content(""),
    m_impl {nullptr},
       m_playerObserver {nullptr},
m_local_source_player {nullptr} {
}

LocalTtsProxy::~LocalTtsProxy() {
    APP_INFO("~AlertsPlayerProxy");
}

void LocalTtsProxy::playLocalSource(const std::string& source,
                                    bool needFocus,
                                    void (*start_callback)(void* arg),
                                    void* start_cb_arg,
                                    void (*finish_callback)()) {
    if (m_local_source_player) {
        m_local_source_player->playLocalSource(source, needFocus, start_callback, start_cb_arg,
                                               finish_callback);
    }
}

void LocalTtsProxy::playTts(const std::string& content, bool needFocus, void (*callback)()) {
    if (m_local_source_player) {
        m_local_source_player->playTts(content, needFocus, callback);
    }
}

MediaPlayerStatus LocalTtsProxy::setSource(
    std::shared_ptr<AttachmentReader> attachmentReader) {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus LocalTtsProxy::setSource(const std::string& url) {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus LocalTtsProxy::setSource(const std::string& tts_content,
        bool repeat) {
    m_tts_content = tts_content;
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus LocalTtsProxy::setOffset(std::chrono::milliseconds offset) {
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus LocalTtsProxy::play() {
    if (!m_tts_content.empty()) {
        m_impl->ttsPlay(m_tts_content);
        return MediaPlayerStatus::SUCCESS;
    } else {
        return MediaPlayerStatus::FAILURE;
    }
}

MediaPlayerStatus LocalTtsProxy::stop() {
    m_impl->ttsStop();
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus LocalTtsProxy::pause() {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus LocalTtsProxy::resume() {
    return MediaPlayerStatus::FAILURE;
}

std::chrono::milliseconds LocalTtsProxy::getOffset() {
    return std::chrono::milliseconds(0);
}

void LocalTtsProxy::setObserver(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> playerObserver) {
    if (playerObserver) {
        m_playerObserver = playerObserver;
    }
}

void LocalTtsProxy::setLocalPlayObserver(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::LocalSourcePlayerInterface> playerObserver) {
    if (playerObserver) {
        m_local_source_player = playerObserver;
    }
}

void LocalTtsProxy::speechStarted() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackStarted();
    }
}

void LocalTtsProxy::speechFinished() {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackFinished();
    }
}

}  // mediaPlayer
}  // duerOSDcsApp