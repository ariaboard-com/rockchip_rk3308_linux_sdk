/*
 * AttachmentWriter.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_ATTACHMENTWRITER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_ATTACHMENTWRITER_H_

#include <cstddef>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace attachment {

/**
 * A class that provides functionality to write data to an @c Attachment.
 */
class AttachmentWriter {
public:
    /**
     * An enum class to communicate the possible states following a @c write() call.
     */
    enum class WriteStatus {
        /// Things are ok.
        OK,
        /// The underlying data representation is no longer writeable.
        CLOSED,
        /// The write could not succeed due to the underlying buffer being full.
        OK_BUFFER_FULL,
        /// The number of bytes in the request is smaller than the word-size of the underlying data representation.
        ERROR_BYTES_LESS_THAN_WORD_SIZE,
        /// A non-specified error occurred.
        ERROR_INTERNAL
    };

    /**
     * Destructor.
     */
    virtual ~AttachmentWriter() = default;

    /**
     * The write function.
     *
     * @param buf The buffer where data should be copied from.
     * @param numBytes The size of the buffer in bytes.
     * @param[out] writeStatus The out-parameter where the resulting state of the write will be expressed.
     * @return The number of bytes written as a result of this call.
     */
    virtual std::size_t write(const void* buf, std::size_t numBytes, WriteStatus* writeStatus) = 0;

    /**
     * The close function.  An implementation will take care of any resource management when a writer no longer
     * needs to use an attachment.
     */
    virtual void close() = 0;
};

}  // namespace attachment
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_ATTACHMENT_ATTACHMENTWRITER_H_
