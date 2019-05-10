#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <alsa/asoundlib.h>
#include <sys/time.h>


#define SAMPLE_RATE 48000
#define CHANNEL 2
#define REC_DEVICE_NAME "fake_record"
#define WRITE_DEVICE_NAME "fake_play"
#define READ_FRAME  1024     //(768)
#define BUFFER_SIZE (4096)  //(SAMPLE_RATE/2)
#define PERIOD_SIZE (1024)  //(SAMPLE_RATE/8)
//#define ALSA_READ_FORMAT SND_PCM_FORMAT_S32_LE
#define ALSA_READ_FORMAT SND_PCM_FORMAT_S16_LE
#define ALSA_WRITE_FORMAT SND_PCM_FORMAT_S16_LE


struct timeval tv_begin, tv_end;
//gettimeofday(&tv_begin, NULL);

extern  int set_sw_params(snd_pcm_t *pcm, snd_pcm_uframes_t buffer_size,
                snd_pcm_uframes_t period_size, char **msg);

void alsa_fake_device_record_open(snd_pcm_t** capture_handle,int channels,uint32_t rate)
{
    snd_pcm_hw_params_t *hw_params;
    snd_pcm_uframes_t periodSize = PERIOD_SIZE;
    snd_pcm_uframes_t bufferSize = BUFFER_SIZE;
    int dir = 0;
    int err;

    err = snd_pcm_open(capture_handle, REC_DEVICE_NAME, SND_PCM_STREAM_CAPTURE, 0);
    if (err)
    {
        printf( "Unable to open capture PCM device: \n");
        exit(1);
    }
    printf("snd_pcm_open\n");
    //err = snd_pcm_hw_params_alloca(&hw_params);

    err = snd_pcm_hw_params_malloc(&hw_params);
    if(err)
    {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",snd_strerror(err));
        exit(1);
    }
    printf("snd_pcm_hw_params_malloc\n");

    err = snd_pcm_hw_params_any(*capture_handle, hw_params);
    if(err)
    {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",snd_strerror(err));
        exit(1);
    }
    printf("snd_pcm_hw_params_any!\n");
    
    err = snd_pcm_hw_params_set_access(*capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
   // err = snd_pcm_hw_params_set_access(*capture_handle, hw_params, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
    if (err)
    {
        printf("Error setting interleaved mode\n");
        exit(1);
    }
    printf("snd_pcm_hw_params_set_access!\n");
    
    err = snd_pcm_hw_params_set_format(*capture_handle, hw_params, ALSA_READ_FORMAT);
    if (err)
    {
        printf("Error setting format: %s\n", snd_strerror(err));
        exit(1);
    }
    printf("snd_pcm_hw_params_set_format\n");
    
    err = snd_pcm_hw_params_set_channels(*capture_handle, hw_params, channels);
    if (err)
    {
        printf("channels = %d\n",channels);
        printf( "Error setting channels: %s\n", snd_strerror(err));
        exit(1);
    }
    printf("channels = %d\n",channels);

    err = snd_pcm_hw_params_set_buffer_size_near(*capture_handle, hw_params, &bufferSize);
    if (err)
    {
        printf("Error setting buffer size (%d): %s\n", bufferSize, snd_strerror(err));
        exit(1);
    }
    printf("bufferSize = %d\n",bufferSize);

    err = snd_pcm_hw_params_set_period_size_near(*capture_handle, hw_params, &periodSize, 0);
    if (err)
    {
        printf("Error setting period time (%d): %s\n", periodSize, snd_strerror(err));
        exit(1);
    }
    printf("periodSize = %d\n",periodSize);
    
    err = snd_pcm_hw_params_set_rate_near(*capture_handle, hw_params, &rate, 0/*&dir*/);
    if (err)
    {
        printf("Error setting sampling rate (%d): %s\n", rate, snd_strerror(err));
        //goto error;
        exit(1);
    }
    printf("rate = %d\n",rate);

    
    /* Write the parameters to the driver */
     err = snd_pcm_hw_params(*capture_handle, hw_params);
     if (err < 0)
     {
         printf( "Unable to set HW parameters: %s\n", snd_strerror(err));
         //goto error;
         exit(1);
     }
    
     printf(" open record device done \n");
     //set_sw_params(*capture_handle,bufferSize,periodSize,NULL);
     if(hw_params)
        snd_pcm_hw_params_free(hw_params);

}



void alsa_fake_device_write_open(snd_pcm_t** write_handle,int channels,uint32_t write_sampleRate)
{
    snd_pcm_hw_params_t *write_params;
    snd_pcm_uframes_t write_periodSize = PERIOD_SIZE;
    snd_pcm_uframes_t write_bufferSize = BUFFER_SIZE;
    int write_err;
	int write_dir;
    
    write_err = snd_pcm_open(write_handle, WRITE_DEVICE_NAME, SND_PCM_STREAM_PLAYBACK, 0);

	if (write_err)
    {
        printf( "Unable to open playback PCM device: \n");
        exit(1);
    }
    printf( "interleaved mode\n");

   // snd_pcm_hw_params_alloca(&write_params);
    snd_pcm_hw_params_malloc(&write_params);
    printf( "snd_pcm_hw_params_alloca\n");

    snd_pcm_hw_params_any(*write_handle, write_params);

    write_err = snd_pcm_hw_params_set_access(*write_handle, write_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    //write_err = snd_pcm_hw_params_set_access(*write_handle,  write_params, SND_PCM_ACCESS_MMAP_NONINTERLEAVED);
    if (write_err)
    {
        printf("Error setting interleaved mode\n");
        exit(1);
    }
    printf( "interleaved mode\n");
    
    write_err = snd_pcm_hw_params_set_format(*write_handle, write_params, ALSA_WRITE_FORMAT);
    if (write_err)
    {
        printf("Error setting format: %s\n", snd_strerror(write_err));
        exit(1);
    }
    printf( "format successed\n");

    write_err = snd_pcm_hw_params_set_channels(*write_handle, write_params, channels);
    if (write_err)
    {
        printf( "Error setting channels: %s\n", snd_strerror(write_err));
        exit(1);
    }
    printf("channels = %d\n",channels);

    write_err = snd_pcm_hw_params_set_buffer_size_near(*write_handle, write_params, &write_bufferSize);
    if (write_err)
    {
        printf("Error setting buffer size (%ld): %s\n", write_bufferSize, snd_strerror(write_err));
        exit(1);
    }
    printf("write_bufferSize = %d\n",write_bufferSize);

    write_err = snd_pcm_hw_params_set_period_size_near(*write_handle, write_params, &write_periodSize, 0);
    if (write_err)
    {
        printf("Error setting period time (%ld): %s\n", write_periodSize, snd_strerror(write_err));
        exit(1);
    }
    printf("write_periodSize = %d\n",write_periodSize);

    write_err = snd_pcm_hw_params_set_rate_near(*write_handle, write_params, &write_sampleRate, 0/*&write_dir*/);
    if (write_err)
    {
        printf("Error setting sampling rate (%d): %s\n", write_sampleRate, snd_strerror(write_err));
        exit(1);
    }
    printf("setting sampling rate (%d)\n", write_sampleRate);
    
    
#if 0
    snd_pcm_uframes_t write_final_buffer;
    write_err = snd_pcm_hw_params_get_buffer_size(write_params, &write_final_buffer);
    printf(" final buffer size %ld \n" , write_final_buffer);

    snd_pcm_uframes_t write_final_period;
    write_err = snd_pcm_hw_params_get_period_size(write_params, &write_final_period, &write_dir);
    printf(" final period size %ld \n" , write_final_period);
#endif
    /* Write the parameters to the driver */
    write_err = snd_pcm_hw_params(*write_handle, write_params);
    if (write_err < 0)
    {
        printf( "Unable to set HW parameters: %s\n", snd_strerror(write_err));
        exit(1);
    }

	printf(" open write device is successful \n");
	
	
	set_sw_params(*write_handle,write_bufferSize,write_periodSize,NULL);
    if(write_params)
	    snd_pcm_hw_params_free(write_params);
	
}

 int set_sw_params(snd_pcm_t *pcm, snd_pcm_uframes_t buffer_size,
                snd_pcm_uframes_t period_size, char **msg) {

        snd_pcm_sw_params_t *params;
        char buf[256];
        int err;

        //snd_pcm_sw_params_alloca(&params);
        snd_pcm_sw_params_malloc(&params);
        if ((err = snd_pcm_sw_params_current(pcm, params)) != 0) {
                snprintf(buf, sizeof(buf), "Get current params: %s", snd_strerror(err));
                //goto fail;
                exit(1);
        }
        
        /* start the transfer when the buffer is full (or almost full) */
        snd_pcm_uframes_t threshold = (buffer_size / period_size) * period_size;
        if ((err = snd_pcm_sw_params_set_start_threshold(pcm, params, threshold)) != 0) {
                snprintf(buf, sizeof(buf), "Set start threshold: %s: %lu", snd_strerror(err), threshold);
               exit(1);
        }
        
        /* allow the transfer when at least period_size samples can be processed */
        if ((err = snd_pcm_sw_params_set_avail_min(pcm, params, period_size)) != 0) {
                snprintf(buf, sizeof(buf), "Set avail min: %s: %lu", snd_strerror(err), period_size);
                exit(1);
        }
       
        if ((err = snd_pcm_sw_params(pcm, params)) != 0) {
                snprintf(buf, sizeof(buf), "%s", snd_strerror(err));
                exit(1);
        }
        if(params)
            snd_pcm_sw_params_free(params);
        return 0;
}


int main()
{

repeat:
    snd_pcm_t *capture_handle;
    snd_pcm_t *write_handle;
    int err;
    short buffer[READ_FRAME * 2] = {0};
    int ret = -1;
    unsigned int sampleRate = SAMPLE_RATE;
    unsigned int channels = CHANNEL;
    int error = 0;
    int runframe =0;



    printf("\n==========EQ/DRC process release version 1.22===============\n");
    alsa_fake_device_record_open(&capture_handle,channels,sampleRate);
    alsa_fake_device_write_open(&write_handle,channels,sampleRate);

    while(1)
    {
        err = snd_pcm_readi(capture_handle, buffer , READ_FRAME);
        if(err != READ_FRAME)
        {
            printf("====read frame error = %d===\n",err);
             //gettimeofday(&tv_begin, NULL);
             //printf("timeread = %ld\n",(tv_begin.tv_sec * 1000 +tv_begin.tv_usec / 1000));
        }
        if (err == -EPIPE) printf( "Overrun occurred: %d\n", err);
        if (err < 0) {
            err = snd_pcm_recover(capture_handle, err, 0);
               // Still an error, need to exit.
            if (err < 0) {
                printf( "Error occured while recording: %s\n", snd_strerror(err));
		        usleep(200 * 1000);
		        if (capture_handle)
                    snd_pcm_close(capture_handle);
                if (write_handle)
		            snd_pcm_close(write_handle);
                goto repeat;
            }
        }
        
        err = snd_pcm_writei(write_handle, buffer, READ_FRAME);
        if(err != READ_FRAME)
        {
            //gettimeofday(&tv_begin, NULL);
            printf("====write frame error = %d===\n",err);
            //printf("time_write = %ld\n",(tv_begin.tv_sec * 1000 +tv_begin.tv_usec / 1000));
        }
        if (err == -EPIPE) printf("Underrun occurred from write: %d\n", err);
        if (err < 0) {
			err = snd_pcm_recover(write_handle, err, 0);
			//std::this_thread::sleep_for(std::chrono::milliseconds(20));				// Still an error, need to exit.
			if (err < 0)
			{
				printf( "Error occured while writing: %s\n", snd_strerror(err));
                usleep(200 * 1000);
                if (capture_handle)
                    snd_pcm_close(capture_handle);
                if (write_handle)
		            snd_pcm_close(write_handle);
                goto repeat;
			}
		}
    }

    ret = 0;
error:
    if (capture_handle) snd_pcm_close(capture_handle);
    if (write_handle)   snd_pcm_close(write_handle);
    printf("eq_drc_process error..\n");
    return ret;
}
