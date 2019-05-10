/*
 * ThreadMoniker.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LOGGER_THREADMONIKER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LOGGER_THREADMONIKER_H_

#include <string>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace logger {

/**
 * Class to provide @c std::this_thread access to unique name for itself.
 * The name ThreadMoniker is used instead of ThreadId to avoid confusion with platform specific thread identifiers
 * or the @c std::thread::id values rendered as a string.
 */
class ThreadMoniker {
public:
    /**
     * Constructor.
     */
    ThreadMoniker();

    /**
     * Get the moniker for @c std::this_thread.
     *
     * @return The moniker for @c std::this_thread.
     */
    static inline const std::string& getThisThreadMoniker();

private:
    /// The current thread's moniker.
    std::string m_moniker;

    /// Per-thread static instance so that @c m_threadMoniker.m_moniker is @c std::this_thread's moniker.
    static thread_local ThreadMoniker m_threadMoniker;
};

const std::string& ThreadMoniker::getThisThreadMoniker() {
    return m_threadMoniker.m_moniker;
}

}  // namespace logger
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LOGGER_THREADMONIKER_H_
