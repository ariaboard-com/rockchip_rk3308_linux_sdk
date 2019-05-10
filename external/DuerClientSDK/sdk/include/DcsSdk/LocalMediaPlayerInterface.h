/*
 * MediaPlayerInterface.h
 *
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_MEDIA_PLAYER_INTERFACE_H
#define DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_MEDIA_PLAYER_INTERFACE_H

#include <cstdint>
#include <memory>

#include "DcsSdk/LocalMediaPlayerObserverInterface.h"
namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief An enum class used to specify the status of an operation performed by the @c MediaPlayer.
 * \else
 *     @brief 本地(蓝牙播放)播放器状态枚举类
 * \endif
 */
enum class LocalMediaPlayerStatus {

    /**
     * \if english
     *     @brief The operation was successful.
     * \else
     *     @brief 本地(蓝牙播放)播放器播放成功
     * \endif
     */
    LOCAL_MEDIA_PLAYER_SUCCESS,

    /**
     * \if english
     *     @brief The operation is pending. If there is an error, it may be notified via onPlaybackError.
     * \else
     *     @brief 本地(蓝牙播放)播放器正在尝试播放中，当前播放成功/失败状态未知
     * \endif
     */
    LOCAL_MEDIA_PLAYER_PENDING,

    /**
     * \if english
     *     @brief An error was encountered and the operation failed.
     * \else
     *     @brief 本地(蓝牙播放)播放器播放失败
     * \endif
     */
    LOCAL_MEDIA_PLAYER_FAILURE
};

/**
 * \if english
 *     @brief LocalMediaPlayerInterface class
 * \else
 *     @brief 本地(蓝牙播放)播放器播放控制接口类
 * \endif
 */
class LocalMediaPlayerInterface {
public:
    /**
     * \if english
     *     @brief Destructor.
     * \else
     *     @brief 析构函数
     * \endif
     */
    virtual ~LocalMediaPlayerInterface() = default;

    /**
     * \if english
     *     @brief Start playing audio. The source should be set before issuing @c play. If @c play is called without
     *     setting source, it will return an error. If @c play is called when audio is already playing,
     *     there is no effect. Status returned will be @c LOCAL_MEDIA_PLAYER_SUCCESS.
     *     If @c play is called again after @c stop on the same source, then the audio plays from the beginning.
     *
     *     @return @c LOCAL_MEDIA_PLAYER_SUCCESS if the state transition to play was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackStarted or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c LOCAL_MEDIA_PLAYER_FAILURE.
     * \else
     *     @brief 本地播放器开始播放
     *     @param void
     *     @return LocalMediaPlayerStatus 播放是否成功
     * \endif
     */
    virtual LocalMediaPlayerStatus play(const LocalMediaPlayerName& name) = 0;

    /**
     * \if english
     *     @brief Stop playing the audio. Once audio has been stopped, starting playback again will start from the beginning.
     *     The source should be set before issuing @c stop. If @c stop is called without setting source, it will
     *     return an error.
     *     If @c stop is called when audio has already stopped, there is no effect. Status returned will be @c LOCAL_MEDIA_PLAYER_SUCCESS.
     *
     *     @return @c LOCAL_MEDIA_PLAYER_SUCCESS if the state transition to stop was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackStarted or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c LOCAL_MEDIA_PLAYER_FAILURE.
     * \else
     *     @brief 本地播放器结束播放
     *     @return LocalMediaPlayerStatus 结束播放是否成功
     * \endif
     */
    virtual LocalMediaPlayerStatus stop(const LocalMediaPlayerName& name) = 0;

    /**
     * \if english
     *     @brief Pause playing the audio. Once audio has been paused, calling @c resume() will start the audio.
     *     The source should be set before issuing @c pause. If @c pause is called without setting source, it will
     *     return an error.
     *     Calling @c pause will only have an effect when audio is currently playing. Calling @c pause in all other states will have no effect,
     *     and result in a return of @c LOCAL_MEDIA_PLAYER_FAILURE.
     *
     *     @return @c LOCAL_MEDIA_PLAYER_SUCCESS if the state transition to pause was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackPaused or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c LOCAL_MEDIA_PLAYER_FAILURE.
     * \else
     *     @brief 本地播放器暂停播放
     *     @return LocalMediaPlayerStatus 暂停播放是否成功
     * \endif
     */
    virtual LocalMediaPlayerStatus pause(const LocalMediaPlayerName& name) = 0;

    /**
     * \if english
     *     @brief Resume playing the paused audio. The source should be set before issuing @c resume. If @c resume is called without setting source, it will
     *     return an error.
     *     Calling @c resume will only have an effect when audio is currently paused. Calling @c resume in other states will have no effect,
     *     and result in a return of @c LOCAL_MEDIA_PLAYER_FAILURE.
     *
     *     @return @c LOCAL_MEDIA_PLAYER_SUCCESS if the state transition to play was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackResumed or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c LOCAL_MEDIA_PLAYER_FAILURE.
     * \else
     *     @brief 本地播放器继续播放
     *     @return LocalMediaPlayerStatus 继续播放是否成功
     * \endif
     */
    virtual LocalMediaPlayerStatus resume(const LocalMediaPlayerName& name) = 0;

    /**
     * \if english
     *     @brief Play Next audio
     *     @return LocalMediaPlayerStatus the local mediaPlayer play status
     * \else
     *     @brief 播放下一首
     *     @return LocalMediaPlayerStatus 播放下一首是否成功
     * \endif
     */
    virtual LocalMediaPlayerStatus playNext(const LocalMediaPlayerName& name) = 0;

    /**
     * \if english
     *     @brief Play Previous audio
     *     @return LocalMediaPlayerStatus the local mediaPlayer play status
     * \else
     *     @brief 播放上一首
     *     @return LocalMediaPlayerStatus 播放上一首是否成功
     * \endif
     */
    virtual LocalMediaPlayerStatus playPrevious(const LocalMediaPlayerName& name) = 0;

    /**
     * \if english
     *     @brief Set Observer/Listener
     *     @param playerObserver
     * \else
     *     @brief 设置本地播放器观察者/监听者对象
     *     @param playerObserver 观察者/监听者对象
     * \endif
     */
    virtual void setObserver(
            std::shared_ptr<LocalMediaPlayerObserverInterface> playerObserver) = 0;
};

} // namespace sdkInterfaces
} // namespace duerOSDcsSDK

#endif // DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_MEDIA_PLAYER_INTERFACE_H
