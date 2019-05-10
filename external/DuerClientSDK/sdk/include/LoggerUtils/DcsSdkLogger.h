/*
 * DcsSdkLogger.h
 *
 * Copyright (c) 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_DCS_SDK_LOGGER_H_
#define DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_DCS_SDK_LOGGER_H_

#include <mutex>
#include <string>
#include <map>

#include "LoggerUtils.h"
#include "Logger.h"

namespace deviceCommonLib{
namespace logger{

enum class LogPrintMode {
    LOG_TO_CONSOLE,
    LOG_TO_FILE
};

/**
 * A simple class that prints to the screen.
 */
class DcsSdkLogger : public logger::Logger {
public:
    /**
     * Constructor.
     */
    DcsSdkLogger();

    virtual ~DcsSdkLogger();

    void rotateCheckFileSize();

    static void setLogPrintMode(LogPrintMode logPrintMode);
    
    /**
     * Prints a message with a pretty format with a \n after.
     *
     * @param stringToPrint The string to print.
     */
    static void prettyPrint(const std::string &stringToPrint);

    void emit(logger::Level level,
            std::chrono::system_clock::time_point time,
            const char *text) override;

    /**
     * Get the handler of DcsSdkLogger.
     * @return Handler of DcsSdkLogger.
     */
    static Logger& instance();

private:
    static std::mutex m_mutex;
    static FILE* m_fp;
    static std::string m_defaultLogFileName;
    static unsigned int m_checkPrintCount;
    static LogPrintMode m_logPrintMode;
    int m_fileSize;

};

extern DcsSdkLogger g_dcsSdkLogger;

}  // namespace logger
}  // deviceCommonLib

#endif // DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_DCS_SDK_LOGGER_H_
