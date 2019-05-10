////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRUPNPTRANSPORT_H
#define DUEROS_DCS_APP_DLNA_DLNADMRUPNPTRANSPORT_H

#include "DlnaDmrVariableContainer.h"
#include "DlnaDmrOutput.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
const int TRANSPORT_VARIABLE_T_MAX_NUM = 32;
typedef enum {
    TRANSPORT_VAR_TRANSPORT_STATUS,
    TRANSPORT_VAR_NEXT_AV_URI,
    TRANSPORT_VAR_NEXT_AV_URI_META,
    TRANSPORT_VAR_CUR_TRACK_META,
    TRANSPORT_VAR_REL_CTR_POS,
    TRANSPORT_VAR_AAT_INSTANCE_ID,
    TRANSPORT_VAR_AAT_SEEK_TARGET,
    TRANSPORT_VAR_PLAY_MEDIUM,
    TRANSPORT_VAR_REL_TIME_POS,
    TRANSPORT_VAR_REC_MEDIA,
    TRANSPORT_VAR_CUR_PLAY_MODE,
    TRANSPORT_VAR_TRANSPORT_PLAY_SPEED,
    TRANSPORT_VAR_PLAY_MEDIA,
    TRANSPORT_VAR_ABS_TIME_POS,
    TRANSPORT_VAR_CUR_TRACK,
    TRANSPORT_VAR_CUR_TRACK_URI,
    TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS,
    TRANSPORT_VAR_NR_TRACKS,
    TRANSPORT_VAR_AV_URI,
    TRANSPORT_VAR_ABS_CTR_POS,
    TRANSPORT_VAR_CUR_REC_QUAL_MODE,
    TRANSPORT_VAR_CUR_MEDIA_DUR,
    TRANSPORT_VAR_AAT_SEEK_MODE,
    TRANSPORT_VAR_AV_URI_META,
    TRANSPORT_VAR_REC_MEDIUM,
    TRANSPORT_VAR_REC_MEDIUM_WR_STATUS,
    TRANSPORT_VAR_LAST_CHANGE,
    TRANSPORT_VAR_CUR_TRACK_DUR,
    TRANSPORT_VAR_TRANSPORT_STATE,
    TRANSPORT_VAR_POS_REC_QUAL_MODE,
    TRANSPORT_VAR_UNKNOWN,
    TRANSPORT_VAR_COUNT
} transport_variable_t;

const int TRANSPORT_CMD_T_MAX_NUM = 19;
enum {
    TRANSPORT_CMD_GETCURRENTTRANSPORTACTIONS,
    TRANSPORT_CMD_GETDEVICECAPABILITIES,
    TRANSPORT_CMD_GETMEDIAINFO,
    TRANSPORT_CMD_GETPOSITIONINFO,
    TRANSPORT_CMD_GETTRANSPORTINFO,
    TRANSPORT_CMD_GETTRANSPORTSETTINGS,
    //TRANSPORT_CMD_NEXT,
    TRANSPORT_CMD_PAUSE,
    TRANSPORT_CMD_PLAY,
    //TRANSPORT_CMD_PREVIOUS,
    TRANSPORT_CMD_SEEK,
    TRANSPORT_CMD_SETAVTRANSPORTURI,
    //TRANSPORT_CMD_SETPLAYMODE,
    TRANSPORT_CMD_STOP,
    TRANSPORT_CMD_SETNEXTAVTRANSPORTURI,
    //TRANSPORT_CMD_RECORD,
    //TRANSPORT_CMD_SETRECORDQUALITYMODE,
    TRANSPORT_CMD_UNKNOWN,
    TRANSPORT_CMD_COUNT
};

enum UPNPTransportError {
    UPNP_TRANSPORT_E_TRANSITION_NA = 701,
    UPNP_TRANSPORT_E_NO_CONTENTS = 702,
    UPNP_TRANSPORT_E_READ_ERROR = 703,
    UPNP_TRANSPORT_E_PLAY_FORMAT_NS = 704,
    UPNP_TRANSPORT_E_TRANSPORT_LOCKED = 705,
    UPNP_TRANSPORT_E_WRITE_ERROR = 706,
    UPNP_TRANSPORT_E_REC_MEDIA_WP = 707,
    UPNP_TRANSPORT_E_REC_FORMAT_NS = 708,
    UPNP_TRANSPORT_E_REC_MEDIA_FULL = 709,
    UPNP_TRANSPORT_E_SEEKMODE_NS = 710,
    UPNP_TRANSPORT_E_ILL_SEEKTARGET = 711,
    UPNP_TRANSPORT_E_PLAYMODE_NS = 712,
    UPNP_TRANSPORT_E_RECQUAL_NS = 713,
    UPNP_TRANSPORT_E_ILLEGAL_MIME = 714,
    UPNP_TRANSPORT_E_CONTENT_BUSY = 715,
    UPNP_TRANSPORT_E_RES_NOT_FOUND = 716,
    UPNP_TRANSPORT_E_PLAYSPEED_NS = 717,
    UPNP_TRANSPORT_E_INVALID_IID = 718,
};
enum transport_state {
    TRANSPORT_STOPPED,
    TRANSPORT_PLAYING,
    TRANSPORT_TRANSITIONING,    /* optional */
    TRANSPORT_PAUSED_PLAYBACK,    /* optional */
    TRANSPORT_PAUSED_RECORDING,    /* optional */
    TRANSPORT_RECORDING,    /* optional */
    TRANSPORT_NO_MEDIA_PRESENT    /* optional */
};


class UpnpTransport {
public:
    static UpnpTransport& get_instance() {
        static UpnpTransport m_instance;
        return m_instance;
    }

    struct service* upnp_transport_get_service(void);

    void upnp_transport_init(struct upnp_device*);

    void upnp_transport_register_variable_listener(variable_change_listener_t cb,
            void* userdata);

private:
    UpnpTransport();

    static void service_lock(void);

    static void service_unlock(void);

    static int obtain_instanceid(struct action_event* event, int* instance);

    static int get_media_info(struct action_event* event);

    static int replace_var(transport_variable_t varnum, const char* new_value);

    static const char* get_var(transport_variable_t varnum);

    static int replace_transport_uri_and_meta(const char* uri, const char* meta);

    static void replace_current_uri_and_meta(const char* uri, const char* meta);

    static void change_transport_state(enum transport_state new_state);

    static void update_meta_from_stream(const struct SongMetaData* meta);

    static int set_avtransport_uri(struct action_event* event);

    static int set_next_avtransport_uri(struct action_event* event);

    static int get_transport_info(struct action_event* event);

    static int get_current_transportactions(struct action_event* event);

    static int get_transport_settings(struct action_event* event);

    static int divide_leave_remainder(long* val, long divisor);

    static void print_upnp_time(char* result, size_t size, long t);

    static long parse_upnp_time(const char* time_string);

    static void* thread_update_track_time(void* userdata);

    static int get_position_info(struct action_event* event);

    static int get_device_caps(struct action_event* event);

    static int stop(struct action_event* event);

    static void inform_play_transition_from_output(enum PlayFeedback fb);

    static int play(struct action_event* event);

    static int pause_stream(struct action_event* event);

    static int seek(struct action_event* event);

};
}
}
#endif //DLNA_DMR_UPNP_TRANSPORT_H
