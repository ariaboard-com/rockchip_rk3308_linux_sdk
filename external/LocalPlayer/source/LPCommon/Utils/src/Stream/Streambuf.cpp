/*
 * Streambuf.cpp
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

#include "LPCommon/Utils/Stream/Streambuf.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace stream {

// There are two casts, as a streambuf uses Type=char.  This requires removing the const and removing the unsigned.
// setg only is for reading, so this operation is safe, although ugly.
Streambuf::Streambuf(const unsigned char* data, size_t length) :
        m_begin(reinterpret_cast<char*>(const_cast<unsigned char*>(data))),
        m_current(m_begin),
        m_end(m_begin + length) {
    setg(m_begin, m_current, m_end);
}

std::streampos Streambuf::seekoff(std::streamoff off, std::ios_base::seekdir way, std::ios_base::openmode which) {
    switch (way) {
        case std::ios_base::beg:
            setg(m_begin, m_begin + off, m_end);
            break;
        case std::ios_base::cur:
            gbump(off);
            break;
        case std::ios_base::end:
            setg(m_begin, m_end + off, m_end);
            break;
        default:
            return std::streampos(std::streamoff(-1));
    }

    return UpdateAndValidate();
}

std::streampos Streambuf::seekpos(std::streampos sp, std::ios_base::openmode which) {
    return seekoff(sp - pos_type(off_type(0)), std::ios_base::beg, which);
}

Streambuf::int_type Streambuf::underflow() {
    m_current = gptr();
    if (m_current == m_end) {
        return Streambuf::traits_type::eof();
    }
    return Streambuf::traits_type::to_int_type(*m_current);
}

Streambuf::int_type Streambuf::uflow() {
    ++m_current;
    setg(m_begin, m_current, m_end);
    return underflow();
}

Streambuf::int_type Streambuf::pbackfail(int_type ch) {
    if (m_current == m_begin || (ch != Streambuf::traits_type::eof() && ch != m_current[-1])) {
        return Streambuf::traits_type::eof();
    }
    return Streambuf::traits_type::to_int_type(*--m_current);
}

std::streamsize Streambuf::showmanyc() {
    return m_end - m_current;
}

std::streampos Streambuf::UpdateAndValidate() {
    m_current = gptr();
    if (!gptr() || gptr() >= egptr() || gptr() < eback()) {
        return std::streampos(std::streamoff(-1));
    }

    return gptr() - eback();
}

}  // namespace stream
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
