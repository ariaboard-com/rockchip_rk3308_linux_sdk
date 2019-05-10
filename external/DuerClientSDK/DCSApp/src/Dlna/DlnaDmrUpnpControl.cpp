////
// Created by eddy on 18-1-30.
//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Dlna/upnp/upnp.h"
#include "Dlna/upnp/ithread.h"
#include "Dlna/DlnaDmrUpnpControl.h"
#include "Dlna/DlnaDmrLogging.h"
#include "Dlna/DlnaDmrUpnpDevice.h"
#include "Dlna/DlnaDmrOutput.h"

const char* CONTROL_TYPE = "urn:schemas-upnp-org:service:RenderingControl:1";
const char* CONTROL_SERVICE_ID = "urn:upnp-org:serviceId:RenderingControl";
const char* CONTROL_SCPD_URL = "/upnp/rendercontrolSCPD.xml";
const char* CONTROL_CONTROL_URL = "/upnp/control/rendercontrol1";
const char* CONTROL_EVENT_URL = "/upnp/event/rendercontrol1";
const char* CONTROL_EVENT_XML_NS = "urn:schemas-upnp-org:metadata-1-0/RCS/";

namespace duerOSDcsApp {
namespace dueros_dlna {
static const char* control_variable_names[CONTROL_VARIABLE_T_MAX_NUM];
static struct var_meta control_var_meta[CONTROL_VARIABLE_T_MAX_NUM];
static const char* control_default_values[CONTROL_VARIABLE_T_MAX_NUM];
static struct argument** argument_list[CONTROL_CMD_MAX_NUM];
static struct action control_actions[CONTROL_CMD_MAX_NUM];
struct service control_service_;

static const char* aat_presetnames[] = {
    "FactoryDefaults",
    "InstallationDefaults",
    "Vendor defined",
    NULL
};

static const char* aat_channels[] = {
    "Master",
    "LF",
    "RF",
    //"CF",
    //"LFE",
    //"LS",
    //"RS",
    //"LFC",
    //"RFC",
    //"SD",
    //"SL",
    //"SR",
    //"T",
    //"B",
    NULL
};

static const float vol_min_db = -60.0;
static const float vol_mid_db = -20.0;
static const float vol_max_db = 0.0;
static const int vol_mid_point = 50;  // volume_range.max / 2

static struct param_range volume_range = {0, 100, 1};
static struct param_range volume_db_range = { -60 * 256, 0, 0}; // volume_min_db

static struct param_range brightness_range = {0, 100, 1};
static struct param_range contrast_range = {0, 100, 1};
static struct param_range sharpness_range = {0, 100, 1};
static struct param_range vid_gain_range = {0, 100, 1};
static struct param_range vid_black_range = {0, 100, 1};
static struct param_range colortemp_range = {0, 65535, 1};
static struct param_range keystone_range = { -32768, 32767, 1};

static variable_container_t* state_variables_ = nullptr;

static ithread_mutex_t control_mutex;

static struct argument* arguments_list_presets[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentPresetNameList", PARAM_DIR_OUT, CONTROL_VAR_PRESET_NAME_LIST},
    nullptr
};

static struct argument* arguments_get_brightness[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentBrightness", PARAM_DIR_OUT, CONTROL_VAR_BRIGHTNESS},
    nullptr
};

static struct argument* arguments_get_contrast[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentContrast", PARAM_DIR_OUT, CONTROL_VAR_CONTRAST},
    nullptr
};

static struct argument* arguments_get_sharpness[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentSharpness", PARAM_DIR_OUT, CONTROL_VAR_SHARPNESS},
    nullptr
};

static struct argument* arguments_get_red_gain[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentRedVideoGain", PARAM_DIR_OUT, CONTROL_VAR_R_GAIN},
    nullptr
};

static struct argument* arguments_get_green_gain[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentGreenVideoGain", PARAM_DIR_OUT, CONTROL_VAR_G_GAIN},
    nullptr
};

static struct argument* arguments_get_blue_gain[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentBlueVideoGain", PARAM_DIR_OUT, CONTROL_VAR_B_GAIN},
    nullptr
};

static struct argument* arguments_get_red_black[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentRedVideoBlackLevel", PARAM_DIR_OUT, CONTROL_VAR_R_BLACK},
    nullptr
};

static struct argument* arguments_get_green_black[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentGreenVideoBlackLevel", PARAM_DIR_OUT, CONTROL_VAR_G_BLACK},
    nullptr
};

static struct argument* arguments_get_blue_black[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentBlueVideoBlackLevel", PARAM_DIR_OUT, CONTROL_VAR_B_BLACK},
    nullptr
};

static struct argument* arguments_get_color_temp[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentColorTemperature", PARAM_DIR_OUT, CONTROL_VAR_COLOR_TEMP},
    nullptr
};

static struct argument* arguments_get_hor_keystone[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentHorizontalKeystone", PARAM_DIR_OUT, CONTROL_VAR_HOR_KEYSTONE},
    nullptr
};

static struct argument* arguments_get_vert_keystone[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentVerticalKeystone", PARAM_DIR_OUT, CONTROL_VAR_VER_KEYSTONE},
    nullptr
};

static struct argument* arguments_get_mute[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"CurrentMute", PARAM_DIR_OUT, CONTROL_VAR_MUTE},
    nullptr
};
static struct argument* arguments_set_mute[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"DesiredMute", PARAM_DIR_IN, CONTROL_VAR_MUTE},
    nullptr
};
static struct argument* arguments_get_vol[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"CurrentVolume", PARAM_DIR_OUT, CONTROL_VAR_VOLUME},
    nullptr
};
static struct argument* arguments_set_vol[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"DesiredVolume", PARAM_DIR_IN, CONTROL_VAR_VOLUME},
    nullptr
};
static struct argument* arguments_get_vol_db[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"CurrentVolume", PARAM_DIR_OUT, CONTROL_VAR_VOLUME_DB},
    nullptr
};
static struct argument* arguments_set_vol_db[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"DesiredVolume", PARAM_DIR_IN, CONTROL_VAR_VOLUME_DB},
    nullptr
};
static struct argument* arguments_get_vol_dbrange[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"MinValue", PARAM_DIR_OUT, CONTROL_VAR_VOLUME_DB},
    new argument{"MaxValue", PARAM_DIR_OUT, CONTROL_VAR_VOLUME_DB},
    nullptr
};
static struct argument* arguments_get_loudness[] = {
    new argument{"InstanceID", PARAM_DIR_IN, CONTROL_VAR_AAT_INSTANCE_ID},
    new argument{"Channel", PARAM_DIR_IN, CONTROL_VAR_AAT_CHANNEL},
    new argument{"CurrentLoudness", PARAM_DIR_OUT, CONTROL_VAR_LOUDNESS},
    nullptr
};



