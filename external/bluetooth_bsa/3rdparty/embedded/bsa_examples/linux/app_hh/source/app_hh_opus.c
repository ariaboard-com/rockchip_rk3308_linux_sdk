/*****************************************************************************
 **
 **  Name:           app_hh_opus.c
 **
 **  Description:    Bluetooth HID Host application
 **
 **  Copyright (c) 2017, Cypress Semiconductor., All Rights Reserved.
 **  Proprietary and confidential.
 **
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <opus/opus.h>

#include "app_hh.h"
#include "bsa_trace.h"
#include "app_utils.h"

#define AHO_SAMPLE_RATE 16000
#define AHO_CHANNELS 1
#define AHO_BITRATE 32000
#define AHO_MAX_FRAME_SIZE 6*960

static OpusDecoder *decoder;


/*******************************************************************************
 **
 ** Function         app_hh_opus_init
 **
 ** Description      This function initialize OPUS decoder
 **
 ** Parameters
 **
 ** Returns          0 if successful, otherwise the error number
 **
 *******************************************************************************/
int app_hh_opus_init(void)
{
   int err;

  /* Create a new decoder state. */
   decoder = opus_decoder_create(AHO_SAMPLE_RATE, AHO_CHANNELS, &err);
   if (err<0)
   {
      APP_INFO1("Failed to create OPUS Decoder:%s", opus_strerror(err));
      return -1;
   }

   return 0;
}

/*******************************************************************************
 **
 ** Function         app_hh_opus_decode
 **
 ** Description      This function processes HID reports containing OPUS encoded
 **                  data.
 **
 ** Parameters
 **
 ** Returns          0 if successful, otherwise the error number
 **
 *******************************************************************************/
int app_hh_opus_decode(tAPP_HH_DEVICE *p_hh_dev, UINT8 *p_data, UINT16 length)
{
    unsigned char pcm_bytes[AHO_MAX_FRAME_SIZE*AHO_CHANNELS*2];
    int frame_size, i, opus_size;
    opus_int16 out[AHO_MAX_FRAME_SIZE*AHO_CHANNELS];

    APP_INFO1("app_hh_opus_decode length:%d", length);
    opus_size = *(p_data+5);
    APP_INFO1("app_hh_opus_decode opus length:%d", opus_size);

    frame_size = opus_decode(decoder, (p_data+10), opus_size, out, AHO_MAX_FRAME_SIZE, 0);
    if (frame_size < 0)
    {
       APP_INFO1("decoder failed: %s\n", opus_strerror(frame_size));
       return -1;
    }

    APP_INFO1("app_hh_opus_decode frame_size:%d", frame_size);

    /* Convert to little-endian ordering. */
    for(i=0;i<AHO_CHANNELS*frame_size;i++)
    {
       pcm_bytes[2*i]=out[i]&0xFF;
       pcm_bytes[2*i+1]=(out[i]>>8)&0xFF;
    }

#ifdef PCM_ALSA
    app_alsa_playback_write(pcm_bytes, AHO_CHANNELS*frame_size*2);
#else /* PCM_ALSA */
    /* write the decoded data to file */
    write(p_hh_dev->audio.fd, pcm_bytes, AHO_CHANNELS*frame_size*2);
#endif /* !PCM_ALSA */

    return 0;
}
