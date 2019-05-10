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

#include <DcsSdk/LocalMediaPlayerInterface.h>
#include "DCSApp/DeviceIoWrapper.h"
#include "BluetoothPlayerImpl.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

std::shared_ptr<BluetoothPlayerImpl> BluetoothPlayerImpl::create() {

    std::shared_ptr<BluetoothPlayerImpl> btPlayerClient(new BluetoothPlayerImpl());
    return btPlayerClient;
};


BluetoothPlayerImpl::BluetoothPlayerImpl()  {

}

BluetoothPlayerImpl::~BluetoothPlayerImpl() {

}

void BluetoothPlayerImpl::play() {
    application::DeviceIoWrapper::getInstance()->btResumePlay();
}

void BluetoothPlayerImpl::stop() {
    application::DeviceIoWrapper::getInstance()->btPausePlay();
}

void BluetoothPlayerImpl::pause() {
    application::DeviceIoWrapper::getInstance()->btPausePlay();
}

void BluetoothPlayerImpl::resume() {
    application::DeviceIoWrapper::getInstance()->btResumePlay();
}

void BluetoothPlayerImpl::playNext() {
    APP_INFO("BluetoothPlayerImpl::playNext not implemented");
}

void BluetoothPlayerImpl::playPrevious() {
    APP_INFO("BluetoothPlayerImpl::playPrevious not implemented");
}

void BluetoothPlayerImpl::registerListener(
    std::shared_ptr<duerOSDcsApp::mediaPlayer::LocalAudioPlayerListener> notify) {
    if (nullptr != notify) {
        m_listener = notify;
    }
}

void BluetoothPlayerImpl::btStartPlay() {
    if (nullptr != m_listener) {
        m_listener->btStartPlay();
    }
}

void BluetoothPlayerImpl::btStopPlay() {
    if (nullptr != m_listener) {
        m_listener->btStopPlay();
    }
}

void BluetoothPlayerImpl::btDisconnect() {
    if (nullptr != m_listener) {
        m_listener->btDisconnect();
    }
}

void BluetoothPlayerImpl::btPowerOff() {
    if (nullptr != m_listener) {
        m_listener->btPowerOff();
    }
}

}
}