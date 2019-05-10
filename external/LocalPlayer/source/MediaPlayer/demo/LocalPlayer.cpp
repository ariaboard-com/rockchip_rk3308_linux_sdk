/*
 * MediaPlayerTest.cpp
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

#include <algorithm>
#include <chrono>
#include <condition_variable>
#include <fstream>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>


#include <LPCommon/RKP/SpeakerConstants/SpeakerConstants.h>
#include <LPCommon/Utils/Logger/Logger.h>
#include <LPCommon/Utils/Memory/Memory.h>
#include <PlaylistParser/PlaylistParser.h>

#include "MediaPlayer/MediaPlayer.h"
#include "LPCommon/Utils/MediaPlayer/MediaPlayerObserverInterface.h"

namespace localPlayerSDK {
namespace mediaPlayer {
namespace test {

using namespace rkpCommon::rkp::attachment;
using namespace rkpCommon::rkp::speakerConstants;
using namespace rkpCommon::sdkInterfaces;
using namespace rkpCommon::utils::mediaPlayer;
using namespace rkpCommon::utils::memory;

/// String to identify log entries originating from this file.
static const std::string TAG("LocalPlayer");

static const MediaPlayer::SourceId ERROR_SOURCE_ID = MediaPlayer::ERROR;

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

/// The path to the input Dir containing the test audio files.
std::string inputsDirPath;

static std::unordered_map<std::string, std::string> urlsToContentTypes;

static std::unordered_map<std::string, std::string> urlsToContent;

/// A mock content fetcher
class MockContentFetcher : public rkpCommon::sdkInterfaces::HTTPContentFetcherInterface {
public:
    MockContentFetcher(const std::string& url) : m_url{url} {
    }

    std::unique_ptr<rkpCommon::utils::HTTPContent> getContent(FetchOptions fetchOption) override {
        if (fetchOption == FetchOptions::CONTENT_TYPE) {
            auto urlAndContentType = urlsToContentTypes.find(m_url);
            if (urlAndContentType == urlsToContentTypes.end()) {
                return nullptr;
            } else {
                std::promise<long> statusPromise;
                auto statusFuture = statusPromise.get_future();
                statusPromise.set_value(200);
                std::promise<std::string> contentTypePromise;
                auto contentTypeFuture = contentTypePromise.get_future();
                contentTypePromise.set_value(urlAndContentType->second);
                return rkpCommon::utils::memory::make_unique<rkpCommon::utils::HTTPContent>(
                    rkpCommon::utils::HTTPContent{std::move(statusFuture), std::move(contentTypeFuture), nullptr});
            }
        } else {
            auto urlAndContent = urlsToContent.find(m_url);
            if (urlAndContent == urlsToContent.end()) {
                return nullptr;
            }
            std::promise<long> statusPromise;
            auto statusFuture = statusPromise.get_future();
            statusPromise.set_value(200);
            std::promise<std::string> contentTypePromise;
            auto contentTypeFuture = contentTypePromise.get_future();
            contentTypePromise.set_value("");
            auto attachment = writeStringIntoAttachment(urlAndContent->second);
            if (!attachment) {
                return nullptr;
            }
            return rkpCommon::utils::memory::make_unique<rkpCommon::utils::HTTPContent>(
                rkpCommon::utils::HTTPContent{std::move(statusFuture), std::move(contentTypeFuture), attachment});
        }
    }

private:
    std::shared_ptr<rkpCommon::rkp::attachment::InProcessAttachment> writeStringIntoAttachment(
        const std::string& string) {
        static int id = 0;
        std::shared_ptr<rkpCommon::rkp::attachment::InProcessAttachment> stream =
            std::make_shared<rkpCommon::rkp::attachment::InProcessAttachment>(std::to_string(id++));
        if (!stream) {
            return nullptr;
        }
        auto writer = stream->createWriter();
        if (!writer) {
            return nullptr;
        }
        rkpCommon::rkp::attachment::AttachmentWriter::WriteStatus writeStatus;
        writer->write(string.data(), string.size(), &writeStatus);
        return stream;
    };

    std::string m_url;
};

/// A mock factory that creates mock content fetchers
class MockContentFetcherFactory : public rkpCommon::sdkInterfaces::HTTPContentFetcherInterfaceFactoryInterface {
    std::unique_ptr<rkpCommon::sdkInterfaces::HTTPContentFetcherInterface> create(const std::string& url) {
        return rkpCommon::utils::memory::make_unique<MockContentFetcher>(url);
    }
};

class MockPlayerObserver : public MediaPlayerObserverInterface {
public:
    /**
     * Destructor.
     */
    ~MockPlayerObserver(){};

    // void onPlaybackStarted() override;
    void onPlaybackStarted(SourceId id) override;

    void onPlaybackFinished(SourceId id) override;

    void onPlaybackError(SourceId id, const ErrorType& type, std::string error) override;

    void onPlaybackPaused(SourceId id) override;

    void onPlaybackResumed(SourceId id) override;

    void onPlaybackStopped(SourceId id) override;

    void onTags(SourceId id, std::unique_ptr<const VectorOfTags> vectorOfTags) override;

    /**
     * Wait for a message to be received.
     *
     * This function waits for a specified number of milliseconds for a message to arrive.
     * @param id The @c SourceId expected in a callback.
     * @param duration Number of milliseconds to wait before giving up.
     * @return true if a message was received within the specified duration, else false.
     */
    bool waitForPlaybackStarted(
        SourceId id,
        const std::chrono::milliseconds duration = std::chrono::milliseconds(5000));

    /**
     * Wait for a message to be received.
     *
     * This function waits for a specified number of milliseconds for a message to arrive.
     * @param id The @c SourceId expected in a callback.
     * @param duration Number of milliseconds to wait before giving up.
     * @return true if a message was received within the specified duration, else false.
     */
    bool waitForPlaybackFinished(
        SourceId id,
        const std::chrono::milliseconds duration = std::chrono::milliseconds(5000));

    /**
     * Wait for a message to be received.
     *
     * This function waits for a specified number of milliseconds for a message to arrive.
     * @param id The @c SourceId expected in a callback.
     * @param duration Number of milliseconds to wait before giving up.
     * @return true if a message was received within the specified duration, else false.
     */
    bool waitForPlaybackPaused(SourceId id, const std::chrono::milliseconds duration = std::chrono::milliseconds(5000));

    /**
     * Wait for a message to be received.
     *
     * This function waits for a specified number of milliseconds for a message to arrive.
     * @param id The @c SourceId expected in a callback.
     * @param duration Number of milliseconds to wait before giving up.
     * @return true if a message was received within the specified duration, else false.
     */
    bool waitForPlaybackResumed(
        SourceId id,
        const std::chrono::milliseconds duration = std::chrono::milliseconds(5000));

    /**
     * Wait for a message to be received.
     *
     * This function waits for a specified number of milliseconds for a message to arrive.
     * @param id The @c SourceId expected in a callback.
     * @param duration Number of milliseconds to wait before giving up.
     * @return true if a message was received within the specified duration, else false.
     */
    bool waitForPlaybackStopped(
        SourceId id,
        const std::chrono::milliseconds duration = std::chrono::milliseconds(5000));

    /**
     * Wait for a message to be received.
     * This function waits for a specified number of milliseconds for a message to arrive.
     *
     * @param id The @c SourceId expected in a callback.
     * @param duration Number of milliseconds to wait before giving up.
     * @return true if a message was received within the specified duration, else false.
     */
    bool waitForTags(SourceId id, const std::chrono::milliseconds duration = std::chrono::milliseconds(5000));

    /**
     * TODO: Make this class a mock and remove this.
     *
     * This gets the number of times onPlaybackStarted was called.
     */
    int getOnPlaybackStartedCallCount();

    /**
     * TODO: Make this class a mock and remove this.
     *
     * This gets the number of times onPlaybackFinished was called.
     */
    int getOnPlaybackFinishedCallCount();

    /**
     * TODO: Make this class a mock and remove this.
     *
     * This gets the number of times onTags was called.
     */
    int getOnTagsCallCount();

