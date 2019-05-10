#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <alsa/asoundlib.h>

#include "msc_tts.h"
#include "log.h"
#include "qtts.h"
#include "msp_cmn.h"
#include "msp_errors.h"
#include "constant.h"

/**
 * @brief TTSProcess::textToSpeech
 * @param text
 * @return success
 */
bool TTSProcess::textToSpeech(const char* text, void *args)
{
    int ret;
    const char* sessionID  = NULL;
    unsigned int audio_len  = 0;
    int  synth_status = MSP_TTS_FLAG_STILL_HAVE_DATA;
    CAEUserData *userData = (CAEUserData*)args;
    bool ret_value = true;

    if(text == NULL)
    {
        Log::info("Params is error!\n");
        return false;
    }

    sessionID = QTTSSessionBegin(Constant::kTTSsessionParams, &ret);
    if(ret != MSP_SUCCESS)
    {
        Log::info("QTTSSessionBegin failed errCode: %d.\n", ret);
        return false;
    }

    ret = QTTSTextPut(sessionID, text, (unsigned int)strlen(text), NULL);
    if(ret != MSP_SUCCESS)
    {
        Log::error("QTTSTextPut failed, error code: %d.\n", ret);
        QTTSSessionEnd(sessionID, "TextPutError");
        return false;
    }

    Log::debug("TTSProcess QTTSTextPut put: %s\n", text);

    int err;
    snd_pcm_t *handle;
    snd_pcm_sframes_t frames;
    const char *device = "default";

    if((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0)
    {
        Log::error("Playback open error: %s\n", snd_strerror(err));
        return false;
    }
    snd_pcm_hw_params_t *hw;

    static int periodsize = 1024;//16 * 1024;
    static int bufsize = 4096;//16 * 1024 * 4;
    static int channels = 1;
    static int rate = 16000;
    static snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;

    snd_pcm_hw_params_alloca(&hw);
    snd_pcm_hw_params_any(handle, hw);
    snd_pcm_hw_params_set_access(handle, hw, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(handle, hw, format);
    snd_pcm_hw_params_set_channels(handle, hw, channels);
    snd_pcm_hw_params_set_rate(handle, hw, rate, 0);
    snd_pcm_hw_params_set_period_size(handle, hw, periodsize, 0);
    snd_pcm_hw_params_set_buffer_size(handle, hw, bufsize);
    if(snd_pcm_hw_params(handle, hw) < 0)
    {
        fprintf(stderr, "snd_pcm_hw_params error\n");
        return false;
    }

    snd_pcm_uframes_t get_period_size = 0;
    snd_pcm_uframes_t get_buffer_size = 0;
    snd_pcm_hw_params_get_period_size(hw, &get_period_size, 0);
    snd_pcm_hw_params_get_buffer_size(hw, &get_buffer_size);
    fprintf(stderr, "get period size:%d \n", get_period_size);
    fprintf(stderr, "get buffer size:%d \n", get_buffer_size);
    /*if ((err = snd_pcm_set_params(handle, SND_PCM_FORMAT_S16_LE,
                                  SND_PCM_ACCESS_RW_INTERLEAVED, 1, 16000, 1, 500000)) < 0)
    {
        Log::error("playback open error: %s\n", snd_strerror(err));
        return false;
    }*/

    while(1)
    {
        if(userData->suspendFlag)
        {
            ret_value = false;
            break;
        }

        const void* data = QTTSAudioGet(sessionID, &audio_len, &synth_status, &ret);
        if(ret != MSP_SUCCESS)
        {
            Log::error("QTTSAudioGet failed \n");
            break;
        }

        uint32_t frame_size = audio_len / 2; //16bit
        Log::debug("frame_size:%d\n", frame_size);
        if(data != NULL && frame_size != 0)
        {
            frames = snd_pcm_writei(handle, data, frame_size);
            if(frames < 0)
            {
                frames = snd_pcm_recover(handle, frames, 0);
            }

            if(frames < 0)
            {
                Log::error("snd_pcm_writei failed: %s\n", snd_strerror(frames));
                break;
            }

            if(frames > 0 && frames < (long)frame_size)
            {
                Log::error("Short write (expected %li, wrote %li)\n", (long)frame_size, frames);
            }
        }

        if(synth_status == MSP_TTS_FLAG_DATA_END)
        {
            break;
        }

        usleep(10 * 1000); // Prevent frequent CPU usage
    }

    //usleep(1000 * 1000);
    // release
    int nonblock = 1;
    snd_pcm_nonblock(handle, 0);
    snd_pcm_drain(handle);
    snd_pcm_nonblock(handle, nonblock);
    snd_pcm_close(handle);
    QTTSSessionEnd(sessionID, "Normal");

    if(userData->suspendFlag)
    {
        userData->suspendFlag = false;
    }

    return ret_value;
}

void TTSProcess::play_feedback_pcm(const char* text, void* args)//used for play feedback words
{
    bool ret = true;
    CAEUserData *userData = (CAEUserData*)args;
    userData->mscInProcess = true;
    ret = textToSpeech(text, args);
    if(!ret)
    {
        Log::error("TTSProcess::play_feedback_pcm() failed, error code: %d.\n", ret);
        goto _exit;
    }

    /*if(get_music_state(gst_player_url) == GST_STATE_PAUSED)   //after tts ,if play music is paused,resume playing
    {
    	usleep(100 * 1000);
    	if(!userData->wakeUping)   //not in wakeup process
    	{
    		resume_music_playing(gst_player_url);
    	}
    }*/
_exit:
    userData->mscInProcess = false;
}

