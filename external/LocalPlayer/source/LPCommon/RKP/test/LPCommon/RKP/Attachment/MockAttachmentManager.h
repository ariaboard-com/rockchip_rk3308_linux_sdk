/*
 * MockAttachmentManager.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_TEST_RKPCOMMON_RKP_ATTACHMENT_MOCKATTACHMENTMANAGER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_TEST_RKPCOMMON_RKP_ATTACHMENT_MOCKATTACHMENTMANAGER_H_

#include <chrono>
#include <string>
#include <memory>

#include <gmock/gmock.h>
#include "LPCommon/RKP/Attachment/AttachmentManagerInterface.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {
namespace attachment {
namespace test {

/// Mock class that implements the AttachmentManager.
class MockAttachmentManager : public AttachmentManagerInterface {
public:
    MOCK_CONST_METHOD2(generateAttachmentId, std::string(const std::string& contextId, const std::string& contentId));
    MOCK_METHOD1(setAttachmentTimeoutMinutes, bool(std::chrono::minutes timeoutMinutes));
    MOCK_METHOD1(createWriter, std::unique_ptr<AttachmentWriter>(const std::string& attachmentId));
    MOCK_METHOD2(
        createReader,
        std::unique_ptr<AttachmentReader>(const std::string& attachmentId, AttachmentReader::Policy policy));
};

}  // namespace test
}  // namespace attachment
}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_TEST_RKPCOMMON_RKP_ATTACHMENT_MOCKATTACHMENTMANAGER_H_
