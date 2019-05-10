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

#include <string>

#ifndef DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_MEDIA_PLAYER_OBSERVER_INTERFACE_H
#define DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_MEDIA_PLAYER_OBSERVER_INTERFACE_H

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief LocalMediaPlayerPlayActivity enum class
 * \else
 *     @brief 本地播放器播放活动状态
 * \endif
 */
enum LocalMediaPlayerPlayActivity {
    /**
     * \if english
     *     @brief initial state, not start play
     * \else
     *     @brief 初始状态，尚未开始播放
     * \endif
     */
    IDLE = 0,
    /**
     * \if english
     *     @brief paused
     * \else
     *     @brief 播放暂停状态
     * \endif
     */
    PAUSED,
    /**
    * \if english
    *     @brief playing
    * \else
    *     @brief 正在播放状态
    * \endif
    */
    PLAYING,
    /**
    * \if english
    *     @brief stopped
    * \else
    *     @brief 播放停止状态
    * \endif
    */
    STOPPED
};

/**
 * \if english
 *     @brief LocalMediaPlayerPlayStatus class.
 * \else
 *     @brief 本地播放器播放播放状态(主要是指前台播放，后台播放，播放器关闭)
 * \endif
 */
enum LocalMediaPlayerPlayStatus {
    /**
     * \if english
     *     @brief foreground
     * \else
     *     @brief 前台播放状态
     * \endif
     */
    FOREGROUND = 0,
    /**
     * \if english
     *     @brief background
     * \else
     *     @brief 后台播放状态
     * \endif
     */
    BACKGROUND,
    /**
     * \if english
     *     @brief closed
     * \else
     *     @brief 关闭状态
     * \endif
     */
    CLOSED

};

/**
 * \if english
 *     @brief LocalMediaPlayerName local mediaPlayer source name
 * \else
 *     @brief 本地播放来源名称
 * \endif
 */
enum LocalMediaPlayerName {
    /**
     * \if english
     *     @brief bluetooth music
     * \else
     *     @brief 蓝牙播放
     * \endif
     */
    BLUETOOTH = 0,
    /**
     * \if english
     *     @brief QQ music
     * \else
     *     @brief QQ音乐播放
     * \endif
     */
    QQ,
    /**
     * \if english
     *     @brief baidu music
     * \else
     *     @brief 百度音乐播放
     * \endif
     */
    BAIDU,
    /**
     * \if english
     *     @brief xiami music
     * \else
     *     @brief 虾米音乐播放
     * \endif
     */
    XIAMI,
    /**
     * \if english
     *     @brief netease cloud music
     * \else
     *     @brief 网易云音乐播放
     * \endif
     */
    NETEASE,
    /**
     * \if english
     *     @brief kuwo music
     * \else
     *     @brief 酷我音乐播放
     * \endif
     */
    KUWO,
    /**
     * \if english
     *     @brief kugou music
     * \else
     *     @brief 酷狗音乐播放
     * \endif
     */
    KUGOU,
    /**
     * \if english
     *  @brief dlna music
     *  \else
     *   @brief dlna 音乐播放
     */
    DLNA
};

/**
 * \if english
 *     @brief LocalMediaPlayerPlayInfo local mediaPlayer play info
 * \else
 *     @brief 本地播放器播放信息
 * \endif
 */
class LocalMediaPlayerPlayInfo {
public:
    /**
     * \if english
     *     @brief LocalMediaPlayerPlayActivity play activity
     * \else
     *     @brief 本地播放器播放活动状态
     * \endif
     */
    LocalMediaPlayerPlayActivity m_playerActivity;

    /**
     * \if english
     * LocalMediaPlayerPlayStatus play status
     *     @brief \else
     * 本地播放器播放状态
     *     @brief \endif
     */
    LocalMediaPlayerPlayStatus m_status;

    /**
     * \if english
     *     @brief LocalMediaPlayerName play source name
     * \else
     *     @brief 本地播放器播放来源名称
     * \endif
     */
    LocalMediaPlayerName m_playerName;

    /**
     * \if english
     *     @brief audioId, default is null
     * \else
     *     @brief 播放音频的Id, 默认为空
     * \endif
     */
    std::string m_audioId;

    /**
     * \if english
     *     @brief title, default is null
     * \else
     *     @brief 播放音频的标题, 默认为空
     * \endif
     */
    std::string m_title;

    /**
     * \if english
     *     @brief artist
     * \else
     *     @brief 歌曲作者名称
     * \endif
     */
    std::string m_artist;

    /**
     * \if english
     *     @brief album
     * \else
     *     @brief 歌曲专辑名称
     * \endif
     */
    std::string m_album;

    /**
     * \if english
     *     @brief year
     * \else
     *     @brief 歌曲年份
     * \endif
     */
    std::string m_year;

    /**
     * \if english
     *     @brief genre
     * \else
     *     @brief 歌曲类型/种类
     * \endif
     */
    std::string m_genre;
};

/**
 * \if english
 *     @brief A local media player observer will receive notifications when the player starts playing or when
 *     it stops playing a stream.
 *     A pointer to the @c LocalMediaPlayerObserverInterface needs to be provided to a @c MediaPlayer for it to notify the observer.
 * \else
 *     @brief LocalMediaPlayerObserverInterface类, 观察者接口回调类
 * \endif
 */
class LocalMediaPlayerObserverInterface {
public:
    /**
     * \if english
     *     @brief Destructor.
     * \else
     *     @brief 析构函数
     * \endif
     */
    virtual ~LocalMediaPlayerObserverInterface() = default;

    /**
     * \if english
     *     @brief setLocalMediaPlayerPlayInfo set local mediaPlayer play info
     *     @param[in] playInfo
     * \else
     *     @brief 设置本地播放器播放信息接口
     *     @param[in] playInfo 播放信息参数
     * \endif
     */
    virtual void setLocalMediaPlayerPlayInfo(const LocalMediaPlayerPlayInfo& playInfo) = 0;
};

} // namespace sdkInterfaces
} // namespace duerOSDcsSDK

#endif // DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_MEDIA_PLAYER_OBSERVER_INTERFACE_H
