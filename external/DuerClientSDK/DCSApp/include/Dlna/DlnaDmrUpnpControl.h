////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRUPNPCONTROL_H
#define DUEROS_DCS_APP_DLNA_DLNADMRUPNPCONTROL_H

#include "DlnaDmrVariableContainer.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
const int CONTROL_VARIABLE_T_MAX_NUM = 23;
typedef enum {
    CONTROL_VAR_G_GAIN,
    CONTROL_VAR_B_BLACK,
    CONTROL_VAR_VER_KEYSTONE,
    CONTROL_VAR_G_BLACK,
    CONTROL_VAR_VOLUME,
    CONTROL_VAR_LOUDNESS,
    CONTROL_VAR_AAT_INSTANCE_ID,
    CONTROL_VAR_R_GAIN,
    CONTROL_VAR_COLOR_TEMP,
    CONTROL_VAR_SHARPNESS,
    CONTROL_VAR_AAT_PRESET_NAME,
    CONTROL_VAR_R_BLACK,
    CONTROL_VAR_B_GAIN,
    CONTROL_VAR_MUTE,
    CONTROL_VAR_LAST_CHANGE,
    CONTROL_VAR_AAT_CHANNEL,
    CONTROL_VAR_HOR_KEYSTONE,
    CONTROL_VAR_VOLUME_DB,
    CONTROL_VAR_PRESET_NAME_LIST,
    CONTROL_VAR_CONTRAST,
    CONTROL_VAR_BRIGHTNESS,
    CONTROL_VAR_UNKNOWN,
    CONTROL_VAR_COUNT
} control_variable_t;

const int CONTROL_CMD_MAX_NUM = 37;
typedef enum {
    CONTROL_CMD_GET_BLUE_BLACK,
    CONTROL_CMD_GET_BLUE_GAIN,
    CONTROL_CMD_GET_BRIGHTNESS,
    CONTROL_CMD_GET_COLOR_TEMP,
    CONTROL_CMD_GET_CONTRAST,
    CONTROL_CMD_GET_GREEN_BLACK,
    CONTROL_CMD_GET_GREEN_GAIN,
    CONTROL_CMD_GET_HOR_KEYSTONE,
    CONTROL_CMD_GET_LOUDNESS,
    CONTROL_CMD_GET_MUTE,
    CONTROL_CMD_GET_RED_BLACK,
    CONTROL_CMD_GET_RED_GAIN,
    CONTROL_CMD_GET_SHARPNESS,
    CONTROL_CMD_GET_VERT_KEYSTONE,
    CONTROL_CMD_GET_VOL,
    CONTROL_CMD_GET_VOL_DB,
    CONTROL_CMD_GET_VOL_DBRANGE,
    CONTROL_CMD_LIST_PRESETS,
    //CONTROL_CMD_SELECT_PRESET,
    //CONTROL_CMD_SET_BLUE_BLACK,
    //CONTROL_CMD_SET_BLUE_GAIN,
    //CONTROL_CMD_SET_BRIGHTNESS,
    //CONTROL_CMD_SET_COLOR_TEMP,
    //CONTROL_CMD_SET_CONTRAST,
    //CONTROL_CMD_SET_GREEN_BLACK,
    //CONTROL_CMD_SET_GREEN_GAIN,
    //CONTROL_CMD_SET_HOR_KEYSTONE,
    //CONTROL_CMD_SET_LOUDNESS,
    CONTROL_CMD_SET_MUTE,
    //CONTROL_CMD_SET_RED_BLACK,
    //CONTROL_CMD_SET_RED_GAIN,
    //CONTROL_CMD_SET_SHARPNESS,
    //CONTROL_CMD_SET_VERT_KEYSTONE,
    CONTROL_CMD_SET_VOL,
    CONTROL_CMD_SET_VOL_DB,
    CONTROL_CMD_UNKNOWN,
    CONTROL_CMD_COUNT
} control_cmd;


class UpnpControl {
public:
    static UpnpControl& get_instance() {
        static UpnpControl m_instance;
        return m_instance;
    }

    void upnp_control_init(struct upnp_device* device);

    struct service* upnp_control_get_service(void);

    void upnp_control_register_variable_listener(variable_change_listener_t cb,
            void* userdata);

private:
    UpnpControl();

    static void service_lock(void);

    static void service_unlock(void);

    static void replace_var(control_variable_t varnum, const char* new_value);

    static void change_volume(const char* volume, const char* db_volume);

    static int cmd_obtain_variable(struct action_event* event,
                                   control_variable_t varnum,
                                   const char* paramname);

    static int list_presets(struct action_event* event);

    static int get_brightness(struct action_event* event);

    static int get_contrast(struct action_event* event);

    static int get_sharpness(struct action_event* event);

    static int get_red_videogain(struct action_event* event);

    static int get_green_videogain(struct action_event* event);

    static int get_blue_videogain(struct action_event* event);

    static int get_red_videoblacklevel(struct action_event* event);

    static int get_green_videoblacklevel(struct action_event* event);

    static int get_blue_videoblacklevel(struct action_event* event);

    static int get_colortemperature(struct action_event* event);

    static int get_horizontal_keystone(struct action_event* event);

    static int get_vertical_keystone(struct action_event* event);

    static int get_mute(struct action_event* event);

    static void set_mute_toggle(int do_mute);

    static int set_mute(struct action_event* event);

    static int get_volume(struct action_event* event);

    static float volume_level_to_decibel(int volume);

    static int volume_decibel_to_level(float decibel);

    static float change_volume_decibel(float raw_decibel);

    static int set_volume_db(struct action_event* event);

    static int set_volume(struct action_event* event);

    static int get_volume_db(struct action_event* event);

    static int get_volume_dbrange(struct action_event* event);

    static int get_loudness(struct action_event* event);
};
}
}
#endif //DLNA_DMR_UPNP_CONTROL_H
