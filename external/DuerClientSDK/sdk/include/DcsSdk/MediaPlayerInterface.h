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

#ifndef DUEROS_DCS_SDK_DCS_SDK_INCLUDE_MEDIA_PLAYER_INTERFACE_H
#define DUEROS_DCS_SDK_DCS_SDK_INCLUDE_MEDIA_PLAYER_INTERFACE_H

#include <chrono>
#include <cstdint>
#include <future>
#include <memory>

#include "DcsSdk/AttachmentReader.h"
#include "DcsSdk/MediaPlayerObserverInterface.h"
#include "DcsSdk/LocalSourcePlayerInterface.h"
///@cxt 20180207
#include "DcsSdk/Stream.h"

namespace duerOSDcsSDK {
namespace sdkInterfaces {

using duerOSDcsSDK::sdkInterfaces::AttachmentReader;

/**
 * \if english
 *     @brief An enum class used to specify the status of an operation performed by the @c MediaPlayer.
 * \else
 *     @brief 表示MediaPlayer成员函数执行结果的枚举类。
 * \endif
 */
enum class MediaPlayerStatus {
    /**
     * \if english
     *     @brief The operation was successful.
     * \else
     *     @brief 执行成功。
     * \endif
     */
    SUCCESS,

    /**
     * \if english
     *     @brief The operation is pending. If there is an error, it may be notified via onPlaybackError.
     * \else
     *     @brief 正在执行。如果发生错误需要通过onPlaybackError上报状态。
     * \endif
     */
    PENDING,

    /**
     * \if english
     *     @brief An error was encountered and the operation failed.
     * \else
     *     @brief 发生错误，执行失败。
     * \endif
     */
    FAILURE
};

/**
 * \if english
 *     @brief Represents offset returned when MediaPlayer is in an invalid state.
 * \else
 *     @brief 表示播放器处于无效状态时所返回的播放时间偏移量。
 * \endif
 */
static const std::chrono::milliseconds MEDIA_PLAYER_INVALID_OFFSET{-1};

/**
 * \if english
 *     @brief A MediaPlayer allows for sourcing, playback control, navigation, and querying the state of media content.
 * \else
 *     @brief 播放器接口。
 * \endif
 */
class MediaPlayerInterface {
public:
    /**
     * \if english
     *     @brief Destructor
     * \else
     *     @brief 析构函数
     * \endif
     */
    virtual ~MediaPlayerInterface() = default;

    /**
     * \if english
     *     @brief Set the source to play. The source should be set before issuing @c play or @c stop.
     *     The @c MediaPlayer can handle only one source at a time.
     *
     *     @param[in] attachmentReader Object with which to read an incoming audio attachment.
     *
     *     @return @c SUCCESS if the the source was set successfully else @c FAILURE.
     * \else
     *     @brief 设置播放资源。在play或stop调用之前需要设置播放资源，MediaPlayer每次只能播放一个资源。
     *
     *     @param[in] attachmentReader 该对象用于读取音频数据。
     *
     *     @return SUCCESS表示设置成功，反之则FAILURE。
     * \endif
     */
    virtual MediaPlayerStatus setSource(
        std::shared_ptr<AttachmentReader> attachmentReader) = 0;

    virtual void setStreamFormat(const std::string& streamFormat) {}

    ///@cxt 20180207
    virtual void setStream(std::shared_ptr<Stream> stream) {}

    /**
     * \if english
     *     @brief Set the source to play. The source should be set before issuing @c play or @c stop.
     *     The @c MediaPlayer can handle only one source at a time.
     *
     *     @param[in] url The url to set as the source.
     *
     *     @return @c SUCCESS if the the source was set successfully else @c FAILURE.
     * \else
     *     @brief 设置播放资源。在play或stop调用之前需要设置播放资源，MediaPlayer每次只能播放一个资源。
     *
     *     @param[in] url 音频资源url。
     *
     *     @return SUCCESS表示设置成功，反之则FAILURE。
     * \endif
     */
    virtual MediaPlayerStatus setSource(const std::string& url) = 0;

    /**
     * \if english
     *     @brief Set the source to play. The source should be set before issuing @c play or @c stop.
     *     The @c MediaPlayer can handle only one source at a time.
     *
     *     @param[in] audio_file_path Object with which to read an incoming audio file.
     *     @param[in] repeat Whether the audio file should be played in a loop until stopped.
     *
     *     @return @c SUCCESS if the the source was set successfully else @c FAILURE.
     * \else
     *     @brief 设置播放资源。在play或stop调用之前需要设置播放资源，MediaPlayer每次只能播放一个资源。
     *
     *     @param[in] audio_file_path 音频资源文件路径。
     *     @param[in] repeat 是否循环播放这个资源。
     *
     *     @return SUCCESS表示设置成功，反之则FAILURE。
     * \endif
     */
    virtual MediaPlayerStatus setSource(const std::string& audio_file_path, bool repeat) = 0;

    /**
     * \if english
     *     @brief Set the offset for playback. A seek will be performed to the offset at the next @c play() command.
     *     @param[in] offset The offset in milliseconds to seek to.
     *     @return @c SUCCESS if the offset was successfully set, and FAILURE for any error.
     * \else
     *     @brief 设置播放位置偏移量。
     *     @param[in] offset 以毫秒为单位的播放偏移值。
     *     @return @c SUCCESS表示设置成功，FAILURE表示设置失败。
     * \endif
     */
    virtual MediaPlayerStatus setOffset(std::chrono::milliseconds offset) {
        return MediaPlayerStatus::FAILURE;
    }

    /**
     * \if english
     *     @brief Start playing audio. The source should be set before issuing @c play. If @c play is called without
     *     setting source, it will return an error. If @c play is called when audio is already playing,
     *     there is no effect. Status returned will be @c SUCCESS.
     *     If @c play is called again after @c stop on the same source, then the audio plays from the beginning.
     *
     *     @return @c SUCCESS if the state transition to play was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackStarted or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c FAILURE.
     * \else
     *     @brief 开始播放。在播放之前需要先设置资源，如果在资源未设置情况下进行播放，将会返回错误。
     *
     *     @return @c SUCCESS表示播放执行成功；FAILURE表示播放执行失败；PENDING表示正在执行，这时播放状态需要通过onPlaybackStarted
     *     或onPlaybackError来通知。
     * \endif
     */
    virtual MediaPlayerStatus play() = 0;

    /**
     * \if english
     *     @brief Stop playing the audio. Once audio has been stopped, starting playback again will start from the beginning.
     *     The source should be set before issuing @c stop. If @c stop is called without setting source, it will
     *     return an error.
     *     If @c stop is called when audio has already stopped, there is no effect. Status returned will be @c SUCCESS.
     *
     *     @return @c SUCCESS if the state transition to play was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackStarted or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c FAILURE.
     * \else
     *     @brief 停止播放。音频播放一旦停止，重新播放将会从头开始。音频资源需要在调用stop之前设置，如果没有设置播放资源就进行stop，将会返回错误。
     *
     *     @return @c SUCCESS表示播放执行成功；FAILURE表示播放执行失败；PENDING表示正在执行，这时播放状态需要通过onPlaybackStarted
     *     或onPlaybackError来通知。
     * \endif
     */
    virtual MediaPlayerStatus stop() = 0;

    /**
     * \if english
     *     @brief Pause playing the audio. Once audio has been paused, calling @c resume() will start the audio.
     *     The source should be set before issuing @c pause. If @c pause is called without setting source, it will
     *     return an error.
     *     Calling @c pause will only have an effect when audio is currently playing. Calling @c pause in all other states
     *     will have no effect, and result in a return of @c FAILURE.
     *
     *     @return @c SUCCESS if the state transition to pause was successful. If state transition is pending then it
     *     returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackPaused or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c FAILURE.
     *
     * \else
     *     @brief 暂停播放。音频播放一旦暂停，调用resume()将会恢复播放。音频资源需要在调用pause之前设置，如果没有设置播放资源就进行pause，将会返回错误。
     *     只有当音频正在播放过程中调用pause才会起作用，在其它所有状态下调用都会返回FAILURE。
     *
     *     @return @c SUCCESS表示播放执行成功；FAILURE表示播放执行失败；PENDING表示正在执行，这时播放状态需要通过onPlaybackStarted
     *     或onPlaybackError来通知。
     * \endif
     */
    virtual MediaPlayerStatus pause() = 0;

