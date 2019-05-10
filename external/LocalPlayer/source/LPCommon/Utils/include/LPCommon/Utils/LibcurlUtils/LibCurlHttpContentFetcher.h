/*
 * LibCurlHttpContentFetcher.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_LIBCURLHTTPCONTENTFETCHER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_LIBCURLHTTPCONTENTFETCHER_H_

#include <atomic>
#include <future>
#include <string>
#include <thread>

#include <LPCommon/SDKInterfaces/HTTPContentFetcherInterface.h>
#include <LPCommon/Utils/LibcurlUtils/CurlEasyHandleWrapper.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace libcurlUtils {

/**
 * A class used to retrieve content from remote URLs. Note that this object will only write to the Attachment while it
 * remains alive. If the object goes out of scope, writing to the Attachment will abort.
 */
class LibCurlHttpContentFetcher : public rkpCommon::sdkInterfaces::HTTPContentFetcherInterface {
public:
    LibCurlHttpContentFetcher(const std::string& url);

    /**
     * @copydoc
     * In this implementation, the function may only be called once. Subsequent calls will return @c nullptr.
     */
    std::unique_ptr<rkpCommon::utils::HTTPContent> getContent(FetchOptions fetchOption) override;

    /*
     * Destructor.
     */
    ~LibCurlHttpContentFetcher() override;

private:
    /// The callback to parse HTTP headers.
    static size_t headerCallback(char* data, size_t size, size_t nmemb, void* userData);

    /// The callback to parse HTTP bodies.
    static size_t bodyCallback(char* data, size_t size, size_t nmemb, void* userData);

    /// A no-op callback to not parse HTTP bodies.
    static size_t noopCallback(char* data, size_t size, size_t nmemb, void* userData);

    /// The URL to fetch from.
    std::string m_url;

    /// A libcurl wrapper.
    CurlEasyHandleWrapper m_curlWrapper;

    /// A promise to the caller of @c getContent() that the HTTP status code will be set.
    std::promise<long> m_statusCodePromise;

    /// A promise to the caller of @c getContent() that the HTTP content type will be set.
    std::promise<std::string> m_contentTypePromise;

    /**
     * A flag to indicate that the body callback has begun. This is used so that we know when header parsing has
     * finished and we can satisfy the promises.
     */
    bool m_bodyCallbackBegan;

    /**
     * The writer used to write the HTTP body to, if desired by the caller of @c getContent().
     */
    std::shared_ptr<rkpCommon::rkp::attachment::AttachmentWriter> m_streamWriter;

    /**
     * The last status code parsed in an HTTP response header. Since we follow redirects, we only want the last status
     * code.
     */
    long m_lastStatusCode;

    /**
     * The last content type parsed in an HTTP response header. Since we follow redirects, we only want the last content
     * type.
     */
    std::string m_lastContentType;

    /// Flag to indicate if a shutdown is occurring.
    std::atomic<bool> m_shuttingDown;

    /**
     * Internal thread that does the curl_easy_perform. The reason for using a thread is that curl_easy_perform may
     * block forever if the URL specified is a live stream.
     */
    std::thread m_thread;

    /// Flag to indicate that a call to @c getContent() has been made. Subsequent calls will not be accepted.
    std::atomic_flag m_hasObjectBeenUsed;
};

}  // namespace libcurlUtils
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_LIBCURLUTILS_LIBCURLHTTPCONTENTFETCHER_H_
