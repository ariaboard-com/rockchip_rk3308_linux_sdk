/*
 * HandlerAndPolicy.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_HANDLERANDPOLICY_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_HANDLERANDPOLICY_H_

#include <memory>
#include "LPCommon/SDKInterfaces/DirectiveHandlerInterface.h"
#include "BlockingPolicy.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * Conjoined @c DirectiveHandler and @c BlockingPolicy values.
 */
class HandlerAndPolicy {
public:
    /**
     * Constructor to initialize with default property values.
     */
    HandlerAndPolicy();

    /**
     * Constructor to initialize with specific property values.
     *
     * @param handlerIn The @c RKPDirectiveHandlerInterface value for this instance.
     * @param policyIn The @c BlockingPolicy value for this instance.
     */
    HandlerAndPolicy(
        std::shared_ptr<rkpCommon::sdkInterfaces::DirectiveHandlerInterface> handlerIn,
        BlockingPolicy policyIn);

    /**
     * Return whether this instance specifies a non-null directive handler and a non-NONE BlockingPolicy.
     *
     * @return Whether this instance specifies a non-null directive handler and a non-NONE BlockingPolicy.
     */
    operator bool() const;

    /// The @c DirectiveHandlerInterface value for this instance.
    std::shared_ptr<rkpCommon::sdkInterfaces::DirectiveHandlerInterface> handler;

    /// The @c BlockingPolicy value for this instance.
    BlockingPolicy policy;
};

/**
 * == operator.
 *
 * @param lhs The HandlerAndPolicy instance on the left hand side of the == operation.
 * @param rhs The HandlerAndPolicy instance on the right hand side of the == operation.
 * @return Whether the @c lhs instance is equal to the @c rhs instance.
 */
bool operator==(const HandlerAndPolicy& lhs, const HandlerAndPolicy& rhs);

/**
 * != operator.
 *
 * @param lhs The HandlerAndPolicy instance on the left hand side of the == operation.
 * @param rhs The HandlerAndPolicy instance on the right hand side of the != operation.
 * @return Whether the @c lhs instance is NOT equal to the @c rhs instance.
 */
bool operator!=(const HandlerAndPolicy& lhs, const HandlerAndPolicy& rhs);

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_HANDLERANDPOLICY_H_
