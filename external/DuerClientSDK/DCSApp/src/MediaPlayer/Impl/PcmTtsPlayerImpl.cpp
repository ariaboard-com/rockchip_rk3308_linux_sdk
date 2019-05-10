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

#include <unistd.h>
#include <LoggerUtils/DcsSdkLogger.h>
#include "PcmTtsPlayerImpl.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
using application::ThreadPoolExecutor;

#define PCM_BUFFER_LEN 256000
#define WAIT_TIME_INTERVAL_MS 100000
#define PLAY_INCREMENT_LEN 2048

std::shared_ptr<PcmTtsPlayerImpl> PcmTtsPlayerImpl::create(const std::string& audio_dev) {
    std::shared_ptr<PcmTtsPlayerImpl> instance(new PcmTtsPlayerImpl(audio_dev));
    return instance;
}

PcmTtsPlayerImpl::PcmTtsPlayerImpl(const std::string& audio_dev) : m_executor(NULL),
    m_alsaCtl(NULL),
    m_pcmBuffer(NULL),
    m_currentLen(0L),
    m_spaceLen(PCM_BUFFER_LEN),
    m_pushingFlag(false),
    m_stopFlag(false),
    m_isFirstPacket(true),
    m_logCtlFlag(true),
    m_aliveFlag(true) {
    m_pcmBuffer = (char*)malloc(PCM_BUFFER_LEN);
    m_executor = ThreadPoolExecutor::getInstance();
    m_alsaCtl = new AlsaController(audio_dev);
    m_alsaCtl->init(16000, 1);
    pthread_mutex_init(&m_playMutex, NULL);
    pthread_cond_init(&m_playCond, NULL);
    pthread_mutex_init(&m_memMutex, NULL);

    pthread_create(&m_playThread, NULL, playFunc, this);
}

PcmTtsPlayerImpl::~PcmTtsPlayerImpl() {
    if (m_pcmBuffer) {
        free(m_pcmBuffer);
        m_pcmBuffer = NULL;
    }

    m_aliveFlag = false;
    pthread_cond_signal(&m_playCond);

    void* play_thread_return = NULL;

    if (m_playThread != 0) {
        pthread_join(m_playThread, &play_thread_return);
    }

    m_streamPool.clearItems();

    m_alsaCtl->aslaAbort();
    m_alsaCtl->alsaClose();

    if (m_alsaCtl) {
        delete m_alsaCtl;
        m_alsaCtl = NULL;
    }

    pthread_mutex_destroy(&m_playMutex);
    pthread_cond_destroy(&m_playCond);
    pthread_mutex_destroy(&m_memMutex);
}

void PcmTtsPlayerImpl::registerListener(std::shared_ptr<TtsPlayerListener> listener) {
    m_listeners.push_back(listener);
}

void PcmTtsPlayerImpl::ttsPlay() {
    m_alsaCtl->alsaPrepare();
    m_isFirstPacket = true;
    m_stopFlag = false;
    pthread_mutex_lock(&m_playMutex);
    m_pushingFlag = true;
    pthread_cond_signal(&m_playCond);
    pthread_mutex_unlock(&m_playMutex);
}

void PcmTtsPlayerImpl::pushData(const char* data, unsigned int len) {
    if (!m_pushingFlag) {
        return;
    }

    if (m_currentLen + len > m_spaceLen) {
        int incre_len = PCM_BUFFER_LEN / 4;
        char* tmp = (char*)malloc(m_spaceLen + incre_len);
        memcpy(tmp, m_pcmBuffer, m_currentLen);
        pthread_mutex_lock(&m_memMutex);
        free(m_pcmBuffer);
        m_pcmBuffer = tmp;
        pthread_mutex_unlock(&m_memMutex);
        m_spaceLen += incre_len;
    }

    memcpy(m_pcmBuffer + m_currentLen, data, len);
    m_currentLen += len;
    m_logCtlFlag = !m_logCtlFlag;

    if (m_logCtlFlag) {
        APP_INFO("push data(%d) to PcmStreamPlayer.", len);
    }
}

void PcmTtsPlayerImpl::ttsEnd() {
    m_pushingFlag = false;
}

void PcmTtsPlayerImpl::ttsStop() {
    m_alsaCtl->aslaAbort();
    m_alsaCtl->alsaClear();
    m_pushingFlag = false;
    m_stopFlag = true;
    executePlaybackStopped();

}

void PcmTtsPlayerImpl::executePlaybackStarted() {
    m_executor->submit([this]() {
        size_t size = m_listeners.size();

        for (size_t i = 0; i < size; ++i) {
            if (NULL != m_listeners[i]) {
                m_listeners[i]->playbackStarted();
            }
        }
    });
}

void PcmTtsPlayerImpl::executePlaybackStopped() {
    size_t size = m_listeners.size();

    for (size_t i = 0; i < size; ++i) {
        if (NULL != m_listeners[i]) {
            m_listeners[i]->playbackStopped(STOP_STATUS_INTERRUPT);
        }
    }
}

void* PcmTtsPlayerImpl::playFunc(void* arg) {
    PcmTtsPlayerImpl* player = (PcmTtsPlayerImpl*) arg;

    while (player->m_aliveFlag) {
        pthread_mutex_lock(&player->m_playMutex);
        pthread_cond_wait(&player->m_playCond, &player->m_playMutex);
        player->m_alsaCtl->alsaPrepare();

        if (!player->m_aliveFlag) {
            break;
        }

        if (!player->m_aliveFlag) {
            break;
        }

        int playLen = 0;

        while (!player->m_stopFlag) {
            if (playLen + PLAY_INCREMENT_LEN < (int)player->m_currentLen) {
                pthread_mutex_lock(&player->m_memMutex);
                player->play_stream(1, player->m_pcmBuffer + playLen, PLAY_INCREMENT_LEN);
                pthread_mutex_unlock(&player->m_memMutex);
                playLen += PLAY_INCREMENT_LEN;
            } else if (player->m_pushingFlag) {
                usleep(WAIT_TIME_INTERVAL_MS);
            } else {
                pthread_mutex_lock(&player->m_memMutex);
                player->play_stream(1, player->m_pcmBuffer + playLen, player->m_currentLen - playLen);
                pthread_mutex_unlock(&player->m_memMutex);
                break;
            }
        }

        if (!player->m_stopFlag) {
            player->executePlaybackStopped();
        }

        memset(player->m_pcmBuffer, 0, player->m_spaceLen);
        player->m_currentLen = 0;
        pthread_mutex_unlock(&player->m_playMutex);
    }

    return NULL;
}

void PcmTtsPlayerImpl::play_stream(unsigned int channels,
                                   const void* buffer,
                                   unsigned int buff_size) {
    if (m_isFirstPacket) {
        executePlaybackStarted();
        m_isFirstPacket = false;
    }

    m_alsaCtl->writeStream(channels, buffer, buff_size);
}

}  // mediaPlayer
}  // duerOSDcsApp