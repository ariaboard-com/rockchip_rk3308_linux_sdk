/*
 *  Copyright (c) 2005-2017 Fuzhou Rockchip Electronics Co.Ltd
 */

/*
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include <linux/version.h>
#include <linux/input.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <ctype.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include "get_event.h"
//#include "rk-leds.h"
#define NUMBER_OF_LEDS		12

#define DEV_INPUT_EVENT "/dev/input"
#define EVENT_DEV_NAME "event"
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)  ((x)%BITS_PER_LONG)
#define BIT(x)  (1UL<<OFF(x))
#define LONG(x) ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

/* alexa volume value */
#define ALEXA_MAX_VOLUME_VAL		200
#define ALEXA_DEFAULT_VOLUME_VAL		ALEXA_MAX_VOLUME_VAL/2
#define ALEXA_VOL_EVERRY_STEP	ALEXA_MAX_VOLUME_VAL/NUMBER_OF_LEDS
/* codec volume value */
#define CODEC_MAX_VOLUME_VAL		26



pthread_mutex_t ev_mutex;
pthread_cond_t ev_pending;
volatile int ev_unprocessed;

struct alexa_key key_read[MAX_KEY_BUFFERED];
struct alexa_support_event_type support_event[] = {
	{KEY_EVENT, "rk29-keypad"},
	{ROTARY_EVENT, "rotary"},
};

/**
 * record the current volume, from 0~200.
 */
volatile int alexa_current_vol = ALEXA_DEFAULT_VOLUME_VAL;
volatile int current_vol_step = ALEXA_DEFAULT_VOLUME_VAL/NUMBER_OF_LEDS;
int is_volume_changed = 0;
struct alexa_vol_control volume_controls[NUMBER_OF_LEDS] = {
	{1, ALEXA_VOL_EVERRY_STEP, 2}, {2, ALEXA_VOL_EVERRY_STEP*2, 4}, 
	{3, ALEXA_VOL_EVERRY_STEP*3, 6},{4, ALEXA_VOL_EVERRY_STEP*4, 8},
	{5, ALEXA_VOL_EVERRY_STEP*5, 10}, {6, ALEXA_VOL_EVERRY_STEP*6, 12},
	{7, ALEXA_VOL_EVERRY_STEP*7, 14}, {8, ALEXA_VOL_EVERRY_STEP*8, 15}, 
	{9, ALEXA_VOL_EVERRY_STEP*9, 16},	{10, ALEXA_VOL_EVERRY_STEP*10, 17},
	{11, ALEXA_VOL_EVERRY_STEP*11, 18}, {12, ALEXA_MAX_VOLUME_VAL, 19},
};

static int enter_wakeup_mode();
static int volume_step_up();
static int volume_step_down();
static int mute_mic();
static int enter_wifi_mode();
static int enter_bt_mode();
static int enter_wifi_bt_mode_with_onekey();

volatile enum alexa_mode alexa_current_mode = ALEXA_NORMAL_MODE;

#if 1
struct alexa_key support_keys [FUNC_LAST_ID] ={
	{207/*KEY_WAKEUP*/, 0, 1, enter_wakeup_mode,},
	{207/*KEY_WIFI_MODE*/, 1, 1, enter_wifi_bt_mode_with_onekey},
};
#else
struct alexa_key support_keys [] ={
	{KEY_PLAY/*WAKEUP*/, 0, 1,  enter_wakeup_mode,},
	{KEY_PLAY/*WIFI_MODE*/, 1, 1, enter_wifi_mode},
	{KEY_VOLUMEDOWN/*VOL_DOWN*/, 0, 1, volume_step_down},
	{KEY_VOLUMEUP/*VOL_UP*/, 0, 1, volume_step_up},
	{KEY_MICMUTE/*MIC_MUTE*/, 0, 1,  mute_mic},
	{KEY_MICMUTE/*BT_MODE*/, 1, 1, enter_bt_mode},
};
#endif
static int enter_wakeup_mode()
{
	return FUNC_KEY_WAKEUP;
}

static int volume_step_up()
{
	//the actually volume process is finished in event_process
	if (++current_vol_step >= NUMBER_OF_LEDS - 1)
		current_vol_step = NUMBER_OF_LEDS - 1;
	is_volume_changed = 1;
	printf("FUNC_KEY_VOL_UP is pressed, step is %d, alexa vol is %d\n",current_vol_step, volume_controls[current_vol_step].alex_vol);
	return FUNC_KEY_VOL_UP;
}
static int volume_step_down()
{
	//the actually volume process is finished in event_process
	if (--current_vol_step <= 0)
		current_vol_step = 0;
	is_volume_changed = 1;
	printf("FUNC_KEY_VOL_DOWN is pressed, step is %d, alexa vol is %d\n",current_vol_step, volume_controls[current_vol_step].alex_vol);
	return FUNC_KEY_VOL_DOWN;
}

static int mute_mic()
{
	static int mic_mode = FUNC_KEY_MIC_UNMUTE;
	int ret;

	if (mic_mode == FUNC_KEY_MIC_UNMUTE) {
		ret = FUNC_KEY_MIC_MUTE;
	}else {
		ret = FUNC_KEY_MIC_UNMUTE;
	}
	
	mic_mode = ret;

	return ret;
}

static int enter_wifi_mode()
{
	int ret = ALEXA_WIFI_MODE;

	if (alexa_current_mode == ALEXA_WIFI_MODE) {
		ret = FUNC_KEY_NORMAL_MODE;
		alexa_current_mode = ALEXA_NORMAL_MODE;
	}else {
		ret = FUNC_KEY_WIFI_MODE;
		alexa_current_mode = ALEXA_WIFI_MODE;
	}
	return ret;
}

static int enter_bt_mode()
{
	int ret = ALEXA_BT_MODE;

	if (alexa_current_mode == ALEXA_BT_MODE) {
		ret = FUNC_KEY_NORMAL_MODE;
		alexa_current_mode = ALEXA_NORMAL_MODE;
	}else {
		ret = FUNC_KEY_BT_MODE;
		alexa_current_mode = ALEXA_BT_MODE;
	}
	
	return ret;
}

/*
 * if we have only one key to switch from wifi/bt/normal mode
 */
static int enter_wifi_bt_mode_with_onekey()
{
	int ret = ALEXA_INVALID_MODE;

	switch (alexa_current_mode) {
		case ALEXA_NORMAL_MODE:
			ret = FUNC_KEY_WIFI_MODE;
			alexa_current_mode = ALEXA_WIFI_MODE;
			break;
		case ALEXA_WIFI_MODE: 
			ret = FUNC_KEY_BT_MODE;
			alexa_current_mode = ALEXA_BT_MODE;
			break;
		case ALEXA_BT_MODE: 
			ret = FUNC_KEY_NORMAL_MODE;
			alexa_current_mode = ALEXA_NORMAL_MODE;
			break;
		defult:
			printf("invalid mode:%d\n",alexa_current_mode);
			break;
	}
	return ret;
}
void alexa_set_mode(enum alexa_mode mode)
{
	pthread_mutex_lock(&ev_mutex);
	alexa_current_mode = mode;
	pthread_mutex_unlock(&ev_mutex);
}
enum alexa_mode alexa_get_mode()
{
	return alexa_current_mode;
}

int codec_set_vol(int vol_step) 
{
	char buf[128];

	//0~26;
	if (vol_step > CODEC_MAX_VOLUME_VAL || vol_step < 0) {
		printf("%s,Illegal volume step %d\n",__func__, vol_step);
		return -1;
	}
	sprintf(buf,"amixer cset numid=1 %d",1+(vol_step-2));//1~69
	system(buf);

	return 0;
}

