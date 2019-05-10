#ifndef __CAE_LIB_H__
#define __CAE_LIB_H__

#include <stdio.h>
#include <stdlib.h>

#include "info_led.h"
#include "cae_intf.h"
#include "dlfcn.h"

typedef struct _CAEUserData
{
    FILE *fp_out;
    std::shared_ptr<InfoLed> infoled;
    bool wakeUpState;//indicated is wakeup
    bool newSession;//indicated is new iat session
    bool mscInProcess;//indicated tts is in process
    bool suspendFlag;//used for tts interrupt
    bool wakeUping;//indicated wakeup is in progress
    char curFunc[20];//indicated current function,music,readbook,bt,or something
} CAEUserData;

/* ------------------------------------------------------------------------
** Global CAE Variable Definitions
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

typedef void* CAE_MODULEHANDLE;
typedef void* CAE_LIBHANDLE;

CAE_LIBHANDLE cae_LoadLibrary(const char* lib_name);

int cae_FreeLibrary(CAE_LIBHANDLE lib_handle);

void* cae_GetProcAddress(CAE_LIBHANDLE lib_handle, const char* fun_name);

#endif	/* __MSP_LIB_H__ */
