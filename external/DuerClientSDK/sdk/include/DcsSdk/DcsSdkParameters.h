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
 * @file DcsSdkParameters.h
 * \if english
 *     @brief SDK initialize parameter.
 * \else
 *     @brief 初始化SDK时，APP传入的参数。
 * \endif
 */

#ifndef DUEROS_DCS_SDK_INCLUDE_DCS_SDK_DCS_SDK_PARAMETERS_H_
#define DUEROS_DCS_SDK_INCLUDE_DCS_SDK_DCS_SDK_PARAMETERS_H_

#include <string>
#include <vector>
#include <unordered_set>

#include <DcsSdk/ApplicationImplementation.h>
#include <DcsSdk/ConnectionStatusObserverInterface.h>
#include <DcsSdk/DialogUXStateObserverInterface.h>
//#if (defined KITTAI_KEY_WORD_DETECTOR) || (defined BDSAI_KEY_WORD_DETECTOR)
#include <DcsSdk/KeyWordObserverInterface.h>
//#endif
#include <DcsSdk/LocalMediaPlayerInterface.h>
#include <DcsSdk/MediaPlayerInterface.h>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

using duerOSDcsSDK::sdkInterfaces::DialogUXStateObserverInterface;

class DcsClient;

/**
 * \if english
 *     @brief SDK initialize parameter.
 * \else
 *     @brief SDK初始化参数。
 * \endif
 */
struct DcsSdkParameters {
public:
    /**
     * \if english
     *     @brief Construct function.
     * \else
     *     @brief 构造函数。
     * \endif
     */
    DcsSdkParameters() :
            m_speakMediaPlayer(nullptr),
            m_audioMediaPlayer(nullptr),
            m_alertsMediaPlayer(nullptr),
            m_localMediaPlayer(nullptr),
            m_connectionObservers(nullptr),
            m_applicationImplementation(nullptr),
            m_localTtsMediaPlayer(nullptr),
            m_localPromptPlayer(nullptr),
            m_sdsUnitSize(2),
            m_debugFlag(true) {
    }

    /**
     * \if english
     *     @brief Set value form member m_pathToConfig(Read only configuration).
     *     @param[in] value Full path of sdk configure file(Read only configuration).
     * \else
     *     @brief 设置SDK配置文件全路径名（只读配置）。
     *     @param[in] value SDK配置文件的全路径名（只读配置）。
     * \endif
     */
    void setPathToConfig(const std::string& value) {
        m_pathToConfig = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_pathToRuntimeConfig(Writeable configuration).
     *     @param[in] value Full path of sdk configure file(Writeable configuration).
     * \else
     *     @brief 设置SDK配置文件全路径名（可变配置）。
     *     @param[in] value SDK配置文件的全路径名（可变配置）。
     * \endif
     */
    void setPathToRuntimeConfig(const std::string& value) {
        m_pathToRuntimeConfig = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_deviceId.
     *     @param[in] value device id.
     * \else
     *     @brief 设置设备的device id。
     *     @param[in] value 设备的device id。
     * \endif
     */
    void setDeviceId(const std::string& value) {
        m_deviceId = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_speakMediaPlayer.
     *     @param[in] value Tts player handler.
     * \else
     *     @brief 设置TTS player的实例指针。
     *     @param[in] value Tts player实例指针。
     * \endif
     */
    void setSpeakMediaPlayer(std::shared_ptr<MediaPlayerInterface> value) {
        m_speakMediaPlayer = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_audioMediaPlayer.
     *     @param[in] value Audio player handler.
     * \else
     *     @brief 设置Audio player的实例指针。
     *     @param[in] value Audio player的实例指针。
     * \endif
     */
    void setAudioMediaPlayer(std::shared_ptr<MediaPlayerInterface> value) {
        m_audioMediaPlayer = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_alertsMediaPlayer.
     *     @param[in] value Alerts player handler.
     * \else
     *     @brief 设置Alerts player的实例指针。
     *     @param[in] value Alerts player的实例指针。
     * \endif
     */
    void setAlertsMediaPlayer(std::shared_ptr<MediaPlayerInterface> value) {
        m_alertsMediaPlayer = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_localMediaPlayer.
     *     @param[in] value Full Local player handler.
     * \else
     *     @brief 设置Local player的实例指针。
     *     @param[in] value Local player的实例指针。
     * \endif
     */
    void setLocalMediaPlayer(std::shared_ptr<LocalMediaPlayerInterface> value) {
        m_localMediaPlayer = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_dialogStateObservers.
     *     @param[in] value DialogUXStateObserverInterface handler set.
     * \else
     *     @brief 设置DialogUXStateObserverInterface实例指针set。
     *     @param[in] value DialogUXStateObserverInterface的实例指针set。
     * \endif
     */
    void setDialogStateObservers(std::unordered_set<std::shared_ptr<DialogUXStateObserverInterface>> value) {
        m_dialogStateObservers = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_connectionObservers.
     *     @param[in] value ConnectionStatusObserverInterface handler.
     * \else
     *     @brief 设置ConnectionStatusObserverInterface的实例指针。
     *     @param[in] value ConnectionStatusObserverInterface实例指针。
     * \endif
     */
    void setConnectionObservers(std::shared_ptr<sdkInterfaces::ConnectionStatusObserverInterface> value) {
        m_connectionObservers = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_applicationImplementation.
     *     @param[in] value ApplicationImplementation handler.
     * \else
     *     @brief 设置ApplicationImplementation的实例指针。
     *     @param[in] value ApplicationImplementation实例指针。
     * \endif
     */
    void setApplicationImplementation(std::shared_ptr<sdkInterfaces::ApplicationImplementation> value) {
        m_applicationImplementation = value;
    }

    /**
     * \if english
     *     @brief Set value form member m_localTtsMediaPlayer.
     *     @param[in] value Local tts player handler.
     * \else
     *     @brief 设置Local tts player的实例指针。
     *     @param[in] value Local tts player的实例指针。
     * \endif
     */
    void setLocalTtsMediaPlayer
            (std::shared_ptr<MediaPlayerInterface> player) {
        m_localTtsMediaPlayer = player;
    }

    /**
     * \if english
     *     @brief Set value form member m_localPromptPlayer.
     *     @param[in] value Prompt player handler.
     * \else
     *     @brief 设置Prompt player实例指针。
     *     @param[in] value Prompt player的实例指针。
     * \endif
     */
    void setLocalPromptPlayer
            (std::shared_ptr<MediaPlayerInterface> player) {
        m_localPromptPlayer = player;
    }

    /**
     * \if english
     *     @brief Set value form member m_customizeDirective.
     *     @param[in] value Customize directive set supported by APP.
     * \else
     *     @brief 设置APP支持的个性化指令集。
     *     @param[in] value 个性化指令集。
     * \endif
     */
    void setCustomizeDirectives(std::vector<std::pair<std::string, std::string>>& customizeDirective) {
        m_customizeDirective = customizeDirective;
    }

//#if (defined KITTAI_KEY_WORD_DETECTOR) || (defined BDSAI_KEY_WORD_DETECTOR)
    /**
     * \if english
     *     @brief Set value form member m_keyWordObserverInterface.
     *     @param[in] value KeyWordObserverInterface handler.
     * \else
     *     @brief 设置KeyWordObserverInterface实例指针。
     *     @param[in] value KeyWordObserverInterface实例指针。
     * \endif
     */
    void setKeyWordObserverInterface(std::shared_ptr<sdkInterfaces::KeyWordObserverInterface> keyWordObserverInterface) {
        m_keyWordObserverInterface = keyWordObserverInterface;
    }
//#endif

    /**
     * \if english
     *     @brief Set clientSecret string
     *     @param[in] clientSecret.
     * \else
     *     @brief 设置clientSecret。
     *     @param[in] clientSecret。
     * \endif
     */
    void setClientSecret(const std::string clientSecret) {
        m_clientSecret = clientSecret;
    }

    /**
     * \if english
     *     @brief Get value of member m_pathToConfig(Read only configuration).
     *     @return Value of member m_pathToConfig(Read only configuration).
     * \else
     *     @brief 获取SDK配置文件的全路径名。
     *     @return SDK配置文件的全路径名。
     * \endif
     */
    const std::string& getPathToConfig() {
        return m_pathToConfig;
    }

    /**
     * \if english
     *     @brief Get value of member m_pathToRuntimeConfig(Writeable configuration).
     *     @return Value of member m_pathToRuntimeConfig(Writeable configuration).
     * \else
     *     @brief 获取SDK配置文件的全路径名（可变配置）。
     *     @return SDK配置文件的全路径名（可变配置）。
     * \endif
     */
    const std::string& getPathToRuntimeConfig() {
        return m_pathToRuntimeConfig;
    }

    /**
     * \if english
     *     @brief Get value of member m_deviceId.
     *     @return Value of member m_deviceId.
     * \else
     *     @brief 获取设备的device id。
     *     @return 设备的device id。
     * \endif
     */
    std::string getDeviceId() {
        return m_deviceId;
    }

    /**
     * \if english
     *     @brief Get value of member m_pathToRuntimeConfig(Writeable configuration).
     *     @return Value of member m_pathToRuntimeConfig(Writeable configuration).
     * \else
     *     @brief 获取SDK配置文件的全路径名（可变配置）。
     *     @return SDK配置文件的全路径名（可变配置）。
     * \endif
     */
    const std::string& getClientSecret() {
        return m_clientSecret;
    }

    /**
     * \if english
     *     @brief Get value of member m_speakMediaPlayer.
     *     @return Value of member m_speakMediaPlayer.
     * \else
     *     @brief 获取Tts player handler。
     *     @return Tts player handler。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> getSpeakMediaPlayer() {
        return m_speakMediaPlayer;
    }

    /**
     * \if english
     *     @brief Get value of member m_audioMediaPlayer.
     *     @return Value of member m_audioMediaPlayer.
     * \else
     *     @brief 获取Audio player handler。
     *     @return Audio player handler。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> getAudioMediaPlayer() {
        return m_audioMediaPlayer;
    }

    /**
     * \if english
     *     @brief Get value of member m_alertsMediaPlayer.
     *     @return Value of member m_alertsMediaPlayer.
     * \else
     *     @brief 获取alerts player handler。
     *     @return Alerts player handler。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> getAlertsMediaPlayer() {
        return m_alertsMediaPlayer;
    }

    /**
     * \if english
     *     @brief Get value of member m_localMediaPlayer.
     *     @return Value of member m_localMediaPlayer.
     * \else
     *     @brief 获取Local player handler。
     *     @return Local player handler。
     * \endif
     */
    std::shared_ptr<LocalMediaPlayerInterface> getLocalMediaPlayer() {
        return m_localMediaPlayer;
    }

    /**
     * \if english
     *     @brief Get value of member m_dialogStateObservers.
     *     @return Value of member m_dialogStateObservers.
     * \else
     *     @brief 获取DialogUXStateObserverInterface handler。
     *     @return DialogUXStateObserverInterface handler。
     * \endif
     */
    std::unordered_set<std::shared_ptr<DialogUXStateObserverInterface>> getDialogStateObservers() {
        return m_dialogStateObservers;
    }

    /**
     * \if english
     *     @brief Get value of member m_connectionObservers.
     *     @return Value of member m_connectionObservers.
     * \else
     *     @brief 获取ConnectionStatusObserverInterface handler。
     *     @return ConnectionStatusObserverInterface handler。
     * \endif
     */
    std::shared_ptr<sdkInterfaces::ConnectionStatusObserverInterface> getConnectionObservers() {
        return m_connectionObservers;
    }

    /**
     * \if english
     *     @brief Get value of member m_applicationImplementation.
     *     @return Value of member m_applicationImplementation.
     * \else
     *     @brief 获取ApplicationImplementation handler。
     *     @return ApplicationImplementation handler。
     * \endif
     */
    std::shared_ptr<sdkInterfaces::ApplicationImplementation> getApplicationImplementation() {
        return m_applicationImplementation;
    }

    /**
     * \if english
     *     @brief Get value of member m_localTtsMediaPlayer.
     *     @return Value of member m_localTtsMediaPlayer.
     * \else
     *     @brief 获取Local tts player handler。
     *     @return Local tts player handler。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> getLocalTtsMediaPlayer(){
        return m_localTtsMediaPlayer;
    }

    /**
     * \if english
     *     @brief Get value of member m_localPromptPlayer.
     *     @return Value of member m_localPromptPlayer.
     * \else
     *     @brief 获取Prompt player handler。
     *     @return Prompt player handler。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> getLocalPromptPlayer() {
        return m_localPromptPlayer;
    }

    /**
     * \if english
     *     @brief Get value of member m_customizeDirective.
     *     @return Value of member m_customizeDirective.
     * \else
     *     @brief 获取自定义指令集。
     *     @return 自定义指令集。
     * \endif
     */
    std::vector<std::pair<std::string, std::string>>& getCustomizeDirective() {
        return m_customizeDirective;
    };

//#if (defined KITTAI_KEY_WORD_DETECTOR) || (defined BDSAI_KEY_WORD_DETECTOR)
        /**
     * \if english
     *     @brief Get value of member m_keyWordObserverInterface.
     *     @return Value of member m_keyWordObserverInterface.
     * \else
     *     @brief 获取KeyWordObserverInterface handler。
     *     @return KeyWordObserverInterface handler。
     * \endif
     */
    std::shared_ptr<sdkInterfaces::KeyWordObserverInterface> getKeyWordObserverInterface() {
        return m_keyWordObserverInterface;
    }
//#endif
    /**
    * \if english
    *     @brief set SDS storage unit size, byte.
    * \else
    *     @brief 设置SDS存储单元，默认2，单位字节。
    * \endif
    */
    void setSDSUnitSize(const size_t& size) {
        m_sdsUnitSize = size;
    }
    /**
    * \if english
    *     @brief Get SDS storage unit size, byte.
    *     @return Value of SDS storage unit size, byte.
    * \else
    *     @brief 获取SDS存储单元，默认2，单位字节。
    *     @return SDS存储单元，单位字节。
    * \endif
    */
    size_t getSDSUnitSize() {
        return m_sdsUnitSize;
    }

    /**
    * \if english
    *     @brief set debug flag.
    * \else
    *     @brief 设置debug flag。
    * \endif
    */
    void setDebugFlag(const bool& flag) {
        m_debugFlag = flag;
    }
    /**
    * \if english
    *     @brief Get debug flag.
    *     @return debug flag.
    * \else
    *     @brief 获取debug flag。
    *     @return debug flag。
    * \endif
    */
    bool getDebugFlag() {
        return m_debugFlag;
    }

    /**
    * \if english
    *     @brief set crab coredump file path.
    * \else
    *     @brief 设置crab崩溃文件路径。
    * \endif
    */
    void setCrabPath(const std::string& path) {
        m_crabPath = path;
    }
    /**
    * \if english
    *     @brief Get crab coredump file path.
    *     @return path.
    * \else
    *     @brief 获取crab崩溃文件路径。
    *     @return 文件路径
    * \endif
    */
    std::string getCrabPath() {
        return m_crabPath;
    }
private:
    /**
     * \if english
     *     @brief SDK configuration full path name(Read only configuration).
     * \else
     *     @brief SDK配置文件名（只读配置）。
     * \endif
     */
    std::string m_pathToConfig;

    /**
     * \if english
     *     @brief SDK configuration full path name(Writeable configuration).
     * \else
     *     @brief SDK配置文件名（可变配置）。
     * \endif
     */
    std::string m_pathToRuntimeConfig;

    /**
     * \if english
     *     @brief Device id.
     * \else
     *     @brief Device id。
     * \endif
     */
    std::string m_deviceId;

    /**
     * \if english
     *     @brief Tts player handler.
     * \else
     *     @brief Tts player实例指针。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> m_speakMediaPlayer;

    /**
     * \if english
     *     @brief Audio player handler.
     * \else
     *     @brief Audio player实例指针。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> m_audioMediaPlayer;

    /**
     * \if english
     *     @brief Alerts player handler.
     * \else
     *     @brief Alerts player实例指针。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> m_alertsMediaPlayer;

    /**
     * \if english
     *     @brief Local player handler.
     * \else
     *     @brief Local player实例指针。
     * \endif
     */
    std::shared_ptr<LocalMediaPlayerInterface> m_localMediaPlayer;

    /**
     * \if english
     *     @brief DialogUXStateObserverInterface handler.
     * \else
     *     @brief DialogUXStateObserverInterface实例指针。
     * \endif
     */
    std::unordered_set<std::shared_ptr<DialogUXStateObserverInterface>>
            m_dialogStateObservers;

    /**
     * \if english
     *     @brief ConnectionStatusObserverInterface handler.
     * \else
     *     @brief ConnectionStatusObserverInterface实例指针。
     * \endif
     */
    std::shared_ptr<sdkInterfaces::ConnectionStatusObserverInterface>
            m_connectionObservers;

    /**
     * \if english
     *     @brief ApplicationImplementation handler.
     * \else
     *     @brief ApplicationImplementation实例指针。
     * \endif
     */
    std::shared_ptr<sdkInterfaces::ApplicationImplementation>
            m_applicationImplementation;

    /**
     * \if english
     *     @brief Local tts player handler.
     * \else
     *     @brief Local tts player实例指针。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> m_localTtsMediaPlayer;

    /**
     * \if english
     *     @brief Prompt player handler.
     * \else
     *     @brief Prompt player实例指针。
     * \endif
     */
    std::shared_ptr<MediaPlayerInterface> m_localPromptPlayer;

    /**
     * \if english
     *     @brief Customize directive set.
     * \else
     *     @brief 自定义指令集。
     * \endif
     */
    std::vector<std::pair<std::string, std::string>> m_customizeDirective;

//#if (defined KITTAI_KEY_WORD_DETECTOR) || (defined BDSAI_KEY_WORD_DETECTOR)
    /**
     * \if english
     *     @brief KeyWordObserverInterface handler.
     * \else
     *     @brief KeyWordObserverInterface实例指针。
     * \endif
     */
    std::shared_ptr<sdkInterfaces::KeyWordObserverInterface> m_keyWordObserverInterface;
//#endif

    /**
     * \if english
     *     @brief clientSecret.
     * \else
     *     @brief clientSecret
     * \endif
     */
    std::string m_clientSecret;

    /**
     * \if english
     *     @brief SDS unit size, byte.
     * \else
     *     @brief SDS 音频数据存储单元，字节
     * \endif
     */
    size_t m_sdsUnitSize;

    /**
     * \if english
     *     @brief debug mode flag， controler of log.
     * \else
     *     @brief 调试模式标记（控制打开或关闭log和adb）
     * \endif
     */
    bool m_debugFlag;

    /**
     * \if english
     *     @brief crab file path.
     * \else
     *     @brief crab 文件路径
     * \endif
     */
    std::string m_crabPath;
};

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_INCLUDE_DCS_SDK_DCS_SDK_PARAMETERS_H_
