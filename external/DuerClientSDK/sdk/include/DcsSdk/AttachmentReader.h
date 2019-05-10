/*
 * AttachmentReader.h
 *
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file AttachmentReader.h
 * \if english
 *     @brief The tts player can read the data from this interface.
 * \else
 *     @brief 应用层通过此接口，从SDK读取TTS数据。
 * \endif
 */

#ifndef DUEROS_DCS_SDK_DCS_COMMON_DCS_INCLUDE_DCS_COMMON_DCS_ATTACHMENT_ATTACHMENT_READER_H_
#define DUEROS_DCS_SDK_DCS_COMMON_DCS_INCLUDE_DCS_COMMON_DCS_ATTACHMENT_ATTACHMENT_READER_H_

#include <chrono>
#include <cstddef>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief A class that provides functionality to read data from an @c Attachment.
 * \else
 *     @brief 应用层通过此接口，从SDK读取TTS数据流。
 * \endif
 */
class AttachmentReader {
public:
    /**
     * \if english
     *     @brief An enum class to allow configuration of the type of reader.
     * \else
     *     @brief 枚举，用于决定read数据时是否阻塞。
     * \endif
     */
    enum class Policy {
        /**
         * \if english
         *     @brief A read of n bytes will not return until n bytes are available, or a timeout occurs.
         * \else
         *     @brief 当数据不够n字节时，阻塞read函数，或者超时时间到。
         * \endif
         */
        BLOCKING,
        /**
         * \if english
         *     @brief A read of n bytes will return immediately, whether n bytes were available or not.
         * \else
         *     @brief read函数直接返回，不管数据够不够n字节时。
         * \endif
         */
        NON_BLOCKING
    };

    /**
     * \if english
     *     @brief An enum class to communicate the possible states following a @c read() call.
     * \else
     *     @brief 枚举，伴随每次read调用返回。
     * \endif
     */
    enum class ReadStatus {
        /**
         * \if english
         *     @brief Things are ok.
         * \else
         *     @brief 正常状态。
         * \endif
         */
        OK,

        /**
         * \if english
         *     @brief On a request for n bytes, less than n bytes were available on a non-blocking read.
         * \else
         *     @brief 用于非阻塞read，当可用数据小于请求数据长度n，立即返回。
         * \endif
         */
        OK_WOULDBLOCK,

        /**
         * \if english
         *     @brief On a request for n bytes, less than n bytes were available on a blocking read.
         * \else
         *     @brief 用于阻塞read，当可用数据小于请求数据长度n，直到超时。
         * \endif
         */
        OK_TIMEDOUT,

        /**
         * \if english
         *     @brief The underlying data representation is no longer readable.
         * \else
         *     @brief 底层数据已关闭，读取数据无效。
         * \endif
         */
        CLOSED,

        /**
         * \if english
         *     @brief The writer has corrupted the reader data.  The attachment is no longer valid.
         * \else
         *     @brief 数据损坏，读取数据无效。
         * \endif
         */
        ERROR_OVERRUN,

        /**
         * \if english
         *     @brief The number of bytes in the request is smaller than the word-size of the underlying data representation.
         * \else
         *     @brief 请求数据长度小于最小数据单位。
         * \endif
         */
        ERROR_BYTES_LESS_THAN_WORD_SIZE,

        /**
         * \if english
         *     @brief A non-specified error occurred.
         * \else
         *     @brief 未定义错误。
         * \endif
         */
        ERROR_INTERNAL
    };

    /**
     * \if english
     *     @brief An enum class to indicate when the @c read() function should stop returning data after a call to @c close().
     * \else
     *     @brief 枚举，指定调用close函数后，read函数的行为。
     * \endif
     */
    enum class ClosePoint {
        /**
         * \if english
         *     @brief Stop returning data immediately after the @c close() call.
         * \else
         *     @brief 调用close函数后，read函数立即返回。
         * \endif
         */
        IMMEDIATELY,

        /**
         * \if english
         *     @brief Stop returning data when all of the data in the buffer at the time @c close() was called has been read.
         * \else
         *     @brief 调用close函数后，read函数持续返回，直到buffer没有可用数据。
         * \endif
         */
        AFTER_DRAINING_CURRENT_BUFFER
    };

    /**
     * \if english
     *     @brief Destructor.
     * \else
     *     @brief 析构函数。
     * \endif
     */
    virtual ~AttachmentReader() = default;

    /**
     * \if english
     *     @brief The read function.
     *     @param[in] buf The buffer where data should be copied to.
     *     @param[in] numBytes The size of the buffer in bytes.
     *     @param[out] readStatus The out-parameter where the resulting state of the read will be
     *     expressed.
     *     @param[in] timeoutMs The timeout for this read call in milliseconds.  This value is only
     *     used for the @c BLOCKING reader policy.
     *     @return The number of bytes read as a result of this call.
     * \else
     *     @brief read函数，通过此函数从SDK读取TTS数据流。
     *     @param[in] buf 目标buffer。
     *     @param[in] numBytes 请求数据长度，以字节为单位。
     *     @param[out] readStatus 读取状态返回，参考ReadStatus枚举。
     *     @param[in] timeoutMs 超时时间，以毫秒为单位，仅用于BLOCKING读取策略。
     *     @return 读取到的真实数据长度，以字节为单位。
     * \endif
     */
    virtual std::size_t read(
        void* buf,
        std::size_t numBytes,
        ReadStatus* readStatus,
        std::chrono::milliseconds timeoutMs = std::chrono::milliseconds(0)) = 0;

    /**
     * \if english
     *     @brief The close function.An implementation will take care of any resource
     *     management when a reader no longer needs to use an attachment.
     *     @param[in] closePoint The point at which the reader should stop reading from the
     *     attachment.
     * \else
     *     @brief 关闭函数，应用层需要认真管理相关资源。
     *     @param[in] closePoint 关闭策略，参考ClosePoint枚举。
     * \endif
     */
    virtual void close(ClosePoint closePoint = ClosePoint::AFTER_DRAINING_CURRENT_BUFFER) = 0;
};

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_DCS_COMMON_DCS_INCLUDE_DCS_COMMON_DCS_ATTACHMENT_ATTACHMENT_READER_H_
