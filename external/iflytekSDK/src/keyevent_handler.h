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

#ifndef _GET_EVENT_H_
#define _GET_EVENT_H_

#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif
#define MAX_KEY_BUFFERED	8

struct alexa_key
{
    int key_code;
    int is_long_press;
    int is_key_valid;
    /*
     * pre_alexa_mode is used if only one key to switch from different mode, such as wifi/bt/mic_mute mode.
     * we can define the previous alexa mode as ALEXA_WIFI_MODE if we want to press one key to switch from
     * wifi mode to other mode.
     * if pre_alexa_mode defined as ALEXA_INVALID_MODE, means this value is not used
     */
    int (*key_process)(void);
    struct timeval time;
};

enum alexa_mode
{
    ALEXA_NORMAL_MODE = 0,
    ALEXA_WIFI_MODE,
    ALEXA_BT_MODE,
    ALEXA_INVALID_MODE,
};

enum event_type
{
    FUNC_KEY_WAKEUP  = 0,
    FUNC_KEY_WIFI_MODE,
    FUNC_KEY_VOL_DOWN,
    FUNC_KEY_VOL_UP,
    FUNC_KEY_MIC_MUTE,
    FUNC_KEY_BT_MODE,
    FUNC_KEY_NORMAL_MODE,
    FUNC_KEY_MIC_UNMUTE,
    FUNC_VOLUME_CHANGE,
    FUNC_LAST_ID,
};

enum support_event_type
{
    EVENT_START = 0,
    KEY_EVENT = EVENT_START, //this is the first type of event
    ROTARY_EVENT,
    EVENT_END = ROTARY_EVENT,
};

struct alexa_support_event_type
{
    int		event_type;
    char		*name; /*event name, used to judge this event is supported or not*/
};

struct alexa_vol_control
{
    int		vol_step;
    int		alex_vol;
    int		codec_vol;
};

enum alexa_ext_mode
{
    ALEXA_EXT_MODE_NORMAL = 0,
    ALEXA_EXT_MODE_WIFI,
    ALEXA_EXT_MODE_BT,
    ALEXA_EXT_MODE_NONE
};

int start_event_thread(void);
void alexa_volume_set_step(int vol);
int alexa_volume_get_step(void);
void alexa_volume_set_mute();
enum alexa_mode alexa_get_mode();
void alexa_set_mode(enum alexa_mode mode);
bool isWifiMode();
bool isMuteMic();

#ifdef __cplusplus
} /* extern "C" */
#endif /* C++ */

#endif
