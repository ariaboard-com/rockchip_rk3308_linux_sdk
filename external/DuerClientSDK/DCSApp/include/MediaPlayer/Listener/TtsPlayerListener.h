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

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_TTSPLAYERLISTENER_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_TTSPLAYERLISTENER_H_

namespace duerOSDcsApp {
namespace mediaPlayer {

enum StopStatus {
    STOP_STATUS_FINISHED,
    STOP_STATUS_INTERRUPT
};

class TtsPlayerListener {
public:
    virtual void playbackStarted() = 0;

    virtual void playbackStopped(StopStatus stopStatus) = 0;
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_TTSPLAYERLISTENER_H_
