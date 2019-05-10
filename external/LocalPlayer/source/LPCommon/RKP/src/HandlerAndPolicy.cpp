/*
 * HandlerAndPolicy.cpp
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

#include <tuple>

#include "LPCommon/RKP/HandlerAndPolicy.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

using namespace sdkInterfaces;

HandlerAndPolicy::HandlerAndPolicy() : policy{BlockingPolicy::NONE} {
}

HandlerAndPolicy::HandlerAndPolicy(std::shared_ptr<DirectiveHandlerInterface> handlerIn, BlockingPolicy policyIn) :
        handler{handlerIn},
        policy{policyIn} {
}

HandlerAndPolicy::operator bool() const {
    return handler && (policy != BlockingPolicy::NONE);
}

bool operator==(const HandlerAndPolicy& lhs, const HandlerAndPolicy& rhs) {
    return std::tie(lhs.handler, lhs.policy) == std::tie(rhs.handler, rhs.policy);
}

bool operator!=(const HandlerAndPolicy& lhs, const HandlerAndPolicy& rhs) {
    return !(lhs == rhs);
}

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
