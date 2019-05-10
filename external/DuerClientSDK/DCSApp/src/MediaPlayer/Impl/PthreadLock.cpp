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

#include "PthreadLock.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

PthreadLock::PthreadLock() {
    pthread_mutex_init(&m_Mutex, NULL);
}

PthreadLock::~PthreadLock() {
    pthread_mutex_destroy(&m_Mutex);
}

void PthreadLock::lock() {
    pthread_mutex_lock(&m_Mutex);
}

void PthreadLock::unlock() {
    pthread_mutex_unlock(&m_Mutex);
}

}
}



