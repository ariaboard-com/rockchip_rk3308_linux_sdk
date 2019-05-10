/*
 * CapabilityAgent.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_CAPABILITYAGENT_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_CAPABILITYAGENT_H_

#include <atomic>
#include <unordered_map>
#include <string>
#include <memory>

#include "LPCommon/RKP/NamespaceAndName.h"
#include "LPCommon/SDKInterfaces/ExceptionEncounteredSenderInterface.h"
#include "LPCommon/SDKInterfaces/ChannelObserverInterface.h"
#include "LPCommon/SDKInterfaces/ContextRequesterInterface.h"
#include "LPCommon/SDKInterfaces/StateProviderInterface.h"
#include "LPCommon/SDKInterfaces/DirectiveHandlerInterface.h"
#include "LPCommon/SDKInterfaces/DirectiveHandlerResultInterface.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * @c CapabilityAgent implements methods which most capability agents will need, namely:
 * @li @c DirectiveHandlerInterface,
 * @li Building the JSON event string given the name, payload and context,
 * @li A map of the message Id to @c RKPDirective and @c DirectiveResultInterface.
 * Derived capability agents may extend this class. They may have to implement the following interfaces:
 * @li @c ChannelObserverInterface: To use the Activity Focus Manager Library,
 * @li @c StateProviderInterface: To provide state to the @c ContextManager.
 * @li @c ContextRequesterInterface: To request context from the @c ContextManager,
 * as necessary.
 */
