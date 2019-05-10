/*
 * LogLevelObserverInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LOGGER_LOGLEVELOBSERVERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LOGGER_LOGLEVELOBSERVERINTERFACE_H_

#include "LPCommon/Utils/Logger/Level.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace logger {

/**
 * This interface class allows notifications from a Logger object (or any derived class) when the
 * logLevel changes.
 */
class LogLevelObserverInterface {
public:
    /**
     * This function will be called when the logLevel changes.
     *
     * @param status The updated logLevel
     */
    virtual void onLogLevelChanged(Level level) = 0;
};

}  // namespace logger
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LOGGER_LOGLEVELOBSERVERINTERFACE_H_
