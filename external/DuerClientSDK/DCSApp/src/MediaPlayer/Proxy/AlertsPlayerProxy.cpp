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

#include "AlertsPlayerProxy.h"
#include "DCSApp/ActivityMonitorSingleton.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

using namespace duerOSDcsApp::application;
using namespace duerOSDcsSDK::sdkInterfaces;

std::shared_ptr<AlertsPlayerProxy> AlertsPlayerProxy::create(const std::string& audio_device) {
    APP_INFO("createCalled");
    std::shared_ptr<AlertsPlayerProxy> mediaPlayer(new AlertsPlayerProxy(audio_device));

    if (mediaPlayer->init()) {
        return mediaPlayer;
    } else {
        return nullptr;
    }
}

bool AlertsPlayerProxy::init() {
    return true;
}

AlertsPlayerProxy::AlertsPlayerProxy(const std::string& audio_device) : m_res_path(""),
    m_playerObserver {nullptr},
m_impl {nullptr} {

}

AlertsPlayerProxy::~AlertsPlayerProxy() {
    APP_INFO("~AlertsPlayerProxy");
}

void AlertsPlayerProxy::setImpl(std::shared_ptr<Mp3FilePlayerInterface> impl) {
    m_impl = impl;
}

MediaPlayerStatus AlertsPlayerProxy::setSource(
    std::shared_ptr<AttachmentReader> attachmentReader) {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus AlertsPlayerProxy::setSource(const std::string& url) {
    m_res_path = url;
    m_impl->setPlayMode(PLAYMODE_NORMAL, 0);
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus AlertsPlayerProxy::setSource(const std::string& audio_file_path,
        bool repeat) {
    m_res_path = audio_file_path;
    m_impl->setPlayMode(PLAYMODE_LOOP, 0);
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus AlertsPlayerProxy::setOffset(std::chrono::milliseconds offset) {
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus AlertsPlayerProxy::play() {
    m_impl->audioPlay(m_res_path.c_str(), 0, 15000);
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus AlertsPlayerProxy::stop() {
    m_impl->audioStop();
    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus AlertsPlayerProxy::pause() {
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus AlertsPlayerProxy::resume() {
    return MediaPlayerStatus::FAILURE;
}

std::chrono::milliseconds AlertsPlayerProxy::getOffset() {
    return std::chrono::milliseconds(0);
}

void AlertsPlayerProxy::setObserver(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> playerObserver) {
    if (playerObserver) {
        m_playerObserver = playerObserver;
    }
}

void AlertsPlayerProxy::setFadeIn(int time_sec) {
    APP_INFO("setFadeIn");

    if (m_impl) {
        m_impl->setFadeIn(time_sec);
    }

    return;
}

void AlertsPlayerProxy::playbackStarted(int offset_ms) {
    if (m_playerObserver) {
        m_playerObserver->onPlaybackStarted();
    }
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_ON);
}

void AlertsPlayerProxy::playbackStopped(int offset_ms) {
}

void AlertsPlayerProxy::playbackPaused(int offset_ms) {
}

void AlertsPlayerProxy::playbackResumed(int offset_ms) {
}

void AlertsPlayerProxy::playbackNearlyFinished(int offset_ms) {
}

void AlertsPlayerProxy::playbackFinished(int offset_ms, AudioPlayerFinishStatus status) {
    //    if (status == bduer::AUDIOPLAYER_FINISHSTATUS_STOP) {
    //        return;
    //    }
    if (m_playerObserver) {
        m_playerObserver->onPlaybackFinished();
    }
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_OFF);
}

void AlertsPlayerProxy::playbackProgress(int offset_ms) {
}

void AlertsPlayerProxy::playbackStreamUnreach() {
}

void AlertsPlayerProxy::playbackError() {
}

void AlertsPlayerProxy::reportBufferTime(long time) {
}

}  // mediaPlayer
}  // duerOSDcsApp
