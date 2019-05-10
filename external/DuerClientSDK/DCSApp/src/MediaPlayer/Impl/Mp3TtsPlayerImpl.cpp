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

#include <unistd.h>
#include "DCSApp/DeviceIoWrapper.h"
#include <LoggerUtils/DcsSdkLogger.h>
#include "Mp3TtsPlayerImpl.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
using application::ThreadPoolExecutor;
using application::DeviceIoWrapper;

#define FINISH_READ_FLAG -1
#define AVIO_CTX_BUFFER_SIZE 512
#define RESAMPLE_BUFFER_SIZE 48000
#define INVALID_VALUE_THRESHOLD 50

std::shared_ptr<Mp3TtsPlayerImpl> Mp3TtsPlayerImpl::create(const std::string& audio_dev) {
    std::shared_ptr<Mp3TtsPlayerImpl> instance(new Mp3TtsPlayerImpl(audio_dev));
    return instance;
}

Mp3TtsPlayerImpl::Mp3TtsPlayerImpl(const std::string& audio_dev) : m_executor(nullptr),
    m_alsaCtl(nullptr),
    m_pcmResampleBuf(nullptr),
    m_avioBuf(nullptr),
    m_packetCount(0L),
    m_pushingFlag(false),
    m_stopFlag(false),
    m_isFirstPacket(true),
    m_removeHeadFlag(false),
    m_logCtlFlag(true),
    m_aliveFlag(true), 
    m_observer(NULL) {
    m_executor = ThreadPoolExecutor::getInstance();
    m_alsaCtl = new AlsaController(audio_dev);
    m_alsaCtl->init(24000, 1);
    m_pcmResampleBuf = (uint8_t*)av_malloc(RESAMPLE_BUFFER_SIZE);
    pthread_mutex_init(&m_playMutex, nullptr);
    pthread_cond_init(&m_playCond, nullptr);

    av_register_all();
    av_log_set_callback(ffmpegLogOutput);
    av_log_set_level(AV_LOG_INFO);

    pthread_create(&m_playThread, nullptr, playFunc, this);
}

Mp3TtsPlayerImpl::~Mp3TtsPlayerImpl() {
    m_aliveFlag = false;
    pthread_cond_signal(&m_playCond);

    void* play_thread_return = nullptr;

    if (m_playThread != 0) {
        pthread_join(m_playThread, &play_thread_return);
    }

    if (m_pcmResampleBuf != nullptr) {
        av_free(m_pcmResampleBuf);
        m_pcmResampleBuf = nullptr;
    }

    m_streamPool.clearItems();

    m_alsaCtl->aslaAbort();
    m_alsaCtl->alsaClose();
    delete m_alsaCtl;
    m_alsaCtl = nullptr;

    pthread_mutex_destroy(&m_playMutex);
    pthread_cond_destroy(&m_playCond);
}

void Mp3TtsPlayerImpl::setPlayerObserver(PlayerAvtivityObserver* observer) {
    m_observer = observer;
}

void Mp3TtsPlayerImpl::registerListener(std::shared_ptr<TtsPlayerListener> listener) {
    m_listeners.push_back(listener);
}

void Mp3TtsPlayerImpl::ttsPlay() {
    m_removeHeadFlag = false;
    m_alsaCtl->alsaPrepare();
    m_isFirstPacket = true;
    m_stopFlag = false;
    m_streamPool.clearItems();
    pthread_mutex_lock(&m_playMutex);
    m_pushingFlag = true;
    pthread_cond_signal(&m_playCond);
    pthread_mutex_unlock(&m_playMutex);
    m_packetCount = 0;
}

void Mp3TtsPlayerImpl::pushData(const char* data, unsigned int len) {
    m_logCtlFlag = !m_logCtlFlag;

    if (m_pushingFlag && len >= 0) {
        if (m_logCtlFlag) {
            APP_INFO("push data(%d) to Mp3TtsPlayerImpl.", len);
        }

        m_streamPool.pushStream(data, len);
    }
}

void Mp3TtsPlayerImpl::ttsEnd() {
    m_pushingFlag = false;
}

