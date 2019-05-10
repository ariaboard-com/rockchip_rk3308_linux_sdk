/*
 * InProcessAttachmentReader.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_INPROCESSATTACHMENTREADER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_INPROCESSATTACHMENTREADER_H_

#include "LPCommon/Utils/SDS/InProcessSDS.h"
#include "LPCommon/Utils/SDS/Reader.h"

#include "AttachmentReader.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace attachment {

/**
 * A class that provides functionality to read data from an @c Attachment following an in-process memory management
 * model.
 *
 * @note This class is not thread-safe beyond the thread-safety provided by the underlying SharedDataStream object.
 */
class InProcessAttachmentReader : public AttachmentReader {
public:
    /// Type aliases for convenience.
    using SDSType = rkpCommon::utils::sds::InProcessSDS;
    using SDSTypeIndex = rkpCommon::utils::sds::InProcessSDS::Index;
    using SDSTypeReader = SDSType::Reader;

    /**
     * Create an InProcessAttachmentReader.
     *
     * @param policy The @c AttachmentReader::Policy of this object.
     * @param sds The underlying @c SharedDataStream which this object will use.
     * @param index If being constructed from an existing @c SharedDataStream, the index indicates where to read from.
     * @param reference The position in the stream @c offset is applied to.  This parameter defaults to 0, indicating
     *     no offset from the specified reference.
     * @return Returns a new InProcessAttachmentReader, or nullptr if the operation failed.  This parameter defaults
     *     to @c ABSOLUTE, indicating offset is relative to the very beginning of the Attachment.
     */
    static std::unique_ptr<InProcessAttachmentReader> create(
        Policy policy,
        std::shared_ptr<SDSType> sds,
        SDSTypeIndex offset = 0,
        SDSTypeReader::Reference reference = SDSTypeReader::Reference::ABSOLUTE);

    /**
     * Destructor.
     */
    ~InProcessAttachmentReader();

    std::size_t read(
        void* buf,
        std::size_t numBytes,
        ReadStatus* readStatus,
        std::chrono::milliseconds timeoutMs = std::chrono::milliseconds(0)) override;

    void close(ClosePoint closePoint = ClosePoint::AFTER_DRAINING_CURRENT_BUFFER) override;

    bool seek(uint64_t offset) override;

private:
    /**
     * Constructor.
     *
     * @param policy The @c AttachmentReader::Policy of this object.
     * @param sds The underlying @c SharedDataStream which this object will use.
     */
    InProcessAttachmentReader(Policy policy, std::shared_ptr<SDSType> sds);

    /// The underlying @c SharedDataStream reader.
    std::shared_ptr<SDSTypeReader> m_reader;
};

}  // namespace attachment
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_INPROCESSATTACHMENTREADER_H_
