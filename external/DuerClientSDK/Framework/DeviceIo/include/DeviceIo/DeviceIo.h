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

#ifndef DUEROS_FRAMEWORK_DEVICEIO_H_
#define DUEROS_FRAMEWORK_DEVICEIO_H_

#include <pthread.h>
#include <string>
#include "DeviceIo/InfoLed.h"
namespace duerOSDcsApp {
namespace framework {

#define PLAYBACK_DEVICE_NUM 1

/* led control cmd */
enum class LedState {
    LED_NET_RECOVERY = 0,
    LED_NET_WAIT_CONNECT,
    LED_NET_DO_CONNECT,
    LED_NET_CONNECT_FAILED,
    LED_NET_CONNECT_SUCCESS,
    LED_NET_WAIT_LOGIN,
    LED_NET_DO_LOGIN,
    LED_NET_LOGIN_FAILED,
    LED_NET_LOGIN_SUCCESS,

    LED_WAKE_UP_DOA,    // param -180 ~ 180
    LED_WAKE_UP,        // no param
    LED_SPEECH_PARSE,
    LED_PLAY_TTS,
    LED_PLAY_RESOURCE,

    LED_BT_WAIT_PAIR,
    LED_BT_DO_PAIR,
    LED_BT_PAIR_FAILED,
    LED_BT_PAIR_SUCCESS,
    LED_BT_PLAY,
    LED_BT_CLOSE,

    LED_VOLUME,
    LED_MUTE,

    LED_DISABLE_MIC,

    LED_ALARM,

    LED_SLEEP_MODE,

    LED_OTA_DOING,
    LED_OTA_SUCCESS,

    LED_CLOSE_A_LAYER,
    LED_ALL_OFF,    // no param
};

/* bt control cmd */
enum class BtControl {
    BT_OPEN,
    BT_CLOSE,
    BT_IS_OPENED,
    BT_IS_CONNECTED,
    BT_UNPAIR,
    BT_PAUSE_PLAY,
    BT_RESUME_PLAY,
    BT_VOLUME_UP,
    BT_VOLUME_DOWN,
    BT_AVRCP_FWD,
    BT_AVRCP_BWD,
    BT_AVRCP_STOP,
    BT_HFP_RECORD,
    BLE_OPEN_SERVER,
    BLE_CLOSE_SERVER,
    BLE_SERVER_SEND,
    BLE_IS_OPENED,
    GET_BT_MAC,
    GET_WIFI_MAC,
    GET_WIFI_IP,
    GET_WIFI_SSID,
    GET_WIFI_BSSID,
    GET_LOCAL_NAME,
};

/* input event */
enum class DeviceInput {
    KEY_ONE_SHORT,
    KEY_ONE_LONG,
    KEY_ONE_DOUBLE,
    KEY_ONE_LONG_10S,
    KEY_BLUETOOTH_SHORT,
    KEY_BLUETOOTH_LONG = 5,
    KEY_MIC_MUTE,
    SLIDER_PRESSED,
    SLIDER_RELEASE,
    VOLUME_CHANGED,

    // for mtk devkit
    KEY_VOLUME_UP = 10,
    KEY_VOLUME_DOWN,
    KEY_VOLUME_MUTE,
    KEY_WAKE_UP,
    KEY_ENTER_AP,
    KEY_EXIT_AP = 15,

    //蓝牙等待配对
    BT_WAIT_PAIR,
    //蓝牙配对中
    BT_DO_PAIR,
    //蓝牙已被绑定,请解绑后再绑定其他设备
    BT_PAIR_FAILED_PAIRED,
    //配对失败,请重试
    BT_PAIR_FAILED_OTHER,
    //蓝牙配对成功
    BT_PAIR_SUCCESS = 20,
    //手机关闭蓝牙蓝牙断开
    BT_DISCONNECT,
    //蓝牙开始播放
    BT_START_PLAY,
    //蓝牙结束播放
    BT_STOP_PLAY,
    //ble
    BLE_CLIENT_CONNECT,
    BLE_CLIENT_DISCONNECT = 25,
    BLE_SERVER_RECV,
    //BT CALL
    BT_HFP_AUDIO_CONNECT,
    BT_HFP_AUDIO_DISCONNECT,
    BT_POWER_OFF,

    DLNA_STOPPED = 30,
    DLNA_PLAYING,
    DLNA_PAUSED,

    KEY_CLOSE_TIMER_ALARM,
    KEY_IS_SLEEP_STATE,
    KEY_PLAY_PAUSE = 35,

    KEY_SHUT_DOWN,
    KEY_SLEEP_MODE
};

/* device input event notify */
class DeviceInNotify {
public:
    virtual void callback(DeviceInput event, void *data, int len) = 0;

    virtual ~DeviceInNotify() = default;
};

class DeviceIo {
public:
    /**
     * @brief get instance
     *
     * @return the instance pointer
     */
    static DeviceIo* getInstance();

    /**
     * @brief release instance
     *
     */
    void releaseInstance();

    /**
     * @brief set the notify
     *
     * @param notify called when some event happened
     *
     */
    void setNotify(DeviceInNotify* notify);

    /**
     * @brief set the notify
     *
     * @param notify called when some event happened
     *
     */
    DeviceInNotify* getNotify();

    /**
     * @brief control the led
     *
     * @param cmd see enum LedState
     * @param data data
     * @param len data length
     *
     * @return 0 on success
     */
    int controlLed(LedState cmd, void *data = NULL, int len = 0);

    /**
     * @brief control the bt
     *
     * @param cmd  see enum BtControl
     * @param data data
     * @param len  data length
     * @return 0 on success
     */
    int controlBt(BtControl cmd, void *data = NULL, int len = 0);

    /**
    * @brief transimit infrared code
    *
    * @param infraredCode the infrared code
    * @return 0 on success
    */
    int transmitInfrared(std::string& infraredCode);

    /**
    * @brief open microphone
    *
    * @return 0 on success
    */
    int openMicrophone();

    /**
     * @brief close microphone
     *
     * @return 0 on success
     */
    int closeMicrophone();

    /**
     * @brief get microphone status
     *
     * @return true on opened
     */
    bool isMicrophoneOpened();

    /**
     * @brief get volume
     *
     * @return 0 ~ 100 volume
     */
    int getVolume(int trackId = 0);

    /**
     * @brief set volume
     *
     * @param vol volume 0 ~ 100
     */
    void setVolume(int vol, int trackId = -1);

    /**
     * @brief get/set sn
     *
     * @return void
     */

    bool getSn(char* sn);

    bool setSn(char* sn);
    bool getPCB(char* sn);
    bool setPCB(char *sn);
    /**
     * @brief get version
     *
     * @return version string
     */
    std::string getVersion();

    /**
     * @brief set mute state
     *
     * @return 0 on success
     */
    int setMute(bool mute);

    /**
     * @brief get mute status
     *
     * @return true on mute
     */
    bool isMute();

    /**
     * @brief get mic array angle
     *
     * @return the angle
     */
    int getAngle();

    bool inOtaMode();

    void rmOtaFile();

private:
    static DeviceIo* m_instance;
    static DeviceInNotify* m_notify;

    static pthread_once_t m_initOnce;
    static pthread_once_t m_destroyOnce;

    DeviceIo();
    ~DeviceIo();
    static void init();
    static void destroy();
};

} // namespace framework
} // namespace duerOSDcsApp

#endif // DUEROS_FRAMEWORK_DEVICEIO_H_
