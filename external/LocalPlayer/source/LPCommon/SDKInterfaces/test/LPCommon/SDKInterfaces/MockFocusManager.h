/*
 * MockFocusManager.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKFOCUSMANAGER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKFOCUSMANAGER_H_

#include "LPCommon/SDKInterfaces/FocusManagerInterface.h"
#include <gmock/gmock.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {
namespace test {

/// Mock class that implements the FocusManager.
class MockFocusManager : public FocusManagerInterface {
public:
    MOCK_METHOD3(
        acquireChannel,
        bool(
            const std::string& channelName,
            std::shared_ptr<rkpCommon::sdkInterfaces::ChannelObserverInterface> channelObserver,
            const std::string& activityId));
    MOCK_METHOD2(
        releaseChannel,
        std::future<bool>(
            const std::string& channelName,
            std::shared_ptr<rkpCommon::sdkInterfaces::ChannelObserverInterface> channelObserver));
    MOCK_METHOD0(stopForegroundActivity, void());
};

}  // namespace test
}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKFOCUSMANAGER_H_
