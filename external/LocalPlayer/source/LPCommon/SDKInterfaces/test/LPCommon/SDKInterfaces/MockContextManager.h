/*
 * MockContextManager.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKCONTEXTMANAGER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKCONTEXTMANAGER_H_

#include "LPCommon/SDKInterfaces/ContextManagerInterface.h"
#include <gmock/gmock.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {
namespace test {

/// Mock class that implements the ContextManager.
class MockContextManager : public ContextManagerInterface {
public:
    MOCK_METHOD0(doShutdown, void());
    MOCK_METHOD2(
        setStateProvider,
        void(const rkp::NamespaceAndName& namespaceAndName, std::shared_ptr<StateProviderInterface> stateProvider));
    MOCK_METHOD4(
        setState,
        SetStateResult(
            const rkp::NamespaceAndName& namespaceAndName,
            const std::string& jsonState,
            const rkp::StateRefreshPolicy& refreshPolicy,
            const unsigned int stateRequestToken));
    MOCK_METHOD1(getContext, void(std::shared_ptr<ContextRequesterInterface> contextRequester));
};

}  // namespace test
}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_TEST_RKPCOMMON_SDKINTERFACES_MOCKCONTEXTMANAGER_H_
