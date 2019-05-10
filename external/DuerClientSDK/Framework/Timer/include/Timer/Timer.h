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


#ifndef DUEROS_FRAMEWORK_TIMER_H_
#define DUEROS_FRAMEWORK_TIMER_H_

#include <list>
#include <string>
#include <pthread.h>
#include <sys/time.h>

using std::list;

namespace duerOSDcsApp {
namespace framework {

class Timer;
class TimerNotify {
public:
    int m_id;                                   /* id, 回调需要可传入 */

    /**
     * @brief 定时器回调, 不要做耗时操作，及时返回
     *
     * @param timer timer指针，可以从中获取关心数据
     *
     */
    virtual void timeIsUp(Timer* timer) = 0;

    virtual ~TimerNotify() = default;
};

class Timer {
public:
    struct timeval  m_delayTime;               /* 定时多久 */
    struct timeval  m_deadLine;                /* 定时器溢出时间 */
    bool m_isPeriod;                           /* 是否为周期定时 */
    bool m_isStartOnCreate;                  /* 是否自动启动 */
    TimerNotify* m_notify;                      /* 回调 */
    int m_id;                                   /* id, 回调需要可传入 */
    char name[50];
};

class TimerManager {
private:
    static list<Timer*> m_stopList;
    static list<Timer*> m_startList;
    static TimerManager* m_instance;
    static pthread_mutex_t m_timerMutex;
    static pthread_once_t m_initOnce;
    static pthread_t m_tid;
    static unsigned int m_timerCount;

    static void* timerThread(void* param);
    static void init(void);
    static void timerInsert(Timer* timer);
    static int timerRemove(Timer* timer);
    static unsigned int getCount(void);

    /**
     * the return value encapsulated in TimerManager
     */
    enum TimerRet {
        TIMER_NOT_EXIST,
        TIMER_IN_ACTIVE,
        TIMER_IN_UNACTIVE,
    };

    TimerManager();
    ~TimerManager();

public:
    /**
     * @brief 获取实例
     *
     * @return 指向实例的指针
     */
    static TimerManager* getInstance(void);

    /**
     * @brief 创建定时器
     *
     * @param seconds 定时时间
     * @param notify 回调对象
     * @param isPeriod 是否为周期定时
     * @param isStartOnCreate 是否自动启动
     * @param name 定时器名字
     *
     * @return 返回一个定时器指针
     */
    Timer* timer_create(double seconds, TimerNotify* notify,
                        bool isPeriod = false, bool isStartOnCreate = true,
                        const char* name = NULL);

    /**
     * @brief 删除定时器
     *
     * @param timer 定时器指针，由创建时返回
     *
     */
    static void timerDelete(Timer* timer);

    /**
     * @brief 启动定时器
     *
     * @param timer 定时器指针，由创建时返回
     *
     */
    static void timerStart(Timer* timer);

    /**
     * @brief 停止定时器
     *
     * @param timer 定时器指针，由创建时返回
     *
     */
    void timerStop(Timer* timer);

    /**
     * @brief 修改定时器时间
     *
     * @param timer 定时器指针，由创建时返回
     *
     */
    void timerModify(Timer* timer, double seconds);

    /**
     * @brief 打印所有timer
     */
    void printTimers(void);
};

} // namespace framework
} // namespace duerOSDcsApp

#endif // DUEROS_FRAMEWORK_TIMER_H_
