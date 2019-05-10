/*
 * UrlContentToAttachmentConverter.h
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

#ifndef LOCAL_PLAYER_SDK_PLAYLIST_PARSER_INCLUDE_PLAYLIST_PARSER_URL_TO_ATTACHMENT_CONVERTER_H_
#define LOCAL_PLAYER_SDK_PLAYLIST_PARSER_INCLUDE_PLAYLIST_PARSER_URL_TO_ATTACHMENT_CONVERTER_H_

#include <memory>

#include <LPCommon/RKP/Attachment/InProcessAttachment.h>
#include <LPCommon/RKP/Attachment/InProcessAttachmentWriter.h>
#include <LPCommon/RKP/Attachment/InProcessAttachmentReader.h>

#include <LPCommon/SDKInterfaces/HTTPContentFetcherInterfaceFactoryInterface.h>
#include <LPCommon/Utils/PlaylistParser/PlaylistParserObserverInterface.h>
#include <LPCommon/Utils/RequiresShutdown.h>
#include <LPCommon/Utils/Threading/Executor.h>

#include "PlaylistParser/PlaylistParser.h"

namespace localPlayerSDK {
namespace playlistParser {

/// Class that handles the streaming of urls containing media into @c Attachments.
class UrlContentToAttachmentConverter
        : public rkpCommon::utils::playlistParser::PlaylistParserObserverInterface
        , public rkpCommon::utils::RequiresShutdown {
public:
    /// Class to observe errors that arise from converting a URL to to an @c Attachment
    class ErrorObserverInterface {
    public:
        /**
         * Notification that an error has occurred in the streaming of content.
         */
        virtual void onError() = 0;
    };

    /**
     * Creates a converter object. Note that calling this function will commence the parsing and streaming of the URL
     * into the internal attachment. If a desired start time is specified, this function will attempt to start streaming
     * at that offset, based on available metadata if the URL points to a playlist file. If no such information is
     * available, stremaing will begin from the beginning. It is up to the caller of this function to make a call to
     * @c getStartStreamingPoint() to find out the actual offset from which streaming began.
     *
     * @param contentFetcherFactory Used to create @c HTTPContentFetchers.
     * @param url The URL to stream from.
     * @param observer An observer to be notified of any errors that may happen during streaming.
     * @param desiredStartTime The desired time to attempt to start streaming from. Note that this will only succeed
     * in cases where the URL points to a playlist with metadata about individual chunks within it. If none are found,
     * streaming will begin from the beginning.
     * @return A @c std::shared_ptr to the new @c UrlContentToAttachmentConverter object or @c nullptr on failure.
     *
     * @note This object is intended to be used once. Subsequent calls to @c convertPlaylistToAttachment() will fail.
     */
    static std::shared_ptr<UrlContentToAttachmentConverter> create(
        std::shared_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory,
        const std::string& url,
        std::shared_ptr<ErrorObserverInterface> observer,
        std::chrono::milliseconds startTime = std::chrono::milliseconds::zero());

    /**
     * Returns the attachment into which the URL content was streamed into.
     *
     * @return A @c std::shared_ptr of an @c InProcessAttachment or @c nullptr on failure.
     */
    std::shared_ptr<rkpCommon::rkp::attachment::InProcessAttachment> getAttachment();

    /**
     * Gets the actual point from which streaming began.
     *
     * @return The point from which streaming began.
     */
    std::chrono::milliseconds getStartStreamingPoint();

    /**
     * Gets the initial desired point of steaming.
     *
     * @return The point from which streaming was desired to begin at.
     */
    std::chrono::milliseconds getDesiredStreamingPoint();

    void doShutdown() override;

private:
    /**
     * Constructor.
     *
     * @param contentFetcherFactory Used to create @c HTTPContentFetcher's.
     * @param url The URL to stream from.
     * @param observer An observer to be notified of any errors that may happen during streaming.
     * @param desiredStartTime The desired time to attempt to start streaming from. Note that this will only succeed
     * in cases where the URL points to a playlist with metadata about individual chunks within it. If none are found,
     * streaming will begin from the beginning.
     */
    UrlContentToAttachmentConverter(
        std::shared_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> contentFetcherFactory,
        const std::string& url,
        std::shared_ptr<ErrorObserverInterface> observer,
        std::chrono::milliseconds startTime);

    void onPlaylistEntryParsed(
        int requestId,
        std::string url,
        rkpCommon::utils::playlistParser::PlaylistParseResult parseResult,
        std::chrono::milliseconds duration = PlaylistParserObserverInterface::INVALID_DURATION) override;

    /**
     * @name Executor Thread Functions
     *
     * These functions (and only these functions) are called by @c m_executor on a single worker thread.  All other
     * functions in this class can be called asynchronously, and pass data to the @c Executor thread through parameters
     * to lambda functions.  No additional synchronization is needed.
     */
    /// @{

    /**
     * Downloads the content from the url and writes it into the internal stream.
     *
     * @return @c true if the content was successfully streamed and written or @c false otherwise.
     */
    bool writeUrlContentIntoStream(std::string url);

    /**
     * Writes the given data into the internal stream.
     *
     * @param buffer The data to write.
     * @param numBytes The number of bytes to write.
     * @return @c true if the data was successully written or @c false otherwise
     */
    bool writeDataIntoStream(const std::vector<char>& buffer, size_t numBytes);

    /// @}

    /// A promise to fulfill once streaming begins.
    std::promise<std::chrono::milliseconds> m_startStreamingPointPromise;

    /// A future which will hold the point at which streaming began.
    std::shared_future<std::chrono::milliseconds> m_startStreamingPointFuture;

    /// The initial desired offset from which streaming should begin.
    std::chrono::milliseconds m_desiredStreamPoint;

    /// Used to retrieve content from URLs
    std::shared_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface> m_contentFetcherFactory;

    /// Used to parse URLS that point to playlists.
    std::shared_ptr<PlaylistParser> m_playlistParser;

    std::shared_ptr<rkpCommon::rkp::attachment::InProcessAttachment> m_stream;

    /// The writer used to write data into the internal master stream.
    std::shared_ptr<rkpCommon::rkp::attachment::AttachmentWriter> m_streamWriter;

    /// The observer to be notified of errors.
    std::shared_ptr<ErrorObserverInterface> m_observer;

    /// Flag to indicate if a shutdown is occurring.
    std::atomic<bool> m_shuttingDown;

    /// A counter to indicate the current running total of durations found in playlist entries.
    std::chrono::milliseconds m_runningTotal;

    /// Indicates whether streaming has begun.
    bool m_startedStreaming;

    /**
     * Used to serialize access to private members across callbacks. This mutex protects access to m_desiredStreamPoint,
     * m_startedStreaming, m_startStreamingPointPromise, m_runningTotal, and m_observer.
     */
    std::mutex m_mutex;

    /**
     * @c Executor which queues up operations from asynchronous API calls.
     *
     * @note This declaration needs to come *after* the Executor Thread Variables above so that the thread shuts down
     *     before the Executor Thread Variables are destroyed.
     */
    rkpCommon::utils::threading::Executor m_executor;
};

}  // namespace playlistParser
}  // namespace localPlayerSDK

#endif  // LOCAL_PLAYER_SDK_PLAYLIST_PARSER_INCLUDE_PLAYLIST_PARSER_URL_TO_ATTACHMENT_CONVERTER_H_