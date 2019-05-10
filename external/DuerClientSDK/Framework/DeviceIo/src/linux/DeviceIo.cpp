/*
 * Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "DeviceIo/DeviceIo.h"

#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <cmath>
#include <pthread.h>
#include <LoggerUtils/DcsSdkLogger.h>
#include "alsa/asoundlib.h"
#include "bluetooth.h"

namespace duerOSDcsApp {
namespace framework {

#define USER_VOL_MIN                    5   // keep some voice
#define USER_VOL_MAX                    100

// #define AUDIO_MIN_VOLUME                0
// #define AUDIO_MAX_VOLUME                100

#define SOFTVOL /*should play a music before ctl the softvol*/
#define SOFTVOL_CARD "default"
#define SOFTVOL_ELEM "name='Master Playback Volume'"

typedef struct {
    int     volume;
    bool    is_mute;
} user_volume_t;

// static snd_mixer_t*      mixer_fd   = nullptr;
// static snd_mixer_elem_t* mixer_elem = nullptr;
static user_volume_t    user_volume = {0, false};
static pthread_mutex_t  user_volume_mutex;

static int cset(char *value_string, int roflag)
{
    int err;
    int ret = 0;
    static snd_ctl_t *handle = NULL;
    snd_ctl_elem_info_t *info;
    snd_ctl_elem_id_t *id;
    snd_ctl_elem_value_t *control;
    snd_ctl_elem_info_alloca(&info);
    snd_ctl_elem_id_alloca(&id);
    snd_ctl_elem_value_alloca(&control);
    char card[64] = SOFTVOL_CARD;
    int keep_handle = 0;

    if (snd_ctl_ascii_elem_id_parse(id, SOFTVOL_ELEM)) {
        fprintf(stderr, "Wrong control identifier: %s\n", SOFTVOL_ELEM);
        return -EINVAL;
    }

    if (handle == NULL &&
        (err = snd_ctl_open(&handle, card, 0)) < 0) {
        APP_ERROR("Control %s open error: %d\n", card, err);
        return err;
    }
    snd_ctl_elem_info_set_id(info, id);
    if ((err = snd_ctl_elem_info(handle, info)) < 0) {
        APP_ERROR("Cannot find the given element from control %s\n", card);
        if (! keep_handle) {
            snd_ctl_close(handle);
            handle = NULL;
        }
        return err;
    }
    snd_ctl_elem_info_get_id(info, id);     /* FIXME: Remove it when hctl find works ok !!! */
    if (!roflag) {
        snd_ctl_elem_value_set_id(control, id);
        if ((err = snd_ctl_elem_read(handle, control)) < 0) {
            APP_ERROR("Cannot read the given element from control %s\n", card);
            if (! keep_handle) {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return err;
        }
        err = snd_ctl_ascii_value_parse(handle, control, info, value_string);
        if (err < 0) {
            APP_ERROR("Control %s parse error: %d\n", card, err);
            if (!keep_handle) {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return  err;
        }
        if ((err = snd_ctl_elem_write(handle, control)) < 0) {
            APP_ERROR("Control %s element write error: %d; errno: %d\n", card,
                      err, errno);
            if (!keep_handle) {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return err;
        } else {
            APP_INFO("Control %s element write volume %s successfully\n", card,
                     value_string);
        }
        system("alsactl store --file=/data/cfg/asound.state");
    } else {
        int vol_l, vol_r;
        snd_ctl_elem_value_set_id(control, id);
        if ((err = snd_ctl_elem_read(handle, control)) < 0) {
            APP_ERROR("Cannot read the given element from control %s\n", card);
            if (! keep_handle) {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return err;
        }
        vol_l = snd_ctl_elem_value_get_integer(control, 0);
        vol_r = snd_ctl_elem_value_get_integer(control, 1);
        APP_ERROR("%s:  cget %d, %d!\n", __func__, vol_l, vol_r);
        ret = (vol_l + vol_r) >> 1;
    }

    if (! keep_handle) {
        snd_ctl_close(handle);
        handle = NULL;
    }
    return ret;
}

static void softvol_set(int vol) {
    char value[128] = {0};

    sprintf(value, "%d%%", vol);
    int ret = cset(value, 0);
    int trytimes = 100;
    // try again, often fail first time
    while (ret && trytimes-- > 0) {
       usleep(100 * 1000);
       ret = cset(value, 0);
    }
}

static int softvol_get() {
    return cset(NULL, 1);
}

#ifndef SOFTVOL
static void mixer_exit() {
    if (mixer_fd != nullptr) {
        snd_mixer_close(mixer_fd);
        mixer_fd = nullptr;
    }

    mixer_elem = nullptr;
}

static int mixer_init(const char* card, const char* elem) {
#ifdef SOFTVOL
    APP_ERROR("mixer init\n");
    return 0;
#endif

    const char* _card = card ? card : "default";
#ifdef MTK8516
    const char* _elem = elem ? elem : "TAS5760";
#else
    const char* _elem = elem ? elem : "Playback";
#endif

    // open mixer
    if (snd_mixer_open(&mixer_fd, 0) < 0) {
        APP_ERROR("%s: snd_mixer_open error!\n", __func__);
        goto failed;
    }

    // Attach an HCTL to an opened mixer
    if (snd_mixer_attach(mixer_fd, _card) < 0) {
        APP_ERROR("%s: snd_mixer_attach error!\n", __func__);
        goto failed;
    }

    // register mixer
    if (snd_mixer_selem_register(mixer_fd, nullptr, nullptr) < 0) {
        APP_ERROR("%s: snd_mixer_selem_register error!\n", __func__);
        goto failed;
    }

    // load mixer
    if (snd_mixer_load(mixer_fd) < 0) {
        APP_ERROR("%s: snd_mixer_load error!\n", __func__);
        goto failed;
    }

    // each for
    for (mixer_elem = snd_mixer_first_elem(mixer_fd); mixer_elem;
            mixer_elem = snd_mixer_elem_next(mixer_elem)) {
        if (snd_mixer_elem_get_type(mixer_elem) == SND_MIXER_ELEM_SIMPLE
                && snd_mixer_selem_is_active(mixer_elem)) {
            if (strcmp(snd_mixer_selem_get_name(mixer_elem), _elem) == 0) {
                return 0;
            }
        }
    }

    APP_ERROR("%s: Cannot find master mixer elem!\n", __func__);
failed:
    mixer_exit();
    return -1;
}

static int mixer_set_volume(unsigned int user_vol) {
    long mix_vol;

    mix_vol = (user_vol > AUDIO_MAX_VOLUME) ? AUDIO_MAX_VOLUME : user_vol;
    mix_vol = (mix_vol < AUDIO_MIN_VOLUME) ? AUDIO_MIN_VOLUME : user_vol;

    if (mixer_elem == nullptr) {
        APP_INFO("%s: mixer_elem is NULL! mixer_init() will be called.\n", __func__);
        mixer_init(nullptr, nullptr);
    } else if (mixer_elem != nullptr) {
        snd_mixer_selem_set_playback_volume_range(mixer_elem, AUDIO_MIN_VOLUME, AUDIO_MAX_VOLUME);
        snd_mixer_selem_set_playback_volume_all(mixer_elem, mix_vol);
    }

    return 0;
}

static unsigned int mixer_get_volume() {
    long int alsa_left = AUDIO_MIN_VOLUME, alsa_right = AUDIO_MIN_VOLUME;
    int mix_vol = 0;

    if (mixer_elem == nullptr) {
        APP_INFO("%s: mixer_elem is NULL! mixer_init() will be called.\n", __func__);
        mixer_init(nullptr, nullptr);
    } else if (mixer_elem != nullptr) {
        snd_mixer_selem_set_playback_volume_range(mixer_elem, AUDIO_MIN_VOLUME, AUDIO_MAX_VOLUME);
        snd_mixer_selem_get_playback_volume(mixer_elem, SND_MIXER_SCHN_FRONT_LEFT,  &alsa_left);
        snd_mixer_selem_get_playback_volume(mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &alsa_right);

        mix_vol = (alsa_left + alsa_right) >> 1;
    }

    return mix_vol;
}
#endif

static void user_set_volume(int user_vol) {
    APP_ERROR("%s, %d\n", __func__, user_vol);
    /* set volume will unmute */
    if (user_volume.is_mute) {
        if (user_vol == 0)
            user_vol = user_volume.volume;
        user_volume.is_mute = false;
    }
    user_vol = std::min(user_vol, USER_VOL_MAX);
    user_vol = std::max(user_vol, USER_VOL_MIN);

#ifdef SOFTVOL
    softvol_set(user_vol);
    user_volume.volume = user_vol;
#else
    double k, audio_vol;
    k = (double)AUDIO_MAX_VOLUME / USER_VOL_MAX;

    audio_vol = k * user_vol;

    mixer_set_volume(audio_vol);
#endif
}

static int user_get_volume() {
    int user_vol = 0;

#ifdef SOFTVOL
    user_vol = softvol_get();
    APP_ERROR("%s: %d\n", __func__, user_vol);
    return user_vol;
#else
    double k, offset, audio_vol;
    audio_vol = mixer_get_volume();

    k = (double)(USER_VOL_MAX - USER_VOL_MIN) / (AUDIO_MAX_VOLUME - AUDIO_MIN_VOLUME);
    offset = USER_VOL_MAX - k * AUDIO_MAX_VOLUME;

    user_vol = ceil(k * audio_vol + offset);

    user_vol = (user_vol > USER_VOL_MAX) ? USER_VOL_MAX : user_vol;
    user_vol = (user_vol < USER_VOL_MIN) ? USER_VOL_MIN : user_vol;

    APP_DEBUG("[%s] audio_vol:%f  user_vol:%d\n", __FUNCTION__, audio_vol, user_vol);
#endif

    return user_vol;
}

DeviceIo* DeviceIo::m_instance = nullptr;
DeviceInNotify* DeviceIo::m_notify = nullptr;
pthread_once_t DeviceIo::m_initOnce = PTHREAD_ONCE_INIT;
pthread_once_t DeviceIo::m_destroyOnce = PTHREAD_ONCE_INIT;
static std::shared_ptr<duerOSDcsApp::framework::InfoLed> g_infoled;

DeviceIo::DeviceIo() {
    int ret = 0;

    m_notify = nullptr;

#ifndef SOFTVOL
    ret = mixer_init(nullptr, nullptr);

    if (ret) {
        APP_ERROR("[%s] error: mixer_init fail, err is:%d\n",  __FUNCTION__, ret);

        return;
    }
#endif

    ret = pthread_mutex_init(&user_volume_mutex, nullptr);

    if (ret) {
        APP_ERROR("[%s] error: pthread_mutex_init fail, err is:%d\n",  __FUNCTION__, ret);

        return;
    }

    user_volume.volume = user_get_volume();

    m_destroyOnce = PTHREAD_ONCE_INIT;
}

DeviceIo::~DeviceIo() {
    pthread_mutex_destroy(&user_volume_mutex);
#ifndef SOFTVOL
    mixer_exit();
#endif
    m_notify = nullptr;
    m_initOnce = PTHREAD_ONCE_INIT;
}

DeviceIo* DeviceIo::getInstance() {
    pthread_once(&m_initOnce, DeviceIo::init);

    return m_instance;
}

void DeviceIo::releaseInstance() {
    pthread_once(&m_destroyOnce, DeviceIo::destroy);
}

void DeviceIo::init() {
    if (m_instance == nullptr) {
        m_instance = new DeviceIo;
        g_infoled = std::make_shared<duerOSDcsApp::framework::InfoLed>();
        g_infoled->init();
        g_infoled->led_open(MODE_BOOTED,0);
    }
}

void DeviceIo::destroy() {
    if (m_instance != nullptr) {
	delete m_instance;
    	m_instance = nullptr;
    }
}

void DeviceIo::setNotify(DeviceInNotify* notify) {
    if (notify) {
        m_notify = notify;
    }
}

DeviceInNotify* DeviceIo::getNotify() {
    return m_notify;
}

int DeviceIo::controlLed(LedState cmd, void *data, int len) {
    APP_ERROR("\ncontrolLed:%d\n",cmd);
    APP_ERROR("\n\n");
    switch(cmd) {
        case LedState::LED_NET_RECOVERY:
            g_infoled->led_open(MODE_SENSORY_STARTED,0);
            break;
        case LedState::LED_NET_WAIT_CONNECT:
            g_infoled->led_open(MODE_WIFI_CONNECT,0);
            break;
        case LedState::LED_NET_DO_CONNECT:
            g_infoled->led_open(MODE_WIFI_CONNECTING,0);
            break;
        case LedState::LED_NET_CONNECT_FAILED:
            g_infoled->led_open(MODE_WIFI_ERR,0);
            break;
        case LedState::LED_NET_CONNECT_SUCCESS:
            g_infoled->led_open(MODE_OFF,0);
            break;
        case LedState::LED_NET_WAIT_LOGIN:
            break;
        case LedState::LED_NET_DO_LOGIN:
            break;
        case LedState::LED_NET_LOGIN_FAILED:
            g_infoled->led_open(MODE_WIFI_ERR,0);
            break;
        case LedState::LED_NET_LOGIN_SUCCESS:
            g_infoled->led_open(MODE_OFF,0);
            break;

        case LedState::LED_WAKE_UP_DOA:    // param 0 ~ 360
            {
                APP_ERROR("LED_WAKE_UP_DOA:%d\n",*(int*)data);
                if (*(int*)data < 0) {
                    g_infoled->led_open(MODE_VP_WAKEUP,0xff);
                    break;
                }
                /*RK3308 EVB DMIC, 逆向, 且偏移60度*/
                int angle = (360 - *(int *)data + 60) % 360;
                if ((angle % 30) < 15)
                    g_infoled->led_open(MODE_VP_WAKEUP, angle / 30);
                else
                    g_infoled->led_open(MODE_VP_WAKEUP, (angle / 30 + 1) % 12);
            }
            break;
        case LedState::LED_WAKE_UP:        // no param
            g_infoled->led_open(MODE_VP_WAKEUP,0xff);
            break;
        case LedState::LED_SPEECH_PARSE:
            g_infoled->led_open(MODE_VP,0);
            break;
        case LedState::LED_PLAY_TTS:
            g_infoled->led_open(MODE_VP,0);
            break;
        case LedState::LED_PLAY_RESOURCE:
            g_infoled->led_open(MODE_VP,0);
            break;

        case LedState::LED_BT_WAIT_PAIR:
            break;
        case LedState::LED_BT_DO_PAIR:
            break;
        case LedState::LED_BT_PAIR_FAILED:
            break;
        case LedState::LED_BT_PAIR_SUCCESS:
            break;
        case LedState::LED_BT_PLAY:
            break;
        case LedState::LED_BT_CLOSE:
            break;

        case LedState::LED_VOLUME:
            g_infoled->led_open(MODE_VOLUME,*(int*)data / 10);
            break;
        case LedState::LED_MUTE:
            g_infoled->led_open(MODE_MIC_MUTE,0);
            break;

        case LedState::LED_DISABLE_MIC:
            g_infoled->led_open(MODE_MIC_MUTE,0);
            break;

        case LedState::LED_ALARM:
            break;

        case LedState::LED_SLEEP_MODE:
            break;

        case LedState::LED_OTA_DOING:
            break;
        case LedState::LED_OTA_SUCCESS:
            break;

        case LedState::LED_CLOSE_A_LAYER:
            g_infoled->led_open(MODE_OFF,0);
            break;
        case LedState::LED_ALL_OFF:    // no param
            g_infoled->led_open(MODE_OFF,0);
            break;
    }
    return 0;
}

int DeviceIo::controlBt(BtControl cmd, void *data, int len) {
    return duer_bt_control(cmd, data, len);
}

int DeviceIo::transmitInfrared(std::string& infraredCode) {
    return 0;
}

int DeviceIo::openMicrophone() {
    return 0;
}

int DeviceIo::closeMicrophone() {
    return 0;
}

bool DeviceIo::isMicrophoneOpened() {
    return true;
}

void DeviceIo::setVolume(int vol, int track_id) {
    pthread_mutex_lock(&user_volume_mutex);
    user_set_volume(vol);
    pthread_mutex_unlock(&user_volume_mutex);
}

int DeviceIo::getVolume(int track_id) {
    int user_vol;

    pthread_mutex_lock(&user_volume_mutex);

    if (user_volume.is_mute) {
        user_vol = 0;
    } else {
        user_vol = user_volume.volume;
    }

    pthread_mutex_unlock(&user_volume_mutex);

    return user_vol;
}

int DeviceIo::setMute(bool mute) {
    int ret = -1;

    pthread_mutex_lock(&user_volume_mutex);

    if (mute && !user_volume.is_mute) {
        user_volume.is_mute = true;
#ifdef SOFTVOL
        softvol_set(0);
#else
        mixer_set_volume(0);
#endif
        ret = 0;
    } else if (!mute && user_volume.is_mute) {
        /* set volume will unmute */
        user_set_volume(0);
        ret = 0;
    }

    pthread_mutex_unlock(&user_volume_mutex);

    return ret;
}

bool DeviceIo::isMute() {
    bool ret;

    pthread_mutex_lock(&user_volume_mutex);
    ret = user_volume.is_mute;
    pthread_mutex_unlock(&user_volume_mutex);

    return ret;
}

int DeviceIo::getAngle() {
    return 0;
}

bool DeviceIo::getSn(char *sn)
{
#define VENDOR_REQ_TAG		0x56524551
#define VENDOR_READ_IO		_IOW('v', 0x01, unsigned int)
#define VENDOR_SN_ID		1

typedef     unsigned short      uint16;
typedef     unsigned int        uint32;
typedef     unsigned char       uint8;

    struct rk_vendor_req {
        uint32 tag;
        uint16 id;
        uint16 len;
        uint8 data[1024];
    };

    int ret ;
    uint8 p_buf[100]; /* malloc req buffer or used extern buffer */
    struct rk_vendor_req *req;

    req = (struct rk_vendor_req *)p_buf;
    memset(p_buf, 0, 100);
    int sys_fd = open("/dev/vendor_storage", O_RDWR, 0);
    if(sys_fd < 0){
        printf("vendor_storage open fail\n");
        return false;
    }

    req->tag = VENDOR_REQ_TAG;
    req->id = VENDOR_SN_ID;
    req->len = 32;

    ret = ioctl(sys_fd, VENDOR_READ_IO, req);

    close(sys_fd);

    if(ret){
        printf("vendor read error %d\n", ret);
        return false;
    }

    memcpy(sn, req->data, req->len);

    return true;
}

bool DeviceIo::setSn(char * sn) {
    return false;
}
bool DeviceIo::getPCB(char *sn){
    return false;
}

bool DeviceIo::setPCB(char *sn){
    return false;
}


std::string DeviceIo::getVersion() {
    return "";
}

bool DeviceIo::inOtaMode() {
    return false;
}

void DeviceIo::rmOtaFile() {

}

} // namespace framework
} // namespace duerOSDcsApp
