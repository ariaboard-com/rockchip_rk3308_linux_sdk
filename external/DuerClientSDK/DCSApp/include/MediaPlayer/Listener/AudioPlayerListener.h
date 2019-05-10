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
 *  See the License for the specific language governing permissions andN
 *  limitations under the License.
 */

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_AUDIOPLAYERLISTENER_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_AUDIOPLAYERLISTENER_H_

namespace duerOSDcsApp {
namespace mediaPlayer {

enum AudioPlayerStatus {
    AUDIOPLAYER_STATUS_IDLE,             //空闲
    AUDIOPLAYER_STATUS_BUFFERING,        //缓冲
    AUDIOPLAYER_STATUS_PLAY,             //播放
    AUDIOPLAYER_STATUS_STOP,             //停止
    AUDIOPLAYER_STATUS_FINISHED          //结束
};

enum AudioPlayerFinishStatus {
    AUDIOPLAYER_FINISHSTATUS_STOP,    //主动调stop
    AUDIOPLAYER_FINISHSTATUS_END,     //音乐自然播放完成
};

enum PlayProgressInfo {
    BEFORE_START,
    DURING_PLAY
};

class AudioPlayerListener {
public:
    virtual ~AudioPlayerListener() {}

    virtual void playbackStarted(int offset_ms) = 0;

    virtual void playbackBufferunderrun(PlayProgressInfo playProgressInfo) {}

    virtual void playbackBufferefilled() {}

    virtual void playbackStopped(int offset_ms) = 0;

    virtual void playbackPaused(int offset_ms) = 0;

    virtual void playbackResumed(int offset_ms) = 0;

    virtual void playbackNearlyFinished(int offset_ms) = 0;

    virtual void playbackFinished(int offset_ms, AudioPlayerFinishStatus status) = 0;

    virtual void playbackProgress(int offset_ms) = 0;

    virtual void playbackStreamUnreach() = 0;

    virtual void playbackError() = 0;

    virtual void reportBufferTime(long time) = 0;
};

}  // mediaPlayer
}  // duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_AUDIOPLAYERLISTENER_H_
