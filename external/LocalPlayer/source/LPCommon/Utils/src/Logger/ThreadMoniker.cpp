/*
 * ThreadMoniker.cpp
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

#include <atomic>
#include <iomanip>
#include <sstream>
#include <thread>
#include "LPCommon/Utils/Logger/ThreadMoniker.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace logger {

thread_local ThreadMoniker ThreadMoniker::m_threadMoniker;

/// Counter to generate (small) unique thread monikers.
static std::atomic<int> g_nextThreadMoniker(1);

ThreadMoniker::ThreadMoniker() {
    std::ostringstream stream;
    stream << std::setw(3) << std::hex << std::right << g_nextThreadMoniker++;
    m_moniker = stream.str();
}

}  // namespace logger
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
