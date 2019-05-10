/*
 * Timer.cpp
 *
 * Copyright 2018 Rockchip.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.rock-chips.com/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include "LPCommon/Utils/Timing/Timer.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace timing {

const std::string Timer::TAG = "Timer";

Timer::Timer() : m_running(false), m_stopping(false) {
}

Timer::~Timer() {
    stop();
}

void Timer::stop() {
    {
        std::lock_guard<std::mutex> lock(m_waitMutex);
        if (m_running) {
            m_stopping = true;
        }
        m_waitCondition.notify_all();
    }

    if (std::this_thread::get_id() != m_thread.get_id() && m_thread.joinable()) {
        m_thread.join();
    }
}

bool Timer::isActive() const {
    return m_running;
}

bool Timer::activate() {
    return !m_running.exchange(true);
}

}  // namespace timing
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