static int get_state(int fd, unsigned int type, unsigned long *array, size_t size)
{
	int rc;

	switch(type) {
	case EV_LED:
		rc = ioctl(fd, EVIOCGLED(size), array);
		break;
	case EV_SND:
		rc = ioctl(fd, EVIOCGSND(size), array);
		break;
	case EV_SW:
		rc = ioctl(fd, EVIOCGSW(size), array);
		break;
	case EV_KEY:
		/* intentionally not printing the value for EV_KEY, let the
		 * repeat handle this */
	default:
		return 1;
	}
	if (rc == -1)
		return 1;

	return 0;
}

/**
 * Print static device information (no events). This information includes
 * version numbers, device name and all bits supported by this device.
 *
 * @param fd The file descriptor to the device.
 * @return 0 on success or 1 otherwise.
 */
static int print_device_info(int fd)
{
	int version;
	unsigned short id[4];
	char name[256] = "Unknown";

	if (ioctl(fd, EVIOCGVERSION, &version)) {
		perror("evtest: can't get version");
		return 1;
	}

	printf("Input driver version is %d.%d.%d\n",
		version >> 16, (version >> 8) & 0xff, version & 0xff);

	ioctl(fd, EVIOCGID, id);
	printf("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
		id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);

	ioctl(fd, EVIOCGNAME(sizeof(name)), name);
	printf("Input device name: \"%s\"\n", name);

	return 0;
}

/**
 * Filter for the AutoDevProbe scandir on /dev/input.
 *
 * @param dir The current directory entry provided by scandir.
 *
 * @return Non-zero if the given directory entry starts with "event", or zero
 * otherwise.
 */
static int is_event_device(const struct dirent *dir) {
	return strncmp(EVENT_DEV_NAME, dir->d_name, 5) == 0;
}

static int versionsort (const struct dirent **a, const struct dirent **b)
{
  return __strverscmp ((*a)->d_name, (*b)->d_name);
}

/**
 * Scans all /dev/input/event*, open according event device and return the fd 
 * by specifying event type.
 * @param dir The current directory entry provided by scandir.
 *
 * @return The event device file fd of the device file selected.
 */
static int find_event_dev(int event_type)
{
	struct dirent **namelist;
	int i, ndev;
	char fname[64];
	int fd = -1, ret = -1;
	char name[256] = "???";

	if (event_type < EVENT_START || event_type > EVENT_END) {
		fprintf(stderr, "Invalid event type:%d\n", event_type);
		return 0;
	}

	ndev = scandir(DEV_INPUT_EVENT, &namelist, is_event_device, versionsort);
	if (ndev <= 0)
		return 0;

	for (i = 0; i < ndev; i++)
	{
		snprintf(fname, sizeof(fname),
			 "%s/%s", DEV_INPUT_EVENT, namelist[i]->d_name);
		fd = open(fname, O_RDONLY);
		if (fd < 0)
			continue;
		ioctl(fd, EVIOCGNAME(sizeof(name)), name);
		fprintf(stderr, "%s:	%s\n", fname, name);
		if (strstr(name, support_event[event_type].name)) {
			/* find according event device */
			printf("find event device:%s\n",namelist[i]->d_name);
			ret = fd;
			print_device_info(fd);
		} else
			close(fd);

		free(namelist[i]);
	}

	if (ret <= 0)
		printf("Can't find device by event_type[%d,%s]\n",event_type,support_event[event_type].name);
	free(namelist);
	return ret;
}

 int event_process(void)
{
	int i, j, ret = -1;

	pthread_mutex_lock(&ev_mutex);

	while (0 == ev_unprocessed)
		pthread_cond_wait(&ev_pending, &ev_mutex);

	ev_unprocessed = 0;
	for (i = 0; i < MAX_KEY_BUFFERED; i++) {
		//printf("key_read[%d],key_code=%x,is_long_press=%d,is_valid=%d\n",i,key_read[i].key_code,
				//key_read[i].is_long_press,key_read[i].is_key_valid);
		if(key_read[i].is_key_valid) {
			for (j = 0; j < sizeof(support_keys) / sizeof(struct alexa_key); j++) {
				if (key_read[i].key_code == support_keys[j].key_code &&
					key_read[i].is_long_press == support_keys[j].is_long_press) {
					//printf("This key[0x%x] has been %s pressed.\n",key_read[i].key_code, key_read[i].is_long_press ? "long":"short");
					if (support_keys[j].key_process)
						ret = support_keys[j].key_process();
				}
			}
			memset(&key_read[i], 0, sizeof(struct alexa_key));
		} else if(((key_read[i].key_code == KEY_VOLUMEDOWN || key_read[i].key_code == KEY_VOLUMEUP ) &&
							key_read[i].is_long_press == 1)) {
			//if the key volume up or down is pressed more than 500 ms, then the key will be recognized as long press, and
			//is_key_valid will be false until the key has been released .
			for (j = 0; j < sizeof(support_keys) / sizeof(struct alexa_key); j++) {
				if (key_read[i].key_code == support_keys[j].key_code) {
					//printf("This key[0x%x] has been %s pressed.\n",key_read[i].key_code, key_read[i].is_long_press ? "long":"short");
					if (support_keys[j].key_process)
						ret = support_keys[j].key_process();
				}
			}
		}
	}
	if (is_volume_changed) {
		ret = FUNC_VOLUME_CHANGE;
		//if current_vol_step < 0 means we need to mute the speaker
		if (current_vol_step < 0) {

		}

		//control the codec volume output
		if (codec_set_vol(volume_controls[current_vol_step].codec_vol)) {
			printf("%s,codec set volume failed\n",__func__);
		}
		is_volume_changed = 0;
	}
	
	pthread_mutex_unlock(&ev_mutex);
	return ret;
}
 
 /**
 * Set volume .
 *
 * @param vol_step The volume step set by alexa client, value is from 0 to NUMBER_OF_LEDS.
 * every step value is ALEXA_MAX_VOLUME_VAL/NUMBER_OF_LEDS.
 * 
 */
