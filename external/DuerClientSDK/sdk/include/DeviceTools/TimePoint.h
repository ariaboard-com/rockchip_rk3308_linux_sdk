/*
 * TimePoint.h
 *
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIMER_POINT_H_
#define DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIMER_POINT_H_

#include <string>
#include <stdint.h>

namespace deviceCommonLib {
namespace deviceTools {

/**
 * A simple utility class that is useful in our SDK to map between ISO-8601 and Unix (epoch) time.
 */
class TimePoint {
public:
    /**
     * Constructor.
     */
    TimePoint();

    /**
     * Sets the time with an ISO-8601 formatted string.  This will update the object's Unix time to the relative value.
     *
     * @param time_ISO_8601 A string representation of time in ISO-8601 format.
     */
    bool setTime_ISO_8601(const std::string& time_ISO_8601);

    /**
     * Returns the time managed by this object in ISO-8601 format.
     *
     * @return The time managed by this object in ISO-8601 format.
     */
    std::string getTime_ISO_8601() const;

    /**
     * Returns the time managed by this object in Unix epoch time format.
     *
     * @return The time managed by this object in Unix epoch time format.
     */
    int64_t getTime_Unix() const;

private:
    /// The scheduled time for the alert in ISO-8601 format.
    std::string m_time_ISO_8601;
    /// The scheduled time for the alert in Unix epoch format.
    int64_t m_time_Unix;
};

}  // namespace deviceTools
}  // namespace deviceCommonLib

#endif  // DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIMER_POINT_H_
