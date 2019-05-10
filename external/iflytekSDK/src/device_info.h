#ifndef DEVICE_INFO_H_
#define DEVICE_INFO_H_

#include <string>
#include "info_led.h"

#define USER_VOL_MIN                    5   // keep some voice
#define USER_VOL_MAX                    100

#define AUDIO_MIN_VOLUME                0
#define AUDIO_MAX_VOLUME                1000

#define SOFTVOL /*should play a music before ctl the softvol*/
#define SOFTVOL_CARD "default"
#define SOFTVOL_ELEM "name='Master Playback Volume'"

typedef struct
{
    int     volume;
    bool    is_mute;
} user_volume_t;

/* input event */
enum class DeviceInput
{
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


class DeviceIo
{
  public:

    /* @brief get instance
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
    int getVolume();

    /**
     * @brief set volume
     *
     * @param vol volume 0 ~ 100
     */
    void setVolume(int vol);

    /**
     * @brief get/set sn
     *
     * @return void
     */

    bool getSn(char* sn);

    bool setSn(char* sn);

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
  private:
    static DeviceIo* m_instance;
    static pthread_once_t m_initOnce;
    static pthread_once_t m_destroyOnce;
    DeviceIo();
    ~DeviceIo();
    static void init();
    static void destroy();
};

#endif
