/*
 *  Copyright (c) 2005-2017 Fuzhou Rockchip Electronics Co.Ltd
 *
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
#include <pthread.h>
#include <linux/input.h>
#include "keyevent_handler.h"
#include "device_info.h"
#include "info_led.h"
#include "log.h"

#define DEV_INPUT_EVENT     "/dev/input"
#define EVENT_DEV_NAME      "event"
#define BITS_PER_LONG       (sizeof(long) * 8)



#define DEFAULT_VOLUME_VAL    USER_VOL_MAX/2
#define VOL_EVERRY_STEP       USER_VOL_MAX/NUMBER_OF_LEDS

#define NBITS(x)    ((((x)-1)/BITS_PER_LONG)+1)
#define OFF(x)      ((x)%BITS_PER_LONG)
#define BIT(x)      (1UL<<OFF(x))
#define LONG(x)     ((x)/BITS_PER_LONG)
#define test_bit(bit, array)	((array[LONG(bit)] >> OFF(bit)) & 1)

pthread_mutex_t ev_mutex;

static char rk29_keypad[] = {"rk29-keypad"};
static char gpio_keys[] = {"gpio-keys"};
static char rk29_rotary[] = {"rotary"};
static char adc_keys[] = {"adc-keys"};
static char rk8xx_pwrkey[] = {"rk8xx_pwrkey"};

struct alexa_key key_read[MAX_KEY_BUFFERED];
struct alexa_support_event_type support_event[] =
{
    {KEY_EVENT, rk29_keypad},
    {KEY_EVENT, gpio_keys},
    {KEY_EVENT, adc_keys},
    {KEY_EVENT, rk8xx_pwrkey},
    {ROTARY_EVENT, rk29_rotary},
};

/**
 * record the current volume, from 0~100.
 */
volatile int current_vol_step = DEFAULT_VOLUME_VAL / NUMBER_OF_LEDS;
struct alexa_vol_control volume_controls[NUMBER_OF_LEDS] =
{
    {1, VOL_EVERRY_STEP, 2}, {2, VOL_EVERRY_STEP * 2, 4},
    {3, VOL_EVERRY_STEP * 3, 6}, {4, VOL_EVERRY_STEP * 4, 8},
    {5, VOL_EVERRY_STEP * 5, 10}, {6, VOL_EVERRY_STEP * 6, 12},
    {7, VOL_EVERRY_STEP * 7, 14}, {8, VOL_EVERRY_STEP * 8, 15},
    {9, VOL_EVERRY_STEP * 9, 16},	{10, VOL_EVERRY_STEP * 10, 17},
    {11, VOL_EVERRY_STEP * 11, 18}, {12, USER_VOL_MAX, 19},
};

static int enter_wakeup_mode();
static int volume_step_up();
static int volume_step_down();
static int mute_mic();
static int enter_wifi_mode();
static int enter_bt_mode();
static int enter_wifi_bt_mode_with_onekey();
static int find_event_dev(int event_type);

volatile enum alexa_mode current_mode = ALEXA_NORMAL_MODE;
#if 0
struct alexa_key support_keys [FUNC_LAST_ID] =
{
    {207/*KEY_WAKEUP*/, 0, 1, enter_wakeup_mode, {0, 0}},
    {207/*KEY_WIFI_MODE*/, 1, 1, enter_wifi_bt_mode_with_onekey, {0, 0}},
};
#else
struct alexa_key support_keys [] =
{
    {KEY_PLAY/*WAKEUP*/, 0, 1,  enter_wakeup_mode,},
    {KEY_PLAY/*WIFI_MODE*/, 1, 1, enter_wifi_mode},
    {KEY_VOLUMEDOWN/*VOL_DOWN*/, 0, 1, volume_step_down},
    {KEY_VOLUMEUP/*VOL_UP*/, 0, 1, volume_step_up},
    {KEY_MICMUTE/*MIC_MUTE*/, 0, 1,  mute_mic},
    {KEY_MICMUTE/*BT_MODE*/, 1, 1, enter_bt_mode},
    /*next just for evb debug*/
    {KEY_MENU/*WAKEUP*/, 0, 1,  enter_wakeup_mode,},
    {KEY_MENU/*WIFI_MODE*/, 1, 1, enter_wifi_mode},
    {KEY_ESC/*MIC_MUTE*/, 0, 1,  mute_mic},
    {KEY_ESC/*BT_MODE*/, 1, 1, enter_bt_mode},
};
#endif

static int enter_wakeup_mode()
{
    return FUNC_KEY_WAKEUP;
}

static int volume_step_up()
{
    int cur_vol = DeviceIo::getInstance()->getVolume();
    cur_vol += 15;
    if(cur_vol > USER_VOL_MAX)
    {
        cur_vol = USER_VOL_MAX;
        Log::debug("---already maximum volume--\n");
    }
    Log::debug("---set volume to %d--\n", cur_vol);
    DeviceIo::getInstance()->setVolume(cur_vol);
    return FUNC_KEY_VOL_UP;
}

static int volume_step_down()
{
    int cur_vol = DeviceIo::getInstance()->getVolume();
    cur_vol -= 15;
    if(cur_vol < USER_VOL_MIN)
    {
        cur_vol = USER_VOL_MIN;
        Log::debug("---already Minimum volum--\n");
    }
    Log::debug("---set volume to %d--\n", cur_vol);
    DeviceIo::getInstance()->setVolume(cur_vol);
    return FUNC_KEY_VOL_DOWN;
}

static int mic_mode = FUNC_KEY_MIC_UNMUTE;
static int mute_mic()
{
    if(isMuteMic)
    {
        DeviceIo::getInstance()->setMute(false);
        mic_mode = FUNC_KEY_MIC_UNMUTE;
    }
    else
    {
        DeviceIo::getInstance()->setMute(true);
        mic_mode = FUNC_KEY_MIC_MUTE;
    }
    return 0;
}

bool isMuteMic()
{
    if(mic_mode == FUNC_KEY_MIC_UNMUTE)
    {
        return false;
    }
    else
    {
        return true;
    }
}

static int enter_wifi_mode()
{
    int ret = ALEXA_WIFI_MODE;

    if(current_mode == ALEXA_WIFI_MODE)
    {
        ret = FUNC_KEY_NORMAL_MODE;
        current_mode = ALEXA_NORMAL_MODE;
    }
    else
    {
        ret = FUNC_KEY_WIFI_MODE;
        current_mode = ALEXA_WIFI_MODE;
    }
    return ret;
}

static int enter_bt_mode()
{
    int ret = ALEXA_BT_MODE;

    if(current_mode == ALEXA_BT_MODE)
    {
        ret = FUNC_KEY_NORMAL_MODE;
        current_mode = ALEXA_NORMAL_MODE;
    }
    else
    {
        ret = FUNC_KEY_BT_MODE;
        current_mode = ALEXA_BT_MODE;
    }

    return ret;
}

/*
 * if we have only one key to switch from wifi/bt/normal mode
 */
static int enter_wifi_bt_mode_with_onekey()
{
    int ret = ALEXA_INVALID_MODE;

    switch(current_mode)
    {
        case ALEXA_NORMAL_MODE:
            ret = FUNC_KEY_WIFI_MODE;
            current_mode = ALEXA_WIFI_MODE;
            break;
        case ALEXA_WIFI_MODE:
            ret = FUNC_KEY_NORMAL_MODE;
            current_mode = ALEXA_NORMAL_MODE;
            break;
        default:
            Log::debug("invalid mode:%d\n", current_mode);
            break;
    }

    return ret;
}

void alexa_set_mode(enum alexa_mode mode)
{
    pthread_mutex_lock(&ev_mutex);
    current_mode = mode;
    pthread_mutex_unlock(&ev_mutex);
}

enum alexa_mode alexa_get_mode()
{
    return current_mode;
}


bool isWifiMode()
{
    return (current_mode == ALEXA_WIFI_MODE);
}



static int get_state(int fd, unsigned int type, unsigned long *array, size_t size)
{
    int rc;

    switch(type)
    {
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
    if(rc == -1)
    {
        return 1;
    }

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

    if(ioctl(fd, EVIOCGVERSION, &version))
    {
        perror("evtest: can't get version");
        return 1;
    }

    Log::debug("Input driver version is %d.%d.%d\n",
               version >> 16, (version >> 8) & 0xff, version & 0xff);

    ioctl(fd, EVIOCGID, id);
    Log::debug("Input device ID: bus 0x%x vendor 0x%x product 0x%x version 0x%x\n",
               id[ID_BUS], id[ID_VENDOR], id[ID_PRODUCT], id[ID_VERSION]);

    ioctl(fd, EVIOCGNAME(sizeof(name)), name);
    Log::debug("Input device name: \"%s\"\n", name);

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
static int is_event_device(const struct dirent *dir)
{
    return strncmp(EVENT_DEV_NAME, dir->d_name, 5) == 0;
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

    if(event_type < EVENT_START || event_type > EVENT_END)
    {
        fprintf(stderr, "Invalid event type:%d\n", event_type);
        return 0;
    }

    ndev = scandir(DEV_INPUT_EVENT, &namelist, is_event_device, versionsort);
    if(ndev <= 0)
    {
        return 0;
    }

    for(i = 0; i < ndev; i++)
    {
        int j;
        int events_nums = sizeof(support_event) / sizeof(struct alexa_support_event_type);
        snprintf(fname, sizeof(fname),
                 "%s/%s", DEV_INPUT_EVENT, namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if(fd < 0)
        {
            continue;
        }
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        fprintf(stderr, "%s:	%s,j=%d\n", fname, name, j);
        for(j = 0; j < events_nums; j++)
        {
            if(support_event[j].event_type == event_type && strstr(name, support_event[j].name))
            {
                /* find according event device */
                Log::debug("find event device:%s\n", namelist[i]->d_name);
                ret = fd;
                print_device_info(fd);
                break;
            }
        }
        free(namelist[i]);
        if(ret < 0)
        {
            close(fd);
        }
        else
        {
            break;
        }
    }

    if(ret <= 0)
    {
        Log::debug("Can't find device by event_type[%d,%s]\n", event_type, support_event[event_type].name);
    }
    free(namelist);
    return ret;
}

/**
 * Scans all /dev/input/event*, open muli event devices
 * by specifying event type.
 * @param fds: the address that fds store
 * @return fd counts.
 .
 */
static int find_multi_event_dev(int event_type, int *fds)
{
    struct dirent **namelist;
    int i, ndev;
    char fname[64];
    int fd = -1, ret = -1;
    char name[256] = "???";
    int count = 0;

    if(event_type < EVENT_START || event_type > EVENT_END)
    {
        fprintf(stderr, "Invalid event type:%d\n", event_type);
        return 0;
    }

    ndev = scandir(DEV_INPUT_EVENT, &namelist, is_event_device, versionsort);
    if(ndev <= 0)
    {
        return 0;
    }

    for(i = 0; i < ndev; i++)
    {
        int j = 0;
        int events_nums = sizeof(support_event) / sizeof(struct alexa_support_event_type);
        snprintf(fname, sizeof(fname),
                 "%s/%s", DEV_INPUT_EVENT, namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if(fd < 0)
        {
            continue;
        }
        ioctl(fd, EVIOCGNAME(sizeof(name)), name);
        fprintf(stderr, "%s:	%s,j=%d\n", fname, name, j);
        ret = -1;
        for(j = 0; j < events_nums; j++)
        {
            if(support_event[j].event_type == event_type && strstr(name, support_event[j].name))
            {
                /* find according event device */
                Log::debug("find event device:%s\n", namelist[i]->d_name);
                ret = fd;
                print_device_info(fd);
                break;
            }
        }
        if(ret < 0)
        {
            close(fd);
        }
        else
        {
            fds[count++] = fd;
        }
        free(namelist[i]);
    }

    if(count == 0)
    {
        Log::debug("Can't find device by event_type[%d,%s]\n", event_type, support_event[event_type].name);
    }
    free(namelist);
    return count;
}

void* event_process(void * arg)
{
    unsigned int i, j;

    while(1)
    {
        pthread_mutex_lock(&ev_mutex);
        for(i = 0; i < MAX_KEY_BUFFERED; i++)
        {
            //Log::debug("key_read[%d],key_code=%x,is_long_press=%d,is_valid=%d\n",i,key_read[i].key_code,
            //key_read[i].is_long_press,key_read[i].is_key_valid);
            if(key_read[i].is_key_valid)
            {
                for(j = 0; j < sizeof(support_keys) / sizeof(struct alexa_key); j++)
                {
                    if(key_read[i].key_code == support_keys[j].key_code &&
                            key_read[i].is_long_press == support_keys[j].is_long_press)
                    {
                        //Log::debug("This key[0x%x] has been %s pressed.\n",key_read[i].key_code, key_read[i].is_long_press ? "long":"short");
                        if(support_keys[j].key_process)
                        {
                            support_keys[j].key_process();
                        }
                    }
                }
                memset(&key_read[i], 0, sizeof(struct alexa_key));
            }
            else if(((key_read[i].key_code == KEY_VOLUMEDOWN || key_read[i].key_code == KEY_VOLUMEUP) &&
                     key_read[i].is_long_press == 1))
            {
                //if the key volume up or down is pressed more than 500 ms, then the key will be recognized as long press, and
                //is_key_valid will be false until the key has been released .
                for(j = 0; j < sizeof(support_keys) / sizeof(struct alexa_key); j++)
                {
                    if(key_read[i].key_code == support_keys[j].key_code)
                    {
                        //Log::debug("This key[0x%x] has been %s pressed.\n",key_read[i].key_code, key_read[i].is_long_press ? "long":"short");
                        if(support_keys[j].key_process)
                        {
                            support_keys[j].key_process();
                        }
                    }
                }
            }
        }
        pthread_mutex_unlock(&ev_mutex);
        usleep(100 * 1000);
    }
}

/**
 * Set volume .
 *
 * @param vol_step The volume step set by alexa client, value is from 0 to NUMBER_OF_LEDS.
 * every step value is USER_VOL_MAX/NUMBER_OF_LEDS.
 *
 */
void alexa_volume_set_step(int vol_step)
{
    pthread_mutex_lock(&ev_mutex);
    current_vol_step = vol_step - 1;
    Log::debug("set volume step to %d, set alexa vol to %d\n", current_vol_step, volume_controls[current_vol_step].alex_vol);
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
    if(current_vol_step < 0)
    {
        pthread_mutex_unlock(&ev_mutex);
        return 0;
    }
    vol_step = volume_controls[current_vol_step].vol_step;
    Log::debug("current alexa volume is: %d,codec vol is %d, step value is : %d\n", volume_controls[current_vol_step].alex_vol,
               volume_controls[current_vol_step].codec_vol, vol_step);
    pthread_mutex_unlock(&ev_mutex);

    return vol_step;
}

void *event_read_thread(void * arg)
{
    int key_fds[10], vol_fd, max_fd;
    int rd, ret;
    unsigned int i, j, k;
    fd_set rdfs;
    struct input_event ev[64];
    struct timeval sel_timeout_tv;
    int key_fds_count;

    if(getuid() != 0)
    {
        fprintf(stderr, "Not running as root, no devices may be available.\n");
        return NULL;
    }

    key_fds_count = find_multi_event_dev(KEY_EVENT, key_fds);

    if(key_fds_count <= 0)
    {
        Log::debug("-------------- key event thread exit because event key fd is null ------------\n");
    }

    vol_fd = find_event_dev(ROTARY_EVENT);
    if(!vol_fd)
    {
        Log::debug("-------------- key event thread exit because event volume fd is null ------------\n");
    }

    if(key_fds_count > 0 || vol_fd > 0)
    {
        max_fd = vol_fd;
        for(i = 0 ; i < key_fds_count; i++)
            if(max_fd < key_fds[i])
            {
                max_fd = key_fds[i];
            }
    }
    else
    {
        fprintf(stderr, "didn't find any valid key fd and valid rotary fd \n");
        return NULL;
    }

    memset(key_read, 0, sizeof(key_read));

    while(1)
    {
        int ev_signaled = 0;
        sel_timeout_tv.tv_sec = 0;
        sel_timeout_tv.tv_usec = 300000;

        FD_ZERO(&rdfs);
        if(key_fds_count > 0)
            for(i = 0 ; i < key_fds_count; i++)
            {
                FD_SET(key_fds[i], &rdfs);
            }
        if(vol_fd > 0)
        {
            FD_SET(vol_fd, &rdfs);
        }

        /*use select timeout to detect long time press, if large than 2 sec, means long time press*/
        ret = select(max_fd + 1, &rdfs, NULL, NULL, &sel_timeout_tv);
        if(ret == 0)
        {
            pthread_mutex_lock(&ev_mutex);
            for(i = 0; i < MAX_KEY_BUFFERED; i++)
            {
                if(key_read[i].key_code != 0 && !key_read[i].is_key_valid)
                {
                    //if the key volume up or down is pressed more than 500 ms, then the key will be recognized as long press key
                    //, and is_key_valid will be false until the key has been released .
                    //the comsumer thread have to continuiously change the volume if the key have not been released more than 800ms.
                    if(key_read[i].key_code == KEY_VOLUMEDOWN || key_read[i].key_code == KEY_VOLUMEUP)
                    {
                        struct timeval tv_vol_change, tv_vol_delta;
                        int delta_vol_time;

                        gettimeofday(&tv_vol_change, 0);
                        tv_vol_delta.tv_sec = tv_vol_change.tv_sec - key_read[i].time.tv_sec;
                        tv_vol_delta.tv_usec = tv_vol_change.tv_usec - key_read[i].time.tv_usec;
                        delta_vol_time = tv_vol_delta.tv_sec * 1000 + tv_vol_delta.tv_usec / 1000;
                        if(delta_vol_time >= 500)
                        {
                            key_read[i].time.tv_sec = tv_vol_change.tv_sec;
                            key_read[i].time.tv_usec = tv_vol_change.tv_usec;
                            key_read[i].is_long_press = 1;
                            ev_signaled = 1;
                        }
                        continue;
                    }

                    if(!key_read[i].is_long_press)
                    {
                        struct timeval tv_now, tv_delta;
                        int delta_time;

                        gettimeofday(&tv_now, 0);
                        Log::debug("Now: time %ld.%06ld\n", tv_now.tv_sec, tv_now.tv_usec);
                        tv_delta.tv_sec = tv_now.tv_sec - key_read[i].time.tv_sec;
                        tv_delta.tv_usec = tv_now.tv_usec - key_read[i].time.tv_usec;
                        delta_time = tv_delta.tv_sec * 1000 + tv_delta.tv_usec / 1000;
                        Log::debug("Delta: time %ld.%06ld\n", tv_delta.tv_sec, tv_delta.tv_usec);
                        if(delta_time > 1500)
                        {
                            Log::debug("key code [0x%x] has been press longer than 1.5 seconds\n", key_read[i].key_code);
                            key_read[i].is_long_press = 1;
                            key_read[i].is_key_valid = 1;
                            ev_signaled = 1;
                        }
                    }
                    else
                    {
                        Log::debug("It is strange, the long press key didn't process by comsumer\n");
                    }
                }
            }
            pthread_mutex_unlock(&ev_mutex);
            continue;
        }
        else if(ret == -1)
        {
            perror("select error\n");
            continue;
        }

        k = 0;
        while(k < key_fds_count)
        {
            int key_fd = key_fds[k++];
            if(FD_ISSET(key_fd, &rdfs))
            {
                rd = read(key_fd, ev, sizeof(ev));

                if(rd < (int) sizeof(struct input_event))
                {
                    Log::debug("[key]expected %d bytes, got %d, ignore the value\n", (int) sizeof(struct input_event), rd);
                    continue;
                }

                pthread_mutex_lock(&ev_mutex);
                for(i = 0; i < rd / sizeof(struct input_event); i++)
                {
                    int type, code;

                    type = ev[i].type;
                    code = ev[i].code;
                    Log::debug("Event: time %ld.%06ld, ", ev[i].time.tv_sec, ev[i].time.tv_usec);

                    if(type == EV_SYN)
                    {
                        Log::debug("-------------- SYN_REPORT ------------\n");
                    }
                    else
                    {
                        Log::debug("input: type=%x,code=%x,key %s\n", type, code, ev[i].value ? "down" : "up");
                        if(ev[i].value == 1)   //press down
                        {
                            for(j = 0; j < MAX_KEY_BUFFERED; j++)
                            {
                                if(!key_read[j].is_key_valid && key_read[j].key_code == 0)
                                {
                                    key_read[j].key_code = code;
                                    key_read[j].time.tv_sec = ev[i].time.tv_sec;
                                    key_read[j].time.tv_usec = ev[i].time.tv_usec;
                                    break;
                                }
                                else if(j == MAX_KEY_BUFFERED - 1)
                                {
                                    Log::debug("Can't find valid buffer, you should increase the value of MAX_KEY_BUFFERED\n");
                                }
                            }
                        }
                        else     //press up, signal wakeword thread to get a valid key
                        {
                            for(j = 0; j < MAX_KEY_BUFFERED; j++)
                            {
                                if(key_read[j].key_code == code && !key_read[j].is_key_valid)
                                {
                                    key_read[j].is_key_valid = 1;
                                    key_read[j].is_long_press = 0;
                                    ev_signaled = 1;
                                }
                                else if(j == MAX_KEY_BUFFERED - 1 && !ev_signaled)
                                {
                                    Log::debug("[Warning]Can't find a down key stored in key_read[] arrays, may be it's been processed as a long press key\n");
                                }
                            }
                        }
                    }
                }
                pthread_mutex_unlock(&ev_mutex);
            }
        }
        if(vol_fd > 0)
        {
            if(FD_ISSET(vol_fd, &rdfs))
            {
                rd = read(vol_fd, ev, sizeof(ev));

                if(rd < (int) sizeof(struct input_event))
                {
                    Log::debug("[rotary]expected %d bytes, got %d, ignore the value\n", (int) sizeof(struct input_event), rd);
                    continue;
                }

                pthread_mutex_lock(&ev_mutex);
                for(i = 0; i < rd / sizeof(struct input_event); i++)
                {
                    unsigned int type, code;

                    type = ev[i].type;
                    code = ev[i].code;

                    if(type == EV_SYN)
                    {
                        Log::debug("-------------- SYN_REPORT ------------\n");
                    }
                    else
                    {
                        Log::debug("input: type=%x,code=%x,value %d\n", type, code, ev[i].value);
                        if(ev[i].value == 1)
                        {
                            current_vol_step = ((current_vol_step + 1) > (NUMBER_OF_LEDS - 1)) ? (NUMBER_OF_LEDS - 1) : (current_vol_step + 1);
                            ev_signaled = 1;
                        }
                        else if(ev[i].value == -1)
                        {
                            current_vol_step = ((current_vol_step - 1) < 0) ? 0 : (current_vol_step - 1);
                            ev_signaled = 1;
                        }
                        else
                        {
                            Log::debug("rotary: illigal rotary input value\n");
                        }
                    }
                }
                pthread_mutex_unlock(&ev_mutex);
            }
        }
    }
}

int start_event_thread(void)
{
    pthread_t evt_reader;
    pthread_t evt_process;

    pthread_mutex_init(&ev_mutex, NULL);
    pthread_create(&evt_reader, NULL, &event_read_thread, NULL);
    return pthread_create(&evt_process, NULL, &event_process, NULL);
}

