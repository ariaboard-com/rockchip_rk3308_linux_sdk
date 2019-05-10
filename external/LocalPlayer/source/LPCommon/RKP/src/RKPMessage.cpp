/*
 * RKPMessage.cpp
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

#include "LPCommon/RKP/RKPMessage.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

RKPMessage::RKPMessage(std::shared_ptr<RKPMessageHeader> rkpMessageHeader, std::string payload) :
        m_header{rkpMessageHeader},
        m_payload{std::move(payload)} {
}

std::string RKPMessage::getNamespace() const {
    return m_header->getNamespace();
}

std::string RKPMessage::getName() const {
    return m_header->getName();
}

std::string RKPMessage::getMessageId() const {
    return m_header->getMessageId();
}

std::string RKPMessage::getDialogRequestId() const {
    return m_header->getDialogRequestId();
}

std::string RKPMessage::getPayload() const {
    return m_payload;
}

std::string RKPMessage::getHeaderAsString() const {
    return m_header->getAsString();
}

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
