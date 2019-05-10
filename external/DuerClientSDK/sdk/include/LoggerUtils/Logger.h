/*
 * Logger.h
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

#ifndef DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOGGER_H_
#define DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOGGER_H_

#include <atomic>
#include <chrono>
#include <mutex>
#include <sstream>
#include <vector>
#include <cstring>

#include "Level.h"
#include "LogEntry.h"

#define LX(event) deviceCommonLib::logger::LogEntry(event)

/**
 * Inner part of LOGGER_CONCATENATE.  Concatenate two expressions in to a token.
 *
 * @param lhs The expression to turn in to the left hand part of the token.
 * @param rhs The expression to turn in to the right hand part of the token.
 */
#define LOGGER_CONCATENATE_INNER(lhs, rhs) lhs##rhs

/**
 * Concatenate two macros in to a token.
 *
 * @param lhs The macro to turn in to the left hand part of the token.
 * @param rhs The macro to turn in to the right hand part of the token.
 */
#define LOGGER_CONCATENATE(lhs, rhs) LOGGER_CONCATENATE_INNER(lhs, rhs)

namespace deviceCommonLib {
namespace logger {

/**
 * @c Logger provides an interface for capturing log entries as well as some core logging functionality.
 * This includes:
 * - Capturing the time, thread ID, and severity level to associate with each log entry.
 * - Accessors for a @c logLevel value that specifies the minimum severity level of a log entries that will be emitted.
 * - Initialization of logging parameters (so far just @c logLevel) from a @c ConfigurationNode.
 *
 * The @c Logger interface is not typically used directly.  Instead, calls to it are usually wrapped in
 * invocations of macros.  These macros provide a way to selectively compile out logging code.  They also
 * provide additional contextual information and direct logs to the appropriate @c Logger instance.
 *
 * Typically, each .cpp source file using @c Logger defines a constant string and a macro near the top of the file.
 * The constant string is typically named @c TAG.  It specifies the name of the @c source of log entries originating
 * from the file.  This is usually the name of the class that is implemented in the file.  The macro is typically
 * named @c LX.  It is used to create a @c LogEntry instance in-line with the expression that builds the text
 * that is to be logged.  The resulting @c LogEntry instance is passed to whatever instance of @c Logger is in use
 * in the file.  @c LX bakes in @c TAG to associate the log entry with its source.  This macro also takes an
 * argument that specifies the name of the @c event that is being logged.  Both the constant string and event name
 * are passed to the @c LogEntry constructor.  Here is an example of the definitions that typically appear at
 * the start of a .cpp file:
 *
 *     static const std::string TAG = "MyClass";
 *     #define LX(event) duerOSDcsSDK::dcsCommon::utils::deviceCommonLib::logger::LogEntry(TAG, event)
 *
 * When an event is to be logged, a wrapper macro named @c ACDK_<LEVEL> is invoked with an expression that starts
 * with an invocation of the @c LX macro.  The value of <LEVEL> is the name of the @c LogLevel value to associate
 * with the @c LogEntry.  Here is an example of a very simple log line that logs a "somethingHappened" event from
 * the source @c TAG with a severity level of @c INFO.
 *
 *     LOGGER_INFO(LX("somethingHappened"));
 *
 * The @c LogEntry class has a builder style method that provides a convenient way to parameterize the event with
 * named properties.  This is done by adding zero or more @c LogEntry::d() method invocations after the
 * @c LX() expression.  The name of the property is always a string, and the value can be of any type for which
 * an @c operator<<(std::ostream&, <type>) has been defined. These pairs are appended to the accumulating log
 * string.  Here is an example log line with two parameters:
 *
 *     LOGGER_WARN(LX("weirdnessHappened").d("<param1Name>", "stringValue").d("<param2Name>", 1 + 1 + 1);
 *
 * The @c LogEntry class also has a rarely used @c m() method that can be used to add an optional free-form
 * string at the end of the @c LogEntry.  When present, this must be the last method appended to the expression.
 * Here is an example of a log line with one parameter and a free-form message:
 *
 *     LOGGER_WARN(LX("weirdnessHappened").d("<param1Name>", "stringValue").m("free form text at the end");
 *
 * The @c LOGGER_<LEVEL> macros allow logs to be selectively eliminated from the generated code.  By default
 * logs of severity @c DEBUG0 and above are included in @c DEBUG builds, and logs of severity
 * @c INFO and above are in included in non @c DEBUG builds.  These macros also perform an in-line @c logLevel
 * check before evaluating the @c LX() expression.  That allows much of the CPU overhead of compiled-in log
 * lines to be selectively bypassed at run-time if the @c Logger's log level is set to not emit them.
 *
 * Logging may also be configured on a per-module basis.  Modules are defined by defining
 * @c LOGGER_LOG_MODULE to a common name for all source files in a module.  This name specifies the name
 * of an object under @c configuration::ConfigurationNode::getRoot().  The named object contains
 * configuration parameters for the per-module logger.  So, for example, to set the @c logLevel of the
 * @c foo module to @c WARN, the JSON used to configure the SDK would look something like this:
 *
 *     {
 *         "foo" : {
 *             "logLevel" : "WARN"
 *         }
 *         [... other propeties]
 *     }
 *
 * The value of @c LOGGER_LOG_MODULE is also used to generate a global inline function that is used to access
 * the logger for that module.  The signature of this function takes the form:
 *
 *     Logger& get<module name>Logger()
 *
 * The value of @c LOGGER_LOG_MODULE is typically defined in the CMakeLists.txt for a module using the
 * @c add_definitions() function.  Here is an example defining the @c foo log module:
 *
 *     add_definitions("-DLOGGER_LOG_MODULE=foo")
 *
 * All logs (module specific or not) are output to a @c Sink @c Logger.  By default, the @c Sink @c Logger
 * is @c ConsoleLogger.  This can be overridden by defining @c LOGGER_LOG_SINK.  The value of @c LOGGER_LOG_SINK
 * specifies a function name of the form:
 *
 *     Logger& get<LOGGER_LOG_SINK>Logger()
 *
 * That function to used to get the @c Sink @c Logger.  When @c LOGGER_LOG_SINK is overridden, it is necessary
 * to provide an implementation of that function that returns the desired @c Logger.
 */
class Logger {
public:
    /**
     * Logger constructor.
     *
     * @param level The lowest severity level of logs to be emitted by this Logger.
     */
    Logger(Level level);

