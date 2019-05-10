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


#include <iostream>
#include "LoggerUtils/DcsSdkLogger.h"
#include "Timer/Timer.h"

namespace duerOSDcsApp {
namespace framework {

#define dbg(fmt, ...) APP_INFO("[duer timer debug] " fmt, ##__VA_ARGS__)
#define err(fmt, ...) APP_INFO("[duer timer error] " fmt, ##__VA_ARGS__)

list<Timer*> TimerManager::m_startList;
list<Timer*> TimerManager::m_stopList;
pthread_once_t TimerManager::m_initOnce = PTHREAD_ONCE_INIT;
pthread_mutex_t TimerManager::m_timerMutex;
TimerManager* TimerManager::m_instance = NULL;
unsigned int TimerManager::m_timerCount = 0;
pthread_t TimerManager::m_tid;

TimerManager::TimerManager() {
}

TimerManager::~TimerManager() {
    if (m_instance) {
        delete m_instance;
        m_instance = NULL;
    }
}

void TimerManager::init(void) {
    int ret = 0;

    m_instance = new TimerManager;

    ret = pthread_mutex_init(&m_timerMutex, NULL);

    if (ret) {
        err("error in [%s]:init mutex fail, err is:%d\n", __FUNCTION__, ret);
        return;
    }

    /* create thread */
    ret = pthread_create(&m_tid, NULL, timerThread, NULL);

    if (ret) {
        err("error in [%s]:create thread fail, err is:%d\n", __FUNCTION__, ret);
        pthread_mutex_destroy(&m_timerMutex);
        return;
    }
}

TimerManager* TimerManager::getInstance() {
    pthread_once(&m_initOnce, TimerManager::init);

    return m_instance;
}

/* 按时间顺序插入链表 */
void TimerManager::timerInsert(Timer* timer) {
    list<Timer*>::iterator it;
    struct timeval time_now;

    gettimeofday(&time_now, NULL);
    timeradd(&time_now, &timer->m_delayTime, &timer->m_deadLine);

    for (it = m_startList.begin(); it != m_startList.end(); ++it) {
        if (timercmp(&timer->m_deadLine, &(*it)->m_deadLine, <)) {
            break;
        }
    }

    m_startList.insert(it, timer);
}

/*
 * 遍历start和stop链表
 * 若找到timer,则将timer从链表中删除，并返回true
 * 若没有找到timer，则证明timer不存在，返回false
 */
int TimerManager::timerRemove(Timer* timer) {
    list<Timer*>::iterator it;

    for (it = m_startList.begin(); it != m_startList.end(); ++it) {
        if (timer == (*it)) {
            m_startList.erase(it);
            return TIMER_IN_ACTIVE;
        }
    }

    for (it = m_stopList.begin(); it != m_stopList.end(); ++it) {
        if (timer == (*it)) {
            m_stopList.erase(it);
            return TimerRet::TIMER_IN_UNACTIVE;
        }
    }

    return TimerRet::TIMER_NOT_EXIST;
}

void* TimerManager::timerThread(void* param) {
    struct timeval time_now;
    struct timeval time_delta;

    while (1) {
        time_delta.tv_sec = 0;
        time_delta.tv_usec = 50000;

        list<Timer*>::iterator it;
        // timer的回调若操作timer则会死锁，所以把超时的timer放入局部变量，最后再回调
        list<Timer*> expired_timer;

        pthread_mutex_lock(&m_timerMutex);

        /* 处理所有超时或即将超时Timer */
        for (it = m_startList.begin(); it != m_startList.end(); ++it) {
            gettimeofday(&time_now, NULL);
            timersub(&(*it)->m_deadLine, &time_now, &time_delta);

            if (time_delta.tv_sec < 0
                    || (time_delta.tv_sec == 0 && time_delta.tv_usec <= 500)) {
                expired_timer.push_back(*it);
            } else {
                break;
            }
        }

        /* select延时时间太长的话，不是太精确 */
        if (time_delta.tv_sec >= 1 || time_delta.tv_usec > 50000) {
            time_delta.tv_sec = 0;
            time_delta.tv_usec = 50000;
        }

        pthread_mutex_unlock(&m_timerMutex);

        for (it = expired_timer.begin(); it != expired_timer.end(); ++it) {
            if ((*it)->m_isPeriod) {
                // 以下两行顺序不能反，否则回调函数调用stop自己无效
                timerStart(*it);
                (*it)->m_notify->timeIsUp(*it);
            } else {
                (*it)->m_notify->timeIsUp(*it);
                timerDelete(*it);
            }
        }

        select(0, NULL, NULL, NULL, &time_delta);
    }
}

Timer* TimerManager::timer_create(double seconds, TimerNotify* notify,
                                  bool is_period, bool is_start_on_create, const char* name) {
    Timer* new_timer = new Timer;

    if (name == NULL) {
        snprintf(new_timer->name, sizeof(new_timer->name), "timer[%d]", getCount());
    } else {
        snprintf(new_timer->name, sizeof(new_timer->name), "%s", name);
    }
     
    new_timer->m_delayTime.tv_sec = (time_t)seconds;
    new_timer->m_delayTime.tv_usec = (long)((seconds - (time_t)seconds) * 1000000);
    new_timer->m_notify = notify;
    new_timer->m_isPeriod = is_period;
    new_timer->m_isStartOnCreate = is_start_on_create;
    new_timer->m_id = notify->m_id;

    pthread_mutex_lock(&m_timerMutex);

    if (is_start_on_create) {
        timerInsert(new_timer);

    } else {
        m_stopList.push_back(new_timer);
    }

    pthread_mutex_unlock(&m_timerMutex);

    return new_timer;
}

void TimerManager::timerDelete(Timer* timer) {

    if ( timer == NULL ){
        return ;
    }

    pthread_mutex_lock(&m_timerMutex);

    if (timerRemove(timer)) {
        delete timer;
    }

    pthread_mutex_unlock(&m_timerMutex);
}

void TimerManager::timerStart(Timer* timer) {

    pthread_mutex_lock(&m_timerMutex);

    if (timerRemove(timer)) {
        timerInsert(timer);
    }

    pthread_mutex_unlock(&m_timerMutex);
}

void TimerManager::timerStop(Timer* timer) {

    pthread_mutex_lock(&m_timerMutex);

    if (timerRemove(timer)) {
        m_stopList.push_back(timer);
    }

    pthread_mutex_unlock(&m_timerMutex);
}

void TimerManager::timerModify(Timer* timer, double seconds) {
    struct timeval time_now;
    int ret = 0;

    pthread_mutex_lock(&m_timerMutex);

    ret = timerRemove(timer);
    if(ret == TimerRet::TIMER_NOT_EXIST){
        pthread_mutex_unlock(&m_timerMutex);
        return ;
    }

    timer->m_delayTime.tv_sec = (time_t) seconds;
    timer->m_delayTime.tv_usec = (long) ((seconds - (time_t) seconds) * 1000000);

    if (ret == TimerRet::TIMER_IN_ACTIVE) { // timer is active
        gettimeofday(&time_now, NULL);
        timeradd(&time_now, &timer->m_delayTime, &timer->m_deadLine);
        timerInsert(timer);
    } else { // timer is inactive
        m_stopList.push_back(timer);
    }

    pthread_mutex_unlock(&m_timerMutex);
}

void TimerManager::printTimers(void) {
    list<Timer*>::iterator it;

    pthread_mutex_lock(&m_timerMutex);

    dbg("*******************active  timer list**********************");
    for (it = m_startList.begin(); it != m_startList.end(); ++it) {
        dbg("timer[%s]", (*it)->name);
    }

    dbg("*******************inactive  timer list**********************");
    for (it = m_stopList.begin(); it != m_stopList.end(); ++it) {
        dbg("timer[%s]", (*it)->name);
    }
  
    pthread_mutex_unlock(&m_timerMutex);
}

unsigned int TimerManager::getCount(void)
{
    pthread_mutex_lock(&m_timerMutex);
    m_timerCount++;
    pthread_mutex_unlock(&m_timerMutex);    
    return m_timerCount;
}

} // namespace framework
} // namespace duerOSDcsApp
