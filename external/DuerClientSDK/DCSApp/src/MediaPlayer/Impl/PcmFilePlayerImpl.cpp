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

#include "PcmFilePlayerImpl.h"
#include "DCSApp/Configuration.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

PcmFilePlayerImpl::PcmFilePlayerImpl(const std::string& audio_dev,
                                     const std::string& file_path) : m_alsa_ctl(nullptr),
    m_dat(nullptr),
    m_len(0),
    m_playing_flag(false),
    m_thread_alive(true),
    m_thread_id(0) {

    pthread_mutex_init(&m_mutex, nullptr);
    pthread_cond_init(&m_cond, nullptr);
    m_alsa_ctl = new AlsaController(audio_dev);
    m_alsa_ctl->init(16000, 2);

    FILE* fp = fopen(file_path.c_str(), "r");
    assert(fp != nullptr);

    if (fp) {
        fseek(fp, 0, SEEK_END);
        m_len = ftell(fp);
        m_dat = (char*)malloc(sizeof(char) * m_len);
        rewind(fp);
        fread(m_dat, sizeof(char), m_len, fp);
        fclose(fp);
    }

    pthread_create(&m_thread_id, nullptr, playFunc, this);
}

PcmFilePlayerImpl::~PcmFilePlayerImpl() {
    m_thread_alive = false;

    free(m_dat);
    m_dat = nullptr;

    delete m_alsa_ctl;
    m_alsa_ctl = nullptr;

    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

void PcmFilePlayerImpl::play() {
    m_playing_flag = true;
    pthread_cond_signal(&m_cond);
}

void PcmFilePlayerImpl::stop() {
    m_playing_flag = false;
}

void* PcmFilePlayerImpl::playFunc(void* arg) {
    auto instance = (PcmFilePlayerImpl*)arg;

    while (instance->m_thread_alive) {
        pthread_mutex_lock(&instance->m_mutex);

        while (!instance->m_playing_flag) {
            pthread_cond_wait(&instance->m_cond, &instance->m_mutex);
        }

        instance->playBuffer();

        instance->m_playing_flag = false;
        pthread_mutex_unlock(&instance->m_mutex);
    }

    return nullptr;
}

void PcmFilePlayerImpl::playBuffer() {
    size_t i = 0;
    size_t nxt_len = 0;

    while (m_playing_flag) {
        nxt_len = i + 128;

        if (nxt_len < m_len) {
            m_alsa_ctl->writeStream(2, m_dat + i, 128);
            i = nxt_len;
        } else {
            m_alsa_ctl->writeStream(2, m_dat + i, m_len - i);
            break;
        }
    }
}

}  // mediaPlayer
}  // duerOSDcsApp
