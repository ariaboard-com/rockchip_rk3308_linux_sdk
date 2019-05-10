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

#ifndef DUEROS_DCS_APP_SAMPLEAPP_SOUNDCONTROLLER_H_
#define DUEROS_DCS_APP_SAMPLEAPP_SOUNDCONTROLLER_H_

#include <DcsSdk/LocalSourcePlayerInterface.h>
#include "PcmFilePlayerImpl.h"
#include <pthread.h>
#include <string>
#include <unordered_set>

namespace duerOSDcsApp {
namespace application {

/**
 * \if english
 *     @brief DCSApp sound control.
 * \else
 *     @brief DCSApp 音效控制类。
 * \endif
 */
class SoundController {
public:
    /**
     * \if english
     *     @brief sound control singleton.
     *     @return sound control singleton.
     * \else
     *     @brief 音效控制类单例模式。
     *     @return 音效控制类单例
     * \endif
     */
    static SoundController *getInstance();

    /**
     * \if english
     *     @brief add local source player interface.
     *     @param[in] mediaInterface local source player interface.
     * \else
     *     @brief 添加本地音频播放控制接口。
     *     @param[in] mediaInterface 音频播放控制接口。
     * \endif
     */
    void addObserver(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::LocalSourcePlayerInterface> mediaInterface);

    /**
     * \if english
     *     @brief release singleton.
     * \else
     *     @brief 释放单例。
     * \endif
     */
    void release();

    /**
     * \if english
     *     @brief play the sound of wakeup.
     * \else
     *     @brief 播放唤醒提示音。
     * \endif
     */
    void wakeUp();

    /**
     * \if english
     *     @brief play the sound of start config network first.
     * \else
     *     @brief 播放首次等待配网提示音。
     * \endif
     */
    void linkStartFirst();

    /**
     * \if english
     *     @brief play the sound of start config network.
     * \else
     *     @brief 播放等待配网提示音。
     * \endif
     */
    void linkStart();

    /**
     * \if english
     *     @brief play the sound of config network connecting.
     * \else
     *     @brief 播放配网中提示音。
     * \endif
     */
    void linkConnecting();

    /**
     * \if english
     *     @brief play the sound of config network success.
     *     @param[in] callback the callback of play sound finished.
     * \else
     *     @brief 播放配网成功提示音。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void linkSuccess(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of config network failed and ping failed.
     *     @param[in] callback the callback of play sound finished.
     * \else
     *     @brief 播放配网失败并且无法连接外网提示音。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void linkFailedPing(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of config network failed and get ip failed.
     *     @param[in] callback the callback of play sound finished.
     * \else
     *     @brief 播放配网失败并且获取不到ip地址提示音。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void linkFailedIp(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of exit config network.
     *     @param[in] callback the callback of play sound finished.
     * \else
     *     @brief 播放退出配网提示音。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void linkExit(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of network reconnecting.
     * \else
     *     @brief 播放恢复联网中提示音。
     * \endif
     */
    void reLink();

    /**
     * \if english
     *     @brief play the sound of network reconnect success.
     *     @param[in] callback the callback of play sound finished.
     * \else
     *     @brief 播放恢复联网成功提示音。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void reLinkSuccess(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of network reconnect failed.
     * \else
     *     @brief 播放恢复联网失败提示音。
     * \endif
     */
    void reLinkFailed();

    /**
     * \if english
     *     @brief play the sound of unpaired bluetooth.
     * \else
     *     @brief 播放蓝牙解绑提示音。
     * \endif
     */
    void btUnpaired();

    /**
     * \if english
     *     @brief play the sound of bluetooth pair success.
     *     @param[in] callback the callback of play sound finished.
     * \else
     *     @brief 播放蓝牙配对成功提示音。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void btPairSuccess(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of bluetooth pair failed and it paired other.
     * \else
     *     @brief 播放蓝牙配对失败且已被绑定提示音。
     * \endif
     */
    void btPairFailedPaired();

    /**
     * \if english
     *     @brief play the sound of bluetooth pair failed.
     * \else
     *     @brief 播放蓝牙配对失败提示音。
     * \endif
     */
    void btPairFailedOther();

    /**
     * \if english
     *     @brief play the sound of bluetooth disconnect.
     *     @param[in] callback the callback of play sound finished.
     * \else
     *     @brief 播放蓝牙断开提示音。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void btDisconnect(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of network connect failed.
     * \else
     *     @brief 播放网络连接失败提示音。
     * \endif
     */
    void networkConnectFailed();

    /**
     * \if english
     *     @brief play the sound of network slow.
     * \else
     *     @brief 播放弱网提示音。
     * \endif
     */
    void networkSlow();

    /**
     * \if english
     *     @brief play the sound of open bluetooth.
     *     @param[in] callback the callback of play sound finished.
     *     @param[in] arg the parameter of callback.
     * \else
     *     @brief 播放打开蓝牙提示音。
     *     @param[in] callback 播放完成回调函数。
     *     @param[in] arg 回调函数参数。
     * \endif
     */
    void openBluetooth(void(*callback)(void *arg), void *arg);

