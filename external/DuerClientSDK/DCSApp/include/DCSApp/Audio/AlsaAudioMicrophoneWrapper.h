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

#ifndef DUEROS_DCS_APP_SAMPLEAPP_ALSAAUDIOMICROPHONEWRAPPER_H_
#define DUEROS_DCS_APP_SAMPLEAPP_ALSAAUDIOMICROPHONEWRAPPER_H_
#include <mutex>
#include <thread>
#include <DcsSdk/DcsSdk.h>
#include "AlsaRecorder.h"
#include "AudioMicrophoneInterface.h"

namespace duerOSDcsApp {
namespace application {

/// This acts as a wrapper base on Alsa, a cross-platform open-source audio I/O library.
class AlsaAudioMicroponeWrapper : public AudioMicrophoneInterface {
public:
    /**
     * Creates a @c AlsaAudioMicrophoneWrapper.
     *
     * @param dcsSdk.
     * @return A unique_ptr to a @c AlsaAudioMicrophoneWrapper if creation was successful and @c nullptr otherwise.
     */
    static std::unique_ptr <AlsaAudioMicroponeWrapper>
    create(std::shared_ptr <duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk);

    /**
     * Stops streaming from the microphone.
     *
     * @return Whether the stop was successful.
     */
    bool stopStreamingMicrophoneData() override;

    /**
     * Starts streaming from the microphone.
     *
     * @return Whether the start was successful.
     */
    bool startStreamingMicrophoneData() override;

    void setRecordDataInputCallback(void (*callBack)(const char *buffer, unsigned int size)) override;

    /**
     * Destructor.
     */
    ~AlsaAudioMicroponeWrapper();

private:
    /**
     * Constructor.
     *
     * @param dcsSdk.
     */
    AlsaAudioMicroponeWrapper(std::shared_ptr <duerOSDcsSDK::sdkInterfaces::DcsSdk> dcsSdk);

    static void recordDataInputCallback(char *buffer, long unsigned int size, void *userdata);


    /// Initializes Alsa Audio
    bool initialize();

    std::shared_ptr <duerOSDcsSDK::sdkInterfaces::DcsSdk> m_dcsSdk;

    AudioRecorder* _m_rec;

    bool running;

    void (*m_callBack)(const char *buffer, unsigned int size);

    /**
     * A lock to seralize access to startStreamingMicrophoneData() and stopStreamingMicrophoneData() between different
     * threads.
     */
    std::mutex m_mutex;

};
}
}
#endif //DUEROS_DCS_APP_SAMPLEAPP_ALSAAUDIOMICROPHONEWRAPPER_H_
