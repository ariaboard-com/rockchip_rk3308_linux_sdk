/*
 * MessageRequest.cpp
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

#include "LPCommon/RKP/MessageRequest.h"
#include "LPCommon/Utils/Logger/Logger.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace rkp {

using namespace sdkInterfaces;

/// String to identify log entries originating from this file.
static const std::string TAG("MessageRequest");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

MessageRequest::MessageRequest(
    const std::string& jsonContent,
    std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> attachmentReader) :
        m_jsonContent{jsonContent},
        m_attachmentReader{attachmentReader} {
}

MessageRequest::~MessageRequest() {
}

std::string MessageRequest::getJsonContent() {
    return m_jsonContent;
}

std::shared_ptr<rkpCommon::rkp::attachment::AttachmentReader> MessageRequest::getAttachmentReader() {
    return m_attachmentReader;
}

void MessageRequest::sendCompleted(rkpCommon::sdkInterfaces::MessageRequestObserverInterface::Status status) {
    std::unique_lock<std::mutex> lock{m_observerMutex};
    auto observers = m_observers;
    lock.unlock();

    for (auto observer : observers) {
        observer->onSendCompleted(status);
    }
}

void MessageRequest::exceptionReceived(const std::string& exceptionMessage) {
    ACSDK_ERROR(LX("onExceptionReceived").d("exception", exceptionMessage));

    std::unique_lock<std::mutex> lock{m_observerMutex};
    auto observers = m_observers;
    lock.unlock();

    for (auto observer : observers) {
        observer->onExceptionReceived(exceptionMessage);
    }
}

void MessageRequest::addObserver(std::shared_ptr<rkpCommon::sdkInterfaces::MessageRequestObserverInterface> observer) {
    if (!observer) {
        ACSDK_ERROR(LX("addObserverFailed").d("reason", "nullObserver"));
        return;
    }

    std::lock_guard<std::mutex> lock{m_observerMutex};
    m_observers.insert(observer);
}

void MessageRequest::removeObserver(
    std::shared_ptr<rkpCommon::sdkInterfaces::MessageRequestObserverInterface> observer) {
    if (!observer) {
        ACSDK_ERROR(LX("removeObserverFailed").d("reason", "nullObserver"));
        return;
    }

    std::lock_guard<std::mutex> lock{m_observerMutex};
    m_observers.erase(observer);
}

using namespace rkpCommon::sdkInterfaces;

std::string MessageRequest::statusToString(MessageRequestObserverInterface::Status status) {
    switch (status) {
        case MessageRequestObserverInterface::Status::PENDING:
            return "PENDING";
        case MessageRequestObserverInterface::Status::SUCCESS:
            return "SUCCESS";
        case MessageRequestObserverInterface::Status::SUCCESS_NO_CONTENT:
            return "SUCCESS_NO_CONTENT";
        case MessageRequestObserverInterface::Status::NOT_CONNECTED:
            return "NOT_CONNECTED";
        case MessageRequestObserverInterface::Status::NOT_SYNCHRONIZED:
            return "NOT_SYNCHRONIZED";
        case MessageRequestObserverInterface::Status::TIMEDOUT:
            return "TIMEDOUT";
        case MessageRequestObserverInterface::Status::PROTOCOL_ERROR:
            return "PROTOCOL_ERROR";
        case MessageRequestObserverInterface::Status::INTERNAL_ERROR:
            return "INTERNAL_ERROR";
        case MessageRequestObserverInterface::Status::SERVER_INTERNAL_ERROR_V2:
            return "SERVER_INTERNAL_ERROR_V2";
        case MessageRequestObserverInterface::Status::REFUSED:
            return "REFUSED";
        case MessageRequestObserverInterface::Status::CANCELED:
            return "CANCELED";
        case MessageRequestObserverInterface::Status::THROTTLED:
            return "THROTTLED";
        case MessageRequestObserverInterface::Status::INVALID_AUTH:
            return "INVALID_AUTH";
        case MessageRequestObserverInterface::Status::BAD_REQUEST:
            return "BAD_REQUEST";
        case MessageRequestObserverInterface::Status::SERVER_OTHER_ERROR:
            return "SERVER_OTHER_ERROR";
    }

    return "sendMessageStatusToString_UNHANDLED_ERROR";
}

bool MessageRequest::isServerStatus(MessageRequestObserverInterface::Status status) {
    switch (status) {
        case MessageRequestObserverInterface::Status::SUCCESS:
        case MessageRequestObserverInterface::Status::SUCCESS_NO_CONTENT:
        case MessageRequestObserverInterface::Status::SERVER_INTERNAL_ERROR_V2:
        case MessageRequestObserverInterface::Status::CANCELED:
        case MessageRequestObserverInterface::Status::THROTTLED:
        case MessageRequestObserverInterface::Status::BAD_REQUEST:
        case MessageRequestObserverInterface::Status::SERVER_OTHER_ERROR:
            return true;
        case MessageRequestObserverInterface::Status::PENDING:
        case MessageRequestObserverInterface::Status::NOT_CONNECTED:
        case MessageRequestObserverInterface::Status::NOT_SYNCHRONIZED:
        case MessageRequestObserverInterface::Status::TIMEDOUT:
        case MessageRequestObserverInterface::Status::PROTOCOL_ERROR:
        case MessageRequestObserverInterface::Status::INTERNAL_ERROR:
        case MessageRequestObserverInterface::Status::REFUSED:
        case MessageRequestObserverInterface::Status::INVALID_AUTH:
            return false;
    }

    return false;
}

}  // namespace rkp
}  // namespace rkpCommon
}  // namespace localPlayerSDK
