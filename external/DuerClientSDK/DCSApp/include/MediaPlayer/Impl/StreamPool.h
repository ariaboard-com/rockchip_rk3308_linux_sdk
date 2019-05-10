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

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_STREAMPOOL_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_STREAMPOOL_H_

#include "PthreadLock.h"
#include "AutoLock.h"
#include <queue>
#include <stdlib.h>
#include <string.h>

namespace duerOSDcsApp {
namespace mediaPlayer {

class StreamItem {
public:
    StreamItem() {
        m_dat = NULL;
        m_size = 0;
    }

    ~StreamItem() {
        if (m_dat != NULL) {
            free(m_dat);
            m_dat = NULL;
            m_size = 0;
        }
    }

    void set(const char* data, int size) {
        m_size = size;
        m_dat = (char*) malloc(size);
        memcpy(m_dat, data, size);
    }

    int getSize() const {
        return m_size;
    }

    char* getData() {
        return m_dat;
    }

private:
    char* m_dat;
    int m_size;
};

class StreamPool {
public:
    StreamPool();

    ~StreamPool();

    void pushStream(const char* buff, unsigned int size);

    StreamItem* popItem();

    void clearItems();

private:
    StreamPool(const StreamPool&);

    StreamPool& operator=(const StreamPool&);

private:
    std::queue<StreamItem*> m_streamItems;
    PthreadLock* m_lock;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_STREAMPOOL_H_