UpnpControl::UpnpControl() {
    //control_variable_names
    char** p = const_cast<char**>(control_variable_names);
    p[CONTROL_VAR_LAST_CHANGE] = (char*) "LastChange";
    p[CONTROL_VAR_PRESET_NAME_LIST] = (char*) "PresetNameList";
    p[CONTROL_VAR_AAT_CHANNEL] = (char*) "A_ARG_TYPE_Channel";
    p[CONTROL_VAR_AAT_INSTANCE_ID] = (char*) "A_ARG_TYPE_InstanceID";
    p[CONTROL_VAR_AAT_PRESET_NAME] = (char*) "A_ARG_TYPE_PresetName";
    p[CONTROL_VAR_BRIGHTNESS] = (char*) "Brightness";
    p[CONTROL_VAR_CONTRAST] = (char*) "Contrast";
    p[CONTROL_VAR_SHARPNESS] = (char*) "Sharpness";
    p[CONTROL_VAR_R_GAIN] = (char*) "RedVideoGain";
    p[CONTROL_VAR_G_GAIN] = (char*) "GreenVideoGain";
    p[CONTROL_VAR_B_GAIN] = (char*) "BlueVideoGain";
    p[CONTROL_VAR_R_BLACK] = (char*) "RedVideoBlackLevel";
    p[CONTROL_VAR_G_BLACK] = (char*) "GreenVideoBlackLevel";
    p[CONTROL_VAR_B_BLACK] = (char*) "BlueVideoBlackLevel";
    p[CONTROL_VAR_COLOR_TEMP] = (char*) "ColorTemperature";
    p[CONTROL_VAR_HOR_KEYSTONE] = (char*) "HorizontalKeystone";
    p[CONTROL_VAR_VER_KEYSTONE] = (char*) "VerticalKeystone";
    p[CONTROL_VAR_MUTE] = (char*) "Mute";
    p[CONTROL_VAR_VOLUME] = (char*) "Volume";
    p[CONTROL_VAR_VOLUME_DB] = (char*) "VolumeDB";
    p[CONTROL_VAR_LOUDNESS] = (char*) "Loudness";
    p[CONTROL_VAR_UNKNOWN] = nullptr;


    //control_var_meta
    control_var_meta[CONTROL_VAR_LAST_CHANGE] = {SENDEVENT_YES, DATATYPE_STRING, NULL, NULL};
    control_var_meta[CONTROL_VAR_PRESET_NAME_LIST] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    control_var_meta[CONTROL_VAR_AAT_CHANNEL] = {SENDEVENT_NO, DATATYPE_STRING, aat_channels, NULL};
    control_var_meta[CONTROL_VAR_AAT_INSTANCE_ID] = {SENDEVENT_NO, DATATYPE_UI4, NULL, NULL};
    control_var_meta[CONTROL_VAR_AAT_PRESET_NAME] =
    {SENDEVENT_NO, DATATYPE_STRING, aat_presetnames, NULL};
    control_var_meta[CONTROL_VAR_BRIGHTNESS] =
    {SENDEVENT_NO, DATATYPE_UI2, NULL, &brightness_range};
    control_var_meta[CONTROL_VAR_CONTRAST] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &contrast_range};
    control_var_meta[CONTROL_VAR_SHARPNESS] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &sharpness_range};
    control_var_meta[CONTROL_VAR_R_GAIN] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &vid_gain_range};
    control_var_meta[CONTROL_VAR_G_GAIN] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &vid_gain_range};
    control_var_meta[CONTROL_VAR_B_GAIN] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &vid_gain_range};
    control_var_meta[CONTROL_VAR_R_BLACK] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &vid_black_range};
    control_var_meta[CONTROL_VAR_G_BLACK] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &vid_black_range};
    control_var_meta[CONTROL_VAR_B_BLACK] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &vid_black_range};
    control_var_meta[CONTROL_VAR_COLOR_TEMP] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &colortemp_range};
    control_var_meta[CONTROL_VAR_HOR_KEYSTONE] = {SENDEVENT_NO, DATATYPE_I2, NULL, &keystone_range};
    control_var_meta[CONTROL_VAR_VER_KEYSTONE] = {SENDEVENT_NO, DATATYPE_I2, NULL, &keystone_range};
    control_var_meta[CONTROL_VAR_MUTE] = {SENDEVENT_NO, DATATYPE_BOOLEAN, NULL, NULL};
    control_var_meta[CONTROL_VAR_VOLUME] = {SENDEVENT_NO, DATATYPE_UI2, NULL, &volume_range};
    control_var_meta[CONTROL_VAR_VOLUME_DB] = {SENDEVENT_NO, DATATYPE_I2, NULL, &volume_db_range};
    control_var_meta[CONTROL_VAR_LOUDNESS] = {SENDEVENT_NO, DATATYPE_BOOLEAN, NULL, NULL};
    control_var_meta[CONTROL_VAR_UNKNOWN] = {SENDEVENT_NO, DATATYPE_UNKNOWN, NULL, NULL};

    p = const_cast<char**>(control_default_values);
    //control_default_values
    p[CONTROL_VAR_LAST_CHANGE] = (char*) "<Event xmlns = "
                                 "\"urn:schemas-upnp-org:metadata-1-0/RCS/\"/>";
    p[CONTROL_VAR_PRESET_NAME_LIST] = (char*) "";
    p[CONTROL_VAR_AAT_CHANNEL] = (char*) "";
    p[CONTROL_VAR_AAT_INSTANCE_ID] = (char*) "0";
    p[CONTROL_VAR_AAT_PRESET_NAME] = (char*) "";
    p[CONTROL_VAR_BRIGHTNESS] = (char*) "0";
    p[CONTROL_VAR_CONTRAST] = (char*) "0";
    p[CONTROL_VAR_SHARPNESS] = (char*) "0";
    p[CONTROL_VAR_R_GAIN] = (char*) "0";
    p[CONTROL_VAR_G_GAIN] = (char*) "0";
    p[CONTROL_VAR_B_GAIN] = (char*) "0";
    p[CONTROL_VAR_R_BLACK] = (char*) "0";
    p[CONTROL_VAR_G_BLACK] = (char*) "0";
    p[CONTROL_VAR_B_BLACK] = (char*) "0";
    p[CONTROL_VAR_COLOR_TEMP] = (char*) "0";
    p[CONTROL_VAR_HOR_KEYSTONE] = (char*) "0";
    p[CONTROL_VAR_VER_KEYSTONE] = (char*) "0";
    p[CONTROL_VAR_MUTE] = (char*) "0";
    p[CONTROL_VAR_VOLUME] = (char*) "0";
    p[CONTROL_VAR_VOLUME_DB] = (char*) "0";
    p[CONTROL_VAR_LOUDNESS] = (char*) "0";
    p[CONTROL_VAR_UNKNOWN] = nullptr;


    argument_list[CONTROL_CMD_LIST_PRESETS] = arguments_list_presets;
    //[CONTROL_CMD_SELECT_PRESET] =         arguments_select_preset;
    argument_list[CONTROL_CMD_GET_BRIGHTNESS] = arguments_get_brightness;
    //[CONTROL_CMD_SET_BRIGHTNESS] =        arguments_set_brightness;
    argument_list[CONTROL_CMD_GET_CONTRAST] = arguments_get_contrast;
    //[CONTROL_CMD_SET_CONTRAST] =          arguments_set_contrast;
    argument_list[CONTROL_CMD_GET_SHARPNESS] = arguments_get_sharpness;
    //[CONTROL_CMD_SET_SHARPNESS] =         arguments_set_sharpness;
    argument_list[CONTROL_CMD_GET_RED_GAIN] = arguments_get_red_gain;
    //[CONTROL_CMD_SET_RED_GAIN] =          arguments_set_red_gain;
    argument_list[CONTROL_CMD_GET_GREEN_GAIN] = arguments_get_green_gain;
    //[CONTROL_CMD_SET_GREEN_GAIN] =        arguments_set_green_gain;
    argument_list[CONTROL_CMD_GET_BLUE_GAIN] = arguments_get_blue_gain;
    //[CONTROL_CMD_SET_BLUE_GAIN] =         arguments_set_blue_gain;
    argument_list[CONTROL_CMD_GET_RED_BLACK] = arguments_get_red_black;
    //[CONTROL_CMD_SET_RED_BLACK] =         arguments_set_red_black;
    argument_list[CONTROL_CMD_GET_GREEN_BLACK] = arguments_get_green_black;
    //[CONTROL_CMD_SET_GREEN_BLACK] =       arguments_set_green_black;
    argument_list[CONTROL_CMD_GET_BLUE_BLACK] = arguments_get_blue_black;
    //[CONTROL_CMD_SET_BLUE_BLACK] =        arguments_set_blue_black;
    argument_list[CONTROL_CMD_GET_COLOR_TEMP] = arguments_get_color_temp;
    //[CONTROL_CMD_SET_COLOR_TEMP] =        arguments_set_color_temp;
    argument_list[CONTROL_CMD_GET_HOR_KEYSTONE] = arguments_get_hor_keystone;
    //[CONTROL_CMD_SET_HOR_KEYSTONE] =      arguments_set_hor_keystone;
    argument_list[CONTROL_CMD_GET_VERT_KEYSTONE] = arguments_get_vert_keystone;
    //[CONTROL_CMD_SET_VERT_KEYSTONE] =     arguments_set_vert_keystone;
    argument_list[CONTROL_CMD_GET_MUTE] = arguments_get_mute;
    argument_list[CONTROL_CMD_SET_MUTE] = arguments_set_mute;
    argument_list[CONTROL_CMD_GET_VOL] = arguments_get_vol;
    argument_list[CONTROL_CMD_SET_VOL] = arguments_set_vol;
    argument_list[CONTROL_CMD_GET_VOL_DB] = arguments_get_vol_db;
    argument_list[CONTROL_CMD_SET_VOL_DB] = arguments_set_vol_db;
    argument_list[CONTROL_CMD_GET_VOL_DBRANGE] = arguments_get_vol_dbrange;
    argument_list[CONTROL_CMD_GET_LOUDNESS] = arguments_get_loudness;
    //[CONTROL_CMD_SET_LOUDNESS] =          arguments_set_loudness;
    argument_list[CONTROL_CMD_UNKNOWN] = nullptr;

    //control_actions
    control_actions[CONTROL_CMD_LIST_PRESETS] = {"ListPresets", list_presets};
    //[CONTROL_CMD_SELECT_PRESET] =         {"SelectPreset", NULL},
    control_actions[CONTROL_CMD_GET_BRIGHTNESS] = {"GetBrightness", get_brightness}; /* optional */
    //[CONTROL_CMD_SET_BRIGHTNESS] =        {"SetBrightness", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_CONTRAST] = {"GetContrast", get_contrast}; /* optional */
    //[CONTROL_CMD_SET_CONTRAST] =          {"SetContrast", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_SHARPNESS] = {"GetSharpness", get_sharpness}; /* optional */
    //[CONTROL_CMD_SET_SHARPNESS] =         {"SetSharpness", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_RED_GAIN] =
    {"GetRedVideoGain", get_red_videogain}; /* optional */
    //[CONTROL_CMD_SET_RED_GAIN] =          {"SetRedVideoGain", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_GREEN_GAIN] =
    {"GetGreenVideoGain", get_green_videogain}; /* * optional */
    //[CONTROL_CMD_SET_GREEN_GAIN] =        {"SetGreenVideoGain", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_BLUE_GAIN] =
    {"GetBlueVideoGain", get_blue_videogain}; /* optional */
    //[CONTROL_CMD_SET_BLUE_GAIN] =         {"SetBlueVideoGain", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_RED_BLACK] =
    {"GetRedVideoBlackLevel", get_red_videoblacklevel};/* optional */
    //[CONTROL_CMD_SET_RED_BLACK] =         {"SetRedVideoBlackLevel", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_GREEN_BLACK] =
    {"GetGreenVideoBlackLevel", get_green_videoblacklevel};/* optional */
    //[CONTROL_CMD_SET_GREEN_BLACK] =       {"SetGreenVideoBlackLevel", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_BLUE_BLACK] =
    {"GetBlueVideoBlackLevel", get_blue_videoblacklevel};
    /* optional */
    //[CONTROL_CMD_SET_BLUE_BLACK] =        {"SetBlueVideoBlackLevel", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_COLOR_TEMP] = {"GetColorTemperature", get_colortemperature}; /*
 * optional */
    //[CONTROL_CMD_SET_COLOR_TEMP] =        {"SetColorTemperature", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_HOR_KEYSTONE] =
    {"GetHorizontalKeystone", get_horizontal_keystone};
    /* optional */
    //[CONTROL_CMD_SET_HOR_KEYSTONE] =      {"SetHorizontalKeystone", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_VERT_KEYSTONE] = {"GetVerticalKeystone", get_vertical_keystone}; /*
 * optional */
    //[CONTROL_CMD_SET_VERT_KEYSTONE] =     {"SetVerticalKeystone", NULL}, /* optional */
    control_actions[CONTROL_CMD_GET_MUTE] = {"GetMute", get_mute}; /* optional */
    control_actions[CONTROL_CMD_SET_MUTE] = {"SetMute", set_mute}; /* optional */
    control_actions[CONTROL_CMD_GET_VOL] = {"GetVolume", get_volume}; /* optional */
    control_actions[CONTROL_CMD_SET_VOL] = {"SetVolume", set_volume}; /* optional */
    control_actions[CONTROL_CMD_GET_VOL_DB] = {"GetVolumeDB", get_volume_db}; /* optional */
    control_actions[CONTROL_CMD_SET_VOL_DB] = {"SetVolumeDB", set_volume_db}; /* optional */
    control_actions[CONTROL_CMD_GET_VOL_DBRANGE] =
    {"GetVolumeDBRange", get_volume_dbrange}; /* optional */
    control_actions[CONTROL_CMD_GET_LOUDNESS] = {"GetLoudness", get_loudness}; /* optional */
    //[CONTROL_CMD_SET_LOUDNESS] =          {"SetLoudness", NULL}, /* optional */
    control_actions[CONTROL_CMD_UNKNOWN] = {nullptr, nullptr};

    //control_service_
    control_service_.service_id = CONTROL_SERVICE_ID;
    control_service_.service_type = CONTROL_TYPE;
    control_service_.scpd_url = CONTROL_SCPD_URL;
    control_service_.control_url = CONTROL_CONTROL_URL;
    control_service_.event_url = CONTROL_EVENT_URL;
    control_service_.event_xml_ns = CONTROL_EVENT_XML_NS;
    control_service_.actions = control_actions;
    control_service_.action_arguments = argument_list;
    control_service_.variable_names = control_variable_names;
    control_service_.variable_container = nullptr;  // set later.
    control_service_.last_change = nullptr;
    control_service_.variable_meta = control_var_meta;
    control_service_.variable_count = CONTROL_VAR_UNKNOWN;
    control_service_.command_count = CONTROL_CMD_UNKNOWN;
    control_service_.service_mutex = &control_mutex;
}

struct service* UpnpControl::upnp_control_get_service(void) {
    if (control_service_.variable_container == NULL) {
        state_variables_ =
            VariableContainer::get_instance().variablecontainer_new(CONTROL_VAR_COUNT,
                    control_variable_names,
                    control_default_values);
        control_service_.variable_container = state_variables_;
    }

    return &control_service_;
}

void UpnpControl::upnp_control_init(struct upnp_device* device) {
    upnp_control_get_service();

    float volume_fraction = 0;

    if (Output::get_instance().output_get_volume(&volume_fraction) == 0) {
        dbg("Output initial volume is %f; setting control variables accordingly.", volume_fraction);
        //change_volume_decibel(20 * log(volume_fraction) / log(10));
        change_volume_decibel(volume_fraction);
    }

    assert(control_service_.last_change == NULL);
    control_service_.last_change =
        VariableContainer::get_instance().upnp_last_change_collector_new(state_variables_,
                CONTROL_EVENT_XML_NS,
                device,
                CONTROL_SERVICE_ID);

    VariableContainer::get_instance()
    .upnp_last_change_collector_add_ignore(control_service_.last_change,
                                        CONTROL_VAR_AAT_CHANNEL);
    VariableContainer::get_instance()
    .upnp_last_change_collector_add_ignore(control_service_.last_change,
                                        CONTROL_VAR_AAT_INSTANCE_ID);
    VariableContainer::get_instance()
    .upnp_last_change_collector_add_ignore(control_service_.last_change,
                                        CONTROL_VAR_AAT_PRESET_NAME);
}

void UpnpControl::upnp_control_register_variable_listener(variable_change_listener_t cb,
        void* userdata) {
    VariableContainer::get_instance()
    .variablecontainer_register_callback(state_variables_, cb, userdata);
}

void UpnpControl::service_lock(void) {
    ithread_mutex_lock(&control_mutex);

    if (control_service_.last_change) {
        VariableContainer::get_instance().upnp_last_change_collector_start(control_service_
                .last_change);
    }
}

void UpnpControl::service_unlock(void) {
    if (control_service_.last_change) {
        VariableContainer::get_instance().upnp_last_change_collector_finish(control_service_
                .last_change);
    }

    ithread_mutex_unlock(&control_mutex);
}

void UpnpControl::replace_var(control_variable_t varnum, const char* new_value) {
    VariableContainer::get_instance().variablecontainer_change(state_variables_, varnum, new_value);
}

void UpnpControl::change_volume(const char* volume, const char* db_volume) {
    replace_var(CONTROL_VAR_VOLUME, volume);
    replace_var(CONTROL_VAR_VOLUME_DB, db_volume);
}

int UpnpControl::cmd_obtain_variable(struct action_event* event,
                                     control_variable_t varnum,
                                     const char* paramname) {
    const char* instance = UpnpDevice::get_instance().upnp_get_string(event, "InstanceID");

    if (instance == NULL) {
        return -1;
    }

    dbg("%s: %s for instance %s\n", __FUNCTION__, paramname, instance);

    UpnpDevice::get_instance().upnp_append_variable(event, varnum, paramname);
    return 0;
}

int UpnpControl::list_presets(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_PRESET_NAME_LIST,
                               "CurrentPresetNameList");
}

int UpnpControl::get_brightness(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_BRIGHTNESS,
                               "CurrentBrightness");
}

int UpnpControl::get_contrast(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_CONTRAST,
                               "CurrentContrast");
}

int UpnpControl::get_sharpness(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_SHARPNESS,
                               "CurrentSharpness");
}

int UpnpControl::get_red_videogain(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_R_GAIN,
                               "CurrentRedVideoGain");
}

int UpnpControl::get_green_videogain(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_G_GAIN,
                               "CurrentGreenVideoGain");
}

int UpnpControl::get_blue_videogain(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_B_GAIN,
                               "CurrentBlueVideoGain");
}

int UpnpControl::get_red_videoblacklevel(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_R_BLACK,
                               "CurrentRedVideoBlackLevel");
}

int UpnpControl::get_green_videoblacklevel(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_G_BLACK,
                               "CurrentGreenVideoBlackLevel");
}

int UpnpControl::get_blue_videoblacklevel(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_B_BLACK,
                               "CurrentBlueVideoBlackLevel");
}

int UpnpControl::get_colortemperature(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_COLOR_TEMP,
                               "CurrentColorTemperature");
}

int UpnpControl::get_horizontal_keystone(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_HOR_KEYSTONE,
                               "CurrentHorizontalKeystone");
}

