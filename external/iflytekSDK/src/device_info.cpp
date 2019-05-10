#include "device_info.h"
#include <cmath>
#include <alsa/asoundlib.h>
#include "log.h"


static snd_mixer_t*      mixer_fd   = nullptr;
static snd_mixer_elem_t* mixer_elem = nullptr;
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

    if(snd_ctl_ascii_elem_id_parse(id, SOFTVOL_ELEM))
    {
        fprintf(stderr, "Wrong control identifier: %s\n", SOFTVOL_ELEM);
        return -EINVAL;
    }

    if(handle == NULL &&
            (err = snd_ctl_open(&handle, card, 0)) < 0)
    {
        Log::error("Control %s open error: %d\n", card, err);
        return err;
    }
    snd_ctl_elem_info_set_id(info, id);
    if((err = snd_ctl_elem_info(handle, info)) < 0)
    {
        Log::error("Cannot find the given element from control %s\n", card);
        if(! keep_handle)
        {
            snd_ctl_close(handle);
            handle = NULL;
        }
        return err;
    }
    snd_ctl_elem_info_get_id(info, id);     /* FIXME: Remove it when hctl find works ok !!! */
    if(!roflag)
    {
        snd_ctl_elem_value_set_id(control, id);
        if((err = snd_ctl_elem_read(handle, control)) < 0)
        {
            Log::error("Cannot read the given element from control %s\n", card);
            if(! keep_handle)
            {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return err;
        }
        err = snd_ctl_ascii_value_parse(handle, control, info, value_string);
        if(err < 0)
        {
            Log::error("Control %s parse error: %d\n", card, err);
            if(!keep_handle)
            {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return  err;
        }
        if((err = snd_ctl_elem_write(handle, control)) < 0)
        {
            Log::error("Control %s element write error: %d\n", card, err);
            if(!keep_handle)
            {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return err;
        }
    }
    else
    {
        int vol_l, vol_r;
        snd_ctl_elem_value_set_id(control, id);
        if((err = snd_ctl_elem_read(handle, control)) < 0)
        {
            Log::error("Cannot read the given element from control %s\n", card);
            if(! keep_handle)
            {
                snd_ctl_close(handle);
                handle = NULL;
            }
            return err;
        }
        vol_l = snd_ctl_elem_value_get_integer(control, 0);
        vol_r = snd_ctl_elem_value_get_integer(control, 1);
        Log::debug("%s:  cget %d, %d!\n", __func__, vol_l, vol_r);
        ret = (vol_l + vol_r) >> 1;
    }

    if(! keep_handle)
    {
        snd_ctl_close(handle);
        handle = NULL;
    }
    return ret;
}

static void softvol_set(int vol)
{
    char value[128] = {0};

    sprintf(value, "%d", vol);
    cset(value, 0);
}

static int softvol_get()
{
    return cset(NULL, 1);
}

static void mixer_exit()
{
    if(mixer_fd != nullptr)
    {
        snd_mixer_close(mixer_fd);
        mixer_fd = nullptr;
    }

    mixer_elem = nullptr;
}

static int mixer_init(const char* card, const char* elem)
{
#ifdef SOFTVOL
    Log::debug("mixer init\n");
    return 0;
#endif
    const char* _card = card ? card : "default";
#ifdef MTK8516
    const char* _elem = elem ? elem : "TAS5760";
#else
    const char* _elem = elem ? elem : "Playback";
#endif

    // open mixer
    if(snd_mixer_open(&mixer_fd, 0) < 0)
    {
        Log::error("%s: snd_mixer_open error!\n", __func__);
        goto failed;
    }

    // Attach an HCTL to an opened mixer
    if(snd_mixer_attach(mixer_fd, _card) < 0)
    {
        Log::error("%s: snd_mixer_attach error!\n", __func__);
        goto failed;
    }

    // register mixer
    if(snd_mixer_selem_register(mixer_fd, nullptr, nullptr) < 0)
    {
        Log::error("%s: snd_mixer_selem_register error!\n", __func__);
        goto failed;
    }

    // load mixer
    if(snd_mixer_load(mixer_fd) < 0)
    {
        Log::error("%s: snd_mixer_load error!\n", __func__);
        goto failed;
    }

    // each for
    for(mixer_elem = snd_mixer_first_elem(mixer_fd); mixer_elem;
            mixer_elem = snd_mixer_elem_next(mixer_elem))
    {
        if(snd_mixer_elem_get_type(mixer_elem) == SND_MIXER_ELEM_SIMPLE
                && snd_mixer_selem_is_active(mixer_elem))
        {
            if(strcmp(snd_mixer_selem_get_name(mixer_elem), _elem) == 0)
            {
                return 0;
            }
        }
    }

    Log::debug("%s: Cannot find master mixer elem!\n", __func__);
failed:
    mixer_exit();
    return -1;
}

static int mixer_set_volume(unsigned int user_vol)
{
    long mix_vol;

    mix_vol = (user_vol > AUDIO_MAX_VOLUME) ? AUDIO_MAX_VOLUME : user_vol;
    mix_vol = (mix_vol < AUDIO_MIN_VOLUME) ? AUDIO_MIN_VOLUME : user_vol;

    if(mixer_elem == nullptr)
    {
        Log::error("%s: mixer_elem is NULL! mixer_init() will be called.\n", __func__);
        mixer_init(nullptr, nullptr);
    }
    else if(mixer_elem != nullptr)
    {
        snd_mixer_selem_set_playback_volume_range(mixer_elem, AUDIO_MIN_VOLUME, AUDIO_MAX_VOLUME);
        snd_mixer_selem_set_playback_volume_all(mixer_elem, mix_vol);
    }

    return 0;
}

static unsigned int mixer_get_volume()
{
    long int alsa_left = AUDIO_MIN_VOLUME, alsa_right = AUDIO_MIN_VOLUME;
    int mix_vol = 0;

    if(mixer_elem == nullptr)
    {
        Log::error("%s: mixer_elem is NULL! mixer_init() will be called.\n", __func__);
        mixer_init(nullptr, nullptr);
    }
    else if(mixer_elem != nullptr)
    {
        snd_mixer_selem_set_playback_volume_range(mixer_elem, AUDIO_MIN_VOLUME, AUDIO_MAX_VOLUME);
        snd_mixer_selem_get_playback_volume(mixer_elem, SND_MIXER_SCHN_FRONT_LEFT,  &alsa_left);
        snd_mixer_selem_get_playback_volume(mixer_elem, SND_MIXER_SCHN_FRONT_RIGHT, &alsa_right);

        mix_vol = (alsa_left + alsa_right) >> 1;
    }

    return mix_vol;
}

static void user_set_volume(double user_vol)
{
    double k, audio_vol;

    Log::debug("%s, %lf\n", __func__, user_vol);

    user_vol = (user_vol > USER_VOL_MAX) ? USER_VOL_MAX : user_vol;
    user_vol = (user_vol < USER_VOL_MIN) ? USER_VOL_MIN : user_vol;

    /* set volume will unmute */
    if(user_volume.is_mute)
    {
        user_vol = user_volume.volume;
        user_volume.is_mute = false;
    }
    else
    {
        user_volume.volume = user_vol;
    }

#ifdef SOFTVOL
    softvol_set(user_vol);
#else
    k = (double)AUDIO_MAX_VOLUME / USER_VOL_MAX;

    audio_vol = k * user_vol;

    mixer_set_volume(audio_vol);
#endif
}

static int user_get_volume()
{
    double k, offset, audio_vol;
    int user_vol = 0;

#ifdef SOFTVOL
    user_vol = softvol_get();

    Log::debug("%s: %d\n", __func__, user_vol);
    return user_vol;
#else
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
pthread_once_t DeviceIo::m_initOnce = PTHREAD_ONCE_INIT;
pthread_once_t DeviceIo::m_destroyOnce = PTHREAD_ONCE_INIT;

DeviceIo::DeviceIo()
{
    int ret = 0;
    ret = mixer_init(nullptr, nullptr);

    if(ret)
    {
        Log::error("[%s] error: mixer_init fail, err is:%d\n",  __FUNCTION__, ret);

        return;
    }

    ret = pthread_mutex_init(&user_volume_mutex, nullptr);

    if(ret)
    {
        Log::error("[%s] error: pthread_mutex_init fail, err is:%d\n",  __FUNCTION__, ret);

        return;
    }

    user_volume.volume = user_get_volume();

}

DeviceIo::~DeviceIo()
{
    pthread_mutex_destroy(&user_volume_mutex);
    mixer_exit();
    m_initOnce = PTHREAD_ONCE_INIT;
}

DeviceIo* DeviceIo::getInstance()
{
    pthread_once(&m_initOnce, DeviceIo::init);
    return m_instance;
}

void DeviceIo::releaseInstance()
{
    pthread_once(&m_destroyOnce, DeviceIo::destroy);
}


void DeviceIo::init()
{
    if(m_instance == nullptr)
    {
        m_instance = new DeviceIo;
    }
}

void DeviceIo::destroy()
{
    delete m_instance;
    m_instance = nullptr;
}


int DeviceIo::openMicrophone()
{
    return 0;
}

int DeviceIo::closeMicrophone()
{
    return 0;
}

bool DeviceIo::isMicrophoneOpened()
{
    return true;
}

void DeviceIo::setVolume(int vol)
{
    pthread_mutex_lock(&user_volume_mutex);
    user_set_volume(vol);
    pthread_mutex_unlock(&user_volume_mutex);
}

int DeviceIo::getVolume()
{
    int user_vol;

    pthread_mutex_lock(&user_volume_mutex);

    if(user_volume.is_mute)
    {
        user_vol = 0;
    }
    else
    {
        user_vol = user_volume.volume;
    }

    pthread_mutex_unlock(&user_volume_mutex);

    return user_vol;
}

int DeviceIo::setMute(bool mute)
{
    int ret = -1;

    pthread_mutex_lock(&user_volume_mutex);

    if(mute && !user_volume.is_mute)
    {
        user_volume.is_mute = true;
#ifdef SOFTVOL
        softvol_set(0);
#else
        mixer_set_volume(0);
#endif
        ret = 0;
    }
    else if(!mute && user_volume.is_mute)
    {
        /* set volume will unmute */
        user_set_volume(0);
        ret = 0;
    }

    pthread_mutex_unlock(&user_volume_mutex);

    return ret;
}

bool DeviceIo::isMute()
{
    bool ret;

    pthread_mutex_lock(&user_volume_mutex);
    ret = user_volume.is_mute;
    pthread_mutex_unlock(&user_volume_mutex);

    return ret;
}

int DeviceIo::getAngle()
{
    return 0;
}

bool DeviceIo::getSn(char *sn)
{
    return false;
}

bool DeviceIo::setSn(char * sn)
{
    return false;
}


std::string DeviceIo::getVersion()
{
    return "";
}



