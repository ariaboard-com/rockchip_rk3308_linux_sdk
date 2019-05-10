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

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3TTSPLAYERIMPL_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3TTSPLAYERIMPL_H_

#include <DcsSdk/PlayerAvtivityObserver.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <vector>
#include "AlsaController.h"
#include "StreamPool.h"
#include "TtsPlayerListener.h"
#include "Mp3TtsPlayerInterface.h"
#include "DCSApp/ThreadPoolExecutor.h"


#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
};
#endif

namespace duerOSDcsApp {
namespace mediaPlayer {
using duerOSDcsSDK::sdkInterfaces::PlayerAvtivityObserver;

class Mp3TtsPlayerImpl : public Mp3TtsPlayerInterface {
public:
    Mp3TtsPlayerImpl(const std::string& audio_dev);

    ~Mp3TtsPlayerImpl();

    void setPlayerObserver(PlayerAvtivityObserver* observer);

    void registerListener(std::shared_ptr<TtsPlayerListener> notify) override;

    void ttsPlay() override;

    void pushData(const char* data, unsigned int len) override;

    void ttsEnd() override;

    void ttsStop() override;

    static std::shared_ptr<Mp3TtsPlayerImpl> create(const std::string& audio_dev);

private:
    Mp3TtsPlayerImpl(const Mp3TtsPlayerImpl&);

    Mp3TtsPlayerImpl& operator=(const Mp3TtsPlayerImpl&);

    void play_stream(unsigned int channels,
                     const void* buffer,
                     unsigned int buff_size);

    void executePlaybackStarted();

    void executePlaybackStopped();

    static void* playFunc(void* arg);

    static int readPacket(void* opaque, uint8_t* buf, int buf_size);

    static void ffmpegLogOutput(void* ptr, int level, const char* fmt, va_list vl);

private:
    application::ThreadPoolExecutor* m_executor;
    AlsaController* m_alsaCtl;
    std::vector<std::shared_ptr<TtsPlayerListener> > m_listeners;
    uint8_t* m_pcmResampleBuf;
    uint8_t* m_avioBuf;
    pthread_t m_playThread;
    pthread_mutex_t m_playMutex;
    pthread_cond_t m_playCond;
    StreamPool m_streamPool;
    unsigned int m_packetCount;
    bool m_pushingFlag;
    bool m_stopFlag;
    bool m_isFirstPacket;
    bool m_removeHeadFlag;
    bool m_logCtlFlag;
    bool m_aliveFlag;
    PlayerAvtivityObserver* m_observer;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3TTSPLAYERIMPL_H_
