/*
 * RKPMessage.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPMESSAGE_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPMESSAGE_H_

#include "RKPMessageHeader.h"

#include <memory>
#include <string>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * This is a base class which allows us to represent a message sent or received from RKP.
 * This class encapsulates the common data elements for all such messages.
 */
class RKPMessage {
public:
    /**
     * Constructor.
     *
     * @param rkpMessageHeader An object that contains the necessary header fields of an RKP message.
     *                         NOTE: This parameter MUST NOT be null.
     * @param payload The payload associated with an RKP message. This is expected to be in the JSON format.
     */
    RKPMessage(std::shared_ptr<RKPMessageHeader> rkpMessageHeader, std::string payload);

    /**
     * Destructor.
     */
    virtual ~RKPMessage() = default;

    /**
     * Returns The namespace of the message.
     *
     * @return The namespace.
     */
    std::string getNamespace() const;

    /**
     * Returns The name of the message, which describes the intent.
     *
     * @return The name.
     */
    std::string getName() const;

    /**
     * Returns The message ID of the message.
     *
     * @return The message ID, a unique ID used to identify a specific message.
     */
    std::string getMessageId() const;

    /**
     * Returns The dialog request ID of the message.
     *
     * @return The dialog request ID, a unique ID for the messages that are part of the same dialog.
     */
    std::string getDialogRequestId() const;

    /**
     * Returns the payload of the message.
     *
     * @return The payload.
     */
    std::string getPayload() const;

    /**
     * Return a string representation of this @c RKPMessage's header.
     *
     * @return A string representation of this @c RKPMessage's header.
     */
    std::string getHeaderAsString() const;

private:
    /// The fields that represent the common items in the header of an RKP message.
    std::shared_ptr<RKPMessageHeader> m_header;
    /// The payload of an RKP message.
    std::string m_payload;
};

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPMESSAGE_H_
