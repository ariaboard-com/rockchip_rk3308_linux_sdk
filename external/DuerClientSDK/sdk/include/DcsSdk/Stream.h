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

#ifndef DUEROS_DCS_SDK_DCS_COMMON_DCS_INCLUDE_DCS_COMMON_DCS_STREAM_H_
#define DUEROS_DCS_SDK_DCS_COMMON_DCS_INCLUDE_DCS_COMMON_DCS_STREAM_H_

///#include <chrono>
#include <cstddef>
#include <string>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

const int kMaxStreamBufferSize = 512;

class Stream {
public:

    enum class StreamStatus {
        BUFFER_IDLE = 0,
        BUFFER_DATA,
        BUFFER_EOF,
        BUFFER_ERROR
    };

    virtual ~Stream() = default;

    virtual void beginWriteData() = 0;
    virtual void endWriteData() = 0;

    virtual void writeData(const char* data, int len) = 0;

    virtual StreamStatus readData(char* output_data, int& output_len) = 0;

    virtual void close() = 0;
};
}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_DCS_COMMON_DCS_INCLUDE_DCS_COMMON_DCS_STREAM_H_
