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

#include "DCSApp/Audio/PortAudioMicrophoneWrapper.h"
#include "DCSApp/DeviceIoWrapper.h"
#include "LoggerUtils/DcsSdkLogger.h"

// the class factories
extern "C" std::shared_ptr<duerOSDcsApp::application::AudioMicrophoneInterface> create(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) {
    return duerOSDcsApp::application::PortAudioMicrophoneWrapper::create(dcsSdk);
}

namespace duerOSDcsApp {
namespace application {
    
static const int NUM_INPUT_CHANNELS = 1;
static const int NUM_OUTPUT_CHANNELS = 0;
static const double SAMPLE_RATE = 16000;
static const unsigned long PREFERRED_SAMPLES_PER_CALLBACK = paFramesPerBufferUnspecified;

std::unique_ptr<PortAudioMicrophoneWrapper> PortAudioMicrophoneWrapper::create(
    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) {
    if (!dcsSdk) {
        APP_ERROR("Invalid dcsSdk passed to PortAudioMicrophoneWrapper");
        return nullptr;
    }
    std::unique_ptr<PortAudioMicrophoneWrapper> portAudioMicrophoneWrapper(new PortAudioMicrophoneWrapper(dcsSdk));
    if (!portAudioMicrophoneWrapper->initialize()) {
        APP_ERROR("Failed to initialize PortAudioMicrophoneWrapper");
        return nullptr;
    }
    return portAudioMicrophoneWrapper;
}

PortAudioMicrophoneWrapper::PortAudioMicrophoneWrapper(std::shared_ptr<duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk) :
        m_dcsSdk{dcsSdk},
        m_paStream{nullptr},
        m_callBack{nullptr} {
}

PortAudioMicrophoneWrapper::~PortAudioMicrophoneWrapper() {
    Pa_StopStream(m_paStream);
    Pa_CloseStream(m_paStream);
    Pa_Terminate();
}

bool PortAudioMicrophoneWrapper::initialize() {
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        APP_ERROR("Failed to initialize PortAudio");
        return false;
    }
    err = Pa_OpenDefaultStream(
        &m_paStream,
        NUM_INPUT_CHANNELS,
        NUM_OUTPUT_CHANNELS,
        paInt16,
        SAMPLE_RATE,
        PREFERRED_SAMPLES_PER_CALLBACK,
        PortAudioCallback,
        this);
    if (err != paNoError) {
        APP_ERROR("Failed to open PortAudio default stream");
        return false;
    }
    return true;
}

bool PortAudioMicrophoneWrapper::startStreamingMicrophoneData() {
    std::lock_guard<std::mutex> lock{m_mutex};
    PaError err = Pa_StartStream(m_paStream);
    if (err != paNoError) {
        APP_ERROR("Failed to start PortAudio stream");
        return false;
    }
    return true;

}

void PortAudioMicrophoneWrapper::setRecordDataInputCallback(void (*callBack)(const char *buffer, unsigned int size)) {
    m_callBack = callBack;
}

bool PortAudioMicrophoneWrapper::stopStreamingMicrophoneData() {
    std::lock_guard<std::mutex> lock{m_mutex};
    PaError err = Pa_StopStream(m_paStream);
    if (err != paNoError) {
        APP_ERROR("Failed to stop PortAudio stream");
        return false;
    }
    return true;
}

int PortAudioMicrophoneWrapper::PortAudioCallback(
    const void* inputBuffer,
    void* outputBuffer,
    unsigned long numSamples,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void* userData) {
    PortAudioMicrophoneWrapper* wrapper = static_cast<PortAudioMicrophoneWrapper*>(userData);

    ssize_t returnCode = 1;
    if (!DeviceIoWrapper::getInstance()->isPhoneConnected()) {
        returnCode = wrapper->m_dcsSdk->writeAudioData(inputBuffer, numSamples);
    }

    if (wrapper->m_callBack && inputBuffer && numSamples != 0) {
        wrapper->m_callBack((const char *)inputBuffer, 2 * numSamples);
    }
    if (returnCode <= 0) {
        APP_ERROR("Failed to write to stream.");
        return paAbort;
    }
    return paContinue;
}

}  // namespace application
}  // namespace duerOSDcsApp