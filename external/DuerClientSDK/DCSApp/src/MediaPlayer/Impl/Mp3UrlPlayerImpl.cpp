/*
 * AudioPlayerImpl.cpp
 *
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

#include <LoggerUtils/DcsSdkLogger.h>
#include <DeviceTools/TimeUtils.h>
#include "Mp3UrlPlayerImpl.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
using application::ThreadPoolExecutor;

#define RESAMPLE_OUTPUT_BUFFER_SIZE 16384
#define PCM_PLAYBUFFER_SIZE 48000
#define PLAYFINISH_DELAY_TIME_MS 400000L
#define PLAY_LOOP_DELAY_TIME_MS 30000L
#define PAUSE_CHECK_INTERVAL_MS 200000L
#define BUFFER_CHECK_INTERVAL_MS 300000L
#define BUFFER_REFILL_MIN_SIZE 2
#define OUTPUT_SAMPLE_RATE 48000
#define NEAR_FINISH_TIME_LEFT 8000
#define MAX_RETRY_OPEN_TIME 3
#define TIMEOUT_MS 15000
#define DECODE_FINISH_TIME_THRESHOLD_MS 15000
#define RETRY_DECODE_TIMEINTERVAL_MS 200000
#define RETRY_INTERVAL 30
#define RETRY_OPEN_TIMEINTERVAL_MS 100000
#define MAX_RETRY_DECODE_NUM 8
#define RETRY_DECODE_TIME_UNIT 300000
#define WAIT_DATA_TIMEINTERVAL_MS 300000
#define MAX_REPORT_ERROR_TIME 3

std::shared_ptr<Mp3UrlPlayerImpl> Mp3UrlPlayerImpl::create(const std::string& audio_dev) {
    std::shared_ptr<Mp3UrlPlayerImpl> instance(new Mp3UrlPlayerImpl(audio_dev));
    return instance;
}

Mp3UrlPlayerImpl::Mp3UrlPlayerImpl(const std::string& audio_dev) : m_formatCtx(nullptr),
    m_codecCtx(nullptr),
    m_codecParam(nullptr),
    m_pcmBuffer(nullptr),
    m_frameBuffer(nullptr),
    m_alsaController(nullptr),
    m_alsaLock(nullptr),
    m_listener(nullptr),
    m_status(AUDIOPLAYER_STATUS_IDLE),
    m_decodeThread(0),
    m_playThread(0),
    m_progressMs(0),
    m_durationMs(0),
    m_bytesPersecond(0),
    m_reportIntervalMs(3000),
    m_startOffsetMs(0),
    m_frameTimestamp(0),
    m_outChannel(1),
    m_progressStartMs(0),
    m_factor(1),
    m_sampleRate(0),
    m_bytesPersample(0),
    m_seekable(false),
    m_pauseFlag(false),
    m_stopFlag(false),
    m_finishFlag(false),
    m_firstPackFlag(true),
    m_nearlyFinishFlag(false),
    m_shouldBreakFlag(false),
    m_threadAlive(true),
    m_pcmBufPool(NULL), 
    m_observer(NULL) {
#if defined (MTK8516) || defined (Rk3308)
    m_outChannel = 2;
#else
    m_outChannel = 1;
#endif

    m_alsaLock = new PthreadLock();
    m_pcmBufPool = new PcmBufPool();
    m_pcmBufPool->setStreamListener(this);
    m_executor = ThreadPoolExecutor::getInstance();
    m_alsaController = new AlsaController(audio_dev);
    m_alsaController->init(48000, m_outChannel);
    pthread_mutex_init(&m_decodeMutex, nullptr);
    pthread_mutex_init(&m_playMutex, nullptr);
    pthread_cond_init(&m_decodeCond, nullptr);
    pthread_cond_init(&m_playCond, nullptr);
    av_log_set_callback(ffmpegLogOutput);
    av_log_set_level(AV_LOG_INFO);
    av_register_all();
    avcodec_register_all();
    avformat_network_init();

    m_pcmBuffer = (uint8_t*) av_malloc(PCM_PLAYBUFFER_SIZE);
    m_frameBuffer = (uint8_t*) av_malloc(RESAMPLE_OUTPUT_BUFFER_SIZE);
    pthread_create(&m_decodeThread, nullptr, decodeFunc, this);
    pthread_create(&m_playThread, nullptr, playFunc, this);
}

Mp3UrlPlayerImpl::~Mp3UrlPlayerImpl() {
    m_threadAlive = false;
    m_stopFlag = true;

    pthread_cond_signal(&m_playCond);
    pthread_cond_signal(&m_decodeCond);

    void* play_thread_return = nullptr;

    if (m_playThread != 0) {
        pthread_join(m_playThread, &play_thread_return);
    }

    void* decode_thread_return = nullptr;

    if (m_decodeThread != 0) {
        pthread_join(m_decodeThread, &decode_thread_return);
    }

    if (m_pcmBuffer != nullptr) {
        av_free(m_pcmBuffer);
        m_pcmBuffer = nullptr;
    }

    if (m_frameBuffer != nullptr) {
        av_free(m_frameBuffer);
        m_frameBuffer = nullptr;
    }

    if (m_pcmBufPool) {
        delete m_pcmBufPool;
        m_pcmBufPool = NULL;
    }

    if (m_alsaLock) {
        delete m_alsaLock;
        m_alsaLock = NULL;
    }

    m_alsaController->aslaAbort();
    m_alsaController->alsaClose();
    delete m_alsaController;
    m_alsaController = nullptr;

    pthread_mutex_destroy(&m_decodeMutex);
    pthread_mutex_destroy(&m_playMutex);
    pthread_cond_destroy(&m_decodeCond);
    pthread_cond_destroy(&m_playCond);
    m_status = AUDIOPLAYER_STATUS_IDLE;
}

void Mp3UrlPlayerImpl::setPlayerActivity(PlayerAvtivityObserver* observer) {
    m_observer = observer;
}

void Mp3UrlPlayerImpl::registerListener(std::shared_ptr<AudioPlayerListener> notify) {
    m_listener = notify;
}

void* Mp3UrlPlayerImpl::decodeFunc(void* arg) {
    auto player = (Mp3UrlPlayerImpl*)arg;

    while (player->m_threadAlive) {
        pthread_mutex_lock(&player->m_decodeMutex);

        while (player->m_url.empty()) {
            pthread_cond_wait(&player->m_decodeCond, &player->m_decodeMutex);

            if (!player->m_threadAlive) {
                return nullptr;
            }
        }

        player->m_alsaController->alsaPrepare();

        //Open the url
        APP_INFO("==open input");
        int open_ret = 0;
        unsigned int l_try_open_count = 0;
        bool l_open_success_flag = false;

        while (l_try_open_count++ < MAX_RETRY_OPEN_TIME) {
            player->m_formatCtx = avformat_alloc_context();
            player->m_formatCtx->interrupt_callback.callback = interruptCallback;
            player->m_formatCtx->interrupt_callback.opaque = player;
            open_ret = avformat_open_input(&player->m_formatCtx,
                                           player->m_url.c_str(),
                                           nullptr,
                                           nullptr);

            if (open_ret == 0) {
                l_open_success_flag = true;
                break;
            } else {
                player->freeFormatContext();
            }
        }

        if (!l_open_success_flag) {
            player->freeFormatContext();
            player->m_url = "";
            player->m_pcmBufPool->setEndFlag(true);
            pthread_mutex_unlock(&player->m_decodeMutex);
            APP_INFO("AudioPlayerImpl open url failed.");
            player->executePlaybackError();
            continue;
        }

        if (avformat_find_stream_info(player->m_formatCtx, nullptr) < 0) {
            player->freeFormatContext();
            player->m_url = "";
            player->m_pcmBufPool->setEndFlag(true);
            pthread_mutex_unlock(&player->m_decodeMutex);
            APP_INFO("AudioPlayerImpl find stream info failed.");
            player->executePlaybackError();
            continue;
        }

        av_dump_format(player->m_formatCtx, 0, player->m_url.c_str(), false);
        player->m_durationMs = (player->m_formatCtx->duration) / 1000;
        player->m_seekable = (player->m_durationMs > 0);

        int audio_stream = -1;

        for (unsigned int i = 0; i < player->m_formatCtx->nb_streams; i++) {
            if (player->m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
                audio_stream = i;
                break;
            }
        }

        if (audio_stream == -1) {
            player->freeFormatContext();
            player->m_url = "";
            player->m_pcmBufPool->setEndFlag(true);
            pthread_mutex_unlock(&player->m_decodeMutex);
            APP_INFO("AudioPlayerImpl find audio stream failed.");
            player->executePlaybackError();
            continue;
        }

        AVStream* a_stream = player->m_formatCtx->streams[audio_stream];
        player->m_codecParam = a_stream->codecpar;
        AVCodec* codec = avcodec_find_decoder(player->m_codecParam->codec_id);
        player->m_codecCtx = avcodec_alloc_context3(codec);
        int error_code = 0;

        if ((error_code = avcodec_parameters_to_context(player->m_codecCtx, player->m_codecParam)) < 0) {
            player->freeCodecContext();
            player->freeFormatContext();
            player->m_url = "";
            player->m_pcmBufPool->setEndFlag(true);
            pthread_mutex_unlock(&player->m_decodeMutex);
            player->executePlaybackError();
            continue;
        }

        if (avcodec_open2(player->m_codecCtx, codec, nullptr) < 0) {
            player->freeCodecContext();
            player->freeFormatContext();
            player->m_url = "";
            player->m_pcmBufPool->setEndFlag(true);
            pthread_mutex_unlock(&player->m_decodeMutex);
            player->executePlaybackError();
            continue;
        }

        player->m_sampleRate = player->m_codecCtx->sample_rate;
        player->m_bytesPersample = av_get_bytes_per_sample(player->m_codecCtx->sample_fmt);
        player->m_bytesPersecond = OUTPUT_SAMPLE_RATE * player->m_outChannel * 2;

        unsigned int out_channels = player->m_outChannel;
        AVSampleFormat out_sample_fmt = AV_SAMPLE_FMT_S16;
        uint64_t out_channel_layout = AV_CH_LAYOUT_MONO;

        if (out_channels == 1) {
            out_channel_layout = AV_CH_LAYOUT_MONO;
        } else if (out_channels == 2) {
            out_channel_layout = AV_CH_LAYOUT_STEREO;
        } else {
        }

        int bytes_per_sample = av_get_bytes_per_sample(out_sample_fmt);
        int64_t in_channel_layout = av_get_default_channel_layout(player->m_codecCtx->channels);

        struct SwrContext* convert_ctx = swr_alloc();
        convert_ctx = swr_alloc_set_opts(convert_ctx,
                                         out_channel_layout,
                                         out_sample_fmt,
                                         OUTPUT_SAMPLE_RATE,
                                         in_channel_layout,
                                         player->m_codecCtx->sample_fmt,
                                         player->m_codecCtx->sample_rate,
                                         0,
                                         nullptr);
        swr_init(convert_ctx);

        if (player->m_seekable && player->m_progressMs != 0) {
            //seek to position
            long int seek_pos = 0;
            //player->_progress_ms = player->_progress_ms > 0 ? player->_progress_ms : 3000;
            seek_pos = player->m_progressMs / 1000 * AV_TIME_BASE;

            if (player->m_formatCtx->start_time != AV_NOPTS_VALUE) {
                seek_pos += player->m_formatCtx->start_time;
            }

            if (av_seek_frame(player->m_formatCtx, -1, seek_pos, AVSEEK_FLAG_BACKWARD) < 0) {
                player->freeCodecContext();
                player->freeFormatContext();
                swr_free(&convert_ctx);
                player->m_url = "";
                player->m_pcmBufPool->setEndFlag(true);
                pthread_mutex_unlock(&player->m_decodeMutex);
                player->executePlaybackError();
                continue;
            }
        }

        APP_INFO("==decode begin");
        int pos = 0;
        int readRetryCount = 0;
        AVPacket* packet = av_packet_alloc();
        AVFrame* frame = av_frame_alloc();

        while (!player->m_stopFlag) {
            if (player->m_pauseFlag) {
                usleep(PAUSE_CHECK_INTERVAL_MS);
                continue;
            }

            player->m_frameTimestamp = deviceCommonLib::deviceTools::currentTimeMs();
            int ret = av_read_frame(player->m_formatCtx, packet);
            if (packet->stream_index != audio_stream) {
                av_packet_unref(packet);
                continue;
            }
            if (ret >= 0) {
                readRetryCount = 0;
                avcodec_send_packet(player->m_codecCtx, packet);
                error_code = avcodec_receive_frame(player->m_codecCtx, frame);

                if (error_code == 0) {
                    uint8_t* target_ptr = player->m_pcmBuffer + pos;
                    memset(player->m_frameBuffer, 0, RESAMPLE_OUTPUT_BUFFER_SIZE);
                    int len = swr_convert(convert_ctx,
                                          &player->m_frameBuffer,
                                          RESAMPLE_OUTPUT_BUFFER_SIZE,
                                          (const uint8_t**) frame->data,
                                          frame->nb_samples);
                    unsigned int resample_buffer_size = len * out_channels * bytes_per_sample;

                    if (pos + resample_buffer_size >= PCM_PLAYBUFFER_SIZE) {
                        //APP_INFO("decodeFunc player->m_progressMs:%d", player->m_progressMs);
                        //pcm_pool->pushPcmChunk(player->m_pcmBuffer, pos, offset_ms);
                        player->m_pcmBufPool->pushPcmChunk(player->m_pcmBuffer, pos, 0);
                        memset(player->m_pcmBuffer, 0, PCM_PLAYBUFFER_SIZE);
                        pos = 0;
                        target_ptr = player->m_pcmBuffer;
                        memcpy(target_ptr, player->m_frameBuffer, resample_buffer_size);
                        pos += resample_buffer_size;
                    } else {
                        memcpy(target_ptr, player->m_frameBuffer, resample_buffer_size);
                        pos += resample_buffer_size;
                    }
                } else {
                    if (error_code == AVERROR_EOF) {
                        av_frame_unref(frame);
                        av_packet_unref(packet);
                        APP_INFO("==eof, decode finish.");
                        break;
                    }
                }

                av_frame_unref(frame);
                av_packet_unref(packet);
            } else {
                //if (ret == AVERROR_EOF || player->_should_break_flag) {
                if (ret == AVERROR_EOF) {
                    int timeDiff = player->m_durationMs - player->m_progressMs;

                    if (timeDiff >= DECODE_FINISH_TIME_THRESHOLD_MS) {
                        APP_INFO("==decode not finish(%d/%d), seek to target pos.",
                                 player->m_progressMs, player->m_durationMs);
                        unsigned int retryCount = 0;
                        bool retryRet = false;

                        while (retryCount++ < MAX_RETRY_DECODE_NUM) {
                            usleep((retryCount + 1) * RETRY_DECODE_TIME_UNIT);
                            retryRet = player->retryDecode();

                            if (retryRet) {
                                break;
                            }
                        }

                        if (retryRet) {
                            APP_INFO("==retry to decode success.");
                            continue;
                        } else {
                            APP_INFO("==retry to decode failed. decode finish.");
                            break;
                        }
                    }

                    APP_INFO("==decode finish.");
                    break;
                } else {
                    usleep(RETRY_DECODE_TIMEINTERVAL_MS);

                    if (readRetryCount >= RETRY_INTERVAL) {
                        APP_INFO("==retry to decode begin.");

                        while (!player->m_stopFlag && !player->retryDecode()) {
                            usleep(RETRY_OPEN_TIMEINTERVAL_MS);
                        }

                        readRetryCount = 0;
                        APP_INFO("==retry to decode end.");
                    }

                    readRetryCount++;
                    continue;
                }
            }
        }

        player->m_url = "";
        av_packet_free(&packet);
        av_frame_free(&frame);
        swr_free(&convert_ctx);
        player->freeCodecContext();
        player->freeFormatContext();

        player->m_pcmBufPool->setEndFlag(true);
        pthread_mutex_unlock(&player->m_decodeMutex);
    }

    return nullptr;
}

bool Mp3UrlPlayerImpl::retryDecode() {
    freeCodecContext();
    freeFormatContext();
    m_formatCtx = avformat_alloc_context();
    m_formatCtx->interrupt_callback.callback = interruptCallback;
    m_formatCtx->interrupt_callback.opaque = this;
    int open_ret = avformat_open_input(&m_formatCtx, m_url.c_str(), nullptr, nullptr);

    if (open_ret < 0) {
        freeFormatContext();
        return false;
    }

    if (avformat_find_stream_info(m_formatCtx, nullptr) < 0) {
        freeFormatContext();
        return false;
    }

    av_dump_format(m_formatCtx, 0, m_url.c_str(), false);
    m_durationMs = (m_formatCtx->duration) / 1000;
    m_seekable = (m_durationMs > 0);

    int audio_stream = -1;

    for (unsigned int i = 0; i < m_formatCtx->nb_streams; i++) {
        if (m_formatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            audio_stream = i;
            break;
        }
    }

    if (audio_stream == -1) {
        freeFormatContext();
        return false;
    }

    AVStream* a_stream = m_formatCtx->streams[audio_stream];
    m_codecParam = a_stream->codecpar;
    AVCodec* codec = avcodec_find_decoder(m_codecParam->codec_id);
    m_codecCtx = avcodec_alloc_context3(codec);
    int error_code = 0;

    if ((error_code = avcodec_parameters_to_context(m_codecCtx, m_codecParam)) < 0) {
        freeCodecContext();
        freeFormatContext();
        return false;
    }

    if (avcodec_open2(m_codecCtx, codec, nullptr) < 0) {
        freeCodecContext();
        freeFormatContext();
        return false;
    }

    m_sampleRate = m_codecCtx->sample_rate;
    m_bytesPersample = av_get_bytes_per_sample(m_codecCtx->sample_fmt);
    m_bytesPersecond = OUTPUT_SAMPLE_RATE * m_outChannel * 2;

    if (m_seekable && m_progressMs != 0) {
        long int seek_pos = m_progressMs / 1000 * AV_TIME_BASE;

        if (m_formatCtx->start_time != AV_NOPTS_VALUE) {
            seek_pos += m_formatCtx->start_time;
        }

        if (av_seek_frame(m_formatCtx, -1, seek_pos, AVSEEK_FLAG_BACKWARD) < 0) {
            freeCodecContext();
            freeFormatContext();
            return false;
        }
    }

    return true;
}

void Mp3UrlPlayerImpl::freeFormatContext() {
    if (m_formatCtx) {
        avformat_close_input(&m_formatCtx);
        avformat_free_context(m_formatCtx);
        m_formatCtx = nullptr;
    }
}

void Mp3UrlPlayerImpl::freeCodecContext() {
    if (m_codecCtx) {
        avcodec_free_context(&m_codecCtx);
        m_codecCtx = nullptr;
    }
}

void Mp3UrlPlayerImpl::handleHungryState() {

    if (m_pcmBufPool->isDirty()) {
        int waitCounter = 0;
        APP_INFO("===wait for audio data");

        while (true) {
            usleep(WAIT_DATA_TIMEINTERVAL_MS);
            ++waitCounter;

            if (m_pcmBufPool->size() != 0) {
                APP_INFO("===return from underbuffer");
                return;
            }

            if (waitCounter >= 6) {
                break;
            }
        }

        APP_INFO("===not data, audio player underbuffer");
        executePlaybackUnderbuffer(false);

        while (true) {
            usleep(BUFFER_CHECK_INTERVAL_MS);

            if (m_stopFlag || m_pcmBufPool->getEndFlag()) {
                break;
            }

            if (m_pcmBufPool->size() > BUFFER_REFILL_MIN_SIZE) {
                APP_INFO("===audio player bufferrefilled");
                executePlaybackBufferrefill();
                break;
            }
        }
    }
}

void* Mp3UrlPlayerImpl::playFunc(void* arg) {
    auto player = (Mp3UrlPlayerImpl*)arg;
    bool shouldReportFinishEvent = false;

    while (player->m_threadAlive) {
        pthread_mutex_lock(&player->m_playMutex);

        while (player->m_url.empty()) {
            pthread_cond_wait(&player->m_playCond, &player->m_playMutex);

            if (!player->m_threadAlive) {
                return nullptr;
            }
        }

        shouldReportFinishEvent = false;

        while (!player->m_stopFlag) {
            if (!player->m_pauseFlag && !player->m_finishFlag) {
                PcmChunk* chunk = player->m_pcmBufPool->popPcmChunk();

                if (chunk != nullptr && chunk->dat != nullptr) {
                    if (player->m_factor != 1) {
                        player->sigScale(chunk->dat, chunk->len, player->m_factor);
                    }

                    player->pushStream(player->m_outChannel, chunk->dat, chunk->len);
                    av_free(chunk->dat);
                    chunk->dat = nullptr;
                    av_free(chunk);
                } else {
                    if (player->m_pcmBufPool->getEndFlag()) {
                        usleep(PLAYFINISH_DELAY_TIME_MS);
                        player->m_status = AUDIOPLAYER_STATUS_FINISHED;
                        player->m_finishFlag = true;

                        shouldReportFinishEvent = true;
                        break;
                    } else {
                        player->handleHungryState();
                    }
                }
            }

            usleep(PLAY_LOOP_DELAY_TIME_MS);
        }

        pthread_mutex_unlock(&player->m_playMutex);

        if (shouldReportFinishEvent) {
            player->executePlaybackFinished();
        }
    }

    return nullptr;
}

void Mp3UrlPlayerImpl::audioPlay(const std::string& url,
                                 unsigned int offset,
                                 unsigned int report_interval) {
    APP_INFO("audioPlay called, call stop first");
    audioStop();
    APP_INFO("audioPlay called");
    pthread_mutex_lock(&m_decodeMutex);
    pthread_mutex_lock(&m_playMutex);
    m_pcmBufPool->clear();
    m_startOffsetMs = offset;
    m_progressMs = m_startOffsetMs;

    int64_t current_time = deviceCommonLib::deviceTools::currentTimeMs();
    m_progressStartMs = current_time;
    m_frameTimestamp = current_time;
    m_reportIntervalMs = report_interval;
    m_seekable = false;
    m_pauseFlag = false;
    m_finishFlag = false;
    m_shouldBreakFlag = false;
    m_firstPackFlag = true;
    m_durationMs = 0;
    m_stopFlag = false;
    m_url = url;
    m_progressMs = offset;
    m_nearlyFinishFlag = false;
    m_status = AUDIOPLAYER_STATUS_PLAY;
    executePlaybackUnderbuffer(true);
    pthread_cond_signal(&m_playCond);
    pthread_cond_signal(&m_decodeCond);
    pthread_mutex_unlock(&m_playMutex);
    pthread_mutex_unlock(&m_decodeMutex);
    APP_INFO("audioPlay called finish");
}

void Mp3UrlPlayerImpl::pushStream(unsigned int channels,
                                  const void* buffer,
                                  unsigned long buff_size) {
    AutoLock autoLock(m_alsaLock);
    if (m_pauseFlag || m_stopFlag) {
        return;
    }

    if (m_firstPackFlag) {
        executePlaybackBufferrefill();
        executePlaybackStarted();
        m_firstPackFlag = false;
    }

    m_alsaController->writeStream(channels, buffer, buff_size);
    //  APP_INFO("AudioPlayerImpl::pushStream m_progressMs1:%d, buff_size:%ld, m_bytesPersecond:%d", m_progressMs, buff_size, m_bytesPersecond);
    m_progressMs += (unsigned long)((buff_size * 1000.0f) / m_bytesPersecond);
    //    APP_INFO("AudioPlayerImpl::pushStream end m_progressMs:%d", m_progressMs);
    int64_t current_time = deviceCommonLib::deviceTools::currentTimeMs();

    if (m_listener != nullptr && current_time - m_progressStartMs >= m_reportIntervalMs) {
        m_listener->playbackProgress(m_progressMs);
        m_progressStartMs = current_time;
    }

    if (!m_nearlyFinishFlag && (m_durationMs - m_progressMs <= NEAR_FINISH_TIME_LEFT)) {
        m_nearlyFinishFlag = true;
        executePlaybackNearlyFinished();
    }
}

void Mp3UrlPlayerImpl::audioStop() {
    APP_INFO("audioStop called");

    if (m_status == AUDIOPLAYER_STATUS_IDLE || m_status == AUDIOPLAYER_STATUS_FINISHED) {
        APP_INFO("audioStop return, status is idle");
        return;
    }

    m_stopFlag = true;
    m_pcmBufPool->clear();
    pthread_mutex_lock(&m_decodeMutex);
    pthread_mutex_lock(&m_playMutex);

    if (!m_alsaController->isAccessable()) {
        freeCodecContext();
        freeFormatContext();
        pthread_mutex_unlock(&m_playMutex);
        pthread_mutex_unlock(&m_decodeMutex);
        APP_INFO("audioStop return, alsa is not accessable");
        return;
    }

    freeCodecContext();
    freeFormatContext();

    m_alsaController->alsaClear();
    m_status = AUDIOPLAYER_STATUS_IDLE;
    executePlaybackStopped();

    pthread_mutex_unlock(&m_playMutex);
    pthread_mutex_unlock(&m_decodeMutex);
    APP_INFO("audioStop called finish");
}

void Mp3UrlPlayerImpl::audioPause() {
    AutoLock autoLock(m_alsaLock);

    if (m_status == AUDIOPLAYER_STATUS_IDLE || m_status == AUDIOPLAYER_STATUS_FINISHED) {
        return;
    }

    if (!m_alsaController->isAccessable()) {
        return;
    }

    m_pauseFlag = true;
    if (m_status != AUDIOPLAYER_STATUS_STOP) {
        if (!m_alsaController->alsaPause()) {
            return;
        }
    }

    executePlaybackPaused();
    m_status = AUDIOPLAYER_STATUS_STOP;
}

void Mp3UrlPlayerImpl::audioResume() {
    if (m_status == AUDIOPLAYER_STATUS_IDLE || m_status == AUDIOPLAYER_STATUS_FINISHED) {
        return;
    }

    if (!m_alsaController->isAccessable()) {
        return;
    }

    m_pauseFlag = false;

    if (!m_alsaController->alsaResume()) {
        return;
    }

    executePlaybackResumed();
    m_status = AUDIOPLAYER_STATUS_PLAY;
}

bool Mp3UrlPlayerImpl::seekTo(unsigned int positionMs) {
    APP_INFO("Mp3UrlPlayerImpl::seekTo begin");

    if (m_status == AUDIOPLAYER_STATUS_IDLE || m_status == AUDIOPLAYER_STATUS_FINISHED) {
        APP_INFO("Mp3UrlPlayerImpl::seekTo m_status %d", m_status);
        return false;
    }

    if (!m_seekable || m_formatCtx == nullptr) {
        APP_INFO("Mp3UrlPlayerImpl::seekTo return");
        return false;
    }

    AutoLock autoLock(m_alsaLock);
    m_pauseFlag = true;
    m_pcmBufPool->clear();
    usleep(30000);
    APP_INFO("seekTo m_pauseFlag true");

    //seek to target position.
    long int seek_pos = positionMs * 1000;

    if (m_formatCtx->start_time != AV_NOPTS_VALUE) {
        seek_pos += m_formatCtx->start_time;
    }

    if (av_seek_frame(m_formatCtx, -1, seek_pos, AVSEEK_FLAG_BACKWARD) < 0) {
        m_pauseFlag = false;
        APP_INFO("Mp3UrlPlayerImpl::seekTo av_seek_frame return");
        return false;
    }

    m_progressMs = seek_pos / 1000;
    //  APP_INFO("Mp3UrlPlayerImpl::seekTo m_progressMs:%d, positionMs:%d, seek_pos:%ld", m_progressMs, positionMs, seek_pos);
    //  usleep(80000);
    m_pcmBufPool->clear();
    m_pauseFlag = false;
    APP_INFO("seekTo m_pauseFlag false");
    return true;
}

bool Mp3UrlPlayerImpl::seekBy(unsigned int offsetMs) {
    if (m_status == AUDIOPLAYER_STATUS_IDLE || m_status == AUDIOPLAYER_STATUS_FINISHED) {
        return false;
    }

    if (!m_seekable || m_formatCtx == nullptr) {
        return false;
    }

    //seek to target position.
    long int seek_pos = (m_progressMs + offsetMs) / 1000 * AV_TIME_BASE;

    if (m_formatCtx->start_time != AV_NOPTS_VALUE) {
        seek_pos += m_formatCtx->start_time;
    }

    if (av_seek_frame(m_formatCtx, -1, seek_pos, AVSEEK_FLAG_BACKWARD) < 0) {
        return false;
    }

    return true;
}

unsigned long Mp3UrlPlayerImpl::getProgress() {
    return m_progressMs;
}

unsigned long Mp3UrlPlayerImpl::getDuration() {
    return m_durationMs;
}

void Mp3UrlPlayerImpl::setFactor(float factor) {
    APP_INFO("AudioPlayerImpl::setFactor %f", factor);
    m_factor = factor;
}

float Mp3UrlPlayerImpl::getFactor() {
    APP_INFO("Mp3UrlPlayerImpl::getFactor %f", m_factor);
    return m_factor;
}

void Mp3UrlPlayerImpl::sigScale(unsigned char* buff, unsigned int len, float factor) {
    if (NULL == buff) {
        return;
    }

    //APP_INFO("Mp3UrlPlayerImpl::sig_scale %f", factor);
    unsigned char* ptr = NULL;
    unsigned int i = 0;

    while (i < len) {
        ptr = buff + i;
        short* sample_num = (short*)ptr;
        *sample_num = (short)(*sample_num * factor);
        i += 2;
    }
}

void Mp3UrlPlayerImpl::ffmpegLogOutput(void* ptr,
                                       int level,
                                       const char* fmt,
                                       va_list vl) {
}

int Mp3UrlPlayerImpl::interruptCallback(void* ctx) {
    auto this_ptr = (Mp3UrlPlayerImpl*)ctx;
    int64_t time_diff = deviceCommonLib::deviceTools::currentTimeMs() - this_ptr->m_frameTimestamp;

    if (this_ptr->m_stopFlag) {
        return -1;
    } else if (time_diff >= TIMEOUT_MS) {
        this_ptr->m_shouldBreakFlag = true;
        return -1;
    } else {
        return 0;
    }
}

void Mp3UrlPlayerImpl::executePlaybackStarted() {
    if (m_observer) {
        m_observer->playStart(DeviceIoWrapper::getInstance()->getCurrentVolume());
    }

    m_playbackFailedTimes = 0;
    m_executor->submit(
    [this]() {
        if (m_listener != nullptr) {
            m_listener->playbackStarted(m_startOffsetMs);
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackUnderbuffer(bool beforeStart) {
    if (m_observer) {
        m_observer->playStart(0);
    }

    m_executor->submit(
    [this, beforeStart]() {
        PlayProgressInfo playProgressInfo;

        if (beforeStart) {
            playProgressInfo = PlayProgressInfo::BEFORE_START;
        } else {
            playProgressInfo = PlayProgressInfo::DURING_PLAY;
        }

        if (m_listener != nullptr) {
            m_listener->playbackBufferunderrun(playProgressInfo);
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackBufferrefill() {
    if (m_observer) {
        m_observer->playStart(DeviceIoWrapper::getInstance()->getCurrentVolume());
    }

    m_executor->submit(
    [this]() {
        if (m_listener != nullptr) {
            m_listener->playbackBufferefilled();
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackError() {
    if (m_observer) {
        m_observer->playStart(0);
    }

    if (m_stopFlag) {
        APP_INFO("stop called, cancel report.");
        return;
    }
    if (m_playbackFailedTimes >= MAX_REPORT_ERROR_TIME) {
        APP_INFO("error report time is enough, cancel report.");
        return;
    }
    m_playbackFailedTimes++;
    m_executor->submit(
    [this]() {
        if (m_listener != nullptr) {
            m_listener->playbackError();
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackStopped() {
    if (m_observer) {
        m_observer->playStart(0);
    }

    m_executor->submit(
    [this]() {
        if (m_listener != nullptr) {
            m_listener->playbackStopped(m_progressMs);
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackPaused() {
    if (m_observer) {
        m_observer->playStart(0);
    }

    m_executor->submit(
    [this]() {
        if (m_listener != nullptr) {
            m_listener->playbackPaused(m_progressMs);
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackResumed() {
    if (m_observer) {
        m_observer->playStart(DeviceIoWrapper::getInstance()->getCurrentVolume());
    }

    m_executor->submit(
    [this]() {
        if (m_listener != nullptr) {
            m_listener->playbackResumed(m_startOffsetMs);
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackFinished() {
    if (m_observer) {
        m_observer->playStart(0);
    }
    m_executor->submit(
    [this]() {
        if (nullptr != m_listener) {
            m_listener->playbackFinished(m_progressMs, AudioPlayerFinishStatus::AUDIOPLAYER_FINISHSTATUS_END);
        }
    }
    );
}

void Mp3UrlPlayerImpl::executePlaybackNearlyFinished() {
    m_executor->submit(
    [this]() {
        if (nullptr != m_listener) {
            m_listener->playbackNearlyFinished(m_progressMs);
        }
    }
    );
}

void Mp3UrlPlayerImpl::onStreamInterrupt() {
    m_executor->submit(
    [this]() {
        if (nullptr != m_listener) {
            m_listener->playbackStreamUnreach();
        }
    }
    );
}

}  // mediaPlayer
}  // duerOSDcsApp
