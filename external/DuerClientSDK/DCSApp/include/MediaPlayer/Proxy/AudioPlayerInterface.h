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

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_AUDIOPLAYERINTERFACE_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_AUDIOPLAYERINTERFACE_H_

#include <string>
#include "AudioPlayerListener.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

enum RES_FORMAT {
    AUDIO_WAV,
    AUDIO_MP3,
    AUDIO_M3U8,
    AUDIO_M4A,
    AUDIO_OTH
};

class IAudioPlayer {
public:
    IAudioPlayer() {}

    virtual ~IAudioPlayer() {}

    virtual void registerListener(AudioPlayerListener* notify) = 0;

    virtual void audioPlay(const std::string& url,
                           RES_FORMAT format,
                           unsigned int offset,
                           unsigned int report_interval) = 0;

    virtual void audioPlay() {}
    virtual void audioPause() = 0;

    virtual void audioResume() = 0;

    virtual void audioStop() = 0;

    void audioPlayNext() {};

    void audioPlayPrevious() {};

    virtual unsigned long getProgress() = 0;

    virtual unsigned long getDuration() = 0;

private:
    IAudioPlayer(const IAudioPlayer&);

    IAudioPlayer& operator=(const IAudioPlayer&);
};

}  // mediaPlayer
}  // duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_AUDIOPLAYERINTERFACE_H_
