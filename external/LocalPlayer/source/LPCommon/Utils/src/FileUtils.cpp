/*
 * FileUtils.cpp
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

#include "LPCommon/Utils/File/FileUtils.h"

#include "LPCommon/Utils/Logger/Logger.h"

#include <cstdio>
#include <fstream>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace file {

/// String to identify log entries originating from this file.
static const std::string TAG("FileUtils");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

/*
 * TODO - make this more portable and dependable.
 *
 * https://issues.labcollab.net/browse/ACSDK-380
 */
bool fileExists(const std::string& filePath) {
    std::ifstream is(filePath);
    return is.good();
}

bool removeFile(const std::string& filePath) {
    if (!fileExists(filePath)) {
        ACSDK_WARN(LX("removeFileFailed").d("reason", "File does not exist."));
        return false;
    }

    int result = std::remove(filePath.c_str());
    if (result != 0) {
        ACSDK_ERROR(LX("removeFileFailed").d("reason", "could not remove file.").d("result", result));
        return false;
    }

    return true;
}

}  // namespace file
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK