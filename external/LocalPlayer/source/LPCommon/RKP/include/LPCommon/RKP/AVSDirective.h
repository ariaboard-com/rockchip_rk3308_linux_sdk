/*
 * RKPDirective.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPDIRECTIVE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPDIRECTIVE_H_

#include <memory>
#include <string>

#include "Attachment/AttachmentManagerInterface.h"
#include "RKPMessage.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * A class representation of the RKP directive.
 */
class RKPDirective : public RKPMessage {
public:
    /**
     * Create an RKPDirective object with the given @c rkpMessageHeader, @c payload and @c attachmentManager.
     *
     * @param unparsedDirective The unparsed directive JSON string from RKP.
     * @param rkpMessageHeader The header fields of the directive.
     * @param payload The payload of the directive.
     * @param attachmentManager The attachment manager.
     * @param attachmentContextId The contextId required to get attachments from the AttachmentManager.
     * @return The created RKPDirective object or @c nullptr if creation failed.
     */
    static std::unique_ptr<RKPDirective> create(
        const std::string& unparsedDirective,
        std::shared_ptr<RKPMessageHeader> rkpMessageHeader,
        const std::string& payload,
        std::shared_ptr<rkpCommon::rkp::attachment::AttachmentManagerInterface> attachmentManager,
        const std::string& attachmentContextId);

    /**
     * Returns a reader for the attachment associated with this directive.
     *
     * @param contentId The contentId associated with the attachment.
     * @param readerPolicy The policy with which to create the @c AttachmentReader.
     * @return An attachment reader or @c nullptr if no attachment was found with the given @c contentId.
     */
    std::unique_ptr<rkpCommon::rkp::attachment::AttachmentReader> getAttachmentReader(
        const std::string& contentId,
        rkpCommon::rkp::attachment::AttachmentReader::Policy readerPolicy) const;

    /**
     * Returns the underlying unparsed directive.
     */
    std::string getUnparsedDirective() const;

private:
    /**
     * Constructor.
     *
     * @param unparsedDirective The unparsed directive JSON string from RKP.
     * @param rkpMessageHeader The object representation of an RKP message header.
     * @param payload The payload of an RKP message.
     * @param attachmentManager The attachment manager object.
     * @param attachmentContextId The contextId required to get attachments from the AttachmentManager.
     */
    RKPDirective(
        const std::string& unparsedDirective,
        std::shared_ptr<RKPMessageHeader> rkpMessageHeader,
        const std::string& payload,
        std::shared_ptr<rkpCommon::rkp::attachment::AttachmentManagerInterface> attachmentManager,
        const std::string& attachmentContextId);

    /// The unparsed directive JSON string from RKP.
    const std::string m_unparsedDirective;
    /// The attachmentManager.
    std::shared_ptr<rkpCommon::rkp::attachment::AttachmentManagerInterface> m_attachmentManager;
    /// The contextId needed to acquire the right attachment from the attachmentManager.
    std::string m_attachmentContextId;
};

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPDIRECTIVE_H_
