//
// Created by zhangtuanqing on 18-4-12.
//


#include <DcsSdk/LocalMediaPlayerInterface.h>
#include "DCSApp/DeviceIoWrapper.h"
#include "Dlna/DlnaDmrOutput.h"
#include "Dlna/DlnaDmrSdk.h"
#include "DlnaPlayerImpl.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

std::shared_ptr<DlnaPlayerImpl> DlnaPlayerImpl::create() {

    std::shared_ptr<DlnaPlayerImpl> instance(new DlnaPlayerImpl());
    return instance;
};


DlnaPlayerImpl::DlnaPlayerImpl()  {

}

DlnaPlayerImpl::~DlnaPlayerImpl() {

}

void DlnaPlayerImpl::play() {
    duerOSDcsApp::dueros_dlna::Output::get_instance().output_resume();
}

void DlnaPlayerImpl::stop() {
    duerOSDcsApp::dueros_dlna::Output::get_instance().output_stop();
}

void DlnaPlayerImpl::pause() {
    duerOSDcsApp::dueros_dlna::Output::get_instance().output_pause();
}

void DlnaPlayerImpl::resume() {
    duerOSDcsApp::dueros_dlna::Output::get_instance().output_resume();
}

void DlnaPlayerImpl::playNext() {
    APP_INFO("DlnaPlayerImpl::playNext not implemented");
}

void DlnaPlayerImpl::playPrevious() {
    APP_INFO("DlnaPlayerImpl::playPrevious not implemented");
}

void DlnaPlayerImpl::registerListener(
    std::shared_ptr<duerOSDcsApp::mediaPlayer::LocalAudioPlayerListener> notify) {
    if (nullptr != notify) {
        m_listener = notify;
    }
}

void DlnaPlayerImpl::dlnaStartPlay() {
    if (nullptr != m_listener) {
        m_listener->dlnaStartPlay();
    }
}

void DlnaPlayerImpl::dlnaStopPlay() {
    if (nullptr != m_listener) {
        m_listener->dlnaStopPlay();
    }
}

void DlnaPlayerImpl::dlnaPausePlay() {
    if (nullptr != m_listener) {
        m_listener->dlnaPausePlay();
    }
}

}
}
