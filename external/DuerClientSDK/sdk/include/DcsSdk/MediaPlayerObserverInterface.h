/*
 * MediaPlayerObserverInterface.h
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

#ifndef DUEROS_DCS_SDK_DCS_SDK_INCLUDE_MEDIA_PLAYER_OBSERVER_INTERFACE_H
#define DUEROS_DCS_SDK_DCS_SDK_INCLUDE_MEDIA_PLAYER_OBSERVER_INTERFACE_H

#include <string>

#include <DcsSdk/ErrorTypes.h>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief A player observer will receive notifications when the player starts playing or when it stops playing a stream.
 *     A pointer to the @c MediaPlayerObserverInterface needs to be provided to a @c MediaPlayer for it to notify the
 *     observer.
 * \else
 *     @brief 播放器状态观察者接口。
 * \endif
 */
class MediaPlayerObserverInterface {
public:
    /**
     * \if english
     *     @brief Destructor
     * \else
     *     @brief 析构函数
     * \endif
     */
    virtual ~MediaPlayerObserverInterface() = default;

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer has starting playing the audio data.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器真正开始播放时回调此方法。
     *
     *     @note 这个方法需要在play执行后尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onPlaybackStarted() = 0;

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer has starting finished the audio data.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器真正播放结束时回调此方法。
     *
     *     @note 这个方法需要在播放结束后尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onPlaybackFinished() = 0;

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer encountered an error. Errors can occur during
     *     playback.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     *
     *     @param[in] type The DCS error to report to DCS
     *     @param[in] error The message to report to DCS
     * \else
     *     @brief 播放器播放遇到错误时回调此方法。
     *
     *     @note 这个方法需要在播放遇到错误时尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     *
     *     @param[in] type 错误消息类型
     *     @param[in] error 错误消息内容
     * \endif
     */
    virtual void onPlaybackError(const ErrorType& type, std::string error) = 0;

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer has paused playing the audio data.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器播放暂停时回调此方法。
     *
     *     @note 这个方法需要在播放暂停后尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onPlaybackPaused(){};

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer has resumed playing the audio data.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器播放恢复时回调此方法。
     *
     *     @note 这个方法需要在播放恢复后尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onPlaybackResumed(){};

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer has stopped the source.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器播放停止时回调此方法。
     *
     *     @note 这个方法需要在播放停止时尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onPlaybackStopped(){};

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer is experiencing a buffer underrun.
     *     This will only be sent after playback has started. Playback will be paused until the buffer is filled.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器读不到数据进入缓冲状态时回调此方法。
     *
     *     @note 这个方法需要在播放进入缓冲状态时尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onBufferUnderrun() {
    }

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer's buffer has refilled. This will only be sent after
     *     playback has started. Playback will resume.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器缓冲到足够数据时回调此方法。
     *
     *     @note 这个方法需要在播放器缓冲到足够数据时尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onBufferRefilled() {
    }

    /**
     * \if english
     *     @brief This is an indication to the observer that the @c MediaPlayer will finish the audio data.
     *
     *     @note The observer has to return soon. Otherwise this could block the @c MediaPlayer from processing other
     *     signals or playback.
     * \else
     *     @brief 当播放器播放即将结束时回调此方法。
     *
     *     @note 这个方法需要在播放即将结束时尽快回调，否则可能会影响MediaPlayer对其它消息的处理。
     * \endif
     */
    virtual void onPlaybackNearlyfinished() {
    }

    /**
     * \if english
     *     @brief SDK will call this function when receive first packet.
     * \else
     *     @brief 播放器接收到第一包数据时回调此方法。
     * \endif
     */
    virtual void onRecvFirstpacket() {

    }
};

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_DCS_SDK_INCLUDE_MEDIA_PLAYER_OBSERVER_INTERFACE_H
