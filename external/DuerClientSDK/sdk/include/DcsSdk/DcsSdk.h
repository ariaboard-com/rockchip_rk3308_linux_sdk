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

/**
 * @file DcsSdk.h
 * \if english
 *     @brief DCS SDK API interface.
 * \else
 *     @brief SDK API接口。
 * \endif
 */

/**
 * \if english
 *     @mainpage DCS SDK (Linux) API
 *     @section Summary
 *         This document used to create app communicate with DuerOS using Linux DCS SDK。
 * \else
 *     @mainpage DCS SDK（Linux版）接口文档
 *     @section 缩略语
 *         @li DuerOS：DuerOS 开放平台主要面向企业级用户及个人开发者，提供对话式操作系统能力的输出（DuerOS智能设备开放平台）及输入（DuerOS技能开放平台）。(https://dueros.baidu.com/didp/doc/overall/open-platform-intro_markdown)
 *         @li DCS：DCS协议是DuerOS服务端与设备端之间的通讯协议，是一套把DuerOS的智能语音交互能力向所有设备开放的API。
 *         @li DCS SDK（Linux）：封装了DCS协议与DuerOS开放平台交互的细节，实现了DCS对外开放的大部分协议，并支持第三方bot的自定义指令，可用于第三方开发者在Linux平台中基于DuerOS平台快速建立自己的语音交互应用。
 *     @section 概述
 *         本文档用于描述DCS SDK（Linux版本）对外的主要接口说明。
 *         @endparblock
 *         APP开发者可以通过DcsSdk::create函数创建一个DCS SDK实例，并实现必要的观察类接口，应用层设备调用接口来实现与DuerOS的交互，通信，以及TTS、Audio的播放。
 *         @endparblock
 *         目前DCS SDK（Linux）支持的DCS协议有：
 *         @li 语音输入
 *         @li 语音输出
 *         @li 扬声器控制
 *         @li 音频播放器
 *         @li 播放控制
 *         @li 闹钟
 *         @li 屏幕展示
 *         @li 系统
 *         @endparblock
 *         并支持通过小度之家与SDK之间进行交互，并控制音箱。
 *         @line end
 * \endif
 */

#ifndef DUEROS_DCS_SDK_INCLUDE_DCS_SDK_DCS_SDK_H_
#define DUEROS_DCS_SDK_INCLUDE_DCS_SDK_DCS_SDK_H_

#include <DcsSdk/DcsSdkParameters.h>
#include <DcsSdk/PlayerAvtivityObserver.h>
#include <DcsSdk/ApplicationImplementation.h>

namespace duerOSDcsSDK {

/**
 * 内部类引用。
 */
namespace dcsClient {
class DcsClient;
}

namespace sdkInterfaces {

/**
 * \if english
 *     @brief DCS SDK API interface main class.
 * \else
 *     @brief DCS SDK API接口入口类。
 * \endif
 */
class DcsSdk : public PlayerAvtivityObserver {
public:
    /**
     * \if english
     *     @brief Creates and initializes a default DCS SDK client.
     *     @param[in] parameters parameter to create the DCS SDK handler.
     *     @return DcsSdk handler or nullptr.
     * \else
     *     @brief 创建并初始化默认的DCS SDK实例。
     *     @param[in] parameters 用于初始化DCS SDK的参数。
     *     @return DcsSdk实例指针或nullptr。
     * \endif
     */
    static std::unique_ptr<DcsSdk> create(DcsSdkParameters& parameters);

    /**
     * \if english
     *     @brief Inform SDK that the speaker had changed mute status.
     *     @param[in] volume speaker volume, value range[0 - 100].
     *     @param[in] muted speaker mute status, True/False: mute on/mute off.
     * \else
     *     @brief 通知SDK音箱改变了静音状态。
     *     @param[in] volume 音量，取值范围[0 - 100]。
     *     @param[in] muted 静音状态，True/False：静音/非静音。
     * \endif
     */
    void muteChanged(int volume, bool muted);

    /**
     * \if english
     *     @brief Inform SDK that the speaker had changed volume.
     *     @param[in] volume speaker volume, value range[0 - 100].
     *     @param[in] muted speaker mute status, True/False: mute on/mute off.
     * \else
     *     @brief 通知SDK音箱改变了音量。
     *     @param[in] volume 音量，取值范围[0 - 100]。
     *     @param[in] muted 静音状态，True/False：静音/非静音。
     * \endif
     */
    void volumeChanged(int volume, bool muted);

#ifdef SUPPORT_DEBUGGER
    /**
     * \if english
     *     @brief Inform the server the debugger mode has started.
     * \else
     *     @brief 通知SDK音箱启动debug模式。
     * \endif
     */
    void debugStarted();

    /**
     * \if english
     *     @brief Inform the server the debugger mode has stoped.
     * \else
     *     @brief 通知SDK音箱退出debug模式。
     * \endif
     */
    void debugStoped();
#endif

    /**
     * \if english
     *     @brief Initiate a playback play event to SDK.
     * \else
     *     @brief 发起playback play事件。
     * \endif
     */
    void issuePlaybackPlayCommand();

    /**
     * \if english
     *     @brief Initiate a playback pause event to SDK.
     * \else
     *     @brief 发起playback pause事件。
     * \endif
     */
    void issuePlaybackPauseCommand();

    /**
     * \if english
     *     @brief Initiate a playback next event to SDK.
     * \else
     *     @brief 发起playback 事件next。
     * \endif
     */
    void issuePlaybackNextCommand();

    /**
     * \if english
     *     @brief Initiate a playback previous event to SDK.
     * \else
     *     @brief 发起playback previous事件。
     * \endif
     */
    void issuePlaybackPreviousCommand();

    /**
     * \if english
     *     @brief Request SDK to close activated alerts.
     * \else
     *     @brief 关闭当前激活状态的闹钟。
     * \endif
     */
    void closeLocalActiveAlert();

    /**
     * \if english
     *     @brief Request SDK to cancel music play.
     * \else
     *     @brief 取消音乐播放。
     * \endif
     */
    void cancelMusicPlay();

    void cancelBluetoothPlay();

    /**
     * \if english
     *     @brief Begins a wake word initiated DCS interaction.
     *     @param[in] beginIndex The begin index of the keyword found within the stream.
     *     @param[in] endIndex The end index of the keyword found within the stream.
     *     @param[in] keyword The keyword that was detected.
     * \else
     *     @brief 通知SDK设备已经被唤醒。
     *     @param[in] beginIndex 唤醒词在音频流中的起始位置.
     *     @param[in] endIndex 唤醒词在音频流中的结束位置.
     *     @param[in] keyword 检测到的唤醒词.
     * \endif
     */
    std::future<bool> notifyOfWakeWord(
            uint64_t beginIndex,
            uint64_t endIndex,
            const std::string& keyword);

    static const auto INVALID_INDEX = std::numeric_limits<uint64_t>::max();
    /**
     * \if english
     *     Begins a tap to talk initiated DCS interaction. Note that this can also be used for wake word engines that
     *     don't support providing both a begin and end index.
     *     @brief Begins a tap to talk initiated DCS interaction.
     *     @param[in] beginIndex An optional parameter indicating where in the stream to start reading from.
     * \else
     *     @brief 通知用户启动了唤醒状态。
     *     @param[in] beginIndex 唤醒词在音频流中的起始位置.
     * \endif
     */
    void notifyOfTapToTalk(uint64_t beginIndex = INVALID_INDEX);

    /**
     * \if english
     *     @brief Destructor function.
     * \else
     *     @brief 析构函数。
     * \endif
     */
    virtual ~DcsSdk() = default;

    /**
     * \if english
     *     @brief Accept the DCS message from device.
     *     @param[in] message DCS message（JSON string）.
     *     @param[in] contextId context id.
     * \else
     *     @brief 接受非云端传入的DCS消息。
     *     @param[in] message DCS消息内容（JSON字符串）。
     *     @param[in] contextId 消息来源标识。
     * \endif
     */
    bool consumeMessage(const std::string& message, const std::string& contextId = "");

    /**
     * \if english
     *     @brief Get client id of current device.
     *     @return client id.
     * \else
     *     @brief 获取当前设备的client id。
     *     @return client id.
     * \endif
     */
    std::string getClientId();

    /**
     * \if english
     *     @brief Get device id of current device.
     *     @return device id.
     * \else
     *     @brief 获取当前设备的device id。
     *     @return device id.
     * \endif
     */
    std::string getDeviceId();

    /**
     * \if english
     *     @brief Inform SDK the connection status.
     *     @param[in] status True/False: online/offline.
     * \else
     *     @brief 通知SDK当前设备的联网状态。
     *     @param[in] status True/False：已联网/未联网。
     * \endif
     */
    void informOnlineStatus(bool status);

    /**
     * \if english
     *     @brief Get SDK status.
     *     @return
     *     enum SdkConnectionState {
     *          SDK_AUTH_FAILED,
     *          SDK_AUTH_SUCCEED,
     *          SDK_CONNECT_FAILED,
     *          SDK_CONNECTING,
     *          SDK_CONNECT_SUCCEED
     *     };
     * \else
     *     @brief 获取当前SDK的状态.
     *     @return
     *     enum SdkConnectionState {
     *          帐号认证失败,
     *          帐号认证成功,
     *          服务器连接失败,
     *          正在连接服务器,
     *          服务器连接成功
     *     };
     * \endif
     */
    SdkConnectionState getSdkConnectionStates();

    /**
     * \if english
     *     @brief If OAuthorize by pass pair.
     * \else
     *     @brief 是否通过配对的方式授权过.
     * \endif
     */
    bool isOAuthByPassPair();

    /**
     * \if english
     *     @brief Inform SDK network status.
     *     @param[in] status True/False: online/offline.
     * \else
     *     @brief 通知SDK网络就绪状态。
     *     @param[in] status True/False：已联网/未联网。
     * \endif
     */
    void notifyNetworkReady(bool status, const std::string& wifiBssid ="");

    void enterSleepMode();

    void enterWakeupMode();

    static bool recordingStatus();

#ifdef KITTAI_KEY_WORD_DETECTOR
    /**
     * \if english
     *     @brief Inform SDK the device change state to play music scene.
     * \else
     *     @brief Inform SDK 进入音乐唤醒场景。
     * \endif
     */
    void enterPlayMusicScene();

    /**
     * \if english
     *     @brief Inform SDK the device had exit the play music scene.
     * \else
     *     @brief Inform SDK 退出音乐唤醒场景。
     * \endif
     */
    void exitPlayMusicScene();
#endif

    /**
     * \if english
     *     @brief Try to acquire audio focus.
     *     @param[in] holdFlag true to acquire focus, false to release focus.
     * \else
     *     @brief 尝试去获取音频焦点。
     *     @param[in] holdFlag 取值为true表示获取焦点，取值为false表示释放焦点。
     * \endif
     */
    void forceHoldFocus(bool holdFlag);

    /**
     * \if english
     *     @brief Write audio stream data to SDK.
     *     @param[in] buffer audio data.
     *     @param[in] numSamples audio data length.
     *     @return writen size.
     * \else
     *     @brief 写入音频数据。
     *     @param[in] buffer 音频数据。
     *     @param[in] numSamples 音频数据长度。
     *     @return 实际写入的数据长度。
     * \endif
     */
    ssize_t writeAudioData(const void* buffer, unsigned long numSamples);

    /**
     * \if english
     *     @brief Notify SDK the system time is ready.
     * \else
     *     @brief 通知SDK系统时间正常，SDK里面依赖于时间的模块（比如闹钟）才可以继续执行。
     * \endif
     */
    void notifySystemTimeReady();

    /**
     * \if english
     *     @brief Set bduss to Dcs SDK.
     *     @param[in] bduss from app.
     *     @return True/False.
     * \else
     *     @brief 通知SDK收到DLP传来的BDUSS信息。
     *     @param[in] bduss 上层下发的bduss信息。
     *     @return True/False：操作成功/失败。
     * \endif
     */
    bool setBDUSS(const std::string& bduss);

    void playStart(int volume) override;

    void playEnd(int volume) override;
private:
    /**
     * \if english
     *     @brief Constructor function.
     * \else
     *     @brief 构造函数。
     * \endif
     */
    DcsSdk();

    /**
     * \if english
     *     @brief Initialize DCS SDK.
     *     @param[in] parameters parameter for DCS SDK.
     *     @return True/False: initialize succeed/failed.
     * \else
     *     @brief 初始化DCS SDK。
     *     @param[in] parameters 用于初始化DCS SDK的参数。
     *     @return True/False：初始化成功/失败。
     * \endif
     */
    bool initialize(DcsSdkParameters& parameters);

    /**
     * \if english
     *     @brief Inner class handler.
     * \else
     *     @brief 内部类句柄。
     * \endif
     */
    std::shared_ptr<dcsClient::DcsClient> m_dcsClient;
};

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_INCLUDE_DCS_SDK_DCS_SDK_H_
