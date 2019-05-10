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
#ifndef DUEROS_DCS_APP_SAMPLEAPP_ACTIVITYMONITORSINGLETON_H_
#define DUEROS_DCS_APP_SAMPLEAPP_ACTIVITYMONITORSINGLETON_H_
#include <pthread.h>
#include <sys/time.h>
#include <string>
#include <fstream>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
namespace duerOSDcsApp {
namespace application {
enum PlayerStatus {
    PLAYER_STATUS_ON,
    PLAYER_STATUS_OFF
};
struct MonitorItem {
    PlayerStatus m_playerStatus;
    unsigned int m_timestamp;
    MonitorItem(PlayerStatus status = PLAYER_STATUS_OFF, unsigned int timestamp = 0) : m_playerStatus(status),
                                                                                       m_timestamp(timestamp) {
    }
};
class ActivityMonitorSingleton {
public:
    static ActivityMonitorSingleton* getInstance();
    static void releaseInstance();
    void updatePlayerStatus(PlayerStatus playerStatus);
    bool isPlayerIdle() const;
private:
    ActivityMonitorSingleton(const ActivityMonitorSingleton& ths);
    ActivityMonitorSingleton& operator=(const ActivityMonitorSingleton& ths);
    ActivityMonitorSingleton();
    ~ActivityMonitorSingleton();
    void updateStatus();
    void updateJsonFile();
    void addPairToDoc(rapidjson::Document& document,
                      const std::string& key,
                      const std::string& value);
    static void init();
    static void destory();
    static void* threadRoutine(void *arg);
private:
    static ActivityMonitorSingleton* s_instance;
    static pthread_once_t s_init_once;
    static pthread_once_t s_destroy_once;
    pthread_t m_threadId;
    pthread_mutex_t m_mutex;
    MonitorItem m_monitorItem;
    std::string m_prevWriteContent;
    bool m_isIdle;
    bool m_threadAlive;
};
}  // namespace application
}  // namespace duerOSDcsApp
#endif  // DUEROS_DCS_APP_SAMPLEAPP_ACTIVITYMONITORSINGLETON_H_
