/*
 * ConsoleLogger.cpp
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

#include <cstdio>
#include <ctime>
#include <iostream>
#include <mutex>

#include "LPCommon/Utils/Logger/ConsoleLogger.h"
#include "LPCommon/Utils/Logger/LoggerUtils.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace logger {

/// Configuration key for DefaultLogger settings
static const std::string CONFIG_KEY_DEFAULT_LOGGER = "consoleLogger";

std::shared_ptr<Logger> ConsoleLogger::instance() {
    static std::shared_ptr<Logger> singleConsoletLogger = std::shared_ptr<ConsoleLogger>(new ConsoleLogger);

    return singleConsoletLogger;
}

void ConsoleLogger::emit(
    Level level,
    std::chrono::system_clock::time_point time,
    const char* threadMoniker,
    const char* text) {
    std::lock_guard<std::mutex> lock(m_coutMutex);
    std::cout << formatLogString(level, time, threadMoniker, text) << std::endl;
}

ConsoleLogger::ConsoleLogger() : Logger(Level::UNKNOWN) {
#ifdef DEBUG
    setLevel(Level::DEBUG0);
#else
    setLevel(Level::INFO);
#endif  // DEBUG
    init(configuration::ConfigurationNode::getRoot()[CONFIG_KEY_DEFAULT_LOGGER]);
}

std::shared_ptr<Logger> getConsoleLogger() {
    return ConsoleLogger::instance();
}

}  // namespace logger
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
