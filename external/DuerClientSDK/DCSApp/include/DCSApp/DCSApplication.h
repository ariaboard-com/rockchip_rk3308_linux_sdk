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

#ifndef DUEROS_DCS_APP_SAMPLEAPP_SAMPLEAPPLICATION_H_
#define DUEROS_DCS_APP_SAMPLEAPP_SAMPLEAPPLICATION_H_

#include <memory>
#include "SoundController.h"
#include "DeviceIoWrapper.h"
#include "DuerLinkWrapper.h"
#include "SystemUpdateRevWrapper.h"
#include "DeviceTools/DyLibrary.h"
#include "DCSKeyHandler.h"

namespace duerOSDcsApp {
namespace application {

/**
 * \if english
 *     @brief DCSApp application.
 * \else
 *     @brief DCSApp 应用程序。
 * \endif
 */
class DCSApplication : public IDuerLinkWrapperCallback {
public:
    /**
     * \if english
     *     @brief Create a DCSApplication.
     *     @return A new DCSApplication, or nullptr if the operation failed.
     * \else
     *     @brief 创建一个DCSApp应用程序。
     *     @return 一个DCSApp应用程序或者如果出错则返回空。
     * \endif
     */
    static std::unique_ptr<DCSApplication> create();

    /**
     * \if english
     *     @brief Runs the application, blocking until the application to quit.
     * \else
     *     @brief 运行应用程序，阻塞着直到应用程序退出。
     * \endif
     */
    void run();

private:
    /**
     * \if english
     *     @brief Initialize a DCSApplication.
     *     @return true if initialization succeeded, else false.
     * \else
     *     @brief 初始化一个DCSApp应用程序。
     *     @return 初始化应用程序成功返回true，否则返回false。
     * \endif
     */
    bool initialize();

    /**
     * \if english
     *     @brief network is ready to work.
     * \else
     *     @brief 网络已经可以正常使用。
     * \endif
     */
    void networkReady() override;

    /**
     * \if english
     *     @brief received data from DuerLink.
     *     @param[in] jsonPackageData received json data.
     *     @param[in] sessionId session id.
     * \else
     *     @brief 从DuerLink接收数据。
     *     @param[in] jsonPackageData 接收到的json数据。
     *     @param[in] sessionId 发送消息的session id。
     * \endif
     */
    void duerlinkNotifyReceivedData(const std::string& jsonPackageData, int sessionId = 0) override;

    /**
     * \if english
     *     @brief received bduss from DuerLink.
     *     @param[in] bdussValue received bduss.
     * \else
     *     @brief 从DuerLink接收bduss。
     *     @param[in] bdussValue 接收到bduss。
     * \endif
     */
    void duerlink_notify_received_bduss(const std::string& bdussValue) override;

    /**
     * \if english
     *     @brief check network status from DuerLink.
     *     @param[in] status online status.
     * \else
     *     @brief 从DuerLink获取目前的网络状态。
     *     @param[in] status 是否在线状态。
     * \endif
     */
    void duerlinkNetworkOnlineStatus(bool status) override;

    /**
     * \if english
     *     @brief detect the if NTP sync successfully.
     * \else
     *     @brief 持续检测NTP时间是否同步成功。
     * \endif
     */
    void detectNTPReady();

#ifdef ENABLE_VAD

    unsigned int read_pid_process();

    unsigned int read_voice_inactive_frames();
#endif

    /**
     * \if english
     *     @brief microphone record data callback.
     *     @param[in] buffer record data buffer。
     *     @param[in] size record data buffer size。
     * \else
     *     @brief 录音数据回调函数。
     *     @param[in] buffer 录音数据。
     *     @param[in] size 录音数据大小。
     * \endif
     */
    static void recordDataInputCallback(const char* buffer, unsigned int size);

    /**
     * \if english
     *     @brief the DCSSDK handler.
     * \else
     *     @brief DCSSDK 实例。
     * \endif
     */
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> m_dcsSdk;

    /**
     * \if english
     *     @brief the system update receive dlp wrapper.
     * \else
     *     @brief 系统升级接收dlp指令处理实例。
     * \endif
     */
    std::unique_ptr<SystemUpdateRevWrapper> m_systemUpdateRevWrapper;

    /**
     * \if english
     *     @brief the timer of detectNTPReady.
     * \else
     *     @brief 检测NTP的定时器。
     * \endif
     */
    deviceCommonLib::deviceTools::Timer m_detectNTPTimer;

    /**
    * \if english
    *     @brief the audio library name.
    * \else
    *     @brief 音频库名。
    * \endif
    */
    std::string m_audioLibName;

    /**
    * \if english
    *     @brief the audio dynamic library.
    * \else
    *     @brief 音频动态库。
    * \endif
    */
    std::shared_ptr<deviceCommonLib::deviceTools::DyLibrary> m_audioDyLib;

	std::unique_ptr<DCSKeyHandler> m_keyHandler;
    duerOSDcsSDK::sdkInterfaces::DcsSdkParameters parameters;
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_SAMPLEAPP_SAMPLEAPPLICATION_H_
