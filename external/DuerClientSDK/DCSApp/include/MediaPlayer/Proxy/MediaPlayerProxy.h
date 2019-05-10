/*
 * Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_MEDIAPLAYERPROXY_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_MEDIAPLAYERPROXY_H_

#include <DcsSdk/MediaPlayerInterface.h>

#include "OffsetManager.h"
#include "Mp3UrlPlayerInterface.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

using duerOSDcsSDK::sdkInterfaces::AttachmentReader;

class MediaPlayerProxy : public duerOSDcsSDK::sdkInterfaces::MediaPlayerInterface,
    public AudioPlayerListener {
public:
    static std::shared_ptr<MediaPlayerProxy> create(const std::string& audio_device = "default");

    ~MediaPlayerProxy();

    void setStreamFormat(const std::string& streamFormat);

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(
        std::shared_ptr<AttachmentReader> attachmentReader) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(const std::string& url) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(const std::string& audio_file_path,
            bool repeat) override;

    void setObserver(
        std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> playerObserver) override;

    /**
     * This function is a setter, storing @c offset to be consumed internally by @c play().
     * The function will always return MediaPlayerStatus::SUCCESS.
     */
    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setOffset(std::chrono::milliseconds offset) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus play() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus stop() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus pause() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus resume() override;

    std::chrono::milliseconds getOffset() override;

    /**
     * Performs a seek to the @c seekPoint.
     *
     * @return A boolean indicating whether the seek operation was successful.
     */
    bool seek(unsigned int positionMs);

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus getPosition(long* track_duration, long* track_pos);

    float getFactor();

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setFactor(float factor);

    void setImpl(std::shared_ptr<Mp3UrlPlayerInterface> impl);

private:
    MediaPlayerProxy(const std::string& audio_device);

    bool init();

    void playbackStarted(int offset_ms) override;

    void playbackBufferunderrun(PlayProgressInfo playProgressInfo) override;

    void playbackBufferefilled() override;

    void playbackStopped(int offset_ms) override;

    void playbackPaused(int offset_ms) override;

    void playbackResumed(int offset_ms) override;

    void playbackNearlyFinished(int offset_ms) override;

    void playbackFinished(int offset_ms, AudioPlayerFinishStatus status) override;

    void playbackProgress(int offset_ms) override;

    void playbackStreamUnreach() override;

    void playbackError() override;

    void reportBufferTime(long time) override;

    /**
     * Used to obtain seeking information about the pipeline.
     *
     * @param isSeekable A boolean indicating whether the stream is seekable.
     * @return A boolean indicating whether the operation was successful.
     */
    bool queryIsSeekable(bool* isSeekable);

    void executePlaybackStart();

    void executeUnderBuffer();

    void executePlaybackStop();

    void executePlaybackPause();

    void executePlaybackResume();

    void executePlaybackNearlyFinish();

    void executePlaybackFinish();

    void executePlaybackError();

    void executePlaybackOnBufferUnderrun(PlayProgressInfo playProgressInfo);

    void executePlaybackOnBufferRefilled();

private:
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> m_playerObserver;

    std::shared_ptr<Mp3UrlPlayerInterface> m_impl;

    std::string m_resource_url;

    unsigned long m_offset_ms;

    unsigned long m_latestNetworkStatusTimestamp;

    /// An instance of the @c OffsetManager.
    OffsetManager m_offsetManager;
};

}  // mediaPlayer
}  // duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_MEDIAPLAYERPROXY_H_
