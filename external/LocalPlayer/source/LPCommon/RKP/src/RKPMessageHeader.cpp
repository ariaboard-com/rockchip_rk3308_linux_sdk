/*
 * RKPMessageHeader.cpp
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

#include "LPCommon/RKP/RKPMessageHeader.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

std::string RKPMessageHeader::getNamespace() const {
    return m_namespace;
}

std::string RKPMessageHeader::getName() const {
    return m_name;
}

std::string RKPMessageHeader::getMessageId() const {
    return m_messageId;
}

std::string RKPMessageHeader::getDialogRequestId() const {
    return m_dialogRequestId;
}

std::string RKPMessageHeader::getAsString() const {
    // clang-format off
    return std::string() +
           "{\"namespace:\"" + m_namespace +
           "\",name:\"" + m_name +
           "\",messageId:\"" + m_messageId +
           "\",dialogRequestId:\"" + m_dialogRequestId +
           "\"}";
    // clang-format on
}

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
