/*
 * HTTPContentFetcherInterface.h
 *
 * Copyright 2016-2018 Rockchip.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_HTTPCONTENTFETCHERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_HTTPCONTENTFETCHERINTERFACE_H_

#include <memory>

#include <LPCommon/Utils/HTTPContent.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * This class allows users to fetch content from remote location using HTTP(S).
 */
class HTTPContentFetcherInterface {
public:
    /// Represents what HTTP content to fetch.
    enum class FetchOptions {
        /// Retrieves the content type part of the HTTP header.
        CONTENT_TYPE,

        /// Retrieves the entire body of the remote location.
        ENTIRE_BODY
    };

    /**
     * Destructor.
     */
    virtual ~HTTPContentFetcherInterface() = default;

    /**
     * This function retrieves content from a remote location. No thread safety is guaranteed.
     *
     * @param option Flag indicating desired content.
     * @return A new @c HTTPContent object or @c nullptr if a failure occured.
     */
    virtual std::unique_ptr<rkpCommon::utils::HTTPContent> getContent(FetchOptions option) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_HTTPCONTENTFETCHERINTERFACE_H_
