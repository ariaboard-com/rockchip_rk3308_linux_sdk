/*
 * Metrics.cpp
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

#include "LPCommon/Utils/Metrics.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {

using namespace localPlayerSDK::rkpCommon::rkp;
using namespace localPlayerSDK::rkpCommon::utils::logger;

const std::string Metrics::locationToString(Metrics::Location location) {
    switch (location) {
        case ADSL_ENQUEUE:
            return "ADSL Enqueue";
        case ADSL_DEQUEUE:
            return "ADSL Dequeue";
        case SPEECH_SYNTHESIZER_RECEIVE:
            return "SpeechSynthesizer Receive";
        case AIP_RECEIVE:
            return "AIP Receive";
        case AIP_SEND:
            return "AIP Send";
        case BUILDING_MESSAGE:
            return "Building Message";
    }

    // UNREACHABLE
    return "unknown";
}

logger::LogEntry& Metrics::d(LogEntry& logEntry, const std::shared_ptr<RKPMessage> msg, Metrics::Location location) {
    return d(logEntry, msg->getName(), msg->getMessageId(), msg->getDialogRequestId(), location);
}

logger::LogEntry& Metrics::d(
    LogEntry& logEntry,
    const std::string& name,
    const std::string& messageId,
    const std::string& dialogRequestId,
    Location location) {
    logEntry.d("Location", locationToString(location)).d("NAME", name);

    if (!messageId.empty() || !dialogRequestId.empty()) {
        logEntry.d("MessageID", messageId).d("DialogRequestID", dialogRequestId);
    };

    return logEntry;
}

}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
