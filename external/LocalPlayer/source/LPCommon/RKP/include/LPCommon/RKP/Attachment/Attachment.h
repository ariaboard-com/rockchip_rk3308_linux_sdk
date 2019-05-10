/*
 * Attachment.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_ATTACHMENT_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_ATTACHMENT_H_

#include <atomic>
#include <mutex>
#include <string>

#include "LPCommon/RKP/Attachment/AttachmentReader.h"
#include "LPCommon/RKP/Attachment/AttachmentWriter.h"
#include "LPCommon/Utils/SDS/InProcessSDS.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace attachment {

/**
 * A class that represents an RKP attachment.
 */
class Attachment {
public:
    /**
     * Constructor.
     *
     * @param attachmentId The id for this attachment.
     */
    Attachment(const std::string& attachmentId);

    /**
     * Destructor.
     */
    virtual ~Attachment() = default;

    /**
     * Creates a writer object, with which the Attachment may be written to.
     *
     * @return a @unique_ptr to an AttachmentWriter.
     */
    virtual std::unique_ptr<AttachmentWriter> createWriter() = 0;

    /**
     * Creates a reader object, with which the Attachment may be read from.
     *
     * @param The policy used to configure the reader.
     * @return a @unique_ptr to an AttachmentReader.
     * */
    virtual std::unique_ptr<AttachmentReader> createReader(AttachmentReader::Policy policy) = 0;

    /**
     * An accessor to get the attachmentId.
     *
     * @return The attachment Id.
     */
    std::string getId() const;

    /**
     * Utility function to tell if a reader has been created for this object.
     *
     * @return Whether a reader has been created for this object.
     */
    bool hasCreatedReader();

    /**
     * Utility function to tell if a writer has been created for this object.
     *
     * @return Whether a writer has been created for this object.
     */
    bool hasCreatedWriter();

protected:
    /// The id for this attachment object.
    const std::string m_id;
    /// mutex to protext access to the createReader and createWriter API.
    std::mutex m_mutex;
    /// An atomic tracking variable to tell whether this object has created a writer.
    std::atomic<bool> m_hasCreatedWriter;
    /// An atomic tracking variable to tell whether this object has created a reader.
    std::atomic<bool> m_hasCreatedReader;
};

}  // namespace attachment
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_ATTACHMENT_H_