void Mp3TtsPlayerImpl::ttsStop() {
    m_alsaCtl->aslaAbort();
    m_alsaCtl->alsaClear();
    m_pushingFlag = false;
    m_stopFlag = true;
    executePlaybackStopped();
}

void Mp3TtsPlayerImpl::executePlaybackStarted() {
    if (m_observer) {
        m_observer->playStart(DeviceIoWrapper::getInstance()->getCurrentVolume());
    }

    m_executor->submit([this]() {
        size_t size = m_listeners.size();

        for (size_t i = 0; i < size; ++i) {
            if (nullptr != m_listeners[i]) {
                m_listeners[i]->playbackStarted();
            }
        }
    });
}

void Mp3TtsPlayerImpl::executePlaybackStopped() {
    if (m_observer) {
        m_observer->playEnd(0);
    }

    size_t size = m_listeners.size();

    for (size_t i = 0; i < size; ++i) {
        if (nullptr != m_listeners[i]) {
            m_listeners[i]->playbackStopped(STOP_STATUS_INTERRUPT);
        }
    }
}

void* Mp3TtsPlayerImpl::playFunc(void* arg) {
    auto player = (Mp3TtsPlayerImpl*)arg;

    while (player->m_aliveFlag) {
        pthread_mutex_lock(&player->m_playMutex);
        pthread_cond_wait(&player->m_playCond, &player->m_playMutex);

        player->m_avioBuf = (uint8_t*) av_mallocz(AVIO_CTX_BUFFER_SIZE);
        //AVInputFormat* inputFormat = nullptr;
        AVInputFormat* inputFormat = av_find_input_format("mp3");
        AVIOContext* avio_ctx = avio_alloc_context(player->m_avioBuf,
                                AVIO_CTX_BUFFER_SIZE,
                                0,
                                player,
                                &readPacket,
                                nullptr,
                                nullptr);

        /*if (av_probe_input_buffer(avio_ctx, &inputFormat, "", nullptr, 0, 0) < 0) {
            pthread_mutex_unlock(&player->m_playMutex);
            continue;
        }*/

        AVFormatContext* fmt_ctx = avformat_alloc_context();
        fmt_ctx->pb = avio_ctx;

        if (avformat_open_input(&fmt_ctx, "", inputFormat, nullptr) < 0) {
            avformat_close_input(&fmt_ctx);
            avformat_free_context(fmt_ctx);
            pthread_mutex_unlock(&player->m_playMutex);
            continue;
        }

        if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
            avformat_close_input(&fmt_ctx);
            avformat_free_context(fmt_ctx);
            pthread_mutex_unlock(&player->m_playMutex);
            continue;
        }

        int audio_stream = -1;

        for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
            if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audio_stream = i;
                break;
            }
        }

        if (audio_stream == -1) {
            avformat_close_input(&fmt_ctx);
            avformat_free_context(fmt_ctx);
            pthread_mutex_unlock(&player->m_playMutex);
            continue;
        }

        AVStream* a_stream = fmt_ctx->streams[audio_stream];
        AVCodecParameters* codec_par = a_stream->codecpar;
        AVCodec* codec = avcodec_find_decoder(codec_par->codec_id);
        AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);

        if (avcodec_parameters_to_context(codec_ctx, codec_par) < 0) {
            av_freep(&avio_ctx->buffer);
            av_freep(&avio_ctx);
            avcodec_free_context(&codec_ctx);
            avformat_close_input(&fmt_ctx);
            avformat_free_context(fmt_ctx);
            continue;
        }

        if (codec == nullptr) {
            av_freep(&avio_ctx->buffer);
            av_freep(&avio_ctx);
            avcodec_free_context(&codec_ctx);
            avformat_close_input(&fmt_ctx);
            avformat_free_context(fmt_ctx);
            continue;
        }

        if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
            av_freep(&avio_ctx->buffer);
            av_freep(&avio_ctx);
            avcodec_free_context(&codec_ctx);
            avformat_close_input(&fmt_ctx);
            avformat_free_context(fmt_ctx);
            pthread_mutex_unlock(&player->m_playMutex);
            continue;
        }

        unsigned int out_channels = 1;
        AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
        uint64_t out_channel_layout = AV_CH_LAYOUT_MONO;

        if (out_channels == 1) {
            out_channel_layout = AV_CH_LAYOUT_MONO;
        } else if (out_channels == 2) {
            out_channel_layout = AV_CH_LAYOUT_STEREO;
        } else {
        }

        int bytes_per_sample = av_get_bytes_per_sample(out_sample_fmt);
        int64_t in_channel_layout = av_get_default_channel_layout(codec_ctx->channels);

        struct SwrContext* au_convert_ctx = nullptr;
        au_convert_ctx = swr_alloc();
        au_convert_ctx = swr_alloc_set_opts(au_convert_ctx,
                                            out_channel_layout,
                                            out_sample_fmt,
                                            24000,
                                            in_channel_layout,
                                            codec_ctx->sample_fmt,
                                            codec_ctx->sample_rate,
                                            0,
                                            nullptr);
        swr_init(au_convert_ctx);

        APP_INFO("open stream success, begin to read frame.");
        AVPacket* packet = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();

        while (true) {
            memset(player->m_pcmResampleBuf, 0, RESAMPLE_BUFFER_SIZE);
            int ret = av_read_frame(fmt_ctx, packet);

            if (ret >= 0) {
                avcodec_send_packet(codec_ctx, packet);
                int error_code = avcodec_receive_frame(codec_ctx, frame);

                if (error_code == 0) {
                    int len = swr_convert(au_convert_ctx,
                                          &player->m_pcmResampleBuf,
                                          RESAMPLE_BUFFER_SIZE,
                                          (const uint8_t**) frame->data,
                                          frame->nb_samples);
                    unsigned int resampled_data_size = len * out_channels * bytes_per_sample;
                    player->play_stream(out_channels,
                                        player->m_pcmResampleBuf,
                                        resampled_data_size);
                } else {
                    if (error_code == AVERROR_EOF) {
                        APP_INFO("error code is eof, exit loop.");
                        av_frame_unref(frame);
                        av_packet_unref(packet);
                        break;
                    }
                }

                av_frame_unref(frame);
                av_packet_unref(packet);
            } else {
                break;
            }
        }

        APP_INFO("play stream finish, packet count(%d), wait for next play.",
                 player->m_packetCount);
        av_freep(&avio_ctx->buffer);
        av_freep(&avio_ctx);
        av_packet_free(&packet);
        av_frame_free(&frame);
        swr_free(&au_convert_ctx);
        avcodec_free_context(&codec_ctx);
        codec_ctx = nullptr;
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);

        if (!player->m_stopFlag) {
            player->executePlaybackStopped();
        }

        pthread_mutex_unlock(&player->m_playMutex);
    }

    return nullptr;
}

