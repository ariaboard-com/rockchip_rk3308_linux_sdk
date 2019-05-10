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

#include "PcmResampler.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

PcmResampler* PcmResampler::s_instance = NULL;

PcmResampler* PcmResampler::getInstance() {
    if (s_instance == NULL) {
        s_instance = new PcmResampler();
    }

    return s_instance;
}

void PcmResampler::releaseInstance() {
    if (s_instance != NULL) {
        delete s_instance;
        s_instance = NULL;
    }
}

PcmResampler::PcmResampler() {
    uint64_t out_channel_layout = AV_CH_LAYOUT_MONO;
    uint64_t in_channel_layout = AV_CH_LAYOUT_MONO;

    AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
    AVSampleFormat in_sample_fmt = AV_SAMPLE_FMT_S16;
    int in_sample_rate = 16000;
    int out_sample_rate = 48000;
    m_convertCtx = swr_alloc();
    m_convertCtx = swr_alloc_set_opts(m_convertCtx,
                                      out_channel_layout,
                                      out_sample_fmt,
                                      out_sample_rate,
                                      in_channel_layout,
                                      in_sample_fmt,
                                      in_sample_rate,
                                      0,
                                      NULL);
    swr_init(m_convertCtx);
}

PcmResampler::~PcmResampler() {
    swr_free(&m_convertCtx);
}

void PcmResampler::pcmResample(uint8_t** out, int out_count, const uint8_t** in, int in_count) {
    swr_convert(m_convertCtx, out, out_count, in, in_count);
}

}  // mediaPlayer
}  // duerOSDcsApp