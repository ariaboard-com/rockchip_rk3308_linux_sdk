/*
 * RKPDirective.cpp
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

#include "LPCommon/RKP/RKPDirective.h"
#include "LPCommon/Utils/Logger/Logger.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

using namespace rkpCommon::utils;
using namespace rkpCommon::rkp::attachment;

/// String to identify log entries originating from this file.
static const std::string TAG("AvsDirective");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

std::unique_ptr<RKPDirective> RKPDirective::create(
    const std::string& unparsedDirective,
    std::shared_ptr<RKPMessageHeader> rkpMessageHeader,
    const std::string& payload,
    std::shared_ptr<AttachmentManagerInterface> attachmentManager,
    const std::string& attachmentContextId) {
    if (!rkpMessageHeader) {
        ACSDK_ERROR(LX("createFailed").d("reason", "nullMessageHeader"));
        return nullptr;
    }
    if (!attachmentManager) {
        ACSDK_ERROR(LX("createFailed").d("reason", "nullAttachmentManager"));
        return nullptr;
    }
    return std::unique_ptr<RKPDirective>(
        new RKPDirective(unparsedDirective, rkpMessageHeader, payload, attachmentManager, attachmentContextId));
}

std::unique_ptr<AttachmentReader> RKPDirective::getAttachmentReader(
    const std::string& contentId,
    AttachmentReader::Policy readerPolicy) const {
    auto attachmentId = m_attachmentManager->generateAttachmentId(m_attachmentContextId, contentId);
    return m_attachmentManager->createReader(attachmentId, readerPolicy);
}

RKPDirective::RKPDirective(
    const std::string& unparsedDirective,
    std::shared_ptr<RKPMessageHeader> rkpMessageHeader,
    const std::string& payload,
    std::shared_ptr<AttachmentManagerInterface> attachmentManager,
    const std::string& attachmentContextId) :
        RKPMessage{rkpMessageHeader, payload},
        m_unparsedDirective{unparsedDirective},
        m_attachmentManager{attachmentManager},
        m_attachmentContextId{attachmentContextId} {
}

std::string RKPDirective::getUnparsedDirective() const {
    return m_unparsedDirective;
}

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
