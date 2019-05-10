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

#include "DCSApp/ThreadPoolExecutor.h"

namespace duerOSDcsApp {
namespace application {

#define THREAD_NUM 2
ThreadPoolExecutor *ThreadPoolExecutor::s_instance = nullptr;
pthread_once_t ThreadPoolExecutor::s_initOnce = PTHREAD_ONCE_INIT;
pthread_once_t ThreadPoolExecutor::s_destroyOnce = PTHREAD_ONCE_INIT;

ThreadPoolExecutor* ThreadPoolExecutor::getInstance() {
    pthread_once(&s_initOnce, &ThreadPoolExecutor::init);
    return s_instance;
}

void ThreadPoolExecutor::releaseInstance() {
    pthread_once(&s_destroyOnce, ThreadPoolExecutor::release);
}

ThreadPoolExecutor::ThreadPoolExecutor(size_t threadNum) : m_threadIds(NULL),
                                                           m_threadNum(threadNum),
                                                           m_shutdownFlag(false) {
    pthread_mutex_init(&m_mutex, NULL);
    pthread_cond_init(&m_cond, NULL);
    m_threadIds = (pthread_t *)malloc(sizeof(pthread_t) * m_threadNum);
    for (size_t i = 0; i < m_threadNum; ++i) {
        pthread_create(m_threadIds + i, NULL, threadRoutine, this);
    }
}

ThreadPoolExecutor::~ThreadPoolExecutor() {
    shutDown();
    free(m_threadIds);
    m_threadIds = NULL;
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

void ThreadPoolExecutor::init() {
    if (s_instance == nullptr) {
        s_instance = new ThreadPoolExecutor(THREAD_NUM);
    }
}

void ThreadPoolExecutor::release() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

void ThreadPoolExecutor::submit(std::function<void()> function) {
    pthread_mutex_lock(&m_mutex);
    m_taskQueue.push(function);
    pthread_mutex_unlock(&m_mutex);
    pthread_cond_signal(&m_cond);
}

void ThreadPoolExecutor::shutDown() {
    if (m_shutdownFlag) {
        return;
    }
    m_shutdownFlag = true;
    pthread_cond_broadcast(&m_cond);
    for (size_t i = 0; i < m_threadNum; ++i) {
        void *thread_return = NULL;
        if (*(m_threadIds + i) != 0) {
            pthread_join(*(m_threadIds + i), &thread_return);
        }
    }
}

void* ThreadPoolExecutor::threadRoutine(void *arg) {
    ThreadPoolExecutor *executor = (ThreadPoolExecutor *)arg;

    while (true) {
        pthread_mutex_lock(&executor->m_mutex);
        while (executor->m_taskQueue.size() == 0 && !executor->m_shutdownFlag) {
            pthread_cond_wait(&executor->m_cond, &executor->m_mutex);
        }

        if (executor->m_shutdownFlag) {
            pthread_mutex_unlock(&executor->m_mutex);
            return NULL;
        }
        std::function<void()> func = executor->m_taskQueue.front();
        executor->m_taskQueue.pop();
        pthread_mutex_unlock(&executor->m_mutex);

        if (func) {
            func();
        }
    }
}

}
}