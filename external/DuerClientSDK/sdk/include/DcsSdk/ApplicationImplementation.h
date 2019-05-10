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
 * @file ApplicationImplementation.h
 * \if english
 *     @brief Pre defined interface, should implemented by APP.
 * \else
 *     @brief SDK定义一系列接口，需要由APP层实现，并在APP生存期间由SDK调用。
 * \endif
 */

#ifndef DUEROS_DCS_SDK_DCS_SDK_INCLUDE_APPLICATION_IMPLEMENTATION_H
#define DUEROS_DCS_SDK_DCS_SDK_INCLUDE_APPLICATION_IMPLEMENTATION_H

#include <string>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief Device information.
 * \else
 *     @brief 设备系统信息。
 * \endif
 */
struct SystemInformation {
    /**
     * \if english
     *     @brief SSID of device.
     * \else
     *     @brief 当前wifi连接的热点名称。
     * \endif
     */
    std::string ssid;

    /**
     * \if english
     *     @brief MAC address of wireless interface.
     * \else
     *     @brief 设备无线网卡的的mac地址。
     * \endif
     */
    std::string mac;

    /**
     * \if english
     *     @brief IP address of wireless interface.
     * \else
     *     @brief 当前无线网卡的ip地址。
     * \endif
     */
    std::string ip;

    /**
     * \if english
     *     @brief Client id of device, this parameter will obtained by SDK.
     * \else
     *     @brief 设备的client id，此参数不需要APP层传入，由SDK自行获取。
     * \endif
     */
    std::string clientId;

    /**
     * \if english
     *     @brief Device id.
     * \else
     *     @brief 设备的device id。
     * \endif
     */
    std::string deviceId;

    /**
     * \if english
     *     @brief Serial number of device.
     * \else
     *     @brief 设备的serial number。
     * \endif
     */
    std::string sn;

    /**
     * \if english
     *     @brief Mac address of blue tooth.
     * \else
     *     @brief 蓝牙的mac地址。
     * \endif
     */
    std::string bluetoothMac;

    /**
     * \if english
     *     @brief Serial number.
     * \else
     *     @brief 设备的serial number。
     * \endif
     */
    std::string deviceVersion;

    /**
     * \if english
     *     @brief Software version of device.
     * \else
     *     @brief 设备的软件版本。
     * \endif
     */
    std::string softwareVersion;

    /**
     * \if english
     *     @brief DuerOS protocol version, this parameter will obtained by SDK.
     * \else
     *     @brief 设备当前支持的DuerOS协议版本，此参数不需要APP层传入，由SDK自行获取。
     * \endif
     */
    std::string protocolVersion;

    /**
     * \if english
     *     @brief Online status. True/False: online/offline.
     * \else
     *     @brief 设备当前的网络连接状态。True/False：网络已连接/网络未连接。
     * \endif
     */
    bool onlineStatus;
};

/**
 * \if english
 *     @brief System update status.
 * \else
 *     @brief 系统更新状态。
 * \endif
 */
enum SystemUpdateState {
    /**
     * \if english
     *     @brief Latest system version.
     * \else
     *     @brief 系统已经是最新的版本。
     * \endif
     */
    NOT_UPDATABLE,

    /**
     * \if english
     *     @brief New system version available.
     * \else
     *     @brief 可以进行系统软件更新。
     * \endif
     */
    UPDATABLE,

    /**
     * \if english
     *     @brief System updating in process.
     * \else
     *     @brief 系统更新中。
     * \endif
     */
    UPDATING
};

/**
 * \if english
 *     @brief System update information.
 * \else
 *     @brief 系统更新信息。
 * \endif
 */
struct SystemUpdateInformation {
    /**
     * \if english
     *     @brief Current version of system.
     * \else
     *     @brief 系统当前版本。
     * \endif
     */
    std::string currentVersion;

    /**
     * \if english
     *     @brief Current version description.
     * \else
     *     @brief 系统当前版本描述信息。
     * \endif
     */
    std::string currentVersionDesc;

    /**
     * \if english
     *     @brief Target version.
     * \else
     *     @brief 系统目标版本。
     * \endif
     */
    std::string targetVersion;

    /**
     * \if english
     *     @brief Target version description.
     * \else
     *     @brief 系统目标版本描述信息。
     * \endif
     */
    std::string targetVersionDesc;

    /**
     * \if english
     *     @brief System updating status.
     * \else
     *     @brief 系统更新状态。
     * \endif
     */
    SystemUpdateState state;
};

/**
 * \if english
 *     @brief Connection status.
 * \else
 *     @brief sdk连接状态。
 * \endif
 */
enum SdkConnectionState {
    /**
     * \if english
     *     @brief Authorization failed.
     * \else
     *     @brief 认证失败。
     * \endif
     */
    SDK_AUTH_FAILED,

    /**
     * \if english
     *     @brief Authorization succeed.
     * \else
     *     @brief 认证成功。
     * \endif
     */
    SDK_AUTH_SUCCEED,

    /**
     * \if english
     *     @brief Connection failed.
     * \else
     *     @brief 连接失败。
     * \endif
     */
    SDK_CONNECT_FAILED,

    /**
    * \if english
    *     @brief Connection ongoing.
    * \else
    *     @brief 正在连接中。
    * \endif
    */
    SDK_CONNECTING,

    /**
     * \if english
     *     @brief Connection succeed.
     * \else
     *     @brief 连接成功。
     * \endif
     */
    SDK_CONNECT_SUCCEED
};

/**
 * \if english
 *     @brief Implement by APP.
 * \else
 *     @brief 本类里的接口需要由APP层实现，并在APP生存期，由SDK进行调用。
 * \endif
 */
class ApplicationImplementation {
public:
    /**
     * \if english
     *     @brief Destructor function.
     * \else
     *     @brief 析构函数。
     * \endif
     */
    virtual ~ApplicationImplementation() = default;

    /**
     * \if english
     *     @brief Set volume of speaker.
     *     @param[in] volume volume of speaker, value range [0 - 100].
     * \else
     *     @brief 设置扬声器的音量。
     *     @param[in] volume 音量，取值范围[0 - 100]。
     * \endif
     */
    virtual void setSpeakerVolume(int64_t volume) = 0;

    /**
     * \if english
     *     @brief Set mute status of speaker.
     *     @param[in] isMute mute status, True/False: mute on/mute off.
     * \else
     *     @brief 取消/设置扬声器为静音状态。
     *     @param[in] isMute 是否静音，True/False: 静音/取消静音。
     * \endif
     */
    virtual void setSpeakerMute(bool isMute) = 0;

    /**
     * \if english
     *     @brief Get volume of speaker.
     *     @return volume of speaker, value range [0 - 100].
     * \else
     *     @brief 获取扬声器的音量。
     *     @return 扬声器的音量，取值范围【0 - 100】。
     * \endif
     */
    virtual int getSpeakerVolume() = 0;

    /**
     * \if english
     *     @brief Get mute status of speaker.
     *     @return mute status of speaker, True/False: mute on/mute off.
     * \else
     *     @brief 获取扬声器的静音状态。
     *     @return 扬声器的静音状态，True/False：静音/未静音。
     * \endif
     */
    virtual bool getSpeakerMuteStatus() = 0;

#ifdef SUPPORT_DEBUGGER
    /**
    * \if english
    *     @brief Start debug mode.
    * \else
    *     @brief 开启调试模式。
    * \endif
    */
    virtual bool setStartDebugMode() = 0;

    /**
     * \if english
     *     @brief Set stop debug mode.
     * \else
     *     @brief 退出调试模式。
     * \endif
     */
    virtual bool setStopDebugMode() = 0;
#endif

    /**
     * \if english
     *     @brief Turn on/off blue tooth.
     *     @param[in] status Turn on/off status, True/False: turn on/turn off.
     * \else
     *     @brief 关闭/开启蓝牙设备。
     *     @param[in] status 打开/关闭蓝牙，True/False：打开蓝牙/关闭蓝牙。
     * \endif
     */
    virtual void setBluetoothStatus(bool status) = 0;

    /**
     * \if english
     *     @brief Link/Unlink blue tooth.
     *     @param[in] status Link/Unlink status, True/False: link/unlink.
     * \else
     *     @brief 连接蓝牙/断开蓝牙连接。
     *     @param[in] status 连接/断开状态，True/False：连接蓝牙/断开蓝牙。
     * \endif
     */
    virtual void setBluetoothConnectionStatus(bool status) {}

    /**
     * \if english
     *     @brief Turn on/off microphone.
     *     @param[in] status Turn on/off status, True/False: turn on/turn off.
     * \else
     *     @brief 关闭/开启麦克风。
     *     @param[in] status 打开/关闭麦克风，True/False：打开麦克风/关闭麦克风。
     * \endif
     */
    virtual void setMicrophoneStatus(bool status) = 0;

    /**
     * \if english
     *     @brief Inquire software version of device.
     * \else
     *     @brief 查询当前设备的版本号，APP层往往需要播放一段tts来播放当前设备的版本信息，或留空本方法的实现。
     * \endif
     */
    virtual void queryCurrentVersion() {}

    /**
     * \if english
     *     @brief enter sleep mode.
     * \else
     *     @brief 进入休眠模式，APP层往往需要执行休眠动作，或留空本方法的实现。
     * \endif
     */
    virtual void powerSleep() {}

    /**
     * \if english
     *     @brief enter shutdown mode.
     * \else
     *     @brief 关机，APP层往往需要执行关机动作，或留空本方法的实现。
     * \endif
     */
    virtual void powerShutdown() {}

    /**
     * \if english
     *     @brief reboot.
     * \else
     *     @brief 重启，APP层往往需要执行重启动作，或留空本方法的实现。
     * \endif
     */
    virtual void powerReboot() {}

    /**
     * \if english
     *     @brief Inform app the sdk connection status.
     *     @param[in] sdkConnectionState login status.
     * \else
     *     @brief 通知APP当前SDK网络连接状态。
     *     @param[in] sdkConnectionState 登录状态。
     * \endif
     */
    virtual void informSdkConnectionStatus(SdkConnectionState sdkConnectionState) = 0;

    /**
     * \if english
     *     @brief Inform app SDK had received directive from DuerOS.
     *     @param[in] directive directive information in JSON format.
     * \else
     *     @brief 通知app收到云端下发的自定义指令。
     *     @param[in] directive 指令的详细信息（JSON格式）。
     * \endif
     */
    virtual void informCustomizeDirective(const std::string& directive) {}

    /**
     * \if english
     *     @brief DLP protocol: start connection operation to third device.
     *     @param[in] deviceType device type.
     *     @param[in] clientId client id.
     *     @param[in] message message sent to third device.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：用于音箱连接第三方设备。
     *     @param[in] deviceType 设备类型。
     *     @param[in] clientId 设备的client id。
     *     @param[in] message 发送给第三方设备的消息。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool linkThirdDevice(
            int deviceType, const std::string& clientId, const std::string& message) {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: disconnect from third device.
     *     @param[in] deviceType device type.
     *     @param[in] message message sent to third device.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：断开音箱与第三方设备的连接。
     *     @param[in] deviceType 设备类型。
     *     @param[in] message 发送给第三方设备的消息。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool unlinkThirdDevice(int deviceType, const std::string& message) {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: send dlp message to third device.
     *     @param[in] deviceType device type.
     *     @param[in] message dlp message(JSON string).
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：发送DLP消息给第三方设备。
     *     @param[in] deviceType 设备类型。
     *     @param[in] message 发送的消息（JSON字符串）
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool onThirdDlpMessage(int deviceType, const std::string& message) {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: receive third party dlp directive to app layer.
     *     @param[in] message dlp message(JSON string).
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：收到小度之家发来的thirdparty指令。。
     *     @param[in] message 发送的消息（JSON字符串）
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool onThirdPartyMessage(const std::string& message) {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: send dlp message to xiaodu app in broadcast mode.
     *     @param[in] context dlp message(JSON string).
     *     @param[in] deviceType device type.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：广播方式发送dlp消息给小度之家APP。
     *     @param[in] context dlp消息(JSON string)。
     *     @param[in] deviceType 设备类型。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual void notifySdkContext(const std::string& context, int deviceType) {};

    /**
     * \if english
     *     @brief DLP protocol: send dlp message to xiaodu app by session id.
     *     @param[in] context dlp message(JSON string).
     *     @param[in] sessionId session id.
     *     @param[in] deviceType device type.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：发送dlp消息给指定的小度之家APP。
     *     @param[in] context dlp消息(JSON string)。
     *     @param[in] sessionId session id。
     *     @param[in] deviceType 设备类型。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual void notifySdkContextBySessionId(const std::string& context,
                                             unsigned short sessionId,
                                             int deviceType) {};

    /**
     * \if english
     *     @brief Get system information.
     *     @param[in] systemInformation system information.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief 获取设备的系统信息。
     *     @param[out] systemInformation 设备信息。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool systemInformationGetStatus(SystemInformation& systemInformation) {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: Inform app restore factory settings.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：通知APP恢复出厂设置，具体实现由厂家定义。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool systemInformationHardReset() {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: Get system software information.
     *     @param[out] systemUpdateInformation store system update information.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：获取系统软件状态信息。
     *     @param[out] systemUpdateInformation 接收APP层传入的系统更新信息。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool systemUpdateGetStatus(SystemUpdateInformation &systemUpdateInformation) {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: Inform app upgrade system.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：通知APP进行系统更新。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool systemUpdateUpdate() {
        return false;
    }

    /**
     * \if english
     *     @brief DLP protocol: Get current connected device info by device type.
     *     @param[in] deviceType Device type defined by DuerLink.
     *     @param[out] clientId Store client id from DuerLink.
     *     @param[out] deviceId Store device id from DuerLink.
     *     @return True/False: Operation succeed/failed.
     * \else
     *     @brief DLP协议：根据设备类型获取当前连接设备的client id和device id。
     *     @param[in] deviceType 设备类型
     *     @param[out] clientId DuerLink返回的client id。
     *     @param[out] deviceId DuerLink返回的device id。
     *     @return True/False：调用成功/调用失败。
     * \endif
     */
    virtual bool getCurrentConnectedDeviceInfo(int deviceType,
                                               std::string& clientId, std::string& deviceId) {
        return false;
    }

    /**
     * \if english
     * @brief get wifi bssid
     * @param[in] void
     * @return string
     * \else
     *  @brief: 获取当前wifi连接的路由器的mac 地址
     *  @param[in]:空
     *  @return string
     *  \endif
     */
    virtual std::string getWifiBssid(){
        return "";
    }

#ifdef SUPPORT_INFRARED
    /**
     * \if english
     *     @brief send infrared ray code request
     *     @param[in] carrierFrequency    infrared ray code carrierFrequency
     *     @param[in] pattern             infrared ray code pattern
     *     @return bool
     * \else
     *     @brief 发射红外控制码
     *     @param[in] carrierFrequency    红外控制码的载波频率，单位是赫兹，取值范围在20000到60000
     *     @param[in] pattern             红外控制码是由多组连续的高低电平时间组成的波形，每个数字表示电平信号时间，单位是微秒。
     *                                    第一个整数表示高电平信号时间，随后是低电平信号时间，随后又是高电平、低电平，如此类推。
     *     @return bool
     *  \endif
     */
    virtual bool sendInfraredRayCodeRequest(int carrierFrequency,
                                            const std::string &pattern) {
        return false;
    }
#endif
};

} // namespace sdkInterfaces
} // namespace duerOSDcsSDK

#endif // DUEROS_DCS_SDK_DCS_SDK_INCLUDE_APPLICATION_IMPLEMENTATION_H
