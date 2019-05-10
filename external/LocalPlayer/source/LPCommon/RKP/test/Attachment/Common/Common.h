/*
 * Common.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_TEST_ATTACHMENT_COMMON_COMMON_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_TEST_ATTACHMENT_COMMON_COMMON_H_

#include <memory>
#include <vector>

#include "LPCommon/Utils/SDS/InProcessSDS.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace test {

/// A test AttachmentId string.
static const std::string TEST_ATTACHMENT_ID_STRING_ONE = "testAttachmentId_1";
/// A second test AttachmentId string.
static const std::string TEST_ATTACHMENT_ID_STRING_TWO = "testAttachmentId_2";
/// A third test AttachmentId string.
static const std::string TEST_ATTACHMENT_ID_STRING_THREE = "testAttachmentId_3";
/// A test buffer size.
static const int TEST_SDS_BUFFER_SIZE_IN_BYTES = 400;
/// A test buffer write size.
static const int TEST_SDS_PARTIAL_WRITE_AMOUNT_IN_BYTES = 150;
/// A test buffer read size.
static const int TEST_SDS_PARTIAL_READ_AMOUNT_IN_BYTES = 150;

/**
 * A utility function to create an SDS.
 *
 * @param size The desired size of the data segment of the SDS.
 * @return An SDS for testing.
 */
std::unique_ptr<rkpCommon::utils::sds::InProcessSDS> createSDS(int size);

/**
 * A utility function to create a random pattern.
 *
 * @param size The size of the random pattern desired.
 * @return A vector containing a random pattern of data.
 */
std::vector<uint8_t> createTestPattern(int size);

}  // namespace test
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_TEST_ATTACHMENT_COMMON_COMMON_H_