    /**
     * \if english
     *     @brief Resume playing the paused audio. The source should be set before issuing @c resume. If @c resume is called
     *     without setting source, it will return an error. Calling @c resume will only have an effect when audio is
     *     currently paused. Calling @c resume in other states will have no effect, and result in a return of @c FAILURE.
     *
     *     @return @c SUCCESS if the state transition to play was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackResumed or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c FAILURE.
     *
     * \else
     *     @brief 恢复播放。音频资源需要在调用resume之前设置，如果没有设置播放资源就进行resume，将会返回错误。
     *     只有当音频正在暂停时调用resume才会起作用，在其它所有状态下调用都会返回FAILURE。
     *
     *     @return @c SUCCESS表示播放执行成功；FAILURE表示播放执行失败；PENDING表示正在执行，这时播放状态需要通过onPlaybackResumed
     *     或onPlaybackError来通知。
     * \endif
     */
    virtual MediaPlayerStatus resume() = 0;

    /**
     * \if english
     *     @brief Set the position from which to play. Calling @c seekTo will only have an effect after the source is set.
     *
     *     @param[in] offsetInMilliseconds offset in unit of milliseconds.
     *
     *     @return @c SUCCESS if the state transition to play was successful. If state transition is pending then it returns
     *     @c PENDING and the state transition status is notified via @c onPlaybackStarted or @c onPlaybackError. If state
     *     transition was unsuccessful, returns @c FAILURE.
     * \else
     *     @brief 设置开始播放的位置。只有当播放资源设置成功后，调用seekTo才有意义。
     *
     *     @param[in] offsetInMilliseconds 以毫秒为单位的时间偏移量。
     *
     *     @return @c SUCCESS表示播放执行成功；FAILURE表示播放执行失败；PENDING表示正在执行，这时播放状态需要通过onPlaybackStarted
     *     或onPlaybackError来通知。
     * \endif
     */
    virtual MediaPlayerStatus seekTo(std::chrono::milliseconds offsetInMilliseconds) {
        return MediaPlayerStatus::FAILURE;
    };

    /**
     * \if english
     *     @brief Get whether the resource is seekable or not.
     *     @return true/false
     * \else
     *     当前播放曲目是否支持seek操作。
     *     @return true/false
     * \endif
     */
    virtual bool seekAble() {
        return false;
    };

    /**
     * \if english
     *     @brief Returns the offset, in milliseconds, of the media stream.
     *
     *     @return If a stream is playing, the offset in milliseconds that the stream has been playing,
     *     if there is no stream playing it returns @c MEDIA_PLAYER_INVALID_OFFSET.
     * \else
     *     @brief 返回当前播放时间偏移量，以毫秒为单位。
     *
     *     @return 当播放器正在播放时，返回当前的播放时间偏移量；如果播放器没有处于播放状态，返回MEDIA_PLAYER_INVALID_OFFSET。
     * \endif
     */
    virtual std::chrono::milliseconds getOffset() = 0;

    /**
     * \if english
     *     @brief Sets an observer to be notified when playback state changes.
     *
     *     @param[in] playerObserver The observer(MediaPlayerObserverInterface) to send the notifications to.
     * \else
     *     @brief 设置播放器观察者，当播放状态变化时观察者会观察到相关状态变化。
     *
     *     @param[in] playerObserver 监听播放状态变化的观察者对象.
     * \endif
     */
    virtual void setObserver(
        std::shared_ptr<MediaPlayerObserverInterface> playerObserver) = 0;

    /**
     * \if english
     *     @brief Sets an observer to be notified when playback state changes.
     *
     *     @param[in] playerObserver The observer(LocalSourcePlayerInterface) to send the notifications to.
     * \else
     *     @brief 设置播放器观察者，当播放状态变化时观察者会观察到相关状态变化。
     *
     *     @param[in] playerObserver 监听播放状态变化的观察者对象.
     * \endif
     */
    virtual void setLocalPlayObserver(
        std::shared_ptr<LocalSourcePlayerInterface> playerObserver) {}
};

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_DCS_SDK_INCLUDE_MEDIA_PLAYER_INTERFACE_H
