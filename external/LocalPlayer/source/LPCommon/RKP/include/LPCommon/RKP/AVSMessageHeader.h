/*
 * RKPMessageHeader.h
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

#ifndef LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPMESSAGEHEADER_H_
#define LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPMESSAGEHEADER_H_

#include <string>

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

/**
 * The RKP message header, which contains the common fields required for an RKP message.
 */
class RKPMessageHeader {
public:
    /**
     * Constructor.
     *
     * @param rkpNamespace The namespace of an RKP message.
     * @param rkpName The name within the namespace of an RKP message.
     * @param rkpMessageId The message ID of an RKP message.
     * @param rkpDialogRequestId The dialog request ID of an RKP message, which is optional.
     */
    RKPMessageHeader(
        const std::string& rkpNamespace,
        const std::string& rkpName,
        const std::string& rkpMessageId,
        const std::string& rkpDialogRequestId = "") :
            m_namespace{rkpNamespace},
            m_name{rkpName},
            m_messageId{rkpMessageId},
            m_dialogRequestId{rkpDialogRequestId} {
    }

    /**
     * Returns the namespace in an RKP message.
     *
     * @return The namespace.
     */
    std::string getNamespace() const;

    /**
     * Returns the name in an RKP message, which describes the intent of the message.
     *
     * @return The name.
     */
    std::string getName() const;

    /**
     * Returns the message ID in an RKP message.
     *
     * @return The message ID, a unique ID used to identify a specific message.
     */
    std::string getMessageId() const;

    /**
     * Returns the dialog request ID in an RKP message.
     *
     * @return The dialog request ID, a unique ID for the messages that are part of the same dialog.
     */
    std::string getDialogRequestId() const;

    /**
     * Return a string representation of this @c RKPMessage's header.
     *
     * @return A string representation of this @c RKPMessage's header.
     */
    std::string getAsString() const;

private:
    /// Namespace of the RKPMessage header.
    const std::string m_namespace;
    /// Name within the namespace of the RKPMessage, which describes the intent of the message.
    const std::string m_name;
    /// A unique ID used to identify a specific message.
    const std::string m_messageId;
    /// A unique ID for the messages that are part of the same dialog.
    const std::string m_dialogRequestId;
};

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_RKPCOMMON_RKP_INCLUDE_RKPCOMMON_RKP_RKPMESSAGEHEADER_H_
