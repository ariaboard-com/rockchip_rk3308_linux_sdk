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

#include "StreamPool.h"
#include "stdio.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

StreamPool::StreamPool() : m_lock(nullptr) {
    m_lock = new PthreadLock();
}

StreamPool::~StreamPool() {
    clearItems();
    if (m_lock) {
        delete m_lock;
        m_lock = nullptr;
    }
}

void StreamPool::pushStream(const char* buff, unsigned int size) {
    AutoLock autoLock(m_lock);
    StreamItem* item = new StreamItem();
    item->set(buff, size);
    m_streamItems.push(item);
}

StreamItem* StreamPool::popItem() {
    AutoLock autoLock(m_lock);
    if (m_streamItems.size() <= 0) {
        return NULL;
    }

    StreamItem* item = m_streamItems.front();
    m_streamItems.pop();
    return item;
}

void StreamPool::clearItems() {
    AutoLock autoLock(m_lock);
    while (m_streamItems.size() > 0) {
        StreamItem* item = m_streamItems.front();
        delete item;
        m_streamItems.pop();
    }
}

}  // mediaPlayer
}  // duerOSDcsApp