/*
 * LogEntry.h
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

#ifndef DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOG_ENTRY_H_
#define DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOG_ENTRY_H_

#include <sstream>
#include <string>
#include "LogEntryStream.h"

namespace deviceCommonLib{
namespace logger {

/// LogEntry is used to compile the log entry text to log via Logger.
class LogEntry {
public:
    /**
     * Constructor.
     * @param source The name of the source of this log entry.
     * @param event The name of the event that this log entry describes.
     */
    LogEntry(const char* event);

    /**
     * Constructor.
     * @param source The name of the source of this log entry.
     * @param event The name of the event that this log entry describes.
     */
    LogEntry(const std::string& event);

    /**
     * Constructor.
     * @param source The name of the source of this log entry.
     * @param event The name of the event that this log entry describes.
     */
    LogEntry(const std::string& source, const char* event);

    /**
     * Constructor.
     * @param source The name of the source of this log entry.
     * @param event The name of the event that this log entry describes.
     */
    LogEntry(const std::string& source, const std::string& event);

    /**
     * Add a @c key, @c value pair to the metadata of this log entry.
     * @param key The key identifying the value to add to this LogEntry.
     * @param value The value to add to this LogEntry.
     * @return This instance to facilitate adding more information to this log entry.
     */
    LogEntry& d(const char* key, const char* value);

    /**
     * Add data (hence the name 'd') in the form of a @c key, @c value pair to the metadata of this log entry.
     * @param key The key identifying the value to add to this LogEntry.
     * @param value The value to add to this LogEntry.
     * @return This instance to facilitate adding more information to this log entry.
     */
    LogEntry& d(const char* key, const std::string& value);

    /**
     * Add data (hence the name 'd') in the form of a @c key, @c value pair to the metadata of this log entry.
     * @param key The key identifying the value to add to this LogEntry.
     * @param value The boolean value to add to this LogEntry.
     * @return This instance to facilitate adding more information to this log entry.
     */
    LogEntry& d(const char* key, bool value);

    /**
     * Add data (hence the name 'd') in the form of a @c key, @c value pair to the metadata of this log entry.
     * @param key The key identifying the value to add to this LogEntry.
     * @param value The value to add to this LogEntry.
     * @return This instance to facilitate adding more information to this log entry.
     */
    template <typename ValueType>
    inline LogEntry& d(const char* key, const ValueType& value);

    /**
     * Add sensitive data in the form of a @c key, @c value pair to the metadata of this log entry.
     * Because the data is 'sensitive' it will only be emitted in DEBUG builds.
     *
     * @param key The key identifying the value to add to this LogEntry.
     * @param value The value to add to this LogEntry.
     * @return This instance to facilitate adding more information to this log entry.
     */
    template <typename ValueType>
    inline LogEntry& sensitive(const char* key, const ValueType& value);

    /**
     * Add an arbitrary message to the end of the text of this LogEntry.  Once this has been called no other
     * additions should be made to this LogEntry.
     * @param message The message to add to the end of the text of this LogEntry.
     * @return This instance to facilitate passing this instance on.
     */
    LogEntry& m(const char* message);

    /**
     * Add an arbitrary message to the end of the text of this LogEntry.  Once this has been called no other
     * additions should be made to this LogEntry.
     * @param message The message to add to the end of the text of this LogEntry.
     * @return This instance to facilitate passing this instance on.
     */
    LogEntry& m(const std::string& message);

    /**
     * Get the rendered text of this LogEntry.
     * @return The rendered text of this LogEntry.  The returned buffer is only guaranteed to be valid for
     * the lifetime of this LogEntry, and only as long as no further modifications are made to it.
     */
    const char* c_str() const;

private:
    /// Character used to separate @c key from @c value text in metadata.
    static const char KEY_VALUE_SEPARATOR = '=';

    /// Character used to separate @c key from @c value text in metadata.
    static const char LEFT_SEPARATOR = '[';

    /// Character used to separate @c key from @c value text in metadata.
    static const char RIGHT_SEPARATOR = ']';

    /// Flag indicating (if true) that some metadata has already been appended to this LogEntry.
    bool m_hasMetadata;

    /// A stream with which to accumulate the text for this LogEntry.
    LogEntryStream m_stream;
};

template <typename ValueType>
LogEntry& LogEntry::d(const char* key, const ValueType& value) {
    m_stream << LEFT_SEPARATOR << key << KEY_VALUE_SEPARATOR << value << RIGHT_SEPARATOR;
    return *this;
}

// Define ACSDK_EMIT_SENSITIVE_LOGS if you want to include sensitive data in log output.
#ifdef ACSDK_EMIT_SENSITIVE_LOGS

template <typename ValueType>
LogEntry& LogEntry::sensitive(const char* key, const ValueType& value) {
    return d(key, value);
}
#else
template <typename ValueType>
LogEntry& LogEntry::sensitive(const char* key, const ValueType& value) {
    return *this;
}
#endif

}  // namespace logger
}  // namespace deviceCommonLib

#endif  // DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOG_ENTRY_H_
