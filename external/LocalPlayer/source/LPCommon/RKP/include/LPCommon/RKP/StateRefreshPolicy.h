/*
 * StateRefreshPolicy.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_STATEREFRESHPOLICY_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_STATEREFRESHPOLICY_H_

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * An enum class used to specify the refresh policy for the state information provided by a @c stateProviderInterface.
 * The @c stateProviderInterface must specify the refresh policy when it updates its state via @c setState.
 */
enum class StateRefreshPolicy {
    /**
     * Indicates to the ContextManager that the state information need not be updated on a @c getContext request. The
     * @c stateProviderInterface is responsible for updating its state manually.
     */
    NEVER,

    /**
     * Indicates to the @c ContextManager that the stateProvider needs to be queried and the state refreshed every time
     * it processes a @c getContext request.
     */
    ALWAYS
};

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_STATEREFRESHPOLICY_H_
