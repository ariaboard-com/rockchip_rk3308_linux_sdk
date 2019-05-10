/*
 * HTTPContentFetcherFactory.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_HTTPCONTENTFETCHERFACTORY_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_HTTPCONTENTFETCHERFACTORY_H_

#include <memory>
#include <string>

#include <LPCommon/SDKInterfaces/HTTPContentFetcherInterface.h>
#include <LPCommon/SDKInterfaces/HTTPContentFetcherInterfaceFactoryInterface.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace libcurlUtils {

/**
 * A class that produces @c HTTPContentFetchers.
 */
class HTTPContentFetcherFactory : public rkpCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface {
public:
    std::unique_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterface> create(const std::string& url) override;
};

}  // namespace libcurlUtils
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_HTTPCONTENTFETCHERFACTORY_H_
