/*
 * LogEntryStream.cpp
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

#include "LPCommon/Utils/Logger/LogEntryStream.h"

#include "LPCommon/Utils/RetryTimer.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace logger {

LogEntryStream::LogEntryStream() : LogEntryBuffer{}, std::ostream{this} {
}

const char* LogEntryStream::LogEntryStream::c_str() const {
    return LogEntryBuffer::c_str();
}

}  // namespace logger
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
