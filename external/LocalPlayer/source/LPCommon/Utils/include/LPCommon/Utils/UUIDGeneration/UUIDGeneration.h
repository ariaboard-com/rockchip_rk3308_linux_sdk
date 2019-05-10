/*
 * UUIDGeneration.h
 *
 * Copyright 2018 Rockchip.com, Inc. or its affiliates.
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_UUIDGENERATION_UUIDGENERATION_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_UUIDGENERATION_UUIDGENERATION_H_

#include <string>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace uuidGeneration {

/**
 * Generates a variant 1, version 4 universally unique identifier (UUID) consisting of 32 hexadecimal digits.
 * The UUID generated is of the format xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx where M indicates the version, and the two
 * most significant bits of N indicates the variant. M is 0100 (binary) for version 4 and N is 10xx(binary) for
 * variant 1.
 * @see https://tools.ietf.org/html/rfc4122.
 *
 * @return A uuid as a string.
 */
const std::string generateUUID();

}  // namespace uuidGeneration
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_UUIDGENERATION_UUIDGENERATION_H_