int UpnpControl::get_vertical_keystone(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_VER_KEYSTONE,
                               "CurrentVerticalKeystone");
}

int UpnpControl::get_mute(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_MUTE, "CurrentMute");
}

void UpnpControl::set_mute_toggle(int do_mute) {
    replace_var(CONTROL_VAR_MUTE, do_mute ? "1" : "0");
    Output::get_instance().output_set_mute(do_mute);
}

int UpnpControl::set_mute(struct action_event* event) {
    const char* value = UpnpDevice::get_instance().upnp_get_string(event, "DesiredMute");
    service_lock();
    const int do_mute = atoi(value);
    set_mute_toggle(do_mute);
    replace_var(CONTROL_VAR_MUTE, do_mute ? "1" : "0");
    service_unlock();
    return 0;
}

int UpnpControl::get_volume(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_VOLUME,
                               "CurrentVolume");
}

float UpnpControl::volume_level_to_decibel(int volume) {
    if (volume < volume_range.min) {
        volume = volume_range.min;
    }

    if (volume > volume_range.max) {
        volume = volume_range.max;
    }

    if (volume < volume_range.max / 2) {
        return vol_min_db
               + (vol_mid_db - vol_min_db) / vol_mid_point * volume;
    } else {
        const int range = volume_range.max - vol_mid_point;
        return vol_mid_db
               + ((vol_max_db - vol_mid_db) / range
                  * (volume - vol_mid_point));
    }
}

