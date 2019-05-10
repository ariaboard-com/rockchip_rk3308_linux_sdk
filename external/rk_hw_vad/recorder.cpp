#include "recorder.h"
int alsa_open(snd_pcm_t** capture_handle,int channels,uint32_t rate,snd_pcm_format_t format) {
    snd_pcm_hw_params_t *hw_params;
    int err;
    if ((err = snd_pcm_open(capture_handle, "default", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "cannot open audio device %s (%s)\n",
                snd_strerror(err));
        return -1; 
    }

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        return -1; 
        
    }

    if ((err = snd_pcm_hw_params_any(*capture_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        return -1; 
    }

    if ((err = snd_pcm_hw_params_set_access(*capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf(stderr, "cannot set access type (%s)\n",
                snd_strerror(err));
        return -1; 
    }

    if ((err = snd_pcm_hw_params_set_format(*capture_handle, hw_params, format)) < 0) {
        fprintf(stderr, "cannot set sample format (%s)\n",
                snd_strerror(err));
        return -1; 
    }

    fprintf(stdout, "hw_params format set\n");
#if 1 
    if ((err = snd_pcm_hw_params_set_rate_near(*capture_handle, hw_params, &rate, 0)) < 0) {
        fprintf(stderr, "cannot set sample rate (%s)\n",
                snd_strerror(err));
        return -1; 
    }

    if ((err = snd_pcm_hw_params_set_channels(*capture_handle, hw_params, channels)) < 0) {
        fprintf(stderr, "cannot set channel count (%s)\n",
                snd_strerror(err));
        return -1; 
    }
#endif
    if ((err = snd_pcm_hw_params(*capture_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot set parameters (%s)\n",
                snd_strerror(err));
        return -1; 
    }

    snd_pcm_hw_params_free(hw_params);

    if ((err = snd_pcm_prepare(*capture_handle)) < 0) {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        return -1; 
    }
    return 0;
}


