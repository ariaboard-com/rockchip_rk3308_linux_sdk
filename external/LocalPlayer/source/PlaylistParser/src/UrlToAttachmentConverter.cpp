/*
 * UrlContentToAttachmentConverter.cpp
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

#include "PlaylistParser/UrlToAttachmentConverter.h"

#include <LPCommon/Utils/Logger/Logger.h>

namespace localPlayerSDK {
namespace playlistParser {

/// String to identify log entries originating from this file.
static const std::string TAG("UrlContentToAttachmentConverter");

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

/// The number of bytes read from the attachment with each read in the read loop.
// Just smaller than the default megabyte size of an Attachment to allow for maximum possible write size at at time
static const size_t CHUNK_SIZE{rkpCommon::rkp::attachment::InProcessAttachment::SDS_BUFFER_DEFAULT_SIZE_IN_BYTES -
                               rkpCommon::rkp::attachment::InProcessAttachment::SDS_BUFFER_DEFAULT_SIZE_IN_BYTES / 4};

static const std::chrono::milliseconds TIME_TO_WAIT_BETWEEN_BLOCKED_WRITES{100};

static const std::chrono::milliseconds UNVALID_DURATION =
    rkpCommon::utils::playlistParser::PlaylistParserObserverInterface::INVALID_DURATION;

std::shared_ptr<UrlContentToAttachmentConverter> UrlContentToAttachmentConverter::create(
    std::shared_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory,
    const std::string& url,
    std::shared_ptr<ErrorObserverInterface> observer,
    std::chrono::milliseconds startTime) {
    if (!contentFetcherFactory) {
        return nullptr;
    }
    auto thisSharedPointer = std::shared_ptr<UrlContentToAttachmentConverter>(
        new UrlContentToAttachmentConverter(contentFetcherFactory, url, observer, startTime));
    auto retVal = thisSharedPointer->m_playlistParser->parsePlaylist(url, thisSharedPointer);
    if (0 == retVal) {
        thisSharedPointer->shutdown();
        return nullptr;
    }
    return thisSharedPointer;
}

std::shared_ptr<rkpCommon::rkp::attachment::InProcessAttachment> UrlContentToAttachmentConverter::getAttachment() {
    return m_stream;
}

UrlContentToAttachmentConverter::UrlContentToAttachmentConverter(
    std::shared_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory,
    const std::string& url,
    std::shared_ptr<ErrorObserverInterface> observer,
    std::chrono::milliseconds startTime) :
        RequiresShutdown{"UrlContentToAttachmentConverter"},
        m_desiredStreamPoint{startTime},
        m_contentFetcherFactory{contentFetcherFactory},
        m_observer{observer},
        m_shuttingDown{false},
        m_runningTotal{0},
        m_startedStreaming{false} {
    m_playlistParser = PlaylistParser::create(m_contentFetcherFactory);
    m_startStreamingPointFuture = m_startStreamingPointPromise.get_future();
    m_stream = std::make_shared<rkpCommon::rkp::attachment::InProcessAttachment>(url);
    // TODO: ACSDK-825 Switch to using a blocking writer
    m_streamWriter = m_stream->createWriter();
}

std::chrono::milliseconds UrlContentToAttachmentConverter::getStartStreamingPoint() {
    return m_startStreamingPointFuture.get();
}

std::chrono::milliseconds UrlContentToAttachmentConverter::getDesiredStreamingPoint() {
    std::lock_guard<std::mutex> lock{m_mutex};
    return m_desiredStreamPoint;
}

void UrlContentToAttachmentConverter::onPlaylistEntryParsed(
    int requestId,
    std::string url,
    rkpCommon::utils::playlistParser::PlaylistParseResult parseResult,
    std::chrono::milliseconds duration) {
    std::lock_guard<std::mutex> lock{m_mutex};
    if (!m_startedStreaming) {
        if (m_desiredStreamPoint.count() > 0) {
            if (duration == UNVALID_DURATION) {
                m_startStreamingPointPromise.set_value(m_runningTotal);
                // Allow to start streaming below
            } else if (m_runningTotal + duration <= m_desiredStreamPoint) {
                m_runningTotal += duration;
                return;
            } else {
                m_startStreamingPointPromise.set_value(m_runningTotal);
                m_runningTotal += duration;
                // Allow to begin streaming below
            }
        } else {
            m_startStreamingPointPromise.set_value(std::chrono::seconds::zero());
        }
    }
    m_startedStreaming = true;
    switch (parseResult) {
        case rkpCommon::utils::playlistParser::PlaylistParseResult::ERROR:
            ACSDK_DEBUG9(LX("onPlaylistEntryParsed").d("status", parseResult));
            m_executor.submit([this]() {
                ACSDK_DEBUG9(LX("closingWriter"));
                m_streamWriter->close();
                std::lock_guard<std::mutex> lock{m_mutex};
                if (m_observer) {
                    m_observer->onError();
                }
            });
            break;
        case rkpCommon::utils::playlistParser::PlaylistParseResult::SUCCESS:
            ACSDK_DEBUG9(LX("onPlaylistEntryParsed").d("status", parseResult));
            m_executor.submit([this, url]() {
                if (!writeUrlContentIntoStream(url)) {
                    ACSDK_ERROR(LX("writeUrlContentToStreamFailed"));
                    std::lock_guard<std::mutex> lock{m_mutex};
                    if (m_observer) {
                        m_observer->onError();
                    }
                }
                ACSDK_DEBUG9(LX("closingWriter"));
                m_streamWriter->close();
            });
            break;
        case rkpCommon::utils::playlistParser::PlaylistParseResult::STILL_ONGOING:
            ACSDK_DEBUG9(LX("onPlaylistEntryParsed").d("status", parseResult));
            m_executor.submit([this, url]() {
                if (!writeUrlContentIntoStream(url)) {
                    ACSDK_ERROR(LX("writeUrlContentToStreamFailed").d("info", "closingWriter"));
                    m_streamWriter->close();
                    std::lock_guard<std::mutex> lock{m_mutex};
                    if (m_observer) {
                        m_observer->onError();
                    }
                }
            });
            break;
        default:
            return;
    }
}

bool UrlContentToAttachmentConverter::writeUrlContentIntoStream(std::string url) {
    /*
     * TODO: ACSDK-826 We currently copy from one SDS with the individual URL data into a master SDS. We could probably
     * optimize this to avoid the extra copy.
     */
    ACSDK_DEBUG9(LX("writeUrlContentIntoStream").d("info", "beginning"));
    auto contentFetcher = m_contentFetcherFactory->create(url);
    auto httpContent =
        contentFetcher->getContent(rkpCommon::sdkInterfaces::HTTPContentFetcherInterface::FetchOptions::ENTIRE_BODY);
    if (!httpContent) {
        ACSDK_ERROR(LX("getContentFailed").d("reason", "nullHTTPContentReceived"));
        return false;
    }
    if (!(*httpContent)) {
        ACSDK_ERROR(LX("getContentFailed").d("reason", "badHTTPContentReceived"));
        return false;
    }
    if (!httpContent->dataStream) {
        ACSDK_ERROR(LX("getContentFailed").d("reason", "badDataStream"));
        return false;
    }
    auto reader = httpContent->dataStream->createReader(rkpCommon::rkp::attachment::AttachmentReader::Policy::BLOCKING);
    if (!reader) {
        ACSDK_ERROR(LX("getContentFailed").d("reason", "failedToCreateStreamReader"));
        return false;
    }
    rkpCommon::rkp::attachment::AttachmentReader::ReadStatus readStatus =
        rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::OK;
    std::vector<char> buffer(CHUNK_SIZE, 0);
    bool streamClosed = false;
    while (!streamClosed && !m_shuttingDown) {
        auto bytesRead = reader->read(buffer.data(), buffer.size(), &readStatus);
        switch (readStatus) {
            case rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::CLOSED:
                streamClosed = true;
                if (bytesRead == 0) {
                    ACSDK_INFO(LX("readFinished").d("reason", "CLOSED"));
                    break;
                }
            /* FALL THROUGH - to add any data received even if closed */
            case rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::OK:
            case rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::OK_WOULDBLOCK:
            case rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::OK_TIMEDOUT:
                if (!writeDataIntoStream(buffer, bytesRead)) {
                    ACSDK_ERROR(LX("writeDataIntoStreamFailed").d("reason", "writeError"));
                    return false;
                }
                break;
            case rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::ERROR_OVERRUN:
            case rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::ERROR_BYTES_LESS_THAN_WORD_SIZE:
            case rkpCommon::rkp::attachment::AttachmentReader::ReadStatus::ERROR_INTERNAL:
                ACSDK_ERROR(LX("readFailed").d("status", "readError"));
                return false;
        }
    }
    if (m_shuttingDown) {
        return false;
    }
    ACSDK_DEBUG9(LX("writeUrlContentIntoStreamSuccess"));
    return true;
}

