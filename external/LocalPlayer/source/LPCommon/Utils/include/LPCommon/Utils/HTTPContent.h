/*
 * HTTPContent.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_HTTPCONTENT_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_HTTPCONTENT_H_

#include <future>
#include <memory>
#include <string>

#include <LPCommon/RKP/Attachment/InProcessAttachment.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {

/**
 * This struct encapsulates content received from HTTP request, specifically the status code, the content-type, and the
 * actual content of the response.
 */
struct HTTPContent {
    /**
     * This function blocks until @c statusCode is set and checks whether it is equal to 200, indicating an HTTP sucess
     * code.
     *
     * @return @c true if `statuscode == 200`, else @c false.
     */
    operator bool() const;

    /// A @c long representing the HTTP status code.
    mutable std::future<long> statusCode;

    /// A @c string representing the content-type of the HTTP content.
    std::future<std::string> contentType;

    /// An @c Attachment from which to read the HTTP content from or @c nullptr if no data was fetched.
    std::shared_ptr<rkpCommon::rkp::attachment::InProcessAttachment> dataStream;
};

inline HTTPContent::operator bool() const {
    return statusCode.get() == 200;
}

}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_HTTPCONTENT_H_
