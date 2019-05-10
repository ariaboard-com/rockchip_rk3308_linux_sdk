/*
 * ExceptionEncounteredSender.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_EXCEPTIONENCOUNTEREDSENDER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_EXCEPTIONENCOUNTEREDSENDER_H_

#include <memory>
#include <string>
#include <LPCommon/RKP/ExceptionErrorType.h>
#include <LPCommon/SDKInterfaces/MessageSenderInterface.h>
#include <LPCommon/SDKInterfaces/ExceptionEncounteredSenderInterface.h>
#include <LPCommon/Utils/Threading/Executor.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * Class creates an ExceptionEncountered event and sends to RKP using @c MessageSenderInterface.
 */
class ExceptionEncounteredSender : public rkpCommon::sdkInterfaces::ExceptionEncounteredSenderInterface {
public:
    /**
     * Creates a new @c ExceptionEncounteredSender instance.
     *
     * @param messageSender The object to use for sending events.
     * @return A @c std::unique_ptr to the new @c ExceptionEncounteredSender instance.
     */
    static std::unique_ptr<ExceptionEncounteredSender> create(
        std::shared_ptr<rkpCommon::sdkInterfaces::MessageSenderInterface> messageSender);

    /**
     * This function asks the @c ExceptionEncounteredSender to send a ExceptionEncountered event to RKP when
     * the client is unable to execute a directive.
     *
     * @param unparsedDirective The directive which the client was unable to execute, must be sent to RKP
     * @param error The @c ExceptionErrorType to be sent to RKP
     * @param errorDescription The error details to be sent for logging and troubleshooting.
     */
    void sendExceptionEncountered(
        const std::string& unparsedDirective,
        rkp::ExceptionErrorType error,
        const std::string& errorDescription) override;

private:
    /**
     * Constructor.
     *
     * @param messageSender The object to use for sending events.
     */
    ExceptionEncounteredSender(std::shared_ptr<rkpCommon::sdkInterfaces::MessageSenderInterface> messageSender);

    /// The object to use for sending events.
    std::shared_ptr<rkpCommon::sdkInterfaces::MessageSenderInterface> m_messageSender;
};

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_EXCEPTIONENCOUNTEREDSENDER_H_