    /// Destructor.
    virtual ~Logger() = default;

    /**
     * Set the lowest severity level to be output by this logger.
     *
     * @param level The lowest severity level to be output by this logger.
     */
    virtual void setLevel(Level level);

    virtual void setEnable(bool enable) {
        m_enable = enable;
    }

    /**
     * Return true of logs of a specified severity should be emitted by this Logger.
     *
     * @param level The Level to check.
     * @return Returns true if logs of the specified Level should be emitted.
     */
    inline bool shouldLog(Level level) const;

    /**
     * Send a log entry to this Logger.
     *
     * @param level The severity Level to associate with this log entry.
     * @param entry Object used to build the text of this log entry.
     */
    void log(Level level, const LogEntry& entry, const std::string& filename = "",
             int lineNo = 0, const std::string& functionName = "", const std::string& module="default");

    /**
     * Emit a log entry.
     * NOTE: This method must be thread-safe.
     * NOTE: Delays in returning from this method may hold up calls to deviceCommonLib::logger::log().
     *
     * @param level The severity Level of this log line.
     * @param time The time that the event to log occurred.
     * @param text The text of the entry to log.
     */
    virtual void emit(Level level,
        std::chrono::system_clock::time_point time,
        const char* text) = 0;

protected:
    /// The lowest severity level of logs to be output by this Logger.
    std::atomic<Level> m_level;

    /// Switch of log output.
    bool m_enable;
};

bool deviceCommonLib::logger::Logger::shouldLog(deviceCommonLib::logger::Level level) const {
    return level >= m_level;
}

/**
 * Get the @c Logger that logs should be sent to.
 *
 * @return The @c Logger that logs should be sent to.
 */
Logger& getDcsSdkLoggerLogger();

}  // namespace logger
}  // deviceCommonLib

#ifndef __LOG_FILENAME__
#define __LOG_FILENAME__ __FILE__
#endif

// 使用配置来设置log级别
#ifndef LOGGER_DEBUG_LOG_ENABLED
#define LOGGER_DEBUG_LOG_ENABLED
#endif

/**
 * Common implementation for sending entries to the log.
 *
 * @param level The log level to associate with the log line.
 * @param entry The text (or builder of the text) for the log entry.
 */
#include <cstdio>

#define LOGGER_LOG(level, entry)                                                                 \
    do {                                                                                         \
        auto& loggerInstance = deviceCommonLib::logger::getDcsSdkLoggerLogger();                 \
        if (loggerInstance.shouldLog(level)) {                                                   \
            loggerInstance.log(level, entry, __LOG_FILENAME__, __LINE__, __FUNCTION__,"SDK");    \
        }                                                                                        \
    } while (false)

/**
* Common implementation for sending entries to the log.
*
* @param level The log level to associate with the log line.
* @param entry The text (or builder of the text) for the log entry.
*/
#define ROOM_LOG(room,level, entry)                                                              \
    do {                                                                                         \
        auto& loggerInstance = deviceCommonLib::logger::getDcsSdkLoggerLogger();                 \
        if (loggerInstance.shouldLog(level)) {                                                   \
            loggerInstance.log(level, entry, __LOG_FILENAME__, __LINE__, __FUNCTION__,room);     \
        }                                                                                        \
    } while (false)
        
#define LOGGER_ENABLE(enable) deviceCommonLib::logger::getDcsSdkLoggerLogger().setEnable(enable)

#ifdef LOGGER_DEBUG_LOG_ENABLED

