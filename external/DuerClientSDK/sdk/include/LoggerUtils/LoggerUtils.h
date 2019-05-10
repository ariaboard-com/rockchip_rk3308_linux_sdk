/*
 * LoggerUtils.h
 *
 * Copyright 2016-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOGGER_UTILS_H_
#define DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOGGER_UTILS_H_

#include <chrono>
#include "Logger.h"

namespace deviceCommonLib {
namespace logger {

/**
 * Log a LEVEL::DEBUG0 severity entry.
 *
 * @param entry A log entry.
 */
void acsdkDebug(const LogEntry& entry, const std::string& module="default");

/**
 * Log a Level::INFO severity entry.
 *
 * @param entry A log entry.
 */
void acsdkInfo(const LogEntry& entry, const std::string& module="default");

/**
 * Log a Level::WARN severity entry.
 *
 * @param entry A log entry.
 */
void acsdkWarn(const LogEntry& entry, const std::string& module="default");

/**
 * Log a Level::ERROR severity entry.
 *
 * @param entry A log entry.
 */
void acsdkError(const LogEntry& entry, const std::string& module="default");

/**
 * Log a Level::CRITICAL severity entry.
 *
 * @param entry A log entry.
 */
void acsdkCritical(const LogEntry& entry, const std::string& module="default");

/**
 * Logs an entry at a severity level using the logger instance defined by
 * ACSDK_GET_LOGGER_FUNCTION (refer to Logger.h).
 *
 * @param level The severity level of the log.
 * @param entry A log entry.
 */
void logEntry(Level level, const LogEntry& entry, const std::string& module);

/**
 * Formats a log message into a printable string with other metadata regarding the log message.
 *
 * @param level The severity Level of this log line.
 * @param time The time that the event to log occurred.
 * @param text The text of the entry to log.
 * @return The formatted string.
 */
std::string formatLogString(
    Level level,
    std::chrono::system_clock::time_point time,
    const char* text);

}  // namespace logger
}  // deviceCommonLib
#endif  // DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOGGER_UTILS_H_
