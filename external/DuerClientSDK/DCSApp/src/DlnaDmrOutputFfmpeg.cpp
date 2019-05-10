////
// Created by eddy on 18-1-30.
//
#include <assert.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "DCSApp/DeviceIoWrapper.h"
#include "Dlna/DlnaDmrLogging.h"
#include "Dlna/DlnaDmrUpnpConnMgr.h"
#include "Dlna/DlnaDmrOutputFfmpeg.h"
#include "LoggerUtils/DcsSdkLogger.h"
#include <DeviceIo/DeviceIo.h>
#include <Mp3UrlPlayerImpl.h>

namespace duerOSDcsApp {
namespace dueros_dlna {

static output_transition_cb_t play_trans_callback_ = nullptr;
static output_update_meta_cb_t meta_update_callback_ = nullptr;
static enum MediaPlayerState m_mediaPlayerState = MEDIAPLAYER_STOPPED;

void OutputFfmpeg::set_next_uri(const std::string uri) {
    APP_INFO("OutputFfmpeg::set_next_uri");
    m_mediaPlayer->setSource(uri);
}

void OutputFfmpeg::set_uri(const std::string uri, output_update_meta_cb_t meta_cb) {
    APP_INFO("OutputFfmpeg::set_uri: %s", uri.c_str());
    meta_update_callback_ = meta_cb;
    m_mediaPlayer->setSource(uri);
}

int OutputFfmpeg::play(output_transition_cb_t callback) {
    APP_INFO("OutputFfmpeg::play");
    play_trans_callback_ = callback;

    if (application::DeviceIoWrapper::getInstance()->isSleepMode()) {
        return -1;
    }

    switch (m_mediaPlayerState) {
    case MEDIAPLAYER_STOPPED:
    case MEDIAPLAYER_FINISHED:
        m_mediaPlayer->play();
        break;

    case MEDIAPLAYER_PAUSED:
        m_mediaPlayer->resume();
        break;

    default:
        APP_INFO("OutputFfmpeg::play m_mediaPlayerState:%d", m_mediaPlayerState);
        break;
    }

    return 0;
}

int OutputFfmpeg::stop(void) {
    APP_INFO("OutputFfmpeg::stop");
    m_mediaPlayer->stop();
    return 0;
}

int OutputFfmpeg::pause(void) {
    APP_INFO("OutputFfmpeg::pause");
    m_mediaPlayer->pause();
    return 0;
}

int OutputFfmpeg::resume(void) {
    APP_INFO("OutputFfmpeg::resume");
    if (application::DeviceIoWrapper::getInstance()->isSleepMode()) {
        return -1;
    }
    m_mediaPlayer->resume();
    return 0;
}

int OutputFfmpeg::seek(long position_nanos) {
    APP_INFO("OutputFfmpeg::seek");
    if (m_mediaPlayerState != MEDIAPLAYER_STOPPED) {
        m_mediaPlayer->seek(position_nanos / 1000000);
        return 0;
    } else {
        APP_INFO("OutputFfmpeg::seek m_mediaPlayerState != MEDIAPLAYER_STOPPED return -1");
        return -1;
    }
}

int OutputFfmpeg::get_position(long* track_duration,
                               long* track_pos) {
    //APP_INFO("OutputFfmpeg::get_position");
    m_mediaPlayer->getPosition(track_duration, track_pos);
    return 0;
}

int OutputFfmpeg::get_volume(float* v) {
    *v = application::DeviceIoWrapper::getInstance()->getCurrentVolume();
    //    *v = m_mediaPlayer->getVolume();
    APP_INFO("OutputFfmpeg::get_volume %f", *v);
    return 0;
}

int OutputFfmpeg::set_volume(float value) {
    APP_INFO("OutputFfmpeg::set_volume %f", value);
    application::DeviceIoWrapper::getInstance()->setCurrentVolume(value);
    //    m_mediaPlayer->setVolume(value/100);
    return 0;
}

int OutputFfmpeg::get_mute(int* m) {
    if (application::DeviceIoWrapper::getInstance()->isMute()) {
        *m = 1;
    } else {
        *m = 0;
    }

    APP_INFO("OutputFfmpeg::get_mute %d", *m);
    return 0;
}

int OutputFfmpeg::set_mute(int m) {
    APP_INFO("OutputFfmpeg::set_mute %d", m);
    if (application::DeviceIoWrapper::getInstance()->isSleepMode()) {
        return -1;
    }

    if (m) {
        application::DeviceIoWrapper::getInstance()->setMute(true);
    } else {
        application::DeviceIoWrapper::getInstance()->setMute(false);
    }

    return 0;
}

int OutputFfmpeg::get_state(void) {
    switch (m_mediaPlayerState) {
    case MEDIAPLAYER_STOPPED:
        APP_INFO("OutputFfmpeg::get_state: MEDIAPLAYER_STOPPED");
        break;

    case MEDIAPLAYER_PLAYING:
        APP_INFO("OutputFfmpeg::get_state: MEDIAPLAYER_PLAYING");
        break;

    case MEDIAPLAYER_PAUSED:
        APP_INFO("OutputFfmpeg::get_state: MEDIAPLAYER_PAUSED");
        break;

    case MEDIAPLAYER_FINISHED:
        APP_INFO("OutputFfmpeg::get_state: MEDIAPLAYER_FINISHED");
        break;

    default:
        APP_INFO("OutputFfmpeg::get_state: %d", m_mediaPlayerState);
        break;
    }

    return m_mediaPlayerState;
}

int OutputFfmpeg::init(void) {
    APP_INFO("OutputFfmpeg::init duer_plug");
    m_mediaPlayer = mediaPlayer::MediaPlayerProxy::create("duer_plug");

    if (!m_mediaPlayer) {
        APP_INFO("Failed to create media player for content!");
        return false;
    }

#if 1
    auto mp3Player = duerOSDcsApp::mediaPlayer::Mp3UrlPlayerImpl::create("duer_plug");
    mp3Player->registerListener(m_mediaPlayer);
    m_mediaPlayer->setImpl(mp3Player);
#endif

    auto outputFfmpeg = std::shared_ptr<OutputFfmpeg>(new OutputFfmpeg());
    m_mediaPlayer->setObserver(outputFfmpeg);
    return 0;
}

void OutputFfmpeg::onPlaybackStarted() {
    APP_INFO("OutputFfmpeg::onPlaybackStarted");
    m_mediaPlayerState = MEDIAPLAYER_PLAYING;

    if (play_trans_callback_) {
        play_trans_callback_(PLAY_PLAYING);
    }

    application::DeviceIoWrapper::getInstance()->callback(framework::DeviceInput::DLNA_PLAYING, NULL, 0);
}

void OutputFfmpeg::onPlaybackStopped() {
    APP_INFO("OutputFfmpeg::onPlaybackStopped");
    m_mediaPlayerState = MEDIAPLAYER_STOPPED;

    if (play_trans_callback_) {
        play_trans_callback_(PLAY_STOPPED);
    }

    application::DeviceIoWrapper::getInstance()->callback(framework::DeviceInput::DLNA_STOPPED, NULL, 0);
}

void OutputFfmpeg::onPlaybackPaused() {
    APP_INFO("OutputFfmpeg::onPlaybackPaused");
    m_mediaPlayerState = MEDIAPLAYER_PAUSED;

    if (play_trans_callback_) {
        play_trans_callback_(PLAY_PAUSED_PLAYBACK);
    }

    application::DeviceIoWrapper::getInstance()->callback(framework::DeviceInput::DLNA_PAUSED, NULL, 0);
}

void OutputFfmpeg::onPlaybackFinished() {
    APP_INFO("OutputFfmpeg::onPlaybackFinished");
    m_mediaPlayerState = MEDIAPLAYER_FINISHED;
    /*
        if (play_trans_callback_) {
            play_trans_callback_(PLAY_STOPPED);
        }
    */
}

void OutputFfmpeg::onPlaybackNearlyfinished() {

}

void OutputFfmpeg::onPlaybackError(const duerOSDcsSDK::sdkInterfaces::ErrorType& type,
                                   std::string error) {
    if (play_trans_callback_) {
        play_trans_callback_(PLAY_NO_MEDIA_PRESENT);
    }
}


void OutputFfmpeg::onPlaybackResumed() {
    APP_INFO("OutputFfmpeg::onPlaybackResumed");
    m_mediaPlayerState = MEDIAPLAYER_PLAYING;

    if (play_trans_callback_) {
        play_trans_callback_(PLAY_PLAYING);
    }

    application::DeviceIoWrapper::getInstance()->callback(framework::DeviceInput::DLNA_PLAYING, NULL, 0);
}

void OutputFfmpeg::onBufferUnderrun() {

}

void OutputFfmpeg::onBufferRefilled() {

}

void OutputFfmpeg::onRecvFirstpacket() {

}

}
}




















