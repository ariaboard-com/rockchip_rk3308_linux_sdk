/* =========================================================================

   DESCRIPTION
   iflytek SDK Demo

   Copyright (c) 2016 by iFLYTEK, Co,LTD.  All Rights Reserved.
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alsa/asoundlib.h>
#include <thread>

#include "cae_intf.h"
#include "cae_lib.h"
#include "cae_errors.h"

#include "constant.h"
#include "info_led.h"
#include "wpa_manager.h"
#include "audio_process.h"
#include "log.h"
#include "gst_player.h"
#include "msc_iat.h"
#include "cmcc.h"
#include "migu.h"
#include "keyevent_handler.h"

//#define SAVE_CAE_OUT_PCM 1 
struct CMCC_MUSIC_LIST *cmcc_music_list;
struct CMCC_READBOOK_INFO *cmcc_readbook_info;
static int initCaeFuncs()
{
    int ret = MSP_SUCCESS;
    void* hInstance = cae_LoadLibrary(Constant::kCaeLibPATH);

    if(hInstance == NULL)
    {
        Log::error("load cae library failed with path: %s\n", Constant::kCaeLibPATH);
        return MSP_ERROR_OPEN_FILE;
    }

    api_cae_new = (Proc_CAENew)cae_GetProcAddress(hInstance, "CAENew");
    api_cae_audio_write = 
        (Proc_CAEAudioWrite)cae_GetProcAddress(hInstance, "CAEAudioWrite");
    api_cae_reset_eng = 
        (Proc_CAEResetEng)cae_GetProcAddress(hInstance, "CAEResetEng");
    api_cae_set_real_beam = 
        (Proc_CAESetRealBeam)cae_GetProcAddress(hInstance, "CAESetRealBeam");
    api_cae_set_wparam = 
        (Proc_CAESetWParam)cae_GetProcAddress(hInstance, "CAESetWParam");
    api_cae_get_version = 
        (Proc_CAEGetVersion)cae_GetProcAddress(hInstance, "CAEGetVersion");
    api_cae_get_channel= 
        (Proc_CAEGetChannel)cae_GetProcAddress(hInstance, "CAEGetChannel");
    api_cae_destroy = 
        (Proc_CAEDestroy)cae_GetProcAddress(hInstance, "CAEDestroy");

    return ret;
}

static void CAEIvwCb(short angle, short channel, float power,
                     short CMScore, short beam, char *param1,
                     void *param2, void *userData)
{
    Log::debug("CAEIvwCb .... angle:%d, channel:%d, power:%f, CMScore: %d, beam: %d\n",angle, channel, power, CMScore, beam);

    CAEUserData *usDta = (CAEUserData*)userData;

    // Check network status
    //if(WPAManager::getInstance()->isNetConnected())
    if(true)
    {
        usDta->wakeUping = true;
        usDta->newSession = true;
        IatProcess::endQISRSession(AudioProcess::getIatSessionId());

        if (!usDta->wakeUpState) {
            usDta->wakeUpState = true;
        }

        if (usDta->mscInProcess) {
            Log::debug("===> wake up suspendFlag\n");
            usDta->suspendFlag = true;
            while(1) {
                if (!usDta->suspendFlag) {
                    break;
                }
                usleep(50 * 1000);
                Log::debug("===>wainting for tts return\n");
            }
        }

        // Led_Style show
        //usDta->infoled->leds_multi_all_blink(LED_MULTI_PURE_COLOR_BLUE, 500, 500);

        if (get_music_state(gst_player_url) == GST_STATE_PLAYING) {
            pause_music_playing(gst_player_url);
            Log::info("----pause_music_playing---\n");
            usleep(100 * 1000);
        }

        play_alert_file(Constant::kCaeWakeupWorkds, true);
        usleep(200 * 1000);
        usDta->wakeUping = false;
    }
    else
    {
        // TODO Network disconnected
    }
}

static void CAEAudioCb(const void *audioData, unsigned int audioLen,
                       int param1, const void *param2, void *userData)
{
    CAEUserData *usDta = (CAEUserData*)userData;
    bool audioPadding;

    if(usDta->wakeUping) {
        Log::debug("---CAEAudioCb waiting for wakeup word finished---\n");
        return;
    }

    if (usDta->wakeUpState && !usDta->suspendFlag) {
        if (get_music_state(gst_player_url) == GST_STATE_PLAYING) {
            pause_music_playing(gst_player_url);
            Log::info("----pause_music_playing---\n");
            return;
        }
        audioPadding = AudioProcess::paddingIatAudio((char*)audioData, audioLen, usDta);

        if(!audioPadding)   //已经查询到结果
        {
            usDta->wakeUpState = false;
            //usDta->infoled->leds_multi_all_off();
        }

        usDta->newSession = false;
    }
}

void alsa_open(snd_pcm_t** capture_handle, int channels,
               uint32_t rate, snd_pcm_format_t format)
{
    snd_pcm_hw_params_t *hw_params;
    int err;

    if ((err = snd_pcm_open(capture_handle, "6mic_loopback", /*"default"*/
                            SND_PCM_STREAM_CAPTURE, 0)) < 0)
    {
        fprintf(stderr, "cannot open audio device %s\n",
                snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params_malloc(&hw_params)) < 0)
    {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params_any(*capture_handle, hw_params)) < 0)
    {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_access(*capture_handle, hw_params,
                                            SND_PCM_ACCESS_RW_INTERLEAVED)) < 0)
    {
        fprintf(stderr, "cannot set access type (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_format(*capture_handle,
                                            hw_params, format)) < 0)
    {
        fprintf(stderr, "cannot set sample format (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_channels(*capture_handle,
                                              hw_params, channels)) < 0)
    {
        fprintf(stderr, "cannot set channel count (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_rate_near(*capture_handle,
                                               hw_params, &rate, 0)) < 0)
    {
        fprintf(stderr, "cannot set sample rate (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    snd_pcm_uframes_t period_frames = 512;
    if((err = snd_pcm_hw_params_set_period_size_near(*capture_handle,
              hw_params, &period_frames, 0)) < 0)
    {
        fprintf(stderr, "cannot set sample rate (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if((err = snd_pcm_hw_params(*capture_handle, hw_params)) < 0)
    {
        fprintf(stderr, "cannot set parameters (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    snd_pcm_hw_params_free(hw_params);
}

int preProcessBuffer(void *data, void *out, int bytes)
{
    int channels = 8;
    int is24bitMode = 0;
    int i = 0, j = 0;

    if(!is24bitMode)
    {
        for(i = 0; i < bytes / 2;)
        {
            for(j = 0; j < channels; j++)
            {
                int tmp = 0;
                short tmp_data = (*((short *)data + i + j));
                /*if(j < 6)   //normal data
                {
                    tmp_data = tmp_data << 3 ;
                }*/
                tmp = ((tmp_data) << 16 | ((j + 1) << 8)) & 0xffffff00;
                *((int *)out + i + j) = tmp;
            }
            i += channels;
        }
    }
    else
    {
        for(i = 0; i < bytes / 4;)
        {
            for(j = 0; j < channels; j++)
            {
                int tmp = 0;
                int tmp_data = (*((int *)data + i + j)) << 1;
                tmp = ((((tmp_data & 0xfffff000) >> 12) << 12) | ((j+1) << 8)) & 0xffffff00;
                *((int *)out + i + j) = tmp;
            }

            i += channels;
        }
    }

    return 0;
}

/**
 * CAEAudioWrite
 *
 * @brief caeAudioWrite
 * @param cae
 */
void caeAudioWrite(CAE_HANDLE cae,void *userData)
{
    int capture_len = 1024, channels = 8, err;
    size_t BUFSIZE = (int)(capture_len * snd_pcm_format_width(SND_PCM_FORMAT_S16_LE) / 8 * channels);
    char buffer[BUFSIZE];

    snd_pcm_t *capture_handle;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    unsigned int rate = 16000;
    int frame_len;
    size_t buffer_len;
    char cae_audio_data[BUFSIZE * 2];
    CAEUserData *usDta = (CAEUserData*)userData;


    alsa_open(&capture_handle, channels, rate, format);
    if((err = snd_pcm_prepare(capture_handle)) < 0)
    {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    while((frame_len = snd_pcm_readi(capture_handle, buffer, capture_len)) >= 0)
    {
        if(usDta->wakeUping) {
            Log::debug("---caeAudioWrite waiting for wakeup word finished---\n");
            continue;
        }
        buffer_len = (int)(frame_len * snd_pcm_format_width(SND_PCM_FORMAT_S16_LE) / 8 * channels);
#if SAVE_CAE_OUT_PCM
        fwrite(buffer,buffer_len, 1, usDta->fp_out);
#endif
        preProcessBuffer(buffer, cae_audio_data, buffer_len);
        err = api_cae_audio_write(cae, cae_audio_data, buffer_len * 2);
        if(err != 0) {
            Log::error("CAEAudioWrite with errCode:%d\n", err);
        }
    }
}

int main(int argc, char *argv[])
{
    static CAEUserData userData;
    int ret = MSP_SUCCESS;
    int leds_num = 0;
    CAE_HANDLE cae = NULL;

    userData.wakeUpState = false;
    userData.newSession = false;
    userData.mscInProcess = false;
    userData.suspendFlag = false;
    userData.wakeUping = false;
	memcpy(userData.curFunc,"normal",6);

    /* Initialize Log class */
    Log::init("iflytec_SDK_Demo", true);

#if SAVE_CAE_OUT_PCM
    userData.fp_out = fopen(Constant::kCaeOutPcmPath, "wb");
    if (userData.fp_out != NULL) {
        Log::error("fopen %d file ....failed\n", Constant::kCaeOutPcmPath);
    }
#endif

    // RK_Led multi_init
    /*auto infoled = std::make_shared<InfoLed>();
    leds_num = infoled->leds_multi_init();
    if(leds_num <= 0)
    {
        Log::error("RKLed init failed\n");
        return -1;
    }
    userData.infoled = infoled;*/
    Log::info("RK_Led multi_init success with leds_num: %d\n", leds_num);

    if(initCaeFuncs() != MSP_SUCCESS)
    {
        Log::error("initCaeFuncs() failed\n");
        return -1;
    }
    printf("initCaeFuncs success\n");

    ret = api_cae_new(&cae, Constant::kIwvResPath, CAEIvwCb, NULL, CAEAudioCb, NULL, &userData);
    if(ret != MSP_SUCCESS)
    {
        Log::error("CAENew ... failed!\n");
        return -1;
    }
    printf("api cae new success\n");
    if(!AudioProcess::MSP_Login(Constant::kMSCLoginParams))
    {
        return -1;
    }
    printf("MSP login\n");

	init_gst_player();//init gstreamer player,only need once

    printf("init gst player\n");

    Log::info("CAENew success and Write_Cae_Audio_Data begin\n");

#ifdef USE_CMCC
    Log::info("----cmcc_tcp_init----\n");
    int socket_fd = cmcc_tcp_init();
    if(socket_fd < 0)
    {
        Log::debug("cmcc tcp init failed\n");
        return -1;
    }

    Log::info("----cmcc init migu music---\n");
    char device_info[] = "{\"mac\":\"88:83:5D:9C:8B:9B\",\"smartDeviceId\":\"1188:83:5D:9C:8B:9B\",\"msisdn\":\"18867105160\"}";
    int migu_ret = migusdk_init(device_info);
    Log::debug("----migusdk_init ret =%d\n",migu_ret);

    Log::info("----cmcc init heartBeat Thread---\n");
    std::thread thread(cmcc_do_heartBeat);
    thread.detach();
    cmcc_music_list = (struct CMCC_MUSIC_LIST*)calloc(sizeof(struct CMCC_MUSIC_LIST), 1);

	//read book info
	cmcc_readbook_info = (struct CMCC_READBOOK_INFO*)calloc(sizeof(struct CMCC_READBOOK_INFO), 1);
#endif
    play_alert_file(Constant::kWifiConnected,true);//play sounds indicated wifi connected

	//key event process thread
	start_event_thread();

    caeAudioWrite(cae, &userData);

    return 0;
}
