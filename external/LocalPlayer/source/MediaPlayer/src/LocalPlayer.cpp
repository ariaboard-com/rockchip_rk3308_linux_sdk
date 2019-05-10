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

//#include <gmock/gmock.h>
//#include <gtest/gtest.h>

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
using namespace ::testing;

/// String to identify log entries originating from this file.
static const std::string TAG("MediaPlayerTest");

static const MediaPlayer::SourceId ERROR_SOURCE_ID = MediaPlayer::ERROR;

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) localPlayerSDK::rkpCommon::utils::logger::LogEntry(TAG, event)

/// The path to the input Dir containing the test audio files.
std::string inputsDirPath;

/// MP3 test file path.
static const std::string MP3_FILE_PATH("/fox_dog.mp3");

static const std::string TEST_M3U_PLAYLIST_URL("fox_dog_playlist.m3u");

static std::string TEST_M3U_PLAYLIST_CONTENT;

/// file URI Prefix
static const std::string FILE_PREFIX("file://");

/// File length for the MP3 test file.
static const std::chrono::milliseconds MP3_FILE_LENGTH(2688);

#ifdef URL_TESTS_RESOLVED

// setOffset timing constants.

/// Offset to start playback at.
static const std::chrono::milliseconds OFFSET(2000);

/// Tolerance when setting expectations.
static const std::chrono::milliseconds TOLERANCE(200);

/// Padding to add to offsets when necessary.
static const std::chrono::milliseconds PADDING(10);

#endif

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

/**
 * Mock AttachmentReader.
 */
class MockAttachmentReader : public AttachmentReader {
public:
    /**
     * Constructor.
     *
     * @param iterations The number of times this AttachmentReader will (re)read the input file before @c read
     * will return a @c CLOSED status.
     * @param receiveSizes An vector of sizes (in bytes) that this @c AttachmentReceiver will simulate receiving.
     * Each successive element in the vector corresponds to a successive 100 millisecond interval starting from
     * the time this @c MockAttachmentReader was created.
     */
    MockAttachmentReader(int iterations = 1, std::vector<size_t> receiveSizes = {std::numeric_limits<size_t>::max()});

    size_t read(void* buf, std::size_t numBytes, ReadStatus* readStatus, std::chrono::milliseconds timeoutMs) override;

    void close(ClosePoint closePoint) override;

    bool seek(uint64_t offset) override {
        return true;
    }

    /**
     * Receive bytes from the test file.
     *
     * @param buf The buffer to receive the bytes.
     * @param size The number of bytes to receive.
     * @return The number of bytes received.
     */
    size_t receiveBytes(char* buf, std::size_t size);

    /// The number of iterations of reading the input file that are left before this reader returns closed.
    int m_iterationsLeft;

    /**
     * The total number of bytes that are supposed to have been received (and made available) by this
     * @c AttachmentReader at 100 millisecond increments from @c m_startTime.
     */
    std::vector<size_t> m_receiveTotals;

    /// The start of time for reading from this AttachmentReader.
    std::chrono::steady_clock::time_point m_startTime;

    /// The number of bytes returned so far by @c read().
    size_t m_totalRead;

    /// The current ifstream (if any) from which to read the attachment.
    std::unique_ptr<std::ifstream> m_stream;
};

MockAttachmentReader::MockAttachmentReader(int iterations, std::vector<size_t> receiveSizes) :
        m_iterationsLeft{iterations},
        m_startTime(std::chrono::steady_clock::now()),
        m_totalRead{0} {
    // Convert human friendly vector of received sizes in to a vector of received totals.
    EXPECT_GT(receiveSizes.size(), 0U);
    m_receiveTotals.reserve(receiveSizes.size());
    size_t total = 0;
    for (size_t ix = 0; ix < receiveSizes.size(); ix++) {
        total += receiveSizes[ix];
        m_receiveTotals.push_back(total);
    }
    EXPECT_EQ(m_receiveTotals.size(), receiveSizes.size());
}

size_t MockAttachmentReader::read(
    void* buf,
    std::size_t numBytes,
    ReadStatus* readStatus,
    std::chrono::milliseconds timeoutMs) {
    // Convert the current time in to an index in to m_receivedTotals (time since @c m_startTime
    // divided by 100 millisecond intervals).
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startTime);
    auto index = static_cast<size_t>(elapsed.count() / 100);
    index = std::min(index, m_receiveTotals.size() - 1);

    // Use the index to find the total number of bytes received by the @c MockAttachmentReader so far. Then subtract
    // the number of bytes consumed by @c read() so far to calculate the number of bytes available for @c read().
    auto receivedTotal = m_receiveTotals[index];
    EXPECT_LE(m_totalRead, receivedTotal);
    auto available = receivedTotal - m_totalRead;

    // Use the avaialable number of bytes to calculate how many bytes @c read() should return and the status
    // that should accompany them.  Also perform the actual read in to the output buffer.
    size_t result = 0;
    auto status = ReadStatus::ERROR_INTERNAL;
    if (available > 0) {
        auto sizeToRead = std::min(available, numBytes);
        result = receiveBytes(static_cast<char*>(buf), sizeToRead);
        if (result > 0) {
            m_totalRead += result;
            status = (result == numBytes) ? ReadStatus::OK : ReadStatus::OK_WOULDBLOCK;
        } else {
            status = ReadStatus::CLOSED;
        }
    } else {
        status = ReadStatus::OK_WOULDBLOCK;
    }
    if (readStatus) {
        *readStatus = status;
    }
    return result;
}

void MockAttachmentReader::close(ClosePoint closePoint) {
    if (m_stream) {
        m_stream->close();
    }
}