void alexa_volume_set_step(int vol_step)
{
	pthread_mutex_lock(&ev_mutex);
	current_vol_step = vol_step - 1;//volume_controls from 0 ~ 11.
	printf("set volume step to %d, set alexa vol to %d\n",current_vol_step, volume_controls[current_vol_step].alex_vol);
	is_volume_changed = 1;
    pthread_cond_signal(&ev_pending);
	ev_unprocessed = 1;
	pthread_mutex_unlock(&ev_mutex);
}

 /**
 * Get volume .
 *
 * @return The value step of volume, value is from 0 to NUMBER_OF_LEDS.
 */
int alexa_volume_get_step(void)
{
	int vol_step;
	pthread_mutex_lock(&ev_mutex);
	//current_vol_step will be <0 only when alexa mute the speaker
	if (current_vol_step < 0) {
		pthread_mutex_unlock(&ev_mutex);
		return 0;
	}
	vol_step = volume_controls[current_vol_step].vol_step;
	printf("current alexa volume is: %d,codec vol is %d, step value is : %d\n",volume_controls[current_vol_step].alex_vol,
			volume_controls[current_vol_step].codec_vol, vol_step);
	pthread_mutex_unlock(&ev_mutex);	

	return vol_step;
}

void alexa_volume_set_mute()
{
	pthread_mutex_lock(&ev_mutex);
	current_vol_step = -1;
	pthread_cond_signal(&ev_pending);
	ev_unprocessed = 1;
	pthread_mutex_unlock(&ev_mutex);
}