class CapabilityAgent
        : public sdkInterfaces::DirectiveHandlerInterface
        , public sdkInterfaces::ChannelObserverInterface
        , public sdkInterfaces::StateProviderInterface
        , public sdkInterfaces::ContextRequesterInterface {
public:
    /**
     * Destructor.
     */
    virtual ~CapabilityAgent() = default;

    /*
     * DirectiveHandlerInterface functions.
     *
     * The following four functions implement the @c DirectiveHandlerInterface. Only the message Id is passed to the
     * @c handleDirective and @c cancelDirective functions, so we need to maintain a mapping of the message Id to the
     * @c RKPDirective and @c DirectiveHandlerResultInterface, i.e., @c DirectiveAndResultInterface. The
     * @c DirectiveHandlerInterface functions call the functions of the same name with the
     * @c DirectiveAndResultInterface as the argument.
     */
    void preHandleDirective(
        std::shared_ptr<RKPDirective> directive,
        std::unique_ptr<sdkInterfaces::DirectiveHandlerResultInterface> result) override final;

    bool handleDirective(const std::string& messageId) override final;

    void cancelDirective(const std::string& messageId) override final;

    void onDeregistered() override;

    void onFocusChanged(FocusState newFocus) override;

    void provideState(const rkpCommon::rkp::NamespaceAndName& stateProviderName, const unsigned int stateRequestToken)
        override;

    void onContextAvailable(const std::string& jsonContext) override;

    void onContextFailure(const sdkInterfaces::ContextRequestError error) override;

protected:
    /**
     * Constructor for a Capability Agent.
     *
     * @param nameSpace The namespace of the capability agent.
     * @param exceptionEncounteredSender Object to use to send ExceptionEncountered messages.
     */
    CapabilityAgent(
        const std::string& nameSpace,
        std::shared_ptr<sdkInterfaces::ExceptionEncounteredSenderInterface> exceptionEncounteredSender);

    /**
     * CapabilityAgent maintains a map from messageId to instances of DirectiveInfo so that CapabilityAgents
     * can track the processing of an @c RKPDirective.
     */
    class DirectiveInfo {
    public:
        /**
         * Constructor.
         *
         * @param directiveIn The @c RKPDirective with which to populate this DirectiveInfo.
         * @param resultIn The @c DirectiveHandlerResultInterface instance with which to populate this DirectiveInfo.
         */
        DirectiveInfo(
            std::shared_ptr<RKPDirective> directiveIn,
            std::unique_ptr<sdkInterfaces::DirectiveHandlerResultInterface> resultIn);

        /**
         * Destructor.
         */
        virtual ~DirectiveInfo() = default;

        /// @c RKPDirective that is passed during preHandle.
        std::shared_ptr<RKPDirective> directive;

        /// @c DirectiveHandlerResultInterface.
        std::shared_ptr<sdkInterfaces::DirectiveHandlerResultInterface> result;

        /// Flag to indicate whether the directive is cancelled.
        std::atomic<bool> isCancelled;
    };

    /**
     * Create a DirectiveInfo instance with which to track the handling of an @c RKPDirective.
     * @note This method is virtual to allow derived CapabilityAgent's to extend DirectiveInfo
     * with additional information.
     *
     * @param directive The @c RKPDirective to be processed.
     * @param result The object with which to communicate the outcome of processing the @c RKPDirective.
     * @return A DirectiveInfo instance with which to track the processing of @c directive.
     */
    virtual std::shared_ptr<DirectiveInfo> createDirectiveInfo(
        std::shared_ptr<RKPDirective> directive,
        std::unique_ptr<sdkInterfaces::DirectiveHandlerResultInterface> result);

    /**
     * Notification that a directive has arrived.  This notification gives the DirectiveHandler a chance to
     * prepare for handling of an @c RKPDirective.
     * If an error occurs during the pre-Handling phase and that error should cancel the handling of subsequent
     * @c RKPDirectives with the same @c DialogRequestId, the @c DirectiveHandler should call the @c setFailed method
     * on the @c result instance passed in to this call.
     *
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     *
     * @param info The @c DirectiveInfo instance for the @c RKPDirective to process.
     */
    virtual void preHandleDirective(std::shared_ptr<DirectiveInfo> info) = 0;

    /**
     * Handle the action specified by the @c RKPDirective in @c info. The handling of subsequent directives with
     * the same @c DialogRequestId may be blocked until the @c DirectiveHandler calls the @c setSucceeded() method
     * of the @c DirectiveHandlingResult present in @c info. If handling of this directive fails @c setFailed()
     * should be called to indicate a failure.
     *
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     *
     * @param info The @c DirectiveInfo instance for the @c RKPDirective to process.
     */
    virtual void handleDirective(std::shared_ptr<DirectiveInfo> info) = 0;

    /**
     * Cancel an ongoing @c preHandleDirective() or @c handleDirective() operation for the @c RKPDirective in
     * @info. Once this has been called the @c CapabilityAgent should not expect to receive further calls
     * regarding this directive.
     *
     * @note The implementation of this method MUST be thread-safe.
     * @note The implementation of this method MUST return quickly. Failure to do so blocks the processing
     * of subsequent @c RKPDirectives.
     *
     * @param info The @c DirectiveInfo instance for the @c RKPDirective to process.
     */
    virtual void cancelDirective(std::shared_ptr<DirectiveInfo> info) = 0;

    /**
     * This function releases resources associated with the @c RKPDirective which is no longer in use by a
     * @c CapabilityAgent.
     *
     * @note This function should be called from @c handleDirective() and @c cancelDirective()
     * implementations after the outcome of @c handleDirective() or @c cancelDirective() has been reported.
     *
     * @param messageId The message Id of the @c RKPDirective.
     */
    void removeDirective(const std::string& messageId);

    /**
     * Builds a JSON event string which includes the header, the @c payload and an optional @c context.
     * The header includes the namespace, name, message Id and an optional @c dialogRequestId.
     * The message Id required for the header is a random string that is generated and added to the
     * header.
     *
     * @param eventName The name of the event to be include in the header.
     * @param dialogRequestIdString The value associated with the "dialogRequestId" key.
     * @param payload The payload value associated with the "payload" key.
     * @param context Optional @c context to be sent with the event message.
     * @return A pair object consisting of the messageId and the event JSON string if successful,
     * else a pair of empty strings.
     */
    const std::pair<std::string, std::string> buildJsonEventString(
        const std::string& eventName,
        const std::string& dialogRequestIdString = "",
        const std::string& payload = "{}",
        const std::string& context = "");

    /// The namespace of the capability agent.
    const std::string m_namespace;

    /// Object to use to send exceptionEncountered messages
    std::shared_ptr<sdkInterfaces::ExceptionEncounteredSenderInterface> m_exceptionEncounteredSender;

private:
    /**
     * Find the DirectiveInfo instance (if any) for the specified messsageId.
     *
     * @param messageId The messageId value to find DirectiveInfo for.
     * @return The DirectiveInfo instance for @c messageId.
     */
    std::shared_ptr<DirectiveInfo> getDirectiveInfo(const std::string& messageId);

    /// Map of message Id to @c DirectiveAndResultInterface.
    std::unordered_map<std::string, std::shared_ptr<DirectiveInfo>> m_directiveInfoMap;

    /// Mutex to protect message Id to @c DirectiveAndResultInterface mapping.
    std::mutex m_mutex;
};

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_CAPABILITYAGENT_H_