int UpnpControl::volume_decibel_to_level(float decibel) {
    if (decibel < vol_min_db) {
        return volume_range.min;
    }

    if (decibel > vol_max_db) {
        return volume_range.max;
    }

    if (decibel < vol_mid_db) {
        return (decibel - vol_min_db) * vol_mid_point / (vol_mid_db - vol_min_db);
    } else {
        const int range = volume_range.max - vol_mid_point;
        return (decibel - vol_mid_db) * range / (vol_max_db - vol_mid_db) + vol_mid_point;
    }
}

float UpnpControl::change_volume_decibel(float raw_decibel) {
    //    int volume_level = volume_decibel_to_level(raw_decibel);
    int volume_level = raw_decibel;
    float decibel = volume_level_to_decibel(volume_level);

    char volume[10];
    snprintf(volume, sizeof(volume), "%d", volume_level);
    char db_volume[10];
    snprintf(db_volume, sizeof(db_volume), "%d", (int)(256 * decibel));

    dbg("Setting volume-db to %.2fdb == #%d", decibel, volume_level);

    change_volume(volume, db_volume);
    return decibel;
}

int UpnpControl::set_volume_db(struct action_event* event) {
    const char* str_decibel_in = UpnpDevice::get_instance().upnp_get_string(event, "DesiredVolume");
    service_lock();
    float raw_decibel_in = atof(str_decibel_in);
    float decibel = change_volume_decibel(raw_decibel_in);

    //    Output::get_instance().output_set_volume(exp(decibel / 20 * log(10)));
    Output::get_instance().output_set_volume(decibel);
    service_unlock();

    return 0;
}

