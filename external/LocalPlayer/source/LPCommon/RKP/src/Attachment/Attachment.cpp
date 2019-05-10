/*
 * Attachment.cpp
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

#include "LPCommon/RKP/Attachment/Attachment.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace attachment {

Attachment::Attachment(const std::string& attachmentId) :
        m_id{attachmentId},
        m_hasCreatedWriter{false},
        m_hasCreatedReader{false} {
}

std::string Attachment::getId() const {
    return m_id;
}

bool Attachment::hasCreatedReader() {
    return m_hasCreatedReader;
}

bool Attachment::hasCreatedWriter() {
    return m_hasCreatedWriter;
}

}  // namespace attachment
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
