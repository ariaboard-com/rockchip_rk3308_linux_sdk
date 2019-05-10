/*
 * EventBuilder.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_EVENTBUILDER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_EVENTBUILDER_H_

#include <string>
#include <utility>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * Builds a JSON event string which includes the header, the @c payload and an optional @c context.
 * The header includes the namespace, name, message Id and an optional @c dialogRequestId.
 * The message Id required for the header is a random string that is generated and added to the
 * header.
 *
 * @param eventName The name of the event to be include in the header.
 * @param dialogRequestIdString The value associated with the "dialogRequestId" key.
 * @param payload The payload value associated with the "payload" key.
 * @param context Optional @c context to be sent with the event message.
 * @return A pair object consisting of the messageId and the event JSON string if successful,
 * else a pair of empty strings.
 */
const std::pair<std::string, std::string> buildJsonEventString(
    const std::string& nameSpace,
    const std::string& eventName,
    const std::string& dialogRequestIdValue = "",
    const std::string& jsonPayloadValue = "{}",
    const std::string& jsonContext = "");

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_EVENTBUILDER_H_
