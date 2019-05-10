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

#ifndef DUEROS_DCS_APP_SAMPLEAPP_VOICEANDTOUCHWAKEUPOBSERVER_H_
#define DUEROS_DCS_APP_SAMPLEAPP_VOICEANDTOUCHWAKEUPOBSERVER_H_

#include <memory>
#include <string>
#include <DcsSdk/DcsSdk.h>
#include <DcsSdk/KeyWordObserverInterface.h>
#include "DCSApp/DeviceIoWrapper.h"

namespace duerOSDcsApp {
namespace application {

using namespace duerOSDcsSDK;

/**
 * \if english
 *     @brief keyword observer implementation.
 * \else
 *     @brief 唤醒模块观察类实现。
 * \endif
 */
class VoiceAndTouchWakeUpObserver : public sdkInterfaces::KeyWordObserverInterface,
                                    public DeviceIoWrapperObserverInterface {
public:
    VoiceAndTouchWakeUpObserver();

    void onKeyWordDetected(
            std::string keyword,
            uint64_t beginIndex,
            uint64_t endIndex) override;

    void onLocationDetected(
            int location) override;

    void touchStartAsr() override;

    void setDcsSdk(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk);

private:
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> m_dcsSdk;

    void wakeupTriggered(bool isVoiceWakeup,
                         uint64_t beginIndex =
                         sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX,
                         uint64_t endIndex =
                         sdkInterfaces::KeyWordObserverInterface::UNSPECIFIED_INDEX,
                         std::string keyword = "");
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_SAMPLEAPP_VOICEANDTOUCHWAKEUPOBSERVER_H_
