/*
 * NamespaceAndName.cpp
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

#include "LPCommon/Utils/functional/hash.h"
#include "LPCommon/RKP/NamespaceAndName.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

NamespaceAndName::NamespaceAndName(const std::string& nameSpaceIn, const std::string& nameIn) :
        nameSpace{nameSpaceIn},
        name{nameIn} {
}

bool operator==(const NamespaceAndName& lhs, const NamespaceAndName& rhs) {
    return std::tie(lhs.nameSpace, lhs.name) == std::tie(rhs.nameSpace, rhs.name);
}

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

namespace std {

size_t hash<localPlayerSDK::rkpCommon::rkp::NamespaceAndName>::operator()(
    const localPlayerSDK::rkpCommon::rkp::NamespaceAndName& in) const {
    std::size_t seed = 0;
    localPlayerSDK::rkpCommon::utils::functional::hashCombine(seed, in.nameSpace);
    localPlayerSDK::rkpCommon::utils::functional::hashCombine(seed, in.name);
    return seed;
};

}  // namespace std
