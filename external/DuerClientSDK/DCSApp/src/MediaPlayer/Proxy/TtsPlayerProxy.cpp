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

#include <cstring>
#include "TtsPlayerProxy.h"
#include "DCSApp/ActivityMonitorSingleton.h"
#include "LoggerUtils/DcsSdkLogger.h"

#include <malloc.h>

namespace duerOSDcsApp {
namespace mediaPlayer {

using namespace duerOSDcsApp::application;
using namespace duerOSDcsSDK::sdkInterfaces;
using application::ThreadPoolExecutor;


#define READ_TIMEOUT_MS 3000
#define RETRY_READ_TIMEOUT_MS 800
#define ATTACHMENT_READ_CHUNK_SIZE 512

#define FORMAT_MP3 "AUDIO_MPEG"
#define FORMAT_PCM "AUDIO_L16_RATE_16000_CHANNELS_1"

std::shared_ptr<TtsPlayerProxy> TtsPlayerProxy::create(const std::string& audio_device) {
    APP_INFO("createCalled");
    std::shared_ptr<TtsPlayerProxy> mediaPlayer(new TtsPlayerProxy(audio_device));

    if (mediaPlayer->init()) {
        return mediaPlayer;
    } else {
        return nullptr;
    }
};

TtsPlayerProxy::TtsPlayerProxy(const std::string& audio_device) :
    m_playerObserver {nullptr},
                 m_mp3Impl {nullptr},
                 m_pcmImpl {nullptr},
m_isFormatMp3 {true} {
    m_executor = ThreadPoolExecutor::getInstance();
}

bool TtsPlayerProxy::init() {
    return true;
}

TtsPlayerProxy::~TtsPlayerProxy() {
    APP_INFO("~TtsPlayerProxy");
    stop();
}

void TtsPlayerProxy::setMp3TtsImpl(std::shared_ptr<Mp3TtsPlayerInterface> impl) {
    m_mp3Impl = impl;
}

void TtsPlayerProxy::setPcmTtsImpl(std::shared_ptr<PcmTtsPlayerInterface> impl) {
    m_pcmImpl = impl;
}

void TtsPlayerProxy::setStreamFormat(const std::string& streamFormat) {
    m_isFormatMp3 = (streamFormat == FORMAT_MP3);
}

MediaPlayerStatus TtsPlayerProxy::setSource(
    std::shared_ptr<AttachmentReader> reader) {
    APP_INFO("setSourceCalled");
    return MediaPlayerStatus::SUCCESS;
}

void TtsPlayerProxy::setStream(std::shared_ptr<Stream> stream) {
    APP_INFO("setStream called ---------------");
    m_attachmentStream = stream;
}

MediaPlayerStatus TtsPlayerProxy::setSource(const std::string& audio_file_path, bool repeat) {
    APP_INFO("setSourceCalled");
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus TtsPlayerProxy::setSource(const std::string& url) {
    APP_INFO("setSourceForUrlCalled");
    return MediaPlayerStatus::FAILURE;
}

MediaPlayerStatus TtsPlayerProxy::play() {
    APP_INFO("playCalled");
    if (m_isFormatMp3) {
        m_executor->submit([this]() {
            executeMp3Play();
        });
    } else {
        m_executor->submit([this]() {
            executePcmPlay();
        });
    }

    return MediaPlayerStatus::SUCCESS;
}

void TtsPlayerProxy::executeMp3Play() {
    APP_INFO("executeMp3Play called ----------");
    m_mp3Impl->ttsPlay();
    char buffer[ATTACHMENT_READ_CHUNK_SIZE];
    APP_INFO("Going to read data from Attachment Stream.");
    bool first_packet_flag = false;

    while (true) {
        int size = 0;

        if (m_attachmentStream) {
            Stream::StreamStatus status = m_attachmentStream->readData(buffer, size);

            if (size > 0 && status == Stream::StreamStatus::BUFFER_DATA) {
                if (!first_packet_flag) {
                    first_packet_flag = true;
                    executeRecvFirstpacket();
                }

                m_mp3Impl->pushData(buffer, size);
            } else {
                APP_INFO("executePlay(), size === 0 -----------");
                APP_INFO("executePlay(), status ==== %d\n", (int)status);

                break;
            }
        } else {
            APP_INFO("AttachmentStream is NULL, break -----------");
            break;
        }
    }

    m_mp3Impl->ttsEnd();
    if (m_attachmentStream) {
        m_attachmentStream->close();
        m_attachmentStream.reset();
    }

    APP_INFO("Finish reading data from Attachment Stream ------------");
}

void TtsPlayerProxy::executePcmPlay() {
    APP_INFO("executePcmPlay called ----------");
    m_pcmImpl->ttsPlay();
    char buffer[ATTACHMENT_READ_CHUNK_SIZE];
    APP_INFO("Going to read data from Attachment Stream.");
    bool first_packet_flag = false;

    while (true) {
        int size = 0;

        if (m_attachmentStream) {
            Stream::StreamStatus status = m_attachmentStream->readData(buffer, size);

            if (size > 0 && status == Stream::StreamStatus::BUFFER_DATA) {
                if (!first_packet_flag) {
                    first_packet_flag = true;
                    executeRecvFirstpacket();
                }

                m_pcmImpl->pushData(buffer, size);
            } else {
                break;
            }
        } else {
            APP_INFO("AttachmentStream is NULL, break -----------");
            break;
        }
    }
    m_pcmImpl->ttsEnd();

    if (m_attachmentStream) {
        m_attachmentStream->close();
        m_attachmentStream.reset();
    }

    APP_INFO("Finish reading data from Attachment Stream ------------");
}

MediaPlayerStatus TtsPlayerProxy::stop() {
    APP_INFO("stopCalled");
    if (m_isFormatMp3) {
        if (m_mp3Impl) {
            m_mp3Impl->ttsStop();
        }
    } else {
        if (m_pcmImpl) {
            m_pcmImpl->ttsStop();
        }
    }

    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus TtsPlayerProxy::pause() {
    APP_INFO("pausedCalled");
    if (m_isFormatMp3) {
        if (m_mp3Impl) {
            m_mp3Impl->ttsStop();
        }
    } else {
        if (m_pcmImpl) {
            m_pcmImpl->ttsStop();
        }
    }

    return MediaPlayerStatus::SUCCESS;
}

MediaPlayerStatus TtsPlayerProxy::resume() {
    return MediaPlayerStatus::SUCCESS;
}

std::chrono::milliseconds TtsPlayerProxy::getOffset() {
    return std::chrono::milliseconds(0);
}

void TtsPlayerProxy::setObserver(std::shared_ptr<MediaPlayerObserverInterface> observer) {
    APP_INFO("setObserverCalled");

    if (observer) {
        m_playerObserver = observer;
    }
}

void TtsPlayerProxy::handleSetAttachmentReaderSource(
    std::shared_ptr<AttachmentReader> reader) {
    APP_INFO("handleSetSourceCalled");
}

void TtsPlayerProxy::executeRecvFirstpacket() {
    APP_INFO("callingexecuteRecvFirstpacket");

    if (m_playerObserver) {
        m_playerObserver->onRecvFirstpacket();
    }
}

void TtsPlayerProxy::playbackStarted() {
    APP_INFO("callingOnPlaybackStarted");

    if (m_playerObserver) {
        m_playerObserver->onPlaybackStarted();
    }
    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_ON);
}

void TtsPlayerProxy::playbackStopped(StopStatus stopStatus) {
    APP_INFO("report PlaybackFinished");

    if (m_playerObserver) {
        m_playerObserver->onPlaybackFinished();
    }

    ActivityMonitorSingleton::getInstance()->updatePlayerStatus(PLAYER_STATUS_OFF);
}

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp
