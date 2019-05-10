/*© 2017 Conexant Systems, LLC


  Permission is hereby granted by Conexant, free of charge, to any developer obtaining a copy
  of this software and associated documentation files (the "Software"), 
  to download, use, copy, modify, merge and distribute the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.  CONEXANT RESERVES THE RIGHT TO MAKE CHANGES TO THE SOFTWARE 
  WITHOUT NOTIFICATION.
  */

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include "cae_intf.h"
#include "cae_lib.h"
#include "cae_errors.h"
#include "InfoLed.h"
#define SLEEP_TIME 50
int sleep_count = SLEEP_TIME;//48k 23ms*50,16k:23ms*50*3
void test_led_on(std::shared_ptr<InfoLed> infoled, int color) {
    infoled->leds_multi_all_on(color);
}

void test_led_scroll(std::shared_ptr<InfoLed> infoled, uint64_t bitmap_color, uint32_t color, uint32_t  bg_color, uint32_t shift, uint32_t delay_ms) {
    infoled->leds_multi_set_scroll(bitmap_color, color, bg_color, shift, delay_ms);
}

void test_led_off(std::shared_ptr<InfoLed> infoled) {
    infoled->leds_multi_all_off();
}

void test_led_num(std::shared_ptr<InfoLed> infoled,int vol) {
    infoled->leds_multi_certain_on((1 << vol), LED_MULTI_PURE_COLOR_WHITE); 
}

/* ------------------------------------------------------------------------
 ** Types
 ** ------------------------------------------------------------------------ */
typedef struct _CAEUserData{
    FILE *fp_out;
    std::shared_ptr<InfoLed> infoled;
}CAEUserData;


/* ------------------------------------------------------------------------
 ** Global Variable Definitions
 ** ------------------------------------------------------------------------ */	
static Proc_CAENew api_cae_new;
static Proc_CAEAudioWrite api_cae_audio_write;
static Proc_CAEResetEng api_cae_reset_eng;
static Proc_CAESetRealBeam api_cae_set_real_beam;
static Proc_CAESetWParam api_cae_set_wparam;
static Proc_CAEGetWParam api_cae_get_wparam;	
static Proc_CAEGetVersion api_cae_get_version;
static Proc_CAEGetChannel api_cae_get_channel;
static Proc_CAESetShowLog api_cae_set_show_log;
static Proc_CAEDestroy api_cae_destroy;

static FILE * finput;
CAE_HANDLE cae = NULL;

static FILE *file_vad_wakeup;
static int voice_wakeup = 0;
/* ------------------------------------------------------------------------
 ** Function Definitions
 ** ------------------------------------------------------------------------ */
static void CAEIvwCb(short angle, short channel, float power, short CMScore, short beam, char *param1, void *param2, void *userData)
{
    printf("\nCAEIvwCb ....\nangle:%d\n  param1:%s\n", angle, param1);
    CAEUserData *usDta = (CAEUserData*)userData;
    voice_wakeup++;
    switch(beam) {
        case 0:
            test_led_num(usDta->infoled,2);
            break;
        case 1:
            test_led_num(usDta->infoled,4);
            break;
        case 2:
            test_led_num(usDta->infoled,6);
            break;
        case 3:
            test_led_num(usDta->infoled,8);
            break;
        case 4:
            test_led_num(usDta->infoled,10);
            break;
        case 5:
            test_led_num(usDta->infoled,0);
            break;
    }
    system("echo 0 > /sys/module/snd_soc_rockchip_vad/parameters/voice_inactive_frames");
    system("gst-play-1.0 -q --no-interactive /usr/lib/wozai.wav &"); 
    /*if(usDta->fp_out == NULL) {
        api_cae_reset_eng(cae);
    }*/
    sleep_count = SLEEP_TIME;
}
static void CAEAudioCb(const void *audioData, unsigned int audioLen, int param1, const void *param2, void *userData)
{
    CAEUserData *usDta = (CAEUserData*)userData;
    if(usDta->fp_out) {
        fwrite(audioData, audioLen, 1, usDta->fp_out);
    }
}

static int initFuncs()
{
    int ret = MSP_SUCCESS;
    const char* libname = "/usr/lib/libcae.so";
    void* hInstance = cae_LoadLibrary(libname);

    if(hInstance == NULL)
    {
        printf("Can not open library!\n");
        return MSP_ERROR_OPEN_FILE;
    }
    api_cae_new = (Proc_CAENew)cae_GetProcAddress(hInstance, "CAENew");
    api_cae_audio_write = (Proc_CAEAudioWrite)cae_GetProcAddress(hInstance, "CAEAudioWrite");
    api_cae_reset_eng = (Proc_CAEResetEng)cae_GetProcAddress(hInstance, "CAEResetEng");
    api_cae_set_real_beam = (Proc_CAESetRealBeam)cae_GetProcAddress(hInstance, "CAESetRealBeam");
    api_cae_set_wparam = (Proc_CAESetWParam)cae_GetProcAddress(hInstance, "CAESetWParam");
    api_cae_get_version = (Proc_CAEGetVersion)cae_GetProcAddress(hInstance, "CAEGetVersion");
    api_cae_get_channel= (Proc_CAEGetChannel)cae_GetProcAddress(hInstance, "CAEGetChannel");
    api_cae_destroy = (Proc_CAEDestroy)cae_GetProcAddress(hInstance, "CAEDestroy");
    return ret;
}

int  mChannels = 8;
int  mIs24bitMode = 0;
int preProcessBuffer(void *data, void *out, int bytes,FILE* file,FILE* file_add_num)
{
    int i = 0, j = 0;

    if (!mIs24bitMode) {
        for (i = 0; i < bytes / 2 ; ) {
            for (j = 0; j < mChannels; j++) {
                int tmp = 0;
                short tmp_data = (*((short *)data + i + j));
                if(j == 6 || j == 7) {
                    tmp_data = 0;
                }
                tmp_data <<= 6;
                if(file) {
                    fwrite((char*)&tmp_data,1,sizeof(short),file);
                }
                tmp = ((tmp_data) << 16 | ((j+1) << 8)) & 0xffffff00;
                if(file_add_num) {
                    fwrite((char*)&tmp,1,sizeof(int),file_add_num);
                }
                *((int *)out + i + j) = tmp;
            }
            i += mChannels;
        }
    } else {
        for (i = 0; i < bytes / 4 ; ) {
            for (j = 0; j < mChannels; j++) {
                int tmp = 0;
                int tmp_data = (*((int *)data + i + j)) << 1;
                tmp = ((((tmp_data & 0xfffff000) >> 12) << 12) | ((j+1) << 8)) & 0xffffff00;
                *((int *)out + i + j) = tmp;
            }
            i += mChannels;
        }
    }
    return 0;
}

void alsa_open(snd_pcm_t** capture_handle,int channels,uint32_t rate,snd_pcm_format_t format) {
    snd_pcm_hw_params_t *hw_params;
    int err;
    if ((err = snd_pcm_open(capture_handle, "vad", SND_PCM_STREAM_CAPTURE, 0)) < 0) {
        fprintf(stderr, "cannot open audio device %s (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_malloc(&hw_params)) < 0) {
        fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_any(*capture_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_access(*capture_handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        fprintf(stderr, "cannot set access type (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_format(*capture_handle, hw_params, format)) < 0) {
        fprintf(stderr, "cannot set sample format (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    fprintf(stdout, "hw_params format set\n");
#if 1 
    if ((err = snd_pcm_hw_params_set_rate_near(*capture_handle, hw_params, &rate, 0)) < 0) {
        fprintf(stderr, "cannot set sample rate (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    if ((err = snd_pcm_hw_params_set_channels(*capture_handle, hw_params, channels)) < 0) {
        fprintf(stderr, "cannot set channel count (%s)\n",
                snd_strerror(err));
        exit(1);
    }
#endif
    if ((err = snd_pcm_hw_params(*capture_handle, hw_params)) < 0) {
        fprintf(stderr, "cannot set parameters (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    snd_pcm_hw_params_free(hw_params);

}

void system_pause() {
    system("echo mem > /sys/power/state");
}

//静音3s进入休眠
const unsigned int voice_inactive_max_count = 16000 * 3; //16k, 1 seconds
unsigned int read_voice_inactive_frames(void)
{
	FILE *fp;
	char buf[100];
	unsigned int frames = 0;

	fp = popen("cat /sys/module/snd_soc_rockchip_vad/parameters/voice_inactive_frames", "r");
	if(!fp) {
		perror("popen");
		exit(EXIT_FAILURE);
	}
    memset(buf, 0, sizeof(buf));
	if (fgets(buf, sizeof(buf) - 1, fp) != 0 ) {
		sscanf(buf, "%ul", &frames);
		//printf("%s frames %lu\n", buf, frames);
    }
	pclose(fp);
	return frames;
}
int main(int argc, char *argv[])
{
    int i,  n = 0, buffer_frames = 1024, channels = 8;
    size_t BUFSIZE = (int)(buffer_frames * snd_pcm_format_width(SND_PCM_FORMAT_S16_LE) / 8 * channels);
    size_t b = BUFSIZE;
    char buffer[b];
    snd_pcm_t *capture_handle;
    unsigned int rate = 16000;
    snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
    int err;

    int ret = MSP_SUCCESS;
    const char *resPath = "/etc/ivw_resource.jet";
    static CAEUserData userData;
    unsigned int pcmSize = 0;
    int totalsize = 0;
    int onec_szie = 0;
    char *pcmBuf = NULL;

    int ledNumber;
    auto infoled = std::make_shared<InfoLed>();
    ledNumber = infoled->leds_multi_init();
    if(ledNumber < 0) {
        printf("led init failed\n");
        //   return -1;
    }

    if(initFuncs() != MSP_SUCCESS)
    {
        printf("load cae library failed\n");	
        return -1;
    }

    printf("api_cae_new in\n");
    userData.infoled = infoled;
    char *cae_write_data = getenv("CAE_WRITE_DATA");
    if(cae_write_data && !strcmp(cae_write_data,"true")) {
        userData.fp_out = fopen("/data/vad_out.pcm","a+");
    } else {
        userData.fp_out = NULL; 
    }
    ret = api_cae_new(&cae, resPath, CAEIvwCb, NULL, CAEAudioCb, NULL, &userData);
    if (MSP_SUCCESS != ret)
    {
        printf("CAENew ....failed\n");
        ret = MSP_ERROR_FAIL;
        return ret;
    }
    printf("api_cae_new out\n");

    int supportChannel = api_cae_get_channel();
    printf("\n CAE current support channel num is : %d\n", supportChannel);
#if 0 
    //set default beam
    int set_beam_value = 1;
    int ret_set_beam = api_cae_set_real_beam(cae, set_beam_value);
    if(ret_set_beam == 0){
        printf("\n set beam param success. param vaule is : %d...\n", set_beam_value);
    }else{
        printf("\n set beam param fail. error code is : %d...\n", ret_set_beam);
    }
#endif
    alsa_open(&capture_handle,channels,rate,format);

    if ((err = snd_pcm_prepare(capture_handle)) < 0) {
        fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                snd_strerror(err));
        exit(1);
    }

    char temp[BUFSIZE*2];
    int count = 1;
    while (snd_pcm_readi(capture_handle, buffer, buffer_frames) >= 0 && count-- > 0)
    {
        b = BUFSIZE;
    }
    snd_pcm_close(capture_handle);
    test_led_off(infoled);
    system_pause();

    char *cae_debug = getenv("CAE_DEBUG");
    char name[128]={0};
    int name_count = 0;
    FILE *file,*file_add_num;
    int vad_wakeup = 1;
    system("cp /data/vad_wakeup /data/vad_wakeup_before");
    while(true) {
        file_vad_wakeup = fopen("/data/vad_wakeup","wb+");
        sprintf(name,"vad_wakeup:%d voice_wakeup:%d\n\0",vad_wakeup++,voice_wakeup);
        fwrite(name,1,sizeof(name),file_vad_wakeup);
        fclose(file_vad_wakeup);

        if(cae_write_data && !strcmp(cae_write_data,"true")) {
            sprintf(name,"/data/vad_%d.pcm",name_count++);
            file = fopen(name,"wb+");
            file_add_num = fopen("/data/vad_add_num.pcm","a+");
        } else {
            file = NULL;
            file_add_num = NULL;
        }
        alsa_open(&capture_handle,channels,rate,format);
        system("amixer -c 1 cset name='ADC ALC Group 3 Left Volume' 31");
        system("amixer -c 1 cset name='ADC ALC Group 3 Right Volume' 31");
        if ((err = snd_pcm_prepare(capture_handle)) < 0) {
            fprintf(stderr, "cannot prepare audio interface for use (%s)\n",
                    snd_strerror(err));
            exit(1);
        }
        infoled->leds_multi_certain_on(1, LED_MULTI_PURE_COLOR_BLUE); 
        while (snd_pcm_readi(capture_handle, buffer, buffer_frames) >= 0)
        {			
            b = BUFSIZE;
            if(cae_debug != NULL) {
                if(!strcmp(cae_debug,"close_vad") || !strcmp(cae_debug,"CLOSE_VAD")) {
                    fprintf(stderr,"close vad \n"); 
                }
            } else if (read_voice_inactive_frames() > voice_inactive_max_count) {
                fprintf(stderr,"voice inactive timeout,go to sleep\n");
                break;
            }

            preProcessBuffer(buffer, temp, BUFSIZE,file,file_add_num);
            err = api_cae_audio_write(cae, temp, BUFSIZE*2);
            if(err < 0) {
                err = api_cae_audio_write(cae, temp, BUFSIZE*2);
                printf("try again err:%d\n",err);
            }
        }
        snd_pcm_close(capture_handle);
        test_led_off(infoled);
        if(cae_write_data && !strcmp(cae_write_data,"true")) {
            fclose(file);
            fclose(file_add_num);
        }
        system_pause();
   }
}
