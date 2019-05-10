/*
 * StreamAndDataAreEqual.cpp
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

#include "LPCommon/Utils/Stream/StreamFunctions.h"

#include <sstream>
#include <string>

#include "LPCommon/Utils/Stream/Streambuf.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {
namespace stream {

std::unique_ptr<std::istream> streamFromData(const unsigned char* data, size_t length) {
    /**
     * This is an std::istream that holds onto the std::streambuf object.  The streambuf cannot be deleted until the
     * istream is destroyed.
     */
    class ResourceStream : public std::istream {
    public:
        ResourceStream(std::unique_ptr<Streambuf> buf) : std::istream(buf.get()), m_buf(std::move(buf)) {
        }

    private:
        std::unique_ptr<Streambuf> m_buf;
    };

    return std::unique_ptr<ResourceStream>(new ResourceStream(std::unique_ptr<Streambuf>(new Streambuf(data, length))));
}

}  // namespace stream
}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
