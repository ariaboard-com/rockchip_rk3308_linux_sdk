/*
 * LogEntryStream.h
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

#ifndef DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOG_ENTRY_STREAM_H_
#define DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOG_ENTRY_STREAM_H_

#include <ostream>
#include "LogEntryBuffer.h"

namespace deviceCommonLib {
namespace logger {

/**
 * A simple override of @c std::ostream specialized to minimizing copying and memory allocations while constructing
 * the text of a LogEntry.
 */
class LogEntryStream
        : private LogEntryBuffer
        , public std::ostream {
public:
    /// Create a new LogEntryStream.
    LogEntryStream();

    /**
     * Access the contents of the accumulated stream as string.
     * @return The contents of the accumulated stream as string. The pointer returned is only guaranteed to be valid
     * for the lifetime of this LogEntryStream, and only as long as no further modifications are made to it.
     */
    const char* c_str() const;
};

}  // namespace logger
}  // deviceCommonLib

#endif  // DEVICE_COMMON_LIB_LOGGER_UTILS_INCLUDE_LOG_ENTRY_STREAM_H_
