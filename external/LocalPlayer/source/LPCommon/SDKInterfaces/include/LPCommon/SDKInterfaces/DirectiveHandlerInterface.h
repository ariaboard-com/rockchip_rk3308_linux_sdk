/*
 * DirectiveHandlerInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIRECTIVEHANDLERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIRECTIVEHANDLERINTERFACE_H_

#include <memory>

#include "LPCommon/RKP/RKPDirective.h"
#include "LPCommon/RKP/DirectiveHandlerConfiguration.h"
#include "LPCommon/SDKInterfaces/DirectiveHandlerResultInterface.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * Interface for handling @c RKPDirectives.  For each @c RKPDirective received, implementations of this interface
 * should expect either a single call to @c handleDirectiveImmediately() or a call to @c preHandleDirective()
 * followed by a call to @c handleDirective() unless @c cancelDirective() is called first.  @c cancelDirective()
 * may also be called after handleDirective().
 *
 * @note The implementation of the methods of this interface MUST be thread-safe.
 * @note The implementation of the methods of this interface MUST return quickly.  Failure to do so blocks
 * the processing of subsequent @c RKPDirectives.
 */
class DirectiveHandlerInterface {
public:
    /**
     * Destructor.
     */
    virtual ~DirectiveHandlerInterface() = default;

    /**
     * Handle the action specified @c RKPDirective. Once this has been called the @c DirectiveHandler should not
     * expect to receive further calls regarding this directive.
     *
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     * @note If this operation fails, an @c ExceptionEncountered message should be sent to RKP.
     *
     * @param directive The directive to handle.
     */
    virtual void handleDirectiveImmediately(std::shared_ptr<rkpCommon::rkp::RKPDirective> directive) = 0;

    /**
     * Notification that a directive has arrived.  This notification gives the DirectiveHandler a chance to
     * prepare for handling the directive.  For example, the @c DirectiveHandler might use this notification
     * to start downloading an asset that will be required when it becomes time to actually handle the directive.
     * The @c DirectiveHandler will be notified when it should start the actual handling of the directive by a
     * subsequent call to handleDirective().  If an error occurs during the pre-Handling phase and that error
     * should cancel the handling of subsequent @c RKPDirectives with the same @c DialogRequestId, the
     * @c DirectiveHandler should call the setFailed() method on the @c result instance passed in to this call.
     *
     * @note If this operation fails, an @c ExceptionEncountered message should be sent to RKP.
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     *
     * @param directive The directive to pre-handle.
     * @param result An object to receive the result of the operation.
     */
    virtual void preHandleDirective(
        std::shared_ptr<rkpCommon::rkp::RKPDirective> directive,
        std::unique_ptr<DirectiveHandlerResultInterface> result) = 0;

    /**
     * Handle the action specified by the directive identified by @c messageId. The handling of subsequent directives
     * with the same @c DialogRequestId may be blocked until the @c DirectiveHandler calls the @c setSucceeded()
     * method of the @c DirectiveHandlingResult instance passed in to the @c preHandleDirective() call for the
     * directive specified by @c messageId.  If handling of this directive fails such that subsequent directives with
     * the same @c DialogRequestId should be cancelled, this @c DirectiveHandler should instead call setFailed() to
     * indicate a failure.
     *
     * @note If this operation fails, an @c ExceptionEncountered message should be sent to RKP.
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     *
     * @param messageId The message ID of a directive previously passed to @c preHandleDirective().
     * @return @c false when @c messageId is not recognized, else @c true.  Any errors related to handling of a valid
     * messageId should be reported using @c DirectiveHandlerResultInterface::setFailed().
     */
    virtual bool handleDirective(const std::string& messageId) = 0;

    /**
     * Cancel an ongoing @c preHandleDirective() or @c handleDirective() operation for the specified @c RKPDirective.
     * Once this has been called the @c DirectiveHandler should not expect to receive further calls regarding this
     * directive.
     *
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     *
     * @param messageId The message ID of a directive previously passed to preHandleDirective().
     */
    virtual void cancelDirective(const std::string& messageId) = 0;

    /**
     * Notification that this handler has been de-registered and will not receive any more calls.
     */
    virtual void onDeregistered() = 0;

    /**
     * Returns the configuration of the directive handler regarding which namespace and name pairs it should handle and
     * the appropriate policy for each pair.
     *
     * @return The @c rkp::DirectiveHandlerConfiguration of the handler.
     */
    virtual rkp::DirectiveHandlerConfiguration getConfiguration() const = 0;
};

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIRECTIVEHANDLERINTERFACE_H_
