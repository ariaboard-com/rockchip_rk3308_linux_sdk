/*
 * ExceptionEncounteredSenderInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_EXCEPTIONENCOUNTEREDSENDERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_EXCEPTIONENCOUNTEREDSENDERINTERFACE_H_

#include "LPCommon/RKP/ExceptionErrorType.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * This specifies the interface to send an ExceptionEncountered event to RKP.
 */
class ExceptionEncounteredSenderInterface {
public:
    /**
     * Virtual destructor to ensure proper cleanup by derived types.
     */
    virtual ~ExceptionEncounteredSenderInterface() = default;

    /**
     * Send a @c System::ExceptionEncountered message to RKP.
     *
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     *
     * @param unparsedDirective The unparsed JSON of the directive.
     * @param error The type of error encountered.
     * @param errorDescription Additional error details for logging and troubleshooting.
     */
    virtual void sendExceptionEncountered(
        const std::string& unparsedDirective,
        rkp::ExceptionErrorType error,
        const std::string& errorDescription) = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_EXCEPTIONENCOUNTEREDSENDERINTERFACE_H_
