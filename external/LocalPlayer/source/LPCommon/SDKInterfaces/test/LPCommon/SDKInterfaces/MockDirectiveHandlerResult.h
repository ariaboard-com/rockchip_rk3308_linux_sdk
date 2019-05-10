/*
 * MockDirectiveHandlerResult.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKDIRECTIVEHANDLERRESULT_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKDIRECTIVEHANDLERRESULT_H_

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "LPCommon/SDKInterfaces/DirectiveHandlerResultInterface.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {
namespace test {

/// Mock @c DirectiveHandlerResultInterface implementation.
class MockDirectiveHandlerResult : public DirectiveHandlerResultInterface {
public:
    MOCK_METHOD0(setCompleted, void());
    MOCK_METHOD1(setFailed, void(const std::string& description));
};

}  // namespace test
}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKDIRECTIVEHANDLERRESULT_H_
