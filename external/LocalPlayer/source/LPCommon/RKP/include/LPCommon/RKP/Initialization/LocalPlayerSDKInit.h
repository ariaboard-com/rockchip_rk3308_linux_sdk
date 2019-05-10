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
#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_INITIALIZATION_LOCALPLAYERSDKINIT_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_INITIALIZATION_LOCALPLAYERSDKINIT_H_

#include <atomic>
#include <iostream>
#include <vector>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace initialization {

class LocalPlayerSDKInit {
public:
    /*
     * Checks whether the Local Player SDK has been initialized.
     *
     * @return Whether the Local Player SDK has been initialized.
     */
    static bool isInitialized();

    /**
     * Initialize the Local Player SDK. This must be called before any Local Player SDK modules are created.
     *
     * This function must be called before any threads in the process have been created by the
     * program; this function is not thread safe. This requirement is present because initialize()
     * calls functions of other libraries that have the same requirements and thread safety.
     * terminate() must be called for each initialize() called.
     *
     * @param jsonConfigurationStreams Vector of @c istreams containing JSON documents from which
     * to parse configuration parameters. Streams are processed in the order they appear in the vector. When a
     * value appears in more than one JSON stream the last processed stream's value overwrites the previous value
     * (and a debug log entry will be created). This allows for specifying default settings (by providing them
     * first) and specifying the configuration from multiple sources (e.g. a separate stream for each component).
     * Documentation of the JSON configuration format and methods to access the resulting global configuration
     * can be found here: rkpCommon::utils::configuration::ConfigurationNode.
     *
     * @return Whether the initialization was successful.
     */
    static bool initialize(const std::vector<std::istream*>& jsonStreams);

    /**
     * Uninitialize the Local Player SDK.
     *
     * You should call uninitialize() once for each call you make to initialize(), after you are done
     * using the Local Player SDK.
     *
     * This function must be called when no other threads in the process are running. this function
     * is not thread safe. This requirement is present because uninitialize() calls functions of other
     * libraries that have the same requirements and thread safety.
     */
    static void uninitialize();

private:
    /// Tracks whether we've initialized the Local Player SDK or not
    static std::atomic_int g_isInitialized;
};

}  // namespace initialization
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_INITIALIZATION_LOCALPLAYERSDKINIT_H_