int *event_read_thread(void * arg)
{
	int key_fd,vol_fd,max_fd;
	int i, j, rd, ret;
	fd_set rdfs;
	struct input_event ev[64];
	struct timeval sel_timeout_tv; 

	if (getuid() != 0) {
		fprintf(stderr, "Not running as root, no devices may be available.\n");
		return NULL;
	}

	key_fd = find_event_dev(KEY_EVENT);
	if(!key_fd) {
		printf("-------------- key event thread exit because event key fd is null ------------\n");
//		return NULL;
	}
	vol_fd = find_event_dev(ROTARY_EVENT);
	if(!vol_fd) {
		printf("-------------- key event thread exit because event volume fd is null ------------\n");
//		return NULL;
	}
	if (key_fd > 0 && vol_fd > 0)
		max_fd = (key_fd > vol_fd) ? key_fd : vol_fd;
	else if (key_fd > 0) {
		fprintf(stderr, "[get_event] didn't find any valid rotary event fd\n");
		max_fd = key_fd;
	} else if(vol_fd > 0) {
		fprintf(stderr, "[get_event] didn't find any valid key event fd\n");
		max_fd = vol_fd;
	} else {
		fprintf(stderr, "didn't find any valid key fd and valid rotary fd \n");
		return NULL;
	}

	memset(key_read, 0 , sizeof(key_read));

	while(1) {
		int ev_signaled = 0;

		sel_timeout_tv.tv_sec = 0;
		sel_timeout_tv.tv_usec = 300000;
		FD_ZERO(&rdfs);
		if(key_fd > 0)
			FD_SET(key_fd, &rdfs);
		if(vol_fd > 0)
			FD_SET(vol_fd, &rdfs);
		/*use select timeout to detect long time press, if large than 2 sec, means long time press*/
		ret = select(max_fd + 1, &rdfs, NULL, NULL, &sel_timeout_tv);
		if (ret == 0) {
			pthread_mutex_lock(&ev_mutex);
			for (i = 0; i < MAX_KEY_BUFFERED; i++) {
				if (key_read[i].key_code != 0 && !key_read[i].is_key_valid) {
					//if the key volume up or down is pressed more than 500 ms, then the key will be recognized as long press key
					//, and is_key_valid will be false until the key has been released .
					//the comsumer thread have to continuiously change the volume if the key have not been released more than 800ms.
					if (key_read[i].key_code == KEY_VOLUMEDOWN || key_read[i].key_code == KEY_VOLUMEUP ) {
						struct timeval tv_vol_change, tv_vol_delta;
						int delta_vol_time;

						gettimeofday(&tv_vol_change,0);
						tv_vol_delta.tv_sec = tv_vol_change.tv_sec - key_read[i].time.tv_sec;
						tv_vol_delta.tv_usec = tv_vol_change.tv_usec - key_read[i].time.tv_usec;
						delta_vol_time = tv_vol_delta.tv_sec *1000 + tv_vol_delta.tv_usec/1000;
						if (delta_vol_time >= 500) {
							key_read[i].time.tv_sec = tv_vol_change.tv_sec;
							key_read[i].time.tv_usec = tv_vol_change.tv_usec;
							key_read[i].is_long_press = 1;
							ev_signaled = 1;
						}
						continue;
					}

					if (!key_read[i].is_long_press) {
						struct timeval tv_now, tv_delta;
						int delta_time;
						
						gettimeofday(&tv_now,0);
						printf("Now: time %ld.%06ld\n", tv_now.tv_sec, tv_now.tv_usec);
						tv_delta.tv_sec = tv_now.tv_sec - key_read[i].time.tv_sec;
						tv_delta.tv_usec = tv_now.tv_usec - key_read[i].time.tv_usec;
						delta_time = tv_delta.tv_sec *1000 + tv_delta.tv_usec/1000;
						printf("Delta: time %ld.%06ld\n", tv_delta.tv_sec, tv_delta.tv_usec);
						if (delta_time > 1500) {
							printf("key code [0x%x] has been press longer than 1.5 seconds\n",key_read[i].key_code);
							key_read[i].is_long_press = 1;
							key_read[i].is_key_valid = 1;
							ev_signaled = 1;
						}
					} else {
						printf("It is strange, the long press key didn't process by comsumer\n");
					}
				}
			}
			if (ev_signaled) {
				printf("signal(long time press) is send to comsumer\n");
				ev_signaled = 0;
				pthread_cond_signal(&ev_pending);
				ev_unprocessed = 1;
			}
			pthread_mutex_unlock(&ev_mutex);
			continue;
		} else if (ret == -1) {
			perror("select error\n");
			continue;
		}

		if (key_fd > 0) {
			if (FD_ISSET(key_fd, &rdfs)) {
				rd = read(key_fd, ev, sizeof(ev));

				if (rd < (int) sizeof(struct input_event)) {
					printf("[key]expected %d bytes, got %d, ignore the value\n", (int) sizeof(struct input_event), rd);
					continue;
				}

				pthread_mutex_lock(&ev_mutex);
				for (i = 0; i < rd / sizeof(struct input_event); i++) {
					unsigned int type, code;

					type = ev[i].type;
					code = ev[i].code;
					printf("Event: time %ld.%06ld, ", ev[i].time.tv_sec, ev[i].time.tv_usec);

					if (type == EV_SYN) {
						printf("-------------- SYN_REPORT ------------\n");
					} else {
						printf("input: type=%x,code=%x,key %s\n", type, code, ev[i].value ? "down" : "up");
						if (ev[i].value == 1) {//press down
							for (j = 0; j < MAX_KEY_BUFFERED; j++) {
								if (!key_read[j].is_key_valid && key_read[j].key_code == 0) {
									key_read[j].key_code = code;
									key_read[j].time.tv_sec = ev[i].time.tv_sec;
									key_read[j].time.tv_usec = ev[i].time.tv_usec;
									break;
								} else if (j == MAX_KEY_BUFFERED - 1)
									printf("Can't find valid buffer, you should increase the value of MAX_KEY_BUFFERED\n");
							}
						} else { //press up, signal wakeword thread to get a valid key
							for (j = 0; j < MAX_KEY_BUFFERED; j++) {
								if (key_read[j].key_code == code && !key_read[j].is_key_valid) {
									key_read[j].is_key_valid = 1;
									key_read[j].is_long_press = 0;
									ev_signaled = 1;
								} else if (j == MAX_KEY_BUFFERED - 1 && !ev_signaled)
									printf("[Warning]Can't find a down key stored in key_read[] arrays, may be it's been processed as a long press key\n");
							}
						}
					}
				}
				if (ev_signaled) {
					ev_signaled = 0;
					printf("signal(short time press) is send to comsumer!\n");
					pthread_cond_signal(&ev_pending);
					ev_unprocessed = 1;
				}
				pthread_mutex_unlock(&ev_mutex);
			}
		}
		if (vol_fd > 0) {
			if(FD_ISSET(vol_fd, &rdfs)) {
				rd = read(vol_fd, ev, sizeof(ev));

				if (rd < (int) sizeof(struct input_event)) {
					printf("[rotary]expected %d bytes, got %d, ignore the value\n", (int) sizeof(struct input_event), rd);
					continue;
				}

				pthread_mutex_lock(&ev_mutex);
				for (i = 0; i < rd / sizeof(struct input_event); i++) {
					unsigned int type, code;

					type = ev[i].type;
					code = ev[i].code;

					if (type == EV_SYN) {
						printf("-------------- SYN_REPORT ------------\n");
					} else {
						printf("input: type=%x,code=%x,value %d\n", type, code, ev[i].value);
						if (ev[i].value == 1) {
							current_vol_step = ((current_vol_step + 1) > (NUMBER_OF_LEDS - 1)) ? (NUMBER_OF_LEDS - 1):(current_vol_step + 1);
							ev_signaled = 1;
						} else if (ev[i].value == -1) {
							current_vol_step = ((current_vol_step - 1) < 0) ? 0:(current_vol_step - 1);
							ev_signaled = 1;
						} else
							printf("rotary: illigal rotary input value\n");
					}
				}
				if (ev_signaled) {
					ev_signaled = 0;
					printf("signal(volume change) is send to comsumer!\n");
					is_volume_changed = 1;
					pthread_cond_signal(&ev_pending);
					ev_unprocessed = 1;
				}
				pthread_mutex_unlock(&ev_mutex);			
			}
		}
	}
	
}

int start_event_thread(void)
{
	pthread_t evt_reader;

	pthread_mutex_init (&ev_mutex,NULL);
	pthread_cond_init(&ev_pending, NULL);
	pthread_create(&evt_reader, NULL, (void *)&event_read_thread, NULL);
}