size_t MockAttachmentReader::receiveBytes(char* buf, std::size_t size) {
    auto pos = buf;
    auto end = buf + size;
    while (pos < end) {
        if (!m_stream || m_stream->eof()) {
            if (m_iterationsLeft-- > 0) {
                m_stream = make_unique<std::ifstream>(inputsDirPath + MP3_FILE_PATH);
                EXPECT_TRUE(m_stream);
                EXPECT_TRUE(m_stream->good());
            } else {
                break;
            }
        }
        m_stream->read(pos, end - pos);
        pos += m_stream->gcount();
    }
    auto result = pos - buf;
    return result;
}

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

class MediaPlayerTest : public ::testing::Test {
public:
    void SetUp() override;

    /**
     * Sets the audio source to play.
     *
     */
    void setAttachmentReaderSource(
        MediaPlayer::SourceId* id,
        int iterations = 1,
        std::vector<size_t> receiveSizes = {std::numeric_limits<size_t>::max()});

    /**
     * Sets IStream source to play.
     *
     * @param repeat Whether to play the stream over and over until stopped.
     */
    void setIStreamSource(MediaPlayer::SourceId* id, bool repeat = false);

    /// An instance of the @c MediaPlayer
    std::shared_ptr<MediaPlayer> m_mediaPlayer;

    /// An observer to whom the playback started and finished notifications need to be sent.
    std::shared_ptr<MockPlayerObserver> m_playerObserver;
};

void MediaPlayerTest::SetUp() {
    m_playerObserver = std::make_shared<MockPlayerObserver>();
    m_mediaPlayer = MediaPlayer::create(std::make_shared<MockContentFetcherFactory>());
    ASSERT_TRUE(m_mediaPlayer);
    m_mediaPlayer->setObserver(m_playerObserver);
}

void MediaPlayerTest::setAttachmentReaderSource(
    MediaPlayer::SourceId* id,
    int iterations,
    std::vector<size_t> receiveSizes) {
    auto returnId =
        m_mediaPlayer->setSource(std::unique_ptr<AttachmentReader>(new MockAttachmentReader(iterations, receiveSizes)));
    ASSERT_NE(ERROR_SOURCE_ID, returnId);
    if (id) {
        *id = returnId;
    }
}

void MediaPlayerTest::setIStreamSource(MediaPlayer::SourceId* id, bool repeat) {
    auto returnId = m_mediaPlayer->setSource(make_unique<std::ifstream>(inputsDirPath + MP3_FILE_PATH), repeat);
    ASSERT_NE(ERROR_SOURCE_ID, returnId);
    if (id) {
        *id = returnId;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << std::string(argv[0]) << " <absolute path to test inputs folder>" << std::endl;
    } else {
        localPlayerSDK::mediaPlayer::test::inputsDirPath = std::string(argv[1]);
        localPlayerSDK::mediaPlayer::test::urlsToContentTypes.insert(
            {localPlayerSDK::mediaPlayer::test::FILE_PREFIX + localPlayerSDK::mediaPlayer::test::inputsDirPath +
                 localPlayerSDK::mediaPlayer::test::MP3_FILE_PATH,
             "audio/mpeg"});
        localPlayerSDK::mediaPlayer::test::urlsToContentTypes.insert(
            {localPlayerSDK::mediaPlayer::test::TEST_M3U_PLAYLIST_URL, "audio/mpegurl"});
        localPlayerSDK::mediaPlayer::test::TEST_M3U_PLAYLIST_CONTENT =
            std::string("EXTINF:2,fox_dog.mp3\n") +
            std::string(
                localPlayerSDK::mediaPlayer::test::FILE_PREFIX + localPlayerSDK::mediaPlayer::test::inputsDirPath +
                localPlayerSDK::mediaPlayer::test::MP3_FILE_PATH) +
            '\n' + std::string("EXTINF:2,fox_dog.mp3\n") +
            std::string(
                localPlayerSDK::mediaPlayer::test::FILE_PREFIX + localPlayerSDK::mediaPlayer::test::inputsDirPath +
                localPlayerSDK::mediaPlayer::test::MP3_FILE_PATH) +
            '\n';
        localPlayerSDK::mediaPlayer::test::urlsToContent.insert(
            {localPlayerSDK::mediaPlayer::test::TEST_M3U_PLAYLIST_URL,
             localPlayerSDK::mediaPlayer::test::TEST_M3U_PLAYLIST_CONTENT});


        /// An instance of the @c MediaPlayer
        std::shared_ptr<MediaPlayer> m_mediaPlayer;

        /// An observer to whom the playback started and finished notifications need to be sent.
        std::shared_ptr<MockPlayerObserver> m_playerObserver;

        m_playerObserver = std::make_shared<MockPlayerObserver>();
        m_mediaPlayer = MediaPlayer::create(std::make_shared<MockContentFetcherFactory>());
        m_mediaPlayer->setObserver(m_playerObserver);

        MediaPlayer::SourceId sourceId;
        bool repeat = false;
        sourceId = m_mediaPlayer->setSource(make_unique<std::ifstream>(inputsDirPath + MP3_FILE_PATH), repeat);
        m_mediaPlayer->play(sourceId);
        m_playerObserver->waitForPlaybackStarted(sourceId);
        std::this_thread::sleep_for(std::chrono::seconds(1));
        m_mediaPlayer->pause(sourceId);
        m_playerObserver->waitForPlaybackPaused(sourceId);
        m_mediaPlayer->resume(sourceId);
        m_playerObserver->waitForPlaybackResumed(sourceId);
        m_playerObserver->waitForPlaybackFinished(sourceId);

        return 0;
    }
}

}  // namespace test
}  // namespace mediaPlayer
}  // namespace localPlayerSDK



