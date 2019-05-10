/*
 * TimeUtils.h
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

#ifndef DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIME_UTILS_H_
#define DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIME_UTILS_H_

#include <string>
//#include "DeviceTools/RetryTimer.h"

namespace deviceCommonLib {
namespace deviceTools {

/**
 * This function converts a string representing time, encoded in the ISO-8601 format, to what is commonly
 * known as Unix time (epoch).
 *
 * For completeness, the expected format of the input string is as follows:
 *
 * YYYY-MM-DDTHH:MM:SS+0000
 *
 * Where (in order of listing) :
 * Y means year
 * M means month
 * D means day
 * H means hour
 * M means minute
 * S means second
 *
 * So, for example:
 *
 * 1986-8-8T21:30:00+0000
 *
 * means the year 1986, August 8th, 9:30pm.
 *
 * @param timeString The time string, formatted as described above.
 * @param[out] unixTime The converted time into Unix epoch time.
 * @return Whether the conversion was successful.
 */
bool convert8601TimeStringToUnix(const std::string& timeString, int64_t* unixTime);

/**
 * Gets the current time in Unix epoch time, as a 64 bit integer.
 *
 * @param[out] currentTime The current time in Unix epoch time, as a 64 bit integer.
 * @return Whether the get time was successful.
 */
bool getCurrentUnixTime(int64_t* currentTime);

/**
 * 获取当前毫秒时间戳
 * @return 当前毫秒时间戳
 */
unsigned int currentTimeMs();

}  // namespace deviceTools
}  // namespace deviceCommonLib

#endif  // DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_TIME_UTILS_H_
