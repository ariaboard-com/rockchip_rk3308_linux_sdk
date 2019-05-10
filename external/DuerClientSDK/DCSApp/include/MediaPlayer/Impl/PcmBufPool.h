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

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_PCMPOOL_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_PCMPOOL_H_

#include "PthreadLock.h"
#include "AutoLock.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <queue>

namespace duerOSDcsApp {
namespace mediaPlayer {

struct PcmChunk {
    uint8_t* dat;
    unsigned int len;
    unsigned int offset_ms;
};

class StreamInterruptListener {
public:
    virtual void onStreamInterrupt() = 0;
};

class PcmBufPool {
public:
    PcmBufPool();

    ~PcmBufPool();

    void setStreamListener(StreamInterruptListener* listener);

    void pushPcmChunk(uint8_t* stream, unsigned int len, unsigned int offset_ms);

    PcmChunk* popPcmChunk();

    void clear();

    size_t size() const;

    void setEndFlag(bool end);

    bool getEndFlag() const;

    bool isDirty() const;

private:
    PcmBufPool(const PcmBufPool&);

    PcmBufPool& operator=(const PcmBufPool&);

    PthreadLock* m_lock;

    StreamInterruptListener* m_listener;

    bool m_endFlag;

    bool m_fullFlag;

    bool m_pushDirtyFlag;

    bool m_hasReportInterruptFlag;

    std::queue<PcmChunk*> m_streamPool;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_PCMPOOL_H_