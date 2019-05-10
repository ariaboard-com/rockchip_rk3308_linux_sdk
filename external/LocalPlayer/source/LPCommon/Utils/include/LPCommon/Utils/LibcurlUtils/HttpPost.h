/*
 * HttpPost.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_HTTPPOST_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_HTTPPOST_H_

#include <chrono>
#include <curl/curl.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>

#include "LPCommon/Utils/LibcurlUtils/HttpPostInterface.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace libcurlUtils {

/// LIBCURL based implementation of HttpPostInterface.
class HttpPost : public HttpPostInterface {
public:
    /// HttpPost destructor
    ~HttpPost();

    /**
     * Deleted copy constructor.
     *
     * @param rhs The 'right hand side' to not copy.
     */
    HttpPost(const HttpPost& rhs) = delete;

    /**
     * Deleted assignment operator.
     *
     * @param rhs The 'right hand side' to not copy.
     * @return The object assigned to.
     */
    HttpPost& operator=(const HttpPost& rhs) = delete;

    /**
     * Create a new HttpPost instance, passing ownership of the new instance on to the caller.
     *
     * @return Retruns an std::unique_ptr to the new HttpPost instance, or @c nullptr of the operation failed.
     */
    static std::unique_ptr<HttpPost> create();

    long doPost(const std::string& m_url, const std::string& data, std::chrono::seconds timeout, std::string& body)
        override;

private:
    /**
     * HttpPost constructor.
     *
     * @param curl CURL handle with which to make requests.
     */
    HttpPost();

    /**
     * init() is used by create() to perform initialization after construction but before returning the
     * HttpPost instance so that clients only get access to fully formed instances.
     *
     * @return @c true if initialization is successful.
     */
    bool init();

    /**
     * Helper function for calling curl_easy_setopt and checking the result.
     *
     * @param option The option parameter to pass through to curl_easy_setopt.
     * @param param The param option to pass through to curl_easy_setopt.
     * @return @c true of the operation was successful.
     */
    template <typename ParamType>
    bool setopt(CURLoption option, ParamType param);

    /**
     * Callback function used to accumulate the body of the HTTP Post response
     * This is called when doPost() is holding @c m_mutex.
     *
     * @param ptr Pointer to the first/next block of received bytes.
     * @param size count of 'nmemb' sized chunks of pointed to by 'ptr'.
     * @param nmemb count of bytes in each chunk received.
     * @param userdata Our 'this' pointer passed through by libcurl.
     * @return The number of bytes processed (size*nmemb upon success).
     */
    static size_t staticWriteCallbackLocked(char* ptr, size_t size, size_t nmemb, void* userdata);

    /// Mutex to serialize access to @c m_curl and @c m_response.
    std::mutex m_mutex;

    /// CURL handle with which to make requests
    CURL* m_curl;

    /// String used to accumuate the response body.
    std::string m_bodyAccumulator;
};

}  // namespace libcurlUtils
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_HTTPPOST_H_
