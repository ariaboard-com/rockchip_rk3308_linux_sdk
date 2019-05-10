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

#ifndef DUEROS_DCS_SDK_DCS_COMMON_UTILS_INCLUDE_DCS_COMMON_UTILS_THREADING_THREAD_POOL_EXECUTOR_H_
#define DUEROS_DCS_SDK_DCS_COMMON_UTILS_INCLUDE_DCS_COMMON_UTILS_THREADING_THREAD_POOL_EXECUTOR_H_

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <functional>
#include <queue>

namespace duerOSDcsApp {
namespace application {

class ThreadPoolExecutor {
public:
    static ThreadPoolExecutor* getInstance();

    static void releaseInstance();

    void submit(std::function<void()> function);

    void shutDown();

private:
    explicit ThreadPoolExecutor(size_t threadNum = 4);

    virtual ~ThreadPoolExecutor();

    ThreadPoolExecutor(const ThreadPoolExecutor&);

    ThreadPoolExecutor& operator=(const ThreadPoolExecutor&);

    static void init();

    static void release();

    static void* threadRoutine(void *arg);

private:
    static ThreadPoolExecutor *s_instance;
    static pthread_once_t s_initOnce;
    static pthread_once_t s_destroyOnce;
    std::queue<std::function<void()>> m_taskQueue;
    pthread_t *m_threadIds;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    size_t m_threadNum;
    bool m_shutdownFlag;
};

}
}


#endif //DUEROS_DCS_SDK_DCS_COMMON_UTILS_INCLUDE_DCS_COMMON_UTILS_THREADING_THREAD_POOL_EXECUTOR_H_
