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

#include "PcmBufPool.h"
#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#ifdef __cplusplus
};
#endif

namespace duerOSDcsApp {
namespace mediaPlayer {
#define BUFPOOL_MAX_SIZE 3

PcmBufPool::PcmBufPool() : m_lock(nullptr),
                           m_listener(nullptr),
                           m_endFlag(false),
                           m_fullFlag(false),
                           m_pushDirtyFlag(false),
                           m_hasReportInterruptFlag(false) {
    m_lock = new PthreadLock();
}

PcmBufPool::~PcmBufPool() {
    clear();
    if (m_lock) {
        delete m_lock;
        m_lock = nullptr;
    }
}

void PcmBufPool::setStreamListener(StreamInterruptListener* listener) {
    m_listener = listener;
}

void PcmBufPool::setEndFlag(bool end) {
    AutoLock autoLock(m_lock);
    m_endFlag = end;
}

bool PcmBufPool::getEndFlag() const {
    return m_endFlag;
}

bool PcmBufPool::isDirty() const {
    return m_pushDirtyFlag;
}

void PcmBufPool::pushPcmChunk(uint8_t* stream, unsigned int len, unsigned int offset_ms) {
    while (true) {
        m_lock->lock();

        if (m_streamPool.size() >= BUFPOOL_MAX_SIZE) {
            m_fullFlag = true;
            m_lock->unlock();
            usleep(80000);
        } else {
            break;
        }
    }

    PcmChunk* single_chunk = (PcmChunk*) av_malloc(sizeof(PcmChunk));
    single_chunk->dat = (uint8_t*) av_malloc(len);
    memcpy(single_chunk->dat, stream, len);
    single_chunk->len = len;
    single_chunk->offset_ms = offset_ms;
    m_pushDirtyFlag = true;

    m_streamPool.push(single_chunk);
    m_lock->unlock();
}

PcmChunk* PcmBufPool::popPcmChunk() {
    AutoLock autoLock(m_lock);
    if (m_streamPool.size() == 0) {
        return NULL;
    }

    PcmChunk* target = m_streamPool.front();
    m_streamPool.pop();
    return target;
}

void PcmBufPool::clear() {
    AutoLock autoLock(m_lock);
    m_fullFlag = false;
    m_pushDirtyFlag = false;
    m_hasReportInterruptFlag = false;

    if (m_streamPool.size() != 0) {
        while (m_streamPool.size() > 0) {
            PcmChunk* ptr = m_streamPool.front();
            m_streamPool.pop();
            av_free(ptr->dat);
            ptr->dat = NULL;
            av_free(ptr);
            ptr = NULL;
        }
    }

    m_endFlag = false;
}

size_t PcmBufPool::size() const {
    return m_streamPool.size();
}

}  // mediaPlayer
}  // duerOSDcsApp
