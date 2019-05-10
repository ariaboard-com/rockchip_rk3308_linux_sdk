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

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3FILEPLAYERIMPL_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3FILEPLAYERIMPL_H_

#include <DcsSdk/PlayerAvtivityObserver.h>
#include "DCSApp/ThreadPoolExecutor.h"
#include "Mp3FilePlayerInterface.h"
#include "AlsaController.h"
#include "AudioDecoder.h"
#include "AudioPlayerListener.h"
#include <atomic>

namespace duerOSDcsApp {
namespace mediaPlayer {
using application::ThreadPoolExecutor;
using duerOSDcsSDK::sdkInterfaces::PlayerAvtivityObserver;

class Mp3FilePlayerImpl : public Mp3FilePlayerInterface {
public:
    Mp3FilePlayerImpl(const std::string& audio_dev);

    ~Mp3FilePlayerImpl();

    void setPlayerObserver(PlayerAvtivityObserver* observer);

    void registerListener(std::shared_ptr<AudioPlayerListener> notify) override;

    /**
     * set play mode of the player.
     *
     * @param mode the mode : PLAYMODE_NORMAL PLAYMODE_LOOP
     * @param val loop times, useful when the mode is PLAYMODE_LOOP; loop forever when 0.
     * @return void
     */
    void setPlayMode(PlayMode mode, unsigned int val) override;

    void audioPlay(const std::string& url,
                   unsigned int offset,
                   unsigned int report_interval) override;

    void audioPause() override;

    void audioResume() override;

    void audioStop() override;

    void setMute() override;

    void setUnmute() override;

    unsigned long getProgress() override;

    unsigned long getDuration() override;

    void setFadeIn(int timeSec) override;

    void executePlaybackStarted();

    void executePlaybackFinished();

    static std::shared_ptr<Mp3FilePlayerImpl> create(const std::string& audio_dev);

private:
    Mp3FilePlayerImpl(const Mp3FilePlayerImpl&);

    Mp3FilePlayerImpl& operator=(const Mp3FilePlayerImpl&);

    static void* playFunc(void* arg);

    float getFadeAttenuate();

    void fadeinReset();

    void fadeinProcess(int  size);

private:
    ThreadPoolExecutor* m_executor;
    AlsaController* m_alsaController;
    AudioDecoder* m_audioDecoder;
    std::shared_ptr<AudioPlayerListener> m_listener;
    uint8_t* m_pcmBuffer;
    uint8_t* m_frameBuffer;
    std::string m_resPath;
    int m_FadeinTimeMs;
    int m_FadeinTimeProgressMs;
    pthread_t m_threadId;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_playCond;
    std::atomic<bool> m_muteFlag;
    std::atomic<bool> m_stopFlag;
    std::atomic<bool> m_playReadyFlag;
    std::atomic<bool> m_exitFlag;
    PlayerAvtivityObserver* m_observer;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3FILEPLAYERIMPL_H_
