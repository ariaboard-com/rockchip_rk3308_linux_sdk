/*
 * TimePoint.cpp
 *
 * Copyright 2018 Rockchip.com, Inc. or its affiliates.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "LPCommon/Utils/Timing/TimePoint.h"

#include "LPCommon/Utils/Logger/Logger.h"
#include "LPCommon/Utils/Timing/TimeUtils.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace timing {

using namespace rkpCommon::utils::logger;

/// String to identify log entries originating from this file.
static const std::string TAG("TimePoint");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

TimePoint::TimePoint() : m_time_Unix{0} {
}

bool TimePoint::setTime_ISO_8601(const std::string& time_ISO_8601) {
    int64_t tempUnixTime = 0;
    if (!convert8601TimeStringToUnix(time_ISO_8601, &tempUnixTime)) {
        ACSDK_ERROR(LX("setTime_ISO_8601Failed").d("input", time_ISO_8601).m("Could not convert to Unix time."));
        return false;
    }

    m_time_ISO_8601 = time_ISO_8601;
    m_time_Unix = tempUnixTime;
    return true;
}

std::string TimePoint::getTime_ISO_8601() const {
    return m_time_ISO_8601;
}

int64_t TimePoint::getTime_Unix() const {
    return m_time_Unix;
}

}  // namespace timing
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK