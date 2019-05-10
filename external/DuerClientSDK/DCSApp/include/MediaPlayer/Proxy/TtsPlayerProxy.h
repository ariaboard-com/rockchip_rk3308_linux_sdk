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

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_TTSPLAYERPROXY_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_TTSPLAYERPROXY_H_

#include <cstdint>
#include <string>

#include <DcsSdk/AttachmentReader.h>
#include <DcsSdk/MediaPlayerInterface.h>
#include "DCSApp/ThreadPoolExecutor.h"

#include "Mp3TtsPlayerInterface.h"
#include "PcmTtsPlayerInterface.h"

///@cxt 20180207
#include <DcsSdk/Stream.h>

namespace duerOSDcsApp {
namespace mediaPlayer {

using duerOSDcsSDK::sdkInterfaces::AttachmentReader;
///@cxt 20180207
using duerOSDcsSDK::sdkInterfaces::Stream;

/**
 * Class that handles creation of audio pipeline and playing of audio data.
 */
class TtsPlayerProxy :
    public duerOSDcsSDK::sdkInterfaces::MediaPlayerInterface,
    public TtsPlayerListener {
public:
    /**
     * Creates an instance of the @c MediaPlayer.
     *
     * @return An instance of the @c MediaPlayer if successful else a @c nullptr.
     */
    static std::shared_ptr<TtsPlayerProxy> create(const std::string& audio_device = "default");

    /**
     * Destructor.
     */
    ~TtsPlayerProxy();

    void setStreamFormat(const std::string& streamFormat);

    /// @name Overridden MediaPlayerInterface methods.
    /// @{
    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(
        std::shared_ptr<AttachmentReader> attachmentReader) override;

    ///@cxt 20180207
    void setStream(std::shared_ptr<Stream> stream) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(
        const std::string& audio_file_path, bool repeat) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(const std::string& url) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus play() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus stop() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus pause() override;
    /**
     * To resume playback after a pause, call @c resume. Calling @c play
     * will reset the pipeline and source, and will not resume playback.
     */
    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus resume() override;

    std::chrono::milliseconds getOffset() override;

    void setObserver(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface>
                     observer) override;

    void setMp3TtsImpl(std::shared_ptr<Mp3TtsPlayerInterface> impl);

    void setPcmTtsImpl(std::shared_ptr<duerOSDcsApp::mediaPlayer::PcmTtsPlayerInterface> impl);

private:
    /**
     * Constructor.
     */
    TtsPlayerProxy(const std::string& audio_device);

    /**
     * Initializes GStreamer and starts a main event loop on a new thread.
     *
     * @return @c SUCCESS if initialization was successful. Else @c FAILURE.
     */
    bool init();

    void executeMp3Play();

    void executePcmPlay();

    /**
     * Worker thread handler for setting the source of audio to play.
     *
     * @param promise A promise to fulfill with a @ MediaPlayerStatus value once the source has been set
     * (or the operation failed).
     * @param reader The @c AttachmentReader with which to receive the audio to play.
     */
    void handleSetAttachmentReaderSource(
        std::shared_ptr<AttachmentReader> reader);

    void executeRecvFirstpacket();

    void playbackStarted() override;

    void playbackStopped(StopStatus stopStatus) override;

private:
    /// @c MediaPlayerObserverInterface instance to notify when the playback state changes.
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> m_playerObserver;

    ///@cxt 20180207
    std::shared_ptr<Stream> m_attachmentStream;

    std::shared_ptr<Mp3TtsPlayerInterface> m_mp3Impl;
    std::shared_ptr<PcmTtsPlayerInterface> m_pcmImpl;

    bool m_isFormatMp3;

    /**
     * @c Executor which queues up operations from asynchronous API calls.
     *
     * @note This declaration needs to come *after* any variables used by the executor thread so that the thread shuts
     *     down before the variables are destroyed.
     */
    application::ThreadPoolExecutor* m_executor;

};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_TTSPLAYERPROXY_H_
