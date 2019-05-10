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

#include "LocalTtsPlayerImpl.h"
#include "DCSApp/Configuration.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
#define TTS_PCM_RATE     48000
#define TTS_PCM_CHANNEL  1

std::shared_ptr<LocalTtsPlayerImpl> LocalTtsPlayerImpl::create(const std::string& audio_dev,
        const std::string& res_path) {
    std::shared_ptr<LocalTtsPlayerImpl> player(new LocalTtsPlayerImpl(audio_dev, res_path));
    return player;
}

LocalTtsPlayerImpl::LocalTtsPlayerImpl(const std::string& audio_dev,
                                       const std::string& synthesize_res_path): m_executor(NULL),
                                                                                m_ttsMgr(NULL),
                                                                                m_alsaCtl(NULL),
                                                                                m_lock(NULL),
                                                                                m_status(TtsPlayerStatusIdle),
                                                                                m_firstPackFlag(false),
                                                                                m_threadAlive(true),
                                                                                m_pushStreamFlag(false) {
    m_executor = ThreadPoolExecutor::getInstance();
    TTSManager::loadTtsEngine(synthesize_res_path);
    m_ttsMgr = TTSManager::getTtsManagerInstance();
    m_ttsMgr->setTtsSynthesizeResultObserver(this);
    m_alsaCtl = new AlsaController(audio_dev);
    m_alsaCtl->init(TTS_PCM_RATE, TTS_PCM_CHANNEL);
    m_lock = new PthreadLock();
    pthread_mutex_init(&m_playMutex, NULL);
    pthread_cond_init(&m_playCond, NULL);
    m_maxValidLen = 4294967295 / 3;

    pthread_create(&m_playThread, NULL, playFunc, this);
}

LocalTtsPlayerImpl::~LocalTtsPlayerImpl() {
    m_threadAlive = false;
    pthread_cond_signal(&m_playCond);
    void* play_thread_return = NULL;

    if (m_playThread != 0) {
        pthread_join(m_playThread, &play_thread_return);
    }

    if (m_lock) {
        delete m_lock;
        m_lock = NULL;
    }
    pthread_mutex_destroy(&m_playMutex);
    pthread_cond_destroy(&m_playCond);
    m_alsaCtl->alsaClear();
    m_alsaCtl->alsaClose();
}

void LocalTtsPlayerImpl::registerListener(std::shared_ptr<LocalTtsPlayerListener> notify) {
    if (notify) {
        m_listeners.push_back(notify);
    }
}

void LocalTtsPlayerImpl::ttsPlay(const std::string& content) {
    m_ttsContent = content;
    m_executor->submit([this]() {
        if (m_alsaCtl->isAccessable()) {
            setStatus(TtsPlayerStatusPlay);
            m_alsaCtl->alsaClear();
            m_alsaCtl->alsaPrepare();
            m_ttsMgr->synthesizeTextToTts(m_ttsContent);
        }
    });
}

void LocalTtsPlayerImpl::ttsStop() {
    if (m_status != TtsPlayerStatusPlay) {
        return;;
    }

    m_executor->submit([this]() {
        setStatus(TtsPlayerStatusStop);
        m_ttsMgr->stopSynthesize();
        m_alsaCtl->alsaClear();
        m_streamPool.clearItems();
    });
}

void* LocalTtsPlayerImpl::playFunc(void* arg) {
    LocalTtsPlayerImpl* tts_player = (LocalTtsPlayerImpl*)arg;

    while (tts_player->m_threadAlive) {
        pthread_mutex_lock(&tts_player->m_playMutex);

        if (tts_player->m_status != TtsPlayerStatusPlay) {
            pthread_cond_wait(&tts_player->m_playCond, &tts_player->m_playMutex);

            if (!tts_player->m_threadAlive) {
                return NULL;
            }
        }

        while (tts_player->m_status != TtsPlayerStatusStop) {
            StreamItem* item = tts_player->m_streamPool.popItem();

            if (item != NULL) {
                tts_player->m_alsaCtl->writeStream(TTS_PCM_CHANNEL,
                                                   item->getData(),
                                                   item->getSize());
                delete item;
            } else {
                if (tts_player->m_pushStreamFlag) {
                    continue;
                } else {
                    break;
                }
            }
        }

        usleep(400000);
        tts_player->m_alsaCtl->alsaClear();
        tts_player->setStatus(TtsPlayerStatusStop);
        tts_player->executeSpeechFinished();
        pthread_mutex_unlock(&tts_player->m_playMutex);
    }

    return NULL;
}

void LocalTtsPlayerImpl::ttsSynthesizeBegin(const std::string& strIndex) {
    if (getStatus() != TtsPlayerStatusPlay) {
        return;
    }

    m_firstPackFlag = true;
    m_pushStreamFlag = true;
    pthread_cond_signal(&m_playCond);
    executeSpeechStarted();
}

void LocalTtsPlayerImpl::ttsSynthesizeAudioData(const char* stream,
        unsigned int length,
        unsigned int charIndex,
        const std::string& strIndex) {
    if (getStatus() != TtsPlayerStatusPlay) {
        return;
    }

    if (m_firstPackFlag) {
        m_firstPackFlag = false;
    }

    if (length > m_maxValidLen) {
    } else {
        unsigned long resample_len = 3 * length;
        uint8_t* resample_data = (uint8_t*)malloc(resample_len);
        PcmResampler::getInstance()->pcmResample(&resample_data, resample_len / 2,
                (const uint8_t**)(&stream), length / 2);
        m_streamPool.pushStream((char*)resample_data, resample_len);
        free(resample_data);
    }
}

void LocalTtsPlayerImpl::ttsSynthesizeFinish(const std::string& strIndex) {
    if (getStatus() != TtsPlayerStatusPlay) {
        return;
    }

    m_pushStreamFlag = false;
}

void LocalTtsPlayerImpl::ttsSynthesizeFailed(int errCode,
        const std::string& errDesc,
        const std::string& strIndex) {
}

void LocalTtsPlayerImpl::setStatus(TtsPlayerStatus status) {
    AutoLock autoLock(m_lock);
    m_status = status;
}

TtsPlayerStatus LocalTtsPlayerImpl::getStatus() {
    AutoLock autoLock(m_lock);
    TtsPlayerStatus status;
    status = m_status;
    return status;
}

void LocalTtsPlayerImpl::executeSpeechStarted() {
    m_executor->submit([this]() {
        size_t len = m_listeners.size();

        for (size_t i = 0; i < len; ++i) {
            if (NULL != m_listeners[i]) {
                m_listeners[i]->speechStarted();
            }
        }
    });
}

void LocalTtsPlayerImpl::executeSpeechFinished() {
    m_executor->submit([this]() {
        size_t len = m_listeners.size();

        for (size_t i = 0; i < len; ++i) {
            if (NULL != m_listeners[i]) {
                m_listeners[i]->speechFinished();
            }
        }
    });
}

}  // mediaPlayer
}  // duerOSDcsApp