void Mp3TtsPlayerImpl::play_stream(unsigned int channels,
                                   const void* buffer,
                                   unsigned int buff_size) {
    if (m_isFirstPacket) {
        executePlaybackStarted();
        m_isFirstPacket = false;
    }

    int blankLen = 0;

    if (!m_removeHeadFlag) {
        for (size_t i = 0; i < buff_size; ++i) {
            if (((char*)buffer)[i] <= INVALID_VALUE_THRESHOLD) {
                blankLen++;
            } else {
                break;
            }
        }

        m_removeHeadFlag = !(buff_size - blankLen < 10);
    }

    m_packetCount++;
    m_alsaCtl->writeStream(channels, (char*)buffer + blankLen, buff_size - blankLen);
}

int Mp3TtsPlayerImpl::readPacket(void* opaque, uint8_t* buf, int buf_size) {
    auto instance = (Mp3TtsPlayerImpl*)opaque;

    while (true) {
        StreamItem* stream = instance->m_streamPool.popItem();

        if (stream != nullptr) {
            memcpy(buf, stream->getData(), stream->getSize());
            int size = stream->getSize();
            delete stream;
            return size;
        } else {
            if (instance->m_pushingFlag) {
                usleep(80000);
            } else {
                return FINISH_READ_FLAG;
            }
        }
    }
}

void Mp3TtsPlayerImpl::ffmpegLogOutput(void* ptr,
                                       int level,
                                       const char* fmt,
                                       va_list vl) {

}

}  // mediaPlayer
}  // duerOSDcsApp