private:
    /// Mutex to protect the flags @c m_playbackStarted and .@c m_playbackFinished.
    std::mutex m_mutex;
    /// Trigger to wake up m_wakePlaybackStarted calls.
    std::condition_variable m_wakePlaybackStarted;
    /// Trigger to wake up m_wakePlaybackStarted calls.
    std::condition_variable m_wakePlaybackFinished;
    /// Trigger to wake up m_wakePlaybackPaused calls.
    std::condition_variable m_wakePlaybackPaused;
    /// Trigger to wake up m_wakePlaybackResumed calls.
    std::condition_variable m_wakePlaybackResumed;
    std::condition_variable m_wakePlaybackStopped;
    /// Trigger to wake up m_wakeTags calls.
    std::condition_variable m_wakeTags;

    // TODO: Make this class a mock and remove these.
    int m_onPlaybackStartedCallCount = 0;
    int m_onPlaybackFinishedCallCount = 0;
    int m_onTagsCallCount = 0;

    /// Flag to set when a playback start message is received.
    bool m_playbackStarted;
    /// Flag to set when a playback finished message is received.
    bool m_playbackFinished;
    /// Flag to set when a playback paused message is received.
    bool m_playbackPaused;
    /// Flag to set when a playback paused message is received.
    bool m_playbackResumed;
    /// Flag to set when a playback stopped message is received.
    bool m_playbackStopped;
    /// Flag to set when a tags message is received.
    bool m_tags;

    SourceId m_lastId = 0;
};

void MockPlayerObserver::onPlaybackStarted(SourceId id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastId = id;
    m_playbackStarted = true;
    m_playbackFinished = false;
    m_playbackStopped = false;
    m_wakePlaybackStarted.notify_all();
    m_onPlaybackStartedCallCount++;
}

void MockPlayerObserver::onPlaybackFinished(SourceId id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastId = id;
    m_playbackFinished = true;
    m_playbackStarted = false;
    m_wakePlaybackFinished.notify_all();
    m_onPlaybackFinishedCallCount++;
}

void MockPlayerObserver::onPlaybackError(SourceId id, const ErrorType& type, std::string error) {
    m_lastId = id;
    ACSDK_ERROR(LX("onPlaybackError").d("type", type).d("error", error));
};

void MockPlayerObserver::onPlaybackPaused(SourceId id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastId = id;
    m_playbackPaused = true;
    m_wakePlaybackPaused.notify_all();
};

void MockPlayerObserver::onPlaybackResumed(SourceId id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastId = id;
    m_playbackResumed = true;
    m_playbackPaused = false;
    m_wakePlaybackResumed.notify_all();
};

void MockPlayerObserver::onPlaybackStopped(SourceId id) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastId = id;
    m_playbackStopped = true;
    m_playbackStarted = false;
    m_wakePlaybackStopped.notify_all();
};

bool MockPlayerObserver::waitForPlaybackStarted(SourceId id, const std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_wakePlaybackStarted.wait_for(lock, duration, [this, id]() { return m_playbackStarted && id == m_lastId; })) {
        return false;
    }
    return true;
}

bool MockPlayerObserver::waitForPlaybackFinished(SourceId id, const std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_wakePlaybackFinished.wait_for(
            lock, duration, [this, id]() { return m_playbackFinished && id == m_lastId; })) {
        return false;
    }
    return true;
}

bool MockPlayerObserver::waitForPlaybackPaused(SourceId id, const std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_wakePlaybackPaused.wait_for(lock, duration, [this, id]() { return m_playbackPaused && id == m_lastId; })) {
        return false;
    }
    return true;
}

bool MockPlayerObserver::waitForPlaybackResumed(SourceId id, const std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_wakePlaybackResumed.wait_for(lock, duration, [this, id]() { return m_playbackResumed && id == m_lastId; })) {
        return false;
    }
    return true;
}

bool MockPlayerObserver::waitForPlaybackStopped(SourceId id, const std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_wakePlaybackStopped.wait_for(lock, duration, [this, id]() { return m_playbackStopped && id == m_lastId; })) {
        return false;
    }
    return true;
}

int MockPlayerObserver::getOnPlaybackStartedCallCount() {
    return m_onPlaybackStartedCallCount;
}

int MockPlayerObserver::getOnPlaybackFinishedCallCount() {
    return m_onPlaybackFinishedCallCount;
}

int MockPlayerObserver::getOnTagsCallCount() {
    return m_onTagsCallCount;
}

void MockPlayerObserver::onTags(SourceId id, std::unique_ptr<const VectorOfTags> vectorOfTags) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_lastId = id;
    m_tags = true;
    m_onTagsCallCount++;
    m_wakeTags.notify_all();
}

bool MockPlayerObserver::waitForTags(SourceId id, const std::chrono::milliseconds duration) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (!m_wakeTags.wait_for(lock, duration, [this, id]() { return m_tags && id == m_lastId; })) {
        return false;
    }
    return true;
}

}  // namespace test
}  // namespace mediaPlayer
}  // namespace localPlayerSDK
int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << std::string(argv[0]) << " <absolute path to test inputs folder>" << std::endl;
    } else {
        localPlayerSDK::mediaPlayer::test::inputsDirPath = std::string(argv[1]);

        /// An instance of the @c MediaPlayer
        std::shared_ptr<localPlayerSDK::mediaPlayer::MediaPlayer> m_mediaPlayer;

        /// An observer to whom the playback started and finished notifications need to be sent.
        std::shared_ptr<localPlayerSDK::mediaPlayer::test::MockPlayerObserver> m_playerObserver;

        m_playerObserver = std::make_shared<localPlayerSDK::mediaPlayer::test::MockPlayerObserver>();
        m_mediaPlayer = localPlayerSDK::mediaPlayer::MediaPlayer::create(std::make_shared<localPlayerSDK::mediaPlayer::test::MockContentFetcherFactory>());
        m_mediaPlayer->setObserver(m_playerObserver);

        localPlayerSDK::mediaPlayer::MediaPlayer::SourceId sourceId;
        bool repeat = false;
        sourceId = m_mediaPlayer->setSource(localPlayerSDK::rkpCommon::utils::memory::make_unique<std::ifstream>(localPlayerSDK::mediaPlayer::test::inputsDirPath), repeat);
        m_mediaPlayer->play(sourceId);
        m_playerObserver->waitForPlaybackStarted(sourceId);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::chrono::milliseconds offset = m_mediaPlayer->getOffset(sourceId);
        fprintf(stderr,"offset:%d\n", offset);

        m_mediaPlayer->pause(sourceId);
        m_playerObserver->waitForPlaybackPaused(sourceId);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        m_mediaPlayer->resume(sourceId);
        m_playerObserver->waitForPlaybackResumed(sourceId);
        m_playerObserver->waitForPlaybackFinished(sourceId);

        return 0;
    }
}




