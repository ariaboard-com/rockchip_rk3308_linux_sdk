/*
 * MessageRequest.h
 *
 * Copyright 2016-2018 Rockchip.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_MESSAGEREQUEST_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_MESSAGEREQUEST_H_

#include <memory>
#include <string>
#include <mutex>
#include <unordered_set>

#include "LPCommon/RKP/Attachment/AttachmentReader.h"
#include <LPCommon/SDKInterfaces/MessageRequestObserverInterface.h>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * This is a wrapper class which allows a client to send a Message to RKP, and be notified when the attempt to
 * send the Message was completed.
 */
class MessageRequest {
public:
    /**
     * Constructor.
     * @param jsonContent The message to be sent to RKP.
     * @param attachmentReader The attachment data (if present) to be sent to RKP along with the message.
     * Defaults to @c nullptr.
     */
    MessageRequest(
        const std::string& jsonContent,
        std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> attachmentReader = nullptr);

    /**
     * Destructor.
     */
    virtual ~MessageRequest();

    /**
     * Retrieves the JSON content to be sent to RKP.
     *
     * @return The JSON content to be sent to RKP.
     */
    std::string getJsonContent();

    /**
     * Retrieves the AttachmentReader of the Attachment data to be sent to RKP.
     *
     * @return The AttachmentReader of the Attachment data to be sent to RKP.
     */
    std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> getAttachmentReader();

    /**
     * This is called once the send request has completed.  The status parameter indicates success or failure.
     * @param status Whether the send request succeeded or failed.
     */
    virtual void sendCompleted(rkpCommon::sdkInterfaces::MessageRequestObserverInterface::Status status);

    /**
     * This function will be called if RKP responds with an exception message to this message request being sent.
     *
     * @param exceptionMessage The exception message.
     */
    virtual void exceptionReceived(const std::string& exceptionMessage);

    /**
     * Add observer of MessageRequestObserverInterface.
     *
     * @param observer The observer to be added to the set.
     */
    void addObserver(std::shared_ptr<rkpCommon::sdkInterfaces::MessageRequestObserverInterface> observer);

    /**
     * Remove observer of MessageRequestObserverInterface.
     *
     * @param observer The observer to be removed from the set.
     */
    void removeObserver(std::shared_ptr<rkpCommon::sdkInterfaces::MessageRequestObserverInterface> observer);

    /**
     * Utility function to convert a modern enum class to a string.
     *
     * @param status The enum value.
     * @return The string representation of the incoming value.
     */
    static std::string statusToString(rkpCommon::sdkInterfaces::MessageRequestObserverInterface::Status status);

    /**
     * A function to evaluate if the given status reflects receipt of the message by the server.
     *
     * @param status The status being queried.
     * @return Whether the status reflects receipt of the message by the server.
     */
    static bool isServerStatus(sdkInterfaces::MessageRequestObserverInterface::Status status);

protected:
    /// Mutex to guard access of m_observers.
    std::mutex m_observerMutex;

    /// Set of observers of MessageRequestObserverInterface.
    std::unordered_set<std::shared_ptr<rkpCommon::sdkInterfaces::MessageRequestObserverInterface>> m_observers;

    /// The JSON content to be sent to RKP.
    std::string m_jsonContent;

    /// The AttachmentReader of the Attachment data to be sent to RKP.
    std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> m_attachmentReader;
};

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_MESSAGEREQUEST_H_
