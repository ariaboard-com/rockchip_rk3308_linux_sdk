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

#include "AudioDecoder.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
AudioDecoder::AudioDecoder(unsigned int channel, unsigned int samplerate) :
        m_decodeMode(DECODE_MODE_NORMAL),
        m_fp(NULL),
        m_repeatTimes(1),
        m_currentTimes(0) {
}

AudioDecoder::~AudioDecoder() {
}

void AudioDecoder::setDecodeMode(DecodeMode mode, int val) {
    m_decodeMode = mode;
    m_repeatTimes = val;
    m_currentTimes = 0;
}

bool AudioDecoder::open(const std::string& url) {
    m_fp = fopen(url.c_str(), "r");
    if (m_fp) {
        return true;
    } else {
        return false;
    }
}

StreamReadResult AudioDecoder::readFrame(uint8_t** frameBuffer, unsigned int bufferLen,
        unsigned int* len) {
    size_t n = fread(m_buffer, 1, FIX_READ_SIZE, m_fp);
    if (n == FIX_READ_SIZE) {
        *len = n;
        memcpy(*frameBuffer, m_buffer, *len);
        return READ_SUCCEED;
    } else {
        if (m_decodeMode == DECODE_MODE_NORMAL) {
            return READ_END;
        } else if (m_decodeMode == DECODE_MODE_LOOP) {
            if (m_repeatTimes == 0) {
                return READ_END;
            } else {
                if (++m_currentTimes < m_repeatTimes) {
                    fseek(m_fp, 0, SEEK_SET);
                    return READ_FAILED;
                } else {
                    return READ_END;
                }
            }
        }
    }

    return READ_END;
}

bool AudioDecoder::close() {
    if (m_fp) {
        fclose(m_fp);
        m_fp = NULL;
    }
    return true;
}

}  // mediaPlayer
}  // duerOSDcsApp