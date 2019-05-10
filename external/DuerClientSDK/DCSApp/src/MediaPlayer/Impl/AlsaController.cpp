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

#include "AlsaController.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

#define ALSA_MAX_BUFFER_TIME 500000

AlsaController::AlsaController(const std::string& audio_dev) : m_pcmHandle(nullptr),
                                                               m_lock(nullptr),
                                                               m_alsaCanPause(false),
                                                               m_abortFlag(false),
                                                               m_chunkBytes(0) {
    assert(!audio_dev.empty());
    m_pcmDevice = audio_dev;
    m_lock = new PthreadLock();
}

AlsaController::~AlsaController() {
    if (m_lock) {
        delete m_lock;
        m_lock = nullptr;
    }
}

bool AlsaController::openDevice() {
    int ret = snd_pcm_open(&m_pcmHandle, m_pcmDevice.c_str(), SND_PCM_STREAM_PLAYBACK, 0);
    return ret >= 0;
}

bool AlsaController::setParams(unsigned int rate, unsigned int channels) {
    snd_pcm_hw_params_t *params = nullptr;
    uint32_t buffer_time = 0;
    uint32_t period_time = 0;
    int ret = 0;
    snd_pcm_hw_params_alloca(&params);

    ret = snd_pcm_hw_params_any(m_pcmHandle, params);
    if (ret < 0) {
        return false;
    }
    ret = snd_pcm_hw_params_set_access(m_pcmHandle, params, SND_PCM_ACCESS_RW_INTERLEAVED);
    if (ret < 0) {
        return false;
    }
    ret = snd_pcm_hw_params_set_format(m_pcmHandle, params, SND_PCM_FORMAT_S16_LE);
    if (ret < 0) {
        return false;
    }
    ret = snd_pcm_hw_params_set_channels(m_pcmHandle, params, channels);
    if (ret < 0) {
        return false;
    }
    ret = snd_pcm_hw_params_set_rate_near(m_pcmHandle, params, &rate, nullptr);
    if (ret < 0) {
        return false;
    }
    ret = snd_pcm_hw_params_get_buffer_time_max(params, &buffer_time, nullptr);
    if (ret < 0) {
        return false;
    }
    buffer_time = buffer_time > ALSA_MAX_BUFFER_TIME ? ALSA_MAX_BUFFER_TIME : buffer_time;
    period_time = buffer_time / 4;
    ret = snd_pcm_hw_params_set_buffer_time_near(m_pcmHandle, params, &buffer_time, nullptr);
    if (ret < 0) {
        return false;
    }
    ret = snd_pcm_hw_params_set_period_time_near(m_pcmHandle, params, &period_time, nullptr);
    if (ret < 0) {
        return false;
    }

    ret = snd_pcm_hw_params(m_pcmHandle, params);
    if (ret < 0) {
        return false;
    }
    m_alsaCanPause = snd_pcm_hw_params_can_pause(params);

    snd_pcm_uframes_t chunk_size = 0;
    snd_pcm_uframes_t buffer_size = 0;
    ret = snd_pcm_hw_params_get_period_size(params, &chunk_size, nullptr);
    if (ret < 0) {
        return false;
    }
    ret = snd_pcm_hw_params_get_buffer_size(params, &buffer_size);
    if (ret < 0) {
        return false;
    }
    m_chunkBytes = chunk_size * snd_pcm_format_physical_width(SND_PCM_FORMAT_S16_LE) / 8;
    return true;
}

bool AlsaController::init(unsigned int rate, unsigned int channels) {
    if (!openDevice()) {
        return false;
    }
    return setParams(rate, channels);
}

bool AlsaController::isAccessable() const {
    AutoLock autoLock(m_lock);
    return m_pcmHandle != nullptr;
}

bool AlsaController::alsaPrepare() {
    AutoLock autoLock(m_lock);
    if (m_pcmHandle != nullptr) {
        snd_pcm_prepare(m_pcmHandle);
        m_abortFlag = false;
        return true;
    }
    return false;
}

bool AlsaController::alsaPause() {
    AutoLock autoLock(m_lock);
    int err = 0;
    if (m_alsaCanPause) {
        if ((err = snd_pcm_pause(m_pcmHandle, 1)) < 0) {
            return false;
        }
    } else {
        if ((err = snd_pcm_drop(m_pcmHandle)) < 0) {
            return false;
        }
    }

    return true;
}

bool AlsaController::alsaResume() {
    AutoLock autoLock(m_lock);
    int err = 0;
    if (snd_pcm_state(m_pcmHandle) == SND_PCM_STATE_SUSPENDED) {
        while ((err = snd_pcm_resume(m_pcmHandle)) == -EAGAIN) {
            sleep(1);
        }
    }

    if (m_alsaCanPause) {
        if ((err = snd_pcm_pause(m_pcmHandle, 0)) < 0) {
            return false;
        }
    } else {
        if ((err = snd_pcm_prepare(m_pcmHandle)) < 0) {
            return false;
        }
    }

    return true;
}

bool AlsaController::aslaAbort() {
    AutoLock autoLock(m_lock);
    if (m_pcmHandle != nullptr) {
        m_abortFlag = true;
        snd_pcm_abort(m_pcmHandle);
        return true;
    }
    return false;
}

void AlsaController::writeStream(unsigned int channels,
                                 const void* buffer,
                                 unsigned long buff_size) {
    AutoLock autoLock(m_lock);
    if (m_abortFlag || m_pcmHandle == nullptr) {
        return;
    }

    int ret = 0;
    while ((ret = snd_pcm_writei(m_pcmHandle, buffer, buff_size / channels / 2)) < 0) {
        if (m_abortFlag) {
            break;
        } else {
            snd_pcm_prepare(m_pcmHandle);
        }
    }
}

bool AlsaController::alsaClear() {
    AutoLock autoLock(m_lock);
    if (nullptr != m_pcmHandle) {
        snd_pcm_drop(m_pcmHandle);
        return true;
    }
    return false;
}

bool AlsaController::alsaClose() {
    AutoLock autoLock(m_lock);
    if (nullptr == m_pcmHandle) {
        return false;
    }
    snd_pcm_drop(m_pcmHandle);
    snd_pcm_close(m_pcmHandle);
    m_pcmHandle = nullptr;
    return true;
}

}  // mediaPlayer
}  // duerOSDcsApp