int UpnpControl::set_volume(struct action_event* event) {
    const char* volume = UpnpDevice::get_instance().upnp_get_string(event, "DesiredVolume");
    service_lock();
    int volume_level = atoi(volume);  // range 0..100

    if (volume_level < volume_range.min) {
        volume_level = volume_range.min;
    }

    if (volume_level > volume_range.max) {
        volume_level = volume_range.max;
    }

    const float decibel = volume_level_to_decibel(volume_level);

    char db_volume[10];
    snprintf(db_volume, sizeof(db_volume), "%d", (int)(256 * decibel));

    const double fraction = exp(decibel / 20 * log(10));
    dbg("set_volume to %.2fdb == #%.2f", decibel, fraction);
    change_volume(volume, db_volume);
    //    Output::get_instance().output_set_volume(fraction);
    Output::get_instance().output_set_volume(volume_level);
    set_mute_toggle(volume_level == 0);
    service_unlock();

    return 0;
}

int UpnpControl::get_volume_db(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_VOLUME_DB,
                               "CurrentVolumeDB");
}

int UpnpControl::get_volume_dbrange(struct action_event* event) {
    char minval[16];
    snprintf(minval, sizeof(minval), "%lld", volume_db_range.min);
    UpnpDevice::get_instance().upnp_add_response(event, "MinValue", minval);
    UpnpDevice::get_instance().upnp_add_response(event, "MaxValue", "0");
    return 0;
}

int UpnpControl::get_loudness(struct action_event* event) {
    return cmd_obtain_variable(event, CONTROL_VAR_LOUDNESS,
                               "CurrentLoudness");
}

}
}




