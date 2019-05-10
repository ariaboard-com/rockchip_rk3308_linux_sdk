/*
 * Streambuf.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_STREAM_STREAMBUF_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_STREAM_STREAMBUF_H_

#include <sstream>
#include <string>

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace stream {

/**
 * This class takes byte array and makes a non-copying std::streambuf out of it.
 */
class Streambuf : public std::streambuf {
public:
    /**
     * Streambuf
     *
     * @param data the beginning of the byte array
     * @param length the size of the byte array
     */
    Streambuf(const unsigned char* data, size_t length);

    std::streampos seekoff(
        std::streamoff off,
        std::ios_base::seekdir way,
        std::ios_base::openmode which = std::ios_base::in) override;
    std::streampos seekpos(std::streampos sp, std::ios_base::openmode which = std::ios_base::in) override;

private:
    int_type underflow() override;
    int_type uflow() override;
    int_type pbackfail(int_type ch) override;
    std::streamsize showmanyc() override;

    /**
     * This function makes sure that the requested operation is valid and does the update.
     *
     * @return the position in the stream
     */
    std::streampos UpdateAndValidate();

    char* const m_begin;
    char* m_current;
    char* const m_end;
};

}  // namespace stream
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_UTILS_INCLUDE_RKPCOMMON_UTILS_STREAM_STREAMBUF_H_
