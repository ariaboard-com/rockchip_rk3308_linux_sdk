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

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMTTSPLAYERIMPL_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMTTSPLAYERIMPL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <vector>
#include "AlsaController.h"
#include "StreamPool.h"
#include "TtsPlayerListener.h"
#include "PcmTtsPlayerInterface.h"
#include "DCSApp/ThreadPoolExecutor.h"

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
}
#endif

namespace duerOSDcsApp {
namespace mediaPlayer {

class PcmTtsPlayerImpl : public PcmTtsPlayerInterface {
public:
    PcmTtsPlayerImpl(const std::string& audio_dev);

    ~PcmTtsPlayerImpl();

    void registerListener(std::shared_ptr<TtsPlayerListener> listener) override;

    void ttsPlay() override;

    void pushData(const char* data, unsigned int len) override;

    void ttsEnd() override;

    void ttsStop() override;

    static std::shared_ptr<PcmTtsPlayerImpl> create(const std::string& audio_dev);

private:
    PcmTtsPlayerImpl(const PcmTtsPlayerImpl&);

    PcmTtsPlayerImpl& operator=(const PcmTtsPlayerImpl&);

    void play_stream(unsigned int channels,
                     const void* buffer,
                     unsigned int buff_size);

    void executePlaybackStarted();

    void executePlaybackStopped();

    static void* playFunc(void* arg);

private:
    application::ThreadPoolExecutor* m_executor;
    AlsaController* m_alsaCtl;
    std::vector<std::shared_ptr<TtsPlayerListener> > m_listeners;
    char* m_pcmBuffer;
    unsigned int m_currentLen;
    unsigned int m_spaceLen;
    pthread_t m_playThread;
    pthread_mutex_t m_playMutex;
    pthread_mutex_t m_memMutex;
    pthread_cond_t m_playCond;
    StreamPool m_streamPool;
    bool m_pushingFlag;
    bool m_stopFlag;
    bool m_isFirstPacket;
    bool m_logCtlFlag;
    bool m_aliveFlag;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMTTSPLAYERIMPL_H_
