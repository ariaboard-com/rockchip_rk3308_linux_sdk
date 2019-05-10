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

#ifndef DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIMER_DURATION_H_
#define DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIMER_DURATION_H_

#include <ctime>
#include <string>

namespace deviceCommonLib {
namespace deviceTools {

class TimeDuration {
public:
    /**
     * Constructor.
     */
    TimeDuration();

    /**
     * 析构函数
     */
    virtual ~TimeDuration() = default;

    bool getAndResetDuration(const std::string& message);

    bool getDuration(const std::string& message);

    bool resetDuration();

private:
    bool offset(long& sec, long& nsec);

    struct timespec m_timeBegin;
    struct timespec m_timeEnd;
};

}  // namespace deviceTools
}  // namespace deviceCommonLib

#endif  // DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIMER_DURATION_H_
