/*
 * DirectiveHandlerConfiguration.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_DIRECTIVEHANDLERCONFIGURATION_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_DIRECTIVEHANDLERCONFIGURATION_H_

#include <unordered_map>

#include "NamespaceAndName.h"
#include "BlockingPolicy.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * Map of @c NamespaceAndName values to @c BlockingPolicy with which to register a @c DirectiveHandlerInterface to a @c
 * DirectiveSequencer.
 */
using DirectiveHandlerConfiguration = std::unordered_map<NamespaceAndName, BlockingPolicy>;

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_DIRECTIVEHANDLERCONFIGURATION_H_
