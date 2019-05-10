/*
 * InProcessAttachment.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_INPROCESSATTACHMENT_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_INPROCESSATTACHMENT_H_

#include "LPCommon/RKP/Attachment/Attachment.h"
#include "LPCommon/RKP/Attachment/InProcessAttachmentReader.h"
#include "LPCommon/RKP/Attachment/InProcessAttachmentWriter.h"

#include "LPCommon/Utils/SDS/InProcessSDS.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace attachment {

/**
 * A class that represents an RKP attachment following an in-process memory management model.
 */
class InProcessAttachment : public Attachment {
public:
    /// Type aliases for convenience.
    using SDSType = rkpCommon::utils::sds::InProcessSDS;
    using SDSBufferType = rkpCommon::utils::sds::InProcessSDSTraits::Buffer;

    /// Default size of underlying SDS when created internally.
    static const int SDS_BUFFER_DEFAULT_SIZE_IN_BYTES = 0x100000;

    /**
     * Constructor.
     *
     * @param id The attachment id.
     * @param sds The underlying @c SharedDataStream object.  If not specified, then this class will create its own.
     */
    InProcessAttachment(const std::string& id, std::unique_ptr<SDSType> sds = nullptr);

    std::unique_ptr<AttachmentWriter> createWriter() override;

    std::unique_ptr<AttachmentReader> createReader(AttachmentReader::Policy policy) override;

private:
    // The sds from which we will create the reader and writer.
    std::shared_ptr<SDSType> m_sds;
};

}  // namespace attachment
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_INPROCESSATTACHMENT_H_
