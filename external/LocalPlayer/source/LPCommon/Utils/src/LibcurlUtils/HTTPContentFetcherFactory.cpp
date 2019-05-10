/*
 * HTTPContentFetcherFactory.cpp
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
#include <LPCommon/Utils/LibcurlUtils/HTTPContentFetcherFactory.h>
#include <LPCommon/Utils/LibcurlUtils/LibCurlHttpContentFetcher.h>
#include <LPCommon/Utils/Memory/Memory.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace libcurlUtils {

std::unique_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterface> HTTPContentFetcherFactory::create(
    const std::string& url) {
    return rkpCommon::utils::memory::make_unique<LibCurlHttpContentFetcher>(url);
}

}  // namespace libcurlUtils
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
