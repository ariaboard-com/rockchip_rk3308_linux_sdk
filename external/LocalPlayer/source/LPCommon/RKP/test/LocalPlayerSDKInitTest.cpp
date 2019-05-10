/*
 * LocalPlayerSDKInitTest.cpp
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

/// @file LocalPlayerSDKInitTest.cpp

#include <sstream>
#include <gtest/gtest.h>

#include "LPCommon/RKP/Initialization/LocalPlayerSDKInit.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace initialization {
namespace test {

using namespace std;

/**
 * Tests @c initialize without any JSON configuration, expecting it to return @c true.
 *
 * @note This test also validates whether libcurl supports HTTP2.
 */
TEST(LocalPlayerSDKInitTest, initializeNoJSONConfig) {
    ASSERT_TRUE(LocalPlayerSDKInit::initialize({}));
    LocalPlayerSDKInit::uninitialize();
}

/**
 * Tests @c initialize with an invalid JSON configuration, expecting it to return @c false.
 *
 * @note This test also validates whether libcurl supports HTTP2.
 */
TEST(LocalPlayerSDKInitTest, initializeInvalidJSONConfig) {
    stringstream invalidJSON;
    invalidJSON << "{";
    ASSERT_FALSE(LocalPlayerSDKInit::initialize({&invalidJSON}));
}

/**
 * Tests @c initialize with a valid JSON configuration, expecting it to return @c true.
 *
 * @note This test also validates whether libcurl supports HTTP2.
 */
TEST(LocalPlayerSDKInitTest, initializeValidJSONConfig) {
    stringstream validJSON;
    validJSON << R"({"key":"value"})";
    ASSERT_TRUE(LocalPlayerSDKInit::initialize({&validJSON}));
    LocalPlayerSDKInit::uninitialize();
}

/**
 * Tests @c isInitialized when the SDK has not been initialized yet, expecting it to return @c false.
 */
TEST(LocalPlayerSDKInitTest, uninitializedIsInitialized) {
    ASSERT_FALSE(LocalPlayerSDKInit::isInitialized());
}

/**
 * Tests @c isInitialized when the SDK is initialized, expecting it to return @c true.
 */
TEST(LocalPlayerSDKInitTest, isInitialized) {
    ASSERT_TRUE(LocalPlayerSDKInit::initialize({}));
    // Expect used to ensure we uninitialize.
    EXPECT_TRUE(LocalPlayerSDKInit::isInitialized());
    LocalPlayerSDKInit::uninitialize();
}

/**
 * Tests @c uninitialize when the SDK has not been initialized yet, expecting no crashes or exceptions.
 */
TEST(LocalPlayerSDKInitTest, uninitialize) {
    LocalPlayerSDKInit::uninitialize();
}

}  // namespace test
}  // namespace initialization
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