/**
 * Send a DEBUG9 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG9(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG9, entry)

/**
 * Send a DEBUG8 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG8(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG8, entry)

/**
 * Send a DEBUG7 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG7(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG7, entry)

/**
 * Send a DEBUG6 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG6(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG6, entry)

/**
 * Send a DEBUG5 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG5(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG5, entry)

/**
 * Send a DEBUG4 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG4(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG4, entry)

/**
 * Send a DEBUG3 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG3(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG3, entry)

/**
 * Send a DEBUG2 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG2(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG2, entry)

/**
 * Send a DEBUG1 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG1(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG1, entry)

/**
 * Send a DEBUG0 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG0(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG0, entry)

/**
 * Send a log line at the default debug level (DEBUG0).
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG0, entry)

#else  // LOGGER_DEBUG_LOG_ENABLED

/**
 * Compile out a DEBUG9 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG9(entry)

/**
 * Compile out a DEBUG8 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG8(entry)

/**
 * Compile out a DEBUG7 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG7(entry)

/**
 * Compile out a DEBUG6 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG6(entry)

/**
 * Compile out a DEBUG5 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG5(entry)

/**
 * Compile out a DEBUG4 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG4(entry)

/**
 * Compile out a DEBUG3 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG3(entry)

/**
 * Compile out a DEBUG2 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG2(entry)

/**
 * Compile out a DEBUG1 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG1(entry)

/**
 * Compile out a DEBUG0 severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG0(entry)

/**
 * Compile out a DEBUG severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_DEBUG(entry)

#endif  // LOGGER_DEBUG_LOG_ENABLED

#define LOGGER_DEBUG(entry) LOGGER_LOG(deviceCommonLib::logger::Level::DEBUG0, entry)

/**
 * Send a INFO severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_INFO(entry) LOGGER_LOG(deviceCommonLib::logger::Level::INFO, entry)

/**
 * Send a WARN severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_WARN(entry) LOGGER_LOG(deviceCommonLib::logger::Level::WARN, entry)

/**
 * Send a ERROR severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_ERROR(entry) LOGGER_LOG(deviceCommonLib::logger::Level::ERROR, entry)

/**
 * Send a CRITICAL severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LOGGER_CRITICAL(entry) LOGGER_LOG(deviceCommonLib::logger::Level::CRITICAL, entry)


#include <time.h>
inline unsigned long GetTickCount()
{
     struct timespec ts;

     clock_gettime(CLOCK_MONOTONIC, &ts);

     return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

inline void PRINT_LOG_WITH_TICK(char *logBuffer) {
    printf("<%lu ms> %s\n", GetTickCount(), logBuffer);
}

#define _2K   2048
extern char logBuffer[_2K];
extern std::mutex g_logBufferMutex;
#define FORMAT_LOG(room,level,formate, ...)                               \
    do {                                                                  \
            std::lock_guard<std::mutex> lock(g_logBufferMutex);           \
            memset(logBuffer, 0, sizeof(logBuffer));                      \
            snprintf(logBuffer, sizeof(logBuffer),formate,##__VA_ARGS__); \
            ROOM_LOG(room,level, LX(logBuffer));                          \
        } while(0); \
        PRINT_LOG_WITH_TICK(logBuffer)

/**
 * Send APP log line.
 *
 * @param logger LEVEL
 * @param entry The text (or builder of the text) for the log entry.
 */
#define APP_DEBUG(formate, ...)  FORMAT_LOG("APP",deviceCommonLib::logger::Level::DEBUG0, formate, ##__VA_ARGS__)

/**
 * Send a INFO severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define APP_INFO(formate, ...)  FORMAT_LOG("APP",deviceCommonLib::logger::Level::INFO, formate, ##__VA_ARGS__)

/**
 * Send a WARN severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define APP_WARN(formate, ...)  FORMAT_LOG("APP",deviceCommonLib::logger::Level::WARN, formate, ##__VA_ARGS__)

/**
 * Send a ERROR severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define APP_ERROR(formate, ...)  FORMAT_LOG("APP",deviceCommonLib::logger::Level::ERROR, formate, ##__VA_ARGS__)

/**
 * Send a CRITICAL severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define APP_CRITICAL(formate, ...) FORMAT_LOG("APP",deviceCommonLib::logger::Level::CRITICAL, formate, ##__VA_ARGS__)

/**
 * Send a INFO severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LINK_INFO(formate, ...)  FORMAT_LOG("LINK",deviceCommonLib::logger::Level::INFO, formate, ##__VA_ARGS__)

/**
 * Send a WARN severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LINK_WARN(formate, ...)  FORMAT_LOG("LINK",deviceCommonLib::logger::Level::WARN, formate, ##__VA_ARGS__)

/**
 * Send a ERROR severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LINK_ERROR(formate, ...)  FORMAT_LOG("LINK",deviceCommonLib::logger::Level::ERROR, formate, ##__VA_ARGS__)

/**
 * Send a CRITICAL severity log line.
 *
 * @param loggerArg The Logger to send the line to.
 * @param entry The text (or builder of the text) for the log entry.
 */
#define LINK_CRITICAL(formate, ...) FORMAT_LOG("LINK",deviceCommonLib::logger::Level::CRITICAL, formate, ##__VA_ARGS__)

#endif  // DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOGGER_H_