    /**
     * \if english
     *     @brief play the sound of close bluetooth.
     *     @param[in] callback the callback of play sound finished.
     *     @param[in] arg the parameter of callback.
     * \else
     *     @brief 播放关闭蓝牙提示音。
     *     @param[in] callback 播放完成回调函数。
     *     @param[in] arg 回调函数参数。
     * \endif
     */
    void closeBluetooth(void(*callback)(void *arg), void *arg);

    /**
     * \if english
     *     @brief play the sound of change volume.
     * \else
     *     @brief 播放调节音量提示音。
     * \endif
     */
    void volume();

    /**
     * \if english
     *     @brief play the tts.
     *     @param[in] content the text of tts.
     *     @param[in] needFocus is need focus.
     *     @param[in] callback the callback of play tts finished.
     * \else
     *     @brief 语音合成播放。
     *     @param[in] content 语音合成文本。
     *     @param[in] needFocus 是否获取焦点，打断其他播放。
     *     @param[in] callback 播放完成回调函数。
     * \endif
     */
    void playTts(const std::string& content, bool needFocus, void (*callback)());

    /**
     * \if english
     *     @brief play the sound of connecting server.
     * \else
     *     @brief 播放正在连接服务器提示音。
     * \endif
     */
    void serverConnecting();

    /**
     * \if english
     *     @brief play the sound of connect server failed.
     * \else
     *     @brief 播放服务器连接失败提示音。
     * \endif
     */
    void serverConnectFailed();

    /**
     * \if english
     *     @brief play the sound of config network from ble.
     * \else
     *     @brief 播放处于“蓝牙配网”过程中提示音。
     * \endif
     */
    void bleNetworkConfig();

    /**
     * \if english
     *     @brief play the sound of account unbound.
     * \else
     *     @brief 播放帐号解绑提示音。
     * \endif
     */
    void accountUnbound(void(*callback)());

    /**
     * \if english
     *     @brief play the sound of connected network hot.
     * \else
     *     @brief 播放配网过程中连接上热点提示音。
     * \endif
     */
    void hotConnected();

    void waitLogin();

private:
    /**
     * \if english
     *     @brief Constructor.
     * \else
     *     @brief 构造函数。
     * \endif
     */
    SoundController();

    /**
     * \if english
     *     @brief Destructor.
     * \else
     *     @brief 析构函数。
     * \endif
     */
    ~SoundController();

    /**
     * \if english
     *     @brief the sound player of change volume.
     * \else
     *     @brief 调节音量的音效播放器。
     * \endif
     */
    mediaPlayer::PcmFilePlayerImpl *m_pcmPlayer;

    /**
     * \if english
     *     @brief play the file of audio.
     *     @param[in] source the file path.
     *     @param[in] needFocus is need focus.
     *     @param[in] start_callback the callback of start play.
     *     @param[in] start_cb_arg the parameter of start play callback.
     *     @param[in] finish_callback the callback of play finished.
     * \else
     *     @brief 播放音频文件。
     *     @param[in] source 音频文件路径。
     *     @param[in] needFocus 是否获取焦点，打断其他播放。
     *     @param[in] start_callback 开始播放回调函数。
     *     @param[in] start_cb_arg 开始播放回调函数参数。
     *     @param[in] finish_callback 播放结束回调函数。
     * \endif
     */
    void audioPlay(const std::string &source,
                   bool needFocus,
                   void (*start_callback)(void *arg),
                   void *start_cb_arg,
                   void (*finish_callback)());

    /**
     * \if english
     *     @brief init function.
     * \else
     *     @brief 初始化函数。
     * \endif
     */
    static void init();

    /**
     * \if english
     *     @brief destory function.
     * \else
     *     @brief 销毁函数。
     * \endif
     */
    static void destory();

    /**
     * \if english
     *     @brief safe thread init parameter.
     * \else
     *     @brief 线程安全init变量。
     * \endif
     */
    static pthread_once_t m_initOnce;

    /**
     * \if english
     *     @brief safe thread destroy parameter
     * \else
     *     @brief 线程安全destroy变量。
     * \endif
     */
    static pthread_once_t m_destroyOnce;

    /**
     * \if english
     *     @brief static sound parameter.
     * \else
     *     @brief 音效类静态成员变量。
     * \endif
     */
    static SoundController *mSoundController;

    /**
     * \if english
     *     @brief mutex parameter.
     * \else
     *     @brief 互斥锁成员变量。
     * \endif
     */
    pthread_mutex_t m_mutex;

    /**
     * \if english
     *     @brief a collection of local source player interface.
     * \else
     *     @brief 音频播放控制接口集合。
     * \endif
     */
    std::unordered_set<std::shared_ptr<duerOSDcsSDK::sdkInterfaces::LocalSourcePlayerInterface>> m_observers;
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_SAMPLEAPP_SOUNDCONTROLLER_H_
