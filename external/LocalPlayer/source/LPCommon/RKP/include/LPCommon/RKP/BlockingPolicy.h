/*
 * BlockingPolicy.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_BLOCKINGPOLICY_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_BLOCKINGPOLICY_H_

#include <iostream>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/// Enumeration of 'blocking policy' values to be associated with @c RKPDirectives.
enum class BlockingPolicy {
    /**
     * Handling of an @c RKPDirective with this @c BlockingPolicy does NOT block the handling of
     * subsequent @c RKPDirectives.
     */
    NON_BLOCKING,

    /**
     * Handling of an @c RKPDirective with this @c BlockingPolicy blocks the handling of subsequent @c RKPDirectives
     * that have the same @c DialogRequestId.
     */
    BLOCKING,

    /**
     * Handling of an @c RKPDirective with this @c BlockingPolicy is done immediately and does NOT block the handling of
     * subsequent @c RKPDirectives.
     */
    HANDLE_IMMEDIATELY,

    /**
     * BlockingPolicy not specified.
     */
    NONE
};

/**
 * Write a @c BlockingPolicy value to an @c ostream as a string.
 *
 * @param stream The stream to write the value to.
 * @param policy The policy value to write to the @c ostream as a string.
 * @return The @c ostream that was passed in and written to.
 */
inline std::ostream& operator<<(std::ostream& stream, BlockingPolicy policy) {
    switch (policy) {
        case BlockingPolicy::NON_BLOCKING:
            stream << "NON_BLOCKING";
            break;
        case BlockingPolicy::BLOCKING:
            stream << "BLOCKING";
            break;
        case BlockingPolicy::HANDLE_IMMEDIATELY:
            stream << "HANDLE_IMMEDIATELY";
            break;
        case BlockingPolicy::NONE:
            stream << "NONE";
            break;
    }
    return stream;
}

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_BLOCKINGPOLICY_H_
