/*
 * DirectiveSequencerInterface.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIRECTIVESEQUENCERINTERFACE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIRECTIVESEQUENCERINTERFACE_H_

#include <memory>
#include <string>

#include "LPCommon/RKP/RKPDirective.h"
#include "LPCommon/SDKInterfaces/DirectiveHandlerInterface.h"
#include "LPCommon/Utils/RequiresShutdown.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace sdkInterfaces {

/**
 * Interface for sequencing and handling a stream of @c RKPDirective instances.
 *
 * Customers of this interface specify a mapping of @c RKPDirectives specified by (namespace, name) pairs to
 * instances of the @c RKPDirectiveHandlerInterface via calls to @c setDirectiveHandlers(). Changes to this
 * mapping can be made at any time by specifying a new mapping. Customers pass @c RKPDirectives in to this
 * interface for processing via calls to @c onDirective(). @c RKPDirectives are processed in the order that
 * they are received. @c RKPDirectives with a non-empty @c dialogRequestId value are filtered by the
 * @c DirectiveSequencer's current @c dialogRequestId value (specified by calls to @c setDialogRequestId()).
 * Only @c RKPDirectives with a @c dialogRequestId that is empty or which matches the last setting of the
 * @c dialogRequestId are handled. All others are ignored. Specifying a new @c DialogRequestId value while
 * @c RKPDirectives are already being handled will cancel the handling of @c RKPDirectives that have the
 * previous @c DialogRequestId and whose handling has not completed.
 */
class DirectiveSequencerInterface : public utils::RequiresShutdown {
public:
    /**
     * Constructor.
     *
     * @param name The name of the class or object which requires shutdown calls.  Used in log messages when problems
     *    are detected in shutdown or destruction sequences.
     */
    DirectiveSequencerInterface(const std::string& name);

    /**
     * Destructor.
     */
    virtual ~DirectiveSequencerInterface() = default;

    /**
     * Add the specified handler as a handler for its specified namespace, name, and policy. Note that implmentations
     * of this should call the handler's getConfiguration() method to get the namespace(s), name(s), and policy(ies) of
     * the handler. If any of the mappings fail, the entire call is refused.
     *
     * @param handler The handler to add.
     * @return Whether the handler was added.
     */
    virtual bool addDirectiveHandler(std::shared_ptr<DirectiveHandlerInterface> handler) = 0;

    /**
     * Remove the specified handler's mapping of @c NamespaceAndName to @c BlockingPolicy values. Note that
     * implementations of this should call the handler's getConfiguration() method to get the namespace(s), name(s), and
     * policy(ies) of the handler. If the handler's configurations are unable to be removed, the entire operation is
     * refused.

     specified mappings from @c NamespaceAndName values to @c HandlerAndPolicy values. If any of
     * the specified mappings do not match an existing mapping, the entire operation is refused.
     *
     * @param handler The handler to remove.
     * @return Whether the handler was removed.
     */
    virtual bool removeDirectiveHandler(std::shared_ptr<DirectiveHandlerInterface> handler) = 0;

    /**
     * Set the current @c DialogRequestId. This value can be set at any time. Setting this value causes a
     * @c DirectiveSequencer to drop unhandled @c RKPDirectives with different (and non-empty) DialogRequestId
     * values.  @c RKPDirectives with a differing @c dialogRequestId value and whose pre-handling or handling
     * is already in progress will be cancelled.
     *
     * @param dialogRequestId The new value for the current @c DialogRequestId.
     */
    virtual void setDialogRequestId(const std::string& dialogRequestId) = 0;

    /**
     * Sequence the handling of an @c RKPDirective.  The actual handling is done by whichever @c DirectiveHandler
     * is associated with the @c RKPDirective's (namespace, name) pair.
     *
     * @param directive The @c RKPDirective to handle.
     * @return Whether or not the directive was accepted.
     */
    virtual bool onDirective(std::shared_ptr<rkpCommon::rkp::RKPDirective> directive) = 0;
};

inline DirectiveSequencerInterface::DirectiveSequencerInterface(const std::string& name) :
        utils::RequiresShutdown{name} {
}

}  // namespace sdkInterfaces
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_SDKINTERFACES_INCLUDE_RKPCOMMON_SDKINTERFACES_DIRECTIVESEQUENCERINTERFACE_H_