bool UrlContentToAttachmentConverter::writeDataIntoStream(const std::vector<char>& buffer, size_t numBytes) {
    rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus writeStatus =
        rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus::OK;
    while (!m_shuttingDown) {
        // TODO: ACSDK-825 Switch to using a blocking writer
        m_streamWriter->write(buffer.data(), numBytes, &writeStatus);
        switch (writeStatus) {
            case rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus::CLOSED:
                // TODO: ACSDK-827 Replace with just the writeStatus once the << operator is added
                ACSDK_ERROR(LX("writeContentFailed").d("reason", "writeStatusCLOSED"));
                return false;
            case rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus::ERROR_BYTES_LESS_THAN_WORD_SIZE:
                ACSDK_ERROR(LX("writeContentFailed").d("reason", "writeStatusBYTESLESSTHANWORDSIZE"));
                return false;
            case rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus::ERROR_INTERNAL:
                ACSDK_ERROR(LX("writeContentFailed").d("reason", "writeStatusERRORINTERNAL"));
                return false;
            case rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus::OK:
                return true;
            case rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus::OK_BUFFER_FULL:
                std::this_thread::sleep_for(TIME_TO_WAIT_BETWEEN_BLOCKED_WRITES);
                continue;
            default:
                ACSDK_ERROR(LX("writeContentFailed").d("reason", "unknownWriteStatus"));
                return false;
        }
    }
    if (m_shuttingDown) {
        return false;
    }
    return true;
}

void UrlContentToAttachmentConverter::doShutdown() {
    {
        std::lock_guard<std::mutex> lock{m_mutex};
        m_observer.reset();
    }
    m_shuttingDown = true;
    m_executor.shutdown();
    m_observer.reset();
    m_playlistParser->shutdown();
    m_playlistParser.reset();
    m_streamWriter.reset();
    if (!m_startedStreaming) {
        m_startStreamingPointPromise.set_value(std::chrono::seconds::zero());
    }
}

}  // namespace playlistParser
}  // namespace localPlayerSDK