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

#ifndef DUEROS_DCS_APP_SAMPLEAPP_SYSTEMUPDATEREVWRAPPER_H_
#define DUEROS_DCS_APP_SAMPLEAPP_SYSTEMUPDATEREVWRAPPER_H_

#include <thread>
#include <atomic>

namespace duerOSDcsApp {
namespace application {

class SystemUpdateRevWrapper {
public:
    static std::unique_ptr<SystemUpdateRevWrapper> create();

    ~SystemUpdateRevWrapper();

private:
    SystemUpdateRevWrapper();

    // The main thread loop
    void mainLoop();

    // Thread management variables
    std::atomic<bool> m_isRunning;

    std::unique_ptr<std::thread> m_thread;
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_SAMPLEAPP_SYSTEMUPDATEREVWRAPPER_H_
