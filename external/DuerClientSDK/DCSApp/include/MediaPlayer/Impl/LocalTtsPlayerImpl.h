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

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_LOCALTTSPLAYERIMPL_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_LOCALTTSPLAYERIMPL_H_

#include <string>
#include <unistd.h>
#include "TTSManager/TTSManager.h"
#include "StreamPool.h"
#include "AlsaController.h"
#include "PcmResampler.h"
#include "LocalTtsPlayerInterface.h"
#include "DCSApp/ThreadPoolExecutor.h"
#include "PthreadLock.h"
#include "AutoLock.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

using duerOSDcsSDK::ttsManager::TTSManager;
using duerOSDcsSDK::ttsManager::TTSSynthesizeResultObserver;
using application::ThreadPoolExecutor;

class LocalTtsPlayerImpl : public LocalTtsPlayerInterface,
    public TTSSynthesizeResultObserver {
public:
    static std::shared_ptr<LocalTtsPlayerImpl> create(const std::string& audio_dev,
            const std::string& res_path);

    virtual ~LocalTtsPlayerImpl();

    void ttsPlay(const std::string& content) override;

    void ttsStop() override;

    void registerListener(std::shared_ptr<LocalTtsPlayerListener> notify) override;

private:
    LocalTtsPlayerImpl(const std::string& audio_dev,
                       const std::string& synthesize_res_path);

    LocalTtsPlayerImpl(const LocalTtsPlayerImpl&);

    LocalTtsPlayerImpl& operator=(const LocalTtsPlayerImpl&);

    TtsPlayerStatus getStatus();

    void setStatus(TtsPlayerStatus status);

    void executeSpeechStarted();

    void executeSpeechFinished();

    void ttsSynthesizeBegin(const std::string& strIndex);

    void ttsSynthesizeAudioData(const char* stream,
                                unsigned int length,
                                unsigned int charIndex,
                                const std::string& strIndex = "");

    void ttsSynthesizeFinish(const std::string& strIndex = "");

    void ttsSynthesizeFailed(int errCode,
                             const std::string& errDesc,
                             const std::string& strIndex = "");

    static void* playFunc(void* arg);

private:
    ThreadPoolExecutor* m_executor;
    TTSManager* m_ttsMgr;
    std::vector<std::shared_ptr<LocalTtsPlayerListener> > m_listeners;
    AlsaController* m_alsaCtl;
    StreamPool m_streamPool;
    pthread_t m_playThread;
    pthread_mutex_t m_playMutex;
    pthread_cond_t m_playCond;
    TtsPlayerStatus m_status;
    PthreadLock* m_lock;
    std::string m_ttsContent;
    bool m_firstPackFlag;
    bool m_threadAlive;
    bool m_pushStreamFlag;
    unsigned int m_maxValidLen;
};

}  // mediaPlayer
}  // duerOSDcsApp

#endif  // DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_LOCALTTSPLAYERIMPL_H_
