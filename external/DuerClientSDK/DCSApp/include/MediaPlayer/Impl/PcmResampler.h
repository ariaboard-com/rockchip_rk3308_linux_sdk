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

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_PCMRESAMPLER_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_PCMRESAMPLER_H_

#ifdef __cplusplus
extern "C"
{
#endif
#include <libavcodec/avcodec.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus
};
#endif

namespace duerOSDcsApp {
namespace mediaPlayer {

class PcmResampler {
public:
    static PcmResampler* getInstance();

    static void releaseInstance();

    void pcmResample(uint8_t** out, int out_count, const uint8_t** in, int in_count);

private:
    PcmResampler();

    ~PcmResampler();

    PcmResampler(const PcmResampler&);

    PcmResampler& operator=(const PcmResampler&);

private:
    static PcmResampler* s_instance;
    struct SwrContext* m_convertCtx;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_PCMRESAMPLER_H_
