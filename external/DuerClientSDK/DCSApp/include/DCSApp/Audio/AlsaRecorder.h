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

#include <alsa/asoundlib.h>
#include <signal.h>
#include <pthread.h>

#ifndef DUEROS_DCS_APP_SAMPLEAPP_ALSARECORDER_H_
#define DUEROS_DCS_APP_SAMPLEAPP_ALSARECORDER_H_

const int ALSA_REC_CHANNELS = 1;
const int ALSA_REC_FRAME_BITS = 16;
const int ALSA_REC_SAMPLE_RATE = 16000;
const int ALSA_REC_BUF_TIME = 500000;// 500ms
const int ALSA_REC_PERIOD_TIME = 10000; // 10ms

const int RECORD_ERR_BASE = 0;
const int RECORD_ERR_GENERAL = 1;
const int RECORD_ERR_MEMFAIL = 2;
const int RECORD_ERR_INVAL = 3;
const int RECORD_ERR_NOT_READY = 4;
const int RECORD_ERR_OPEN = 5;
const int RECORD_ERR_HANDLE = 6;
///recorder object
struct recorder {
    void (*on_data_ind)(char *data, unsigned long len, void *userdata);

    void * wavein_hdl;
    /* thread id may be a struct. by implementation
     * void * will not be ported!! */
    pthread_t rec_thread;
    /*void * rec_thread_hdl;*/

    char *audiobuf;
    int bits_per_frame;
    unsigned int buffer_time;
    unsigned int period_time;
    size_t period_frames;
    size_t buffer_frames;
    const char *alsa_rec_device;
    volatile sig_atomic_t running;
    void *userdata;
};

namespace duerOSDcsApp {
namespace application {
class AudioRecorder {
public:
    AudioRecorder();
    ~AudioRecorder();
    /**
     * @brief open a recorder.
     *
     * @param callback called when pcm data is ready.
     *
     * @return the error code, 0 on successful.
     */
    int recorderOpen(const char* alsa_rec_device,
             void(*callback)(char* data, unsigned long len, void *userdata),
             void *userdata);

    /**
     * @brief close a recorder.
     *
     * @param none.
     *
     * @return the error code, 0 on successful.
     */
    int recorderClose(void);
private:
    int setHwparams(struct recorder* rec);
    int setSwparams(struct recorder* rec) ;
    int prepareRecBuffer(struct recorder* rec);
    void freeRecBuffer(struct recorder* rec);
    int openRecorder(struct recorder* rec);
    struct recorder* _m_rec;
};
}
}
#endif
