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

#include "DCSApp/Audio/AlsaAudioMicrophoneWrapper.h"
#include "DCSApp/DeviceIoWrapper.h"
#include "DCSApp/RecordAudioManager.h"
#include "LoggerUtils/DcsSdkLogger.h"

// the class factories
extern "C" std::shared_ptr<duerOSDcsApp::application::AudioMicrophoneInterface> create(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) {
    return duerOSDcsApp::application::AlsaAudioMicroponeWrapper::create(dcsSdk);
}

namespace duerOSDcsApp {
namespace application {
std::unique_ptr<AlsaAudioMicroponeWrapper> AlsaAudioMicroponeWrapper::create(
        std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) {
    if (!dcsSdk) {
        APP_ERROR("Invalid dcsSdk passed to AlsaAudioMicrophoneWrapper");
        return nullptr;
    }
    std::unique_ptr<AlsaAudioMicroponeWrapper> alsaAudioMicrophoneWrapper(new AlsaAudioMicroponeWrapper(dcsSdk));
    if (!alsaAudioMicrophoneWrapper->initialize()) {
        APP_ERROR("Failed to initialize AlsaAudioMicrophoneWrapper");
        return nullptr;
    }
    return alsaAudioMicrophoneWrapper;
}

AlsaAudioMicroponeWrapper::AlsaAudioMicroponeWrapper(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) :
        m_dcsSdk{dcsSdk},
        _m_rec{nullptr},
        m_callBack{nullptr} {
}

AlsaAudioMicroponeWrapper::~AlsaAudioMicroponeWrapper() {
    _m_rec->recorderClose();
}

void AlsaAudioMicroponeWrapper::recordDataInputCallback(char* buffer, long unsigned int size, void *userdata) {
    AlsaAudioMicroponeWrapper* wrapper = static_cast<AlsaAudioMicroponeWrapper*>(userdata);
    if (wrapper->running == false) {
        return;
    }
    ssize_t returnCode = 1;
    if (!DeviceIoWrapper::getInstance()->isPhoneConnected()) {
        returnCode = wrapper->m_dcsSdk->writeAudioData(buffer, size/2);
    }

    if (wrapper->m_callBack && buffer && size != 0) {
        wrapper->m_callBack((const char *)buffer, size);
    }

    if (returnCode <= 0) {
        APP_ERROR("Failed to write to stream.");
        return;
    }
    return;
}

bool AlsaAudioMicroponeWrapper::initialize() {

    int ret  = 0;
    APP_INFO("use alsa recorder");
    _m_rec = new AudioRecorder();
    if (_m_rec != nullptr) {
    ret = _m_rec->recorderOpen("default", recordDataInputCallback, this);
    if (0 != ret) {
            APP_ERROR("Failed to open recorder");
            return false;
        }
    } else {
        APP_ERROR("Failed to new AudioRecorder");
        return false;
    }
    return true;

}

bool AlsaAudioMicroponeWrapper::startStreamingMicrophoneData() {
    std::lock_guard<std::mutex> lock{m_mutex};
    running = true;
    return true;
}

void AlsaAudioMicroponeWrapper::setRecordDataInputCallback(void (*callBack)(const char *buffer, unsigned int size)) {
    m_callBack = callBack;
}

bool AlsaAudioMicroponeWrapper::stopStreamingMicrophoneData() {
    std::lock_guard<std::mutex> lock{m_mutex};

    running = false;
    return true;

}

}  // namespace application
}  // namespace duerOSDcsApp