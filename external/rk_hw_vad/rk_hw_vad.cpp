#include <string.h>  
#include <errno.h>  
#include <string>
#include <time.h>
#include <thread>
#include <pthread.h>
#include <sys/time.h>
#include "recorder.h"
char msg[128];
pthread_mutex_t recorder_close_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t recorder_close_cond=PTHREAD_COND_INITIALIZER;
pthread_mutex_t system_resume_lock=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t system_resume_cond=PTHREAD_COND_INITIALIZER;
void wait_device_mode_timeout_ms(pthread_mutex_t lock,pthread_cond_t cond,int microseconds)
{
    struct timeval tv;
    long long absmsec;
    struct timespec abstime;

    gettimeofday(&tv, NULL);
    absmsec = tv.tv_sec * 1000ll + tv.tv_usec / 1000ll;
    absmsec += microseconds;

    abstime.tv_sec = absmsec / 1000ll;
    abstime.tv_nsec = absmsec % 1000ll * 1000000ll;

    printf("#### public sleep mode ####");
    pthread_mutex_lock(&lock);
    pthread_cond_timedwait(&cond, &lock, &abstime);
    pthread_mutex_unlock(&lock);
    printf("#### return sleep mode succeed ####");
}

void signal_device(pthread_mutex_t lock,pthread_cond_t cond) {
    pthread_mutex_lock(&lock);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

void do_system_sleep() {
    system("echo mem > /sys/power/state");
}

const unsigned int voice_inactive_max_count = 16000 * 5; //16k, 5 seconds
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
void clear_active_frame() {
    system("echo 0 > /sys/module/snd_soc_rockchip_vad/parameters/voice_inactive_frames");
}

bool sleep_check(void) {
    unsigned int inactive_frames = read_voice_inactive_frames();

    printf("inactive frames %d\n",inactive_frames);
    if ((inactive_frames > voice_inactive_max_count)) {
        clear_active_frame();
        return true;
    } else {
        return false;
    }
}

void* recorder_func(void*) {
  snd_pcm_t *capture_handle;
  unsigned int rate = 16000;
  snd_pcm_format_t format = SND_PCM_FORMAT_S16_LE;
  int channels = 4;
  int buffer_frames = 1024;
  size_t BUFSIZE = (int)(buffer_frames * snd_pcm_format_width(format) / 8 * channels);
  char buffer[BUFSIZE];
  int ret = 0; 

  while(true) {
      ret = alsa_open(&capture_handle,channels,rate,format);
      if(ret < 0) {
          printf("alsa_open retry\n");
          alsa_open(&capture_handle,channels,rate,format);
      }
      while (strcmp((char*)msg,"sleep\n") && 
              snd_pcm_readi(capture_handle, buffer, buffer_frames) >= 0) {
          printf("read data\n");
      }
      snd_pcm_close(capture_handle);
      signal_device(recorder_close_lock,recorder_close_cond);
      wait_device_mode_timeout_ms(system_resume_lock,system_resume_cond,1000);//wait system resume
  }
}

void* soft_vad_check_func(void*) {
    while(true) {
        if(sleep_check()) {
            printf("vad_service sleep\n");
            memcpy((char*)msg,"sleep\n",6);
            wait_device_mode_timeout_ms(recorder_close_lock,recorder_close_cond,1000);
            do_system_sleep();
            memcpy((char*)msg,"resume\n",6);
            signal_device(system_resume_lock,system_resume_cond);
            printf("vad_service resume\n");
        }
        usleep(1000*1000);
    }
}

void hw_vad_init() {
    system("echo 0x60 0x40ff01C2 > /sys/kernel/debug/vad/reg");
    system("echo 0x5c 0x000e2020 > /sys/kernel/debug/vad/reg");

    system("amixer cset name='vad switch' 0");
    system("amixer cset name='vad switch' 1");

    system("amixer cset name='ADC MIC Group 1 Left Volume' 3");
    system("amixer cset name='ADC MIC Group 2 Left Volume' 3");
    system("amixer cset name='ADC MIC Group 2 Right Volume' 3");

    system("amixer cset name='ADC ALC Group 2 Left Volume' 25"); 
    system("amixer cset name='ADC ALC Group 2 Right Volume' 25");
}

int main(int argc, char** argv)  
{  
    hw_vad_init();
    pthread_t recorder_thread; 
    pthread_t soft_vad_thread; 
    int ret = pthread_create(&recorder_thread,NULL,recorder_func,NULL); 
    ret = pthread_create(&soft_vad_thread,NULL,soft_vad_check_func,NULL); 
    while(1) {
        sleep(2);
    }
}



