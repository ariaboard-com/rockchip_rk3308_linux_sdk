#ifndef _RECORDER_
#define _RECORDER_
#include <alsa/asoundlib.h>
int alsa_open(snd_pcm_t** capture_handle,int channels,uint32_t rate,snd_pcm_format_t format);
#endif
