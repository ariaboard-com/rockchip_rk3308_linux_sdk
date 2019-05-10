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

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMFILEPLAYERIMPL_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMFILEPLAYERIMPL_H_

#include "AlsaController.h"
#include "PcmFilePlayerInterface.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>

namespace duerOSDcsApp {
namespace mediaPlayer {

class PcmFilePlayerImpl : public PcmFilePlayerInterface {
public:
    PcmFilePlayerImpl(const std::string& audio_dev, const std::string& file_path);

    ~PcmFilePlayerImpl();

    void play() override;

    void stop() override;

private:
    void playBuffer();

    static void* playFunc(void* arg);

private:
    AlsaController* m_alsa_ctl;
    char* m_dat;
    size_t m_len;
    bool m_playing_flag;
    bool m_thread_alive;
    pthread_t m_thread_id;
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMFILEPLAYERIMPL_H_
