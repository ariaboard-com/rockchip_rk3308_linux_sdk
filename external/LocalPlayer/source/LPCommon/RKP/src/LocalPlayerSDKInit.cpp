/*
 * LocalPlayerSDKInit.h
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

#include <curl/curl.h>

#include "LPCommon/Utils/Configuration/ConfigurationNode.h"
#include "LPCommon/RKP/Initialization/LocalPlayerSDKInit.h"
#include "LPCommon/Utils/Logger/Logger.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace initialization {

/// String to identify log entries originating from this file.
static const std::string TAG("LocalPlayerSdkInit");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

/// Tracks whether we've initialized the Local Player SDK or not
std::atomic_int LocalPlayerSDKInit::g_isInitialized{0};

bool LocalPlayerSDKInit::isInitialized() {
    return g_isInitialized > 0;
}

bool LocalPlayerSDKInit::initialize(const std::vector<std::istream*>& jsonStreams) {
    if (!(curl_version_info(CURLVERSION_NOW)->features & CURL_VERSION_HTTP2)) {
        ACSDK_ERROR(LX("initializeFailed").d("reason", "curlDoesNotSupportHTTP2"));
        return false;
    }

    if (!utils::configuration::ConfigurationNode::initialize(jsonStreams)) {
        ACSDK_ERROR(LX("initializeFailed").d("reason", "ConfigurationNode::initializeFailed"));
        return false;
    }

    if (CURLE_OK != curl_global_init(CURL_GLOBAL_ALL)) {
        ACSDK_ERROR(LX("initializeFailed").d("reason", "curl_global_initFailed"));
        utils::configuration::ConfigurationNode::uninitialize();
        return false;
    }

    g_isInitialized++;
    return true;
}

void LocalPlayerSDKInit::uninitialize() {
    if (0 == g_isInitialized) {
        ACSDK_ERROR(LX("initializeError").d("reason", "notInitialized"));
        return;
    }
    g_isInitialized--;
    curl_global_cleanup();
    utils::configuration::ConfigurationNode::uninitialize();
}

}  // namespace initialization
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
