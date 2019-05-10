////
// Created by eddy on 18-1-30.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include "Dlna/upnp/upnp.h"
#include "Dlna/upnp/ithread.h"
#include "Dlna/DlnaDmrUpnpTransport.h"
#include "Dlna/DlnaDmrUpnpDevice.h"

const char* TRANSPORT_TYPE = "urn:schemas-upnp-org:service:AVTransport:1";
const char* TRANSPORT_SERVICE_ID = "urn:upnp-org:serviceId:AVTransport";
const char* TRANSPORT_SCPD_URL = "/upnp/rendertransportSCPD.xml";
const char* TRANSPORT_CONTROL_URL = "/upnp/control/rendertransport1";
const char* TRANSPORT_EVENT_URL = "/upnp/event/rendertransport1";
const char* TRANSPORT_EVENT_XML_NS = "urn:schemas-upnp-org:metadata-1-0/AVT/";

namespace duerOSDcsApp {
namespace dueros_dlna {
static const char* transport_variable_names[TRANSPORT_VARIABLE_T_MAX_NUM];
static const char* kZeroTime = "0:00:00";
static const char* transport_default_values[TRANSPORT_VARIABLE_T_MAX_NUM];
static struct var_meta transport_var_meta[TRANSPORT_VARIABLE_T_MAX_NUM];

static const char* transport_states[] = {
    "STOPPED",
    "PLAYING",
    "TRANSITIONING",
    "PAUSED_PLAYBACK",
    "PAUSED_RECORDING",
    "RECORDING",
    "NO_MEDIA_PRESENT",
    NULL
};
static const char* transport_stati[] = {
    "OK",
    "ERROR_OCCURRED",
    " vendor-defined ",
    NULL
};
static const char* media[] = {
    "UNKNOWN",
    "DV",
    "MINI-DV",
    "VHS",
    "W-VHS",
    "S-VHS",
    "D-VHS",
    "VHSC",
    "VIDEO8",
    "HI8",
    "CD-ROM",
    "CD-DA",
    "CD-R",
    "CD-RW",
    "VIDEO-CD",
    "SACD",
    "MD-AUDIO",
    "MD-PICTURE",
    "DVD-ROM",
    "DVD-VIDEO",
    "DVD-R",
    "DVD+RW",
    "DVD-RW",
    "DVD-RAM",
    "DVD-AUDIO",
    "DAT",
    "LD",
    "HDD",
    "MICRO-MV",
    "NETWORK",
    "NONE",
    "NOT_IMPLEMENTED",
    " vendor-defined ",
    NULL
};

static const char* playmodi[] = {
    "NORMAL",
    //"SHUFFLE",
    //"REPEAT_ONE",
    "REPEAT_ALL",
    //"RANDOM",
    //"DIRECT_1",
    "INTRO",
    NULL
};

static const char* playspeeds[] = {
    "1",
    " vendor-defined ",
    NULL
};

static const char* rec_write_stati[] = {
    "WRITABLE",
    "PROTECTED",
    "NOT_WRITABLE",
    "UNKNOWN",
    "NOT_IMPLEMENTED",
    NULL
};

static const char* rec_quality_modi[] = {
    "0:EP",
    "1:LP",
    "2:SP",
    "0:BASIC",
    "1:MEDIUM",
    "2:HIGH",
    "NOT_IMPLEMENTED",
    " vendor-defined ",
    NULL
};

static const char* aat_seekmodi[] = {
    "ABS_TIME",
    "REL_TIME",
    "ABS_COUNT",
    "REL_COUNT",
    "TRACK_NR",
    "CHANNEL_FREQ",
    "TAPE-INDEX",
    "FRAME",
    NULL
};

static struct param_range track_range = {
    0,
    4294967295LL,
    1
};

static struct param_range track_nr_range = {
    0,
    4294967295LL,
    0
};


static struct argument* arguments_setavtransporturi[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentURI", PARAM_DIR_IN, TRANSPORT_VAR_AV_URI},
    new argument{"CurrentURIMetaData", PARAM_DIR_IN, TRANSPORT_VAR_AV_URI_META},
    nullptr
};

static struct argument* arguments_setnextavtransporturi[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"NextURI", PARAM_DIR_IN, TRANSPORT_VAR_NEXT_AV_URI},
    new argument{"NextURIMetaData", PARAM_DIR_IN, TRANSPORT_VAR_NEXT_AV_URI_META},
    nullptr
};

static struct argument* arguments_getmediainfo[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"NrTracks", PARAM_DIR_OUT, TRANSPORT_VAR_NR_TRACKS},
    new argument{"MediaDuration", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_MEDIA_DUR},
    new argument{"CurrentURI", PARAM_DIR_OUT, TRANSPORT_VAR_AV_URI},
    new argument{"CurrentURIMetaData", PARAM_DIR_OUT, TRANSPORT_VAR_AV_URI_META},
    new argument{"NextURI", PARAM_DIR_OUT, TRANSPORT_VAR_NEXT_AV_URI},
    new argument{"NextURIMetaData", PARAM_DIR_OUT, TRANSPORT_VAR_NEXT_AV_URI_META},
    new argument{"PlayMedium", PARAM_DIR_OUT, TRANSPORT_VAR_PLAY_MEDIUM},
    new argument{"RecordMedium", PARAM_DIR_OUT, TRANSPORT_VAR_REC_MEDIUM},
    new argument{"WriteStatus", PARAM_DIR_OUT, TRANSPORT_VAR_REC_MEDIUM_WR_STATUS},
    nullptr
};

static struct argument* arguments_gettransportinfo[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"CurrentTransportState", PARAM_DIR_OUT, TRANSPORT_VAR_TRANSPORT_STATE},
    new argument{"CurrentTransportStatus", PARAM_DIR_OUT,
        TRANSPORT_VAR_TRANSPORT_STATUS
    },
    new argument{"CurrentSpeed", PARAM_DIR_OUT, TRANSPORT_VAR_TRANSPORT_PLAY_SPEED},
    nullptr
};

static struct argument* arguments_getpositioninfo[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"Track", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_TRACK},
    new argument{"TrackDuration", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_TRACK_DUR},
    new argument{"TrackMetaData", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_TRACK_META},
    new argument{"TrackURI", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_TRACK_URI},
    new argument{"RelTime", PARAM_DIR_OUT, TRANSPORT_VAR_REL_TIME_POS},
    new argument{"AbsTime", PARAM_DIR_OUT, TRANSPORT_VAR_ABS_TIME_POS},
    new argument{"RelCount", PARAM_DIR_OUT, TRANSPORT_VAR_REL_CTR_POS},
    new argument{"AbsCount", PARAM_DIR_OUT, TRANSPORT_VAR_ABS_CTR_POS},
    nullptr
};

static struct argument* arguments_getdevicecapabilities[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"PlayMedia", PARAM_DIR_OUT, TRANSPORT_VAR_PLAY_MEDIA},
    new argument{"RecMedia", PARAM_DIR_OUT, TRANSPORT_VAR_REC_MEDIA},
    new argument{"RecQualityModes", PARAM_DIR_OUT, TRANSPORT_VAR_POS_REC_QUAL_MODE},
    nullptr
};

static struct argument* arguments_gettransportsettings[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"PlayMode", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_PLAY_MODE},
    new argument{"RecQualityMode", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_REC_QUAL_MODE},
    nullptr
};

static struct argument* arguments_stop[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    nullptr
};
static struct argument* arguments_play[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"Speed", PARAM_DIR_IN, TRANSPORT_VAR_TRANSPORT_PLAY_SPEED},
    nullptr
};
static struct argument* arguments_pause[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    NULL
};


static struct argument* arguments_seek[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"Unit", PARAM_DIR_IN, TRANSPORT_VAR_AAT_SEEK_MODE},
    new argument{"Target", PARAM_DIR_IN, TRANSPORT_VAR_AAT_SEEK_TARGET},
    nullptr
};

static struct argument* arguments_getcurrenttransportactions[] = {
    new argument{"InstanceID", PARAM_DIR_IN, TRANSPORT_VAR_AAT_INSTANCE_ID},
    new argument{"Actions", PARAM_DIR_OUT, TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS},
    nullptr
};


static enum transport_state transport_state_ = TRANSPORT_STOPPED;
extern struct service transport_service_;   // Defined below.
static variable_container_t* state_variables_ = NULL;


static ithread_mutex_t transport_mutex;
static struct action transport_actions[TRANSPORT_CMD_T_MAX_NUM];


static struct argument** argument_list[TRANSPORT_CMD_T_MAX_NUM];

struct service transport_service_;

UpnpTransport::UpnpTransport() {
    //transport_variable_names
    transport_variable_names[TRANSPORT_VAR_TRANSPORT_STATE] = "TransportState";
    transport_variable_names[TRANSPORT_VAR_TRANSPORT_STATUS] = "TransportStatus";
    transport_variable_names[TRANSPORT_VAR_PLAY_MEDIUM] = "PlaybackStorageMedium";
    transport_variable_names[TRANSPORT_VAR_REC_MEDIUM] = "RecordStorageMedium";
    transport_variable_names[TRANSPORT_VAR_PLAY_MEDIA] = "PossiblePlaybackStorageMedia";
    transport_variable_names[TRANSPORT_VAR_REC_MEDIA] = "PossibleRecordStorageMedia";
    transport_variable_names[TRANSPORT_VAR_CUR_PLAY_MODE] = "CurrentPlayMode";
    transport_variable_names[TRANSPORT_VAR_TRANSPORT_PLAY_SPEED] = "TransportPlaySpeed";
    transport_variable_names[TRANSPORT_VAR_REC_MEDIUM_WR_STATUS] = "RecordMediumWriteStatus";
    transport_variable_names[TRANSPORT_VAR_CUR_REC_QUAL_MODE] = "CurrentRecordQualityMode";
    transport_variable_names[TRANSPORT_VAR_POS_REC_QUAL_MODE] = "PossibleRecordQualityModes";
    transport_variable_names[TRANSPORT_VAR_NR_TRACKS] = "NumberOfTracks";
    transport_variable_names[TRANSPORT_VAR_CUR_TRACK] = "CurrentTrack";
    transport_variable_names[TRANSPORT_VAR_CUR_TRACK_DUR] = "CurrentTrackDuration";
    transport_variable_names[TRANSPORT_VAR_CUR_MEDIA_DUR] = "CurrentMediaDuration";
    transport_variable_names[TRANSPORT_VAR_CUR_TRACK_META] = "CurrentTrackMetaData";
    transport_variable_names[TRANSPORT_VAR_CUR_TRACK_URI] = "CurrentTrackURI";
    transport_variable_names[TRANSPORT_VAR_AV_URI] = "AVTransportURI";
    transport_variable_names[TRANSPORT_VAR_AV_URI_META] = "AVTransportURIMetaData";
    transport_variable_names[TRANSPORT_VAR_NEXT_AV_URI] = "NextAVTransportURI";
    transport_variable_names[TRANSPORT_VAR_NEXT_AV_URI_META] = "NextAVTransportURIMetaData";
    transport_variable_names[TRANSPORT_VAR_REL_TIME_POS] = "RelativeTimePosition";
    transport_variable_names[TRANSPORT_VAR_ABS_TIME_POS] = "AbsoluteTimePosition";
    transport_variable_names[TRANSPORT_VAR_REL_CTR_POS] = "RelativeCounterPosition";
    transport_variable_names[TRANSPORT_VAR_ABS_CTR_POS] = "AbsoluteCounterPosition";
    transport_variable_names[TRANSPORT_VAR_LAST_CHANGE] = "LastChange";
    transport_variable_names[TRANSPORT_VAR_AAT_SEEK_MODE] = "A_ARG_TYPE_SeekMode";
    transport_variable_names[TRANSPORT_VAR_AAT_SEEK_TARGET] = "A_ARG_TYPE_SeekTarget";
    transport_variable_names[TRANSPORT_VAR_AAT_INSTANCE_ID] = "A_ARG_TYPE_InstanceID";
    transport_variable_names[TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS] = "CurrentTransportActions";
    transport_variable_names[TRANSPORT_VAR_UNKNOWN] = nullptr;

    //transport_default_values
    transport_default_values[TRANSPORT_VAR_TRANSPORT_STATE] = "STOPPED";
    transport_default_values[TRANSPORT_VAR_TRANSPORT_STATUS] = "OK";
    transport_default_values[TRANSPORT_VAR_PLAY_MEDIUM] = "UNKNOWN";
    transport_default_values[TRANSPORT_VAR_REC_MEDIUM] = "NOT_IMPLEMENTED";
    transport_default_values[TRANSPORT_VAR_PLAY_MEDIA] = "NETWORK,UNKNOWN";
    transport_default_values[TRANSPORT_VAR_REC_MEDIA] = "NOT_IMPLEMENTED";
    transport_default_values[TRANSPORT_VAR_CUR_PLAY_MODE] = "NORMAL";
    transport_default_values[TRANSPORT_VAR_TRANSPORT_PLAY_SPEED] = "1";
    transport_default_values[TRANSPORT_VAR_REC_MEDIUM_WR_STATUS] = "NOT_IMPLEMENTED";
    transport_default_values[TRANSPORT_VAR_CUR_REC_QUAL_MODE] = "NOT_IMPLEMENTED";
    transport_default_values[TRANSPORT_VAR_POS_REC_QUAL_MODE] = "NOT_IMPLEMENTED";
    transport_default_values[TRANSPORT_VAR_NR_TRACKS] = "0";
    transport_default_values[TRANSPORT_VAR_CUR_TRACK] = "0";
    transport_default_values[TRANSPORT_VAR_CUR_TRACK_DUR] = kZeroTime;
    transport_default_values[TRANSPORT_VAR_CUR_MEDIA_DUR] = "";
    transport_default_values[TRANSPORT_VAR_CUR_TRACK_META] = "";
    transport_default_values[TRANSPORT_VAR_CUR_TRACK_URI] = "";
    transport_default_values[TRANSPORT_VAR_AV_URI] = "";
    transport_default_values[TRANSPORT_VAR_AV_URI_META] = "";
    transport_default_values[TRANSPORT_VAR_NEXT_AV_URI] = "";
    transport_default_values[TRANSPORT_VAR_NEXT_AV_URI_META] = "";
    transport_default_values[TRANSPORT_VAR_REL_TIME_POS] = kZeroTime;
    transport_default_values[TRANSPORT_VAR_ABS_TIME_POS] = "NOT_IMPLEMENTED";
    transport_default_values[TRANSPORT_VAR_REL_CTR_POS] = "2147483647";
    transport_default_values[TRANSPORT_VAR_ABS_CTR_POS] = "2147483647";
    transport_default_values[TRANSPORT_VAR_LAST_CHANGE] =
        "<Event xmlns=\"urn:schemas-upnp-org:metadata-1-0/AVT/\"/>";
    transport_default_values[TRANSPORT_VAR_AAT_SEEK_MODE] = "TRACK_NR";
    transport_default_values[TRANSPORT_VAR_AAT_SEEK_TARGET] = "";
    transport_default_values[TRANSPORT_VAR_AAT_INSTANCE_ID] = "0";
    transport_default_values[TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS] = "PLAY";
    transport_default_values[TRANSPORT_VAR_UNKNOWN] = nullptr;

    //transport_var_meta
    transport_var_meta[TRANSPORT_VAR_TRANSPORT_STATE] = {
        SENDEVENT_NO, DATATYPE_STRING, transport_states,
        NULL
    };
    transport_var_meta[TRANSPORT_VAR_TRANSPORT_STATUS] = {
        SENDEVENT_NO, DATATYPE_STRING, transport_stati,
        NULL
    };
    transport_var_meta[TRANSPORT_VAR_PLAY_MEDIUM] = {SENDEVENT_NO, DATATYPE_STRING, media, NULL};
    transport_var_meta[TRANSPORT_VAR_REC_MEDIUM] = {SENDEVENT_NO, DATATYPE_STRING, media, NULL};
    transport_var_meta[TRANSPORT_VAR_PLAY_MEDIA] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_REC_MEDIA] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_CUR_PLAY_MODE] = {
        SENDEVENT_NO, DATATYPE_STRING, playmodi, NULL,
        "NORMAL"
    };
    transport_var_meta[TRANSPORT_VAR_TRANSPORT_PLAY_SPEED] = {
        SENDEVENT_NO, DATATYPE_STRING, playspeeds,
        NULL
    };
    transport_var_meta[TRANSPORT_VAR_REC_MEDIUM_WR_STATUS] = {SENDEVENT_NO, DATATYPE_STRING,
                                                              rec_write_stati, NULL
                                                             };
    transport_var_meta[TRANSPORT_VAR_CUR_REC_QUAL_MODE] = {
        SENDEVENT_NO, DATATYPE_STRING, rec_quality_modi,
        NULL
    };
    transport_var_meta[TRANSPORT_VAR_POS_REC_QUAL_MODE] =
    {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL},
    transport_var_meta[TRANSPORT_VAR_NR_TRACKS] = {SENDEVENT_NO, DATATYPE_UI4, NULL,
                                                   &track_nr_range
                                                  }; /* no step */
    transport_var_meta[TRANSPORT_VAR_CUR_TRACK] = {SENDEVENT_NO, DATATYPE_UI4, NULL, &track_range};
    transport_var_meta[TRANSPORT_VAR_CUR_TRACK_DUR] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_CUR_MEDIA_DUR] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_CUR_TRACK_META] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_CUR_TRACK_URI] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_AV_URI] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_AV_URI_META] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_NEXT_AV_URI] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_NEXT_AV_URI_META] =
    {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_REL_TIME_POS] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_ABS_TIME_POS] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_REL_CTR_POS] = {SENDEVENT_NO, DATATYPE_I4, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_ABS_CTR_POS] = {SENDEVENT_NO, DATATYPE_I4, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_LAST_CHANGE] = {SENDEVENT_YES, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_AAT_SEEK_MODE] = {SENDEVENT_NO, DATATYPE_STRING, aat_seekmodi,
                                                       NULL
                                                      };
    transport_var_meta[TRANSPORT_VAR_AAT_SEEK_TARGET] = {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_AAT_INSTANCE_ID] = {SENDEVENT_NO, DATATYPE_UI4, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS] =
    {SENDEVENT_NO, DATATYPE_STRING, NULL, NULL};
    transport_var_meta[TRANSPORT_VAR_UNKNOWN] = {SENDEVENT_NO, DATATYPE_UNKNOWN, NULL, NULL};

    //argument_list
    argument_list[TRANSPORT_CMD_SETAVTRANSPORTURI] = arguments_setavtransporturi;
    argument_list[TRANSPORT_CMD_GETDEVICECAPABILITIES] = arguments_getdevicecapabilities;
    argument_list[TRANSPORT_CMD_GETMEDIAINFO] = arguments_getmediainfo;
    argument_list[TRANSPORT_CMD_SETNEXTAVTRANSPORTURI] = arguments_setnextavtransporturi;
    argument_list[TRANSPORT_CMD_GETTRANSPORTINFO] = arguments_gettransportinfo;
    argument_list[TRANSPORT_CMD_GETPOSITIONINFO] = arguments_getpositioninfo;
    argument_list[TRANSPORT_CMD_GETTRANSPORTSETTINGS] = arguments_gettransportsettings;
    argument_list[TRANSPORT_CMD_STOP] = arguments_stop;
    argument_list[TRANSPORT_CMD_PLAY] = arguments_play;
    argument_list[TRANSPORT_CMD_PAUSE] = arguments_pause;
    //[TRANSPORT_CMD_RECORD] =                    arguments_record,
    argument_list[TRANSPORT_CMD_SEEK] = arguments_seek;
    //[TRANSPORT_CMD_NEXT] =                      arguments_next,
    //[TRANSPORT_CMD_PREVIOUS] =                  arguments_previous,
    //[TRANSPORT_CMD_SETPLAYMODE] =               arguments_setplaymode,
    //[TRANSPORT_CMD_SETRECORDQUALITYMODE] =      arguments_setrecordqualitymode,
    argument_list[TRANSPORT_CMD_GETCURRENTTRANSPORTACTIONS] = arguments_getcurrenttransportactions;
    argument_list[TRANSPORT_CMD_UNKNOWN] = nullptr;

    //transport_actions
    transport_actions[TRANSPORT_CMD_GETCURRENTTRANSPORTACTIONS] = {"GetCurrentTransportActions",
                                                                   get_current_transportactions
                                                                  };
    transport_actions[TRANSPORT_CMD_GETDEVICECAPABILITIES] =
    {"GetDeviceCapabilities", get_device_caps};
    transport_actions[TRANSPORT_CMD_GETMEDIAINFO] = {"GetMediaInfo", get_media_info};
    transport_actions[TRANSPORT_CMD_SETAVTRANSPORTURI] = {"SetAVTransportURI",
                                                          set_avtransport_uri
                                                         };    /* RC9800i */
    transport_actions[TRANSPORT_CMD_SETNEXTAVTRANSPORTURI] = {"SetNextAVTransportURI",
                                                              set_next_avtransport_uri
                                                             };
    transport_actions[TRANSPORT_CMD_GETTRANSPORTINFO] = {"GetTransportInfo", get_transport_info};
    transport_actions[TRANSPORT_CMD_GETPOSITIONINFO] = {"GetPositionInfo", get_position_info};
    transport_actions[TRANSPORT_CMD_GETTRANSPORTSETTINGS] = {"GetTransportSettings",
                                                             get_transport_settings
                                                            };
    transport_actions[TRANSPORT_CMD_STOP] = {"Stop", stop};
    transport_actions[TRANSPORT_CMD_PLAY] = {"Play", play};
    transport_actions[TRANSPORT_CMD_PAUSE] = {"Pause", pause_stream};
    //[TRANSPORT_CMD_RECORD] =                    {"Record", NULL}, /* optional */
    transport_actions[TRANSPORT_CMD_SEEK] = {"Seek", seek};
    //[TRANSPORT_CMD_NEXT] =                      {"Next", next},
    //[TRANSPORT_CMD_PREVIOUS] =                  {"Previous", previous},
    //[TRANSPORT_CMD_SETPLAYMODE] =               {"SetPlayMode", NULL},    /* optional */
    //[TRANSPORT_CMD_SETRECORDQUALITYMODE] =      {"SetRecordQualityMode", NULL},   /* optional */
    transport_actions[TRANSPORT_CMD_UNKNOWN] = {nullptr, nullptr};

    //transport_service_
    transport_service_.service_id = TRANSPORT_SERVICE_ID;
    transport_service_.service_type = TRANSPORT_TYPE;
    transport_service_.scpd_url = TRANSPORT_SCPD_URL;
    transport_service_.control_url = TRANSPORT_CONTROL_URL;
    transport_service_.event_url = TRANSPORT_EVENT_URL;
    transport_service_.event_xml_ns = TRANSPORT_EVENT_XML_NS;
    transport_service_.actions = transport_actions;
    transport_service_.action_arguments = argument_list;
    transport_service_.variable_names = transport_variable_names;
    transport_service_.variable_container = NULL; // set later.
    transport_service_.last_change = NULL;
    transport_service_.variable_meta = transport_var_meta;
    transport_service_.variable_count = TRANSPORT_VAR_UNKNOWN;
    transport_service_.command_count = TRANSPORT_CMD_UNKNOWN;
    transport_service_.service_mutex = &transport_mutex;
}

struct service* UpnpTransport::upnp_transport_get_service(void) {
    if (transport_service_.variable_container == NULL) {
        state_variables_ =
            VariableContainer::get_instance().variablecontainer_new(TRANSPORT_VAR_COUNT,
                    transport_variable_names,
                    transport_default_values);
        transport_service_.variable_container = state_variables_;
    }

    return &transport_service_;
}

void UpnpTransport::upnp_transport_init(struct upnp_device* device) {
    assert(transport_service_.last_change == NULL);
    transport_service_.last_change =
        VariableContainer::get_instance().upnp_last_change_collector_new(state_variables_,
                TRANSPORT_EVENT_XML_NS,
                device,
                TRANSPORT_SERVICE_ID);

    VariableContainer::get_instance()
    .upnp_last_change_collector_add_ignore(transport_service_.last_change,
                                        TRANSPORT_VAR_REL_TIME_POS);
    VariableContainer::get_instance()
    .upnp_last_change_collector_add_ignore(transport_service_.last_change,
                                        TRANSPORT_VAR_ABS_TIME_POS);
    VariableContainer::get_instance()
    .upnp_last_change_collector_add_ignore(transport_service_.last_change,
                                        TRANSPORT_VAR_REL_CTR_POS);
    VariableContainer::get_instance()
    .upnp_last_change_collector_add_ignore(transport_service_.last_change,
                                        TRANSPORT_VAR_ABS_CTR_POS);

    pthread_t thread;
    pthread_create(&thread, NULL, thread_update_track_time, NULL);
}

void UpnpTransport::upnp_transport_register_variable_listener(variable_change_listener_t cb,
        void* userdata) {
    VariableContainer::get_instance()
    .variablecontainer_register_callback(state_variables_, cb, userdata);
}


void UpnpTransport::service_lock(void) {
    ithread_mutex_lock(&transport_mutex);

    if (transport_service_.last_change) {
        VariableContainer::get_instance()
        .upnp_last_change_collector_start(transport_service_.last_change);
    }
}

void UpnpTransport::service_unlock(void) {
    if (transport_service_.last_change) {
        VariableContainer::get_instance()
        .upnp_last_change_collector_finish(transport_service_.last_change);
    }

    ithread_mutex_unlock(&transport_mutex);
}

int UpnpTransport::obtain_instanceid(struct action_event* event, int* instance) {
    const char* value = UpnpDevice::get_instance().upnp_get_string(event, "InstanceID");

    if (value == NULL) {
        UpnpDevice::get_instance().upnp_set_error(event, UPNP_SOAP_E_INVALID_ARGS,
                "Missing InstanceID");
        return -1;
    }

    // TODO - parse value, and store in *instance, if instance!=NULL

    return 0;
}

int UpnpTransport::get_media_info(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_NR_TRACKS, "NrTracks");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_CUR_MEDIA_DUR,
            "MediaDuration");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_AV_URI, "CurrentURI");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_AV_URI_META,
            "CurrentURIMetaData");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_NEXT_AV_URI, "NextURI");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_NEXT_AV_URI_META,
            "NextURIMetaData");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_REC_MEDIA, "PlayMedium");
    UpnpDevice::get_instance()
    .upnp_append_variable(event, TRANSPORT_VAR_REC_MEDIUM, "RecordMedium");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_REC_MEDIUM_WR_STATUS,
            "WriteStatus");
    return 0;
}

int UpnpTransport::replace_var(transport_variable_t varnum, const char* new_value) {
    return VariableContainer::get_instance()
           .variablecontainer_change(state_variables_, varnum, new_value);
}

const char* UpnpTransport::get_var(transport_variable_t varnum) {
    return VariableContainer::get_instance().variablecontainer_get(state_variables_, varnum, NULL);
}

int UpnpTransport::replace_transport_uri_and_meta(const char* uri, const char* meta) {
    replace_var(TRANSPORT_VAR_AV_URI, uri);
    replace_var(TRANSPORT_VAR_AV_URI_META, meta);

    const char* tracks = (uri != NULL && strlen(uri) > 0) ? "1" : "0";
    replace_var(TRANSPORT_VAR_NR_TRACKS, tracks);

    const int requires_stream_meta_callback = (strlen(meta) == 0)
            || strstr(meta, "object.item.audioItem");
    return requires_stream_meta_callback;
}

void UpnpTransport::replace_current_uri_and_meta(const char* uri, const char* meta) {
    const char* tracks = (uri != NULL && strlen(uri) > 0) ? "1" : "0";
    replace_var(TRANSPORT_VAR_CUR_TRACK, tracks);
    replace_var(TRANSPORT_VAR_CUR_TRACK_URI, uri);
    replace_var(TRANSPORT_VAR_CUR_TRACK_META, meta);
}

void UpnpTransport::change_transport_state(enum transport_state new_state) {
    transport_state_ = new_state;
    assert(new_state >= TRANSPORT_STOPPED
           && new_state <= TRANSPORT_NO_MEDIA_PRESENT);

    if (!replace_var(TRANSPORT_VAR_TRANSPORT_STATE,
                     transport_states[new_state])) {
        return;  // no change.
    }

    const char* available_actions = NULL;

    switch (new_state) {
    case TRANSPORT_STOPPED:
        if (strlen(get_var(TRANSPORT_VAR_AV_URI)) == 0) {
            available_actions = "PLAY";
        } else {
            available_actions = "PLAY,SEEK";
        }

        break;

    case TRANSPORT_PLAYING:
        available_actions = "PAUSE,STOP,SEEK";
        break;

    case TRANSPORT_PAUSED_PLAYBACK:
        available_actions = "PLAY,STOP,SEEK";
        break;

    case TRANSPORT_NO_MEDIA_PRESENT:
        available_actions = "PLAY";
        break;

    case TRANSPORT_TRANSITIONING:
    case TRANSPORT_PAUSED_RECORDING:
    case TRANSPORT_RECORDING:
//    case TRANSPORT_NO_MEDIA_PRESENT:
        // We should not switch to this state.
        break;
    }

    if (available_actions) {
        replace_var(TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS,
                    available_actions);
    }
}

void UpnpTransport::update_meta_from_stream(const struct SongMetaData* meta) {
    if (meta->title == NULL || strlen(meta->title) == 0) {
        return;
    }

    const char* original_xml = get_var(TRANSPORT_VAR_AV_URI_META);
    char* didl = SongMetaDataC::get_instance().songmetadata_to_didl(meta, original_xml);
    service_lock();
    replace_var(TRANSPORT_VAR_AV_URI_META, didl);
    replace_var(TRANSPORT_VAR_CUR_TRACK_META, didl);
    service_unlock();
    free(didl);
}

int UpnpTransport::set_avtransport_uri(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    const char* uri = UpnpDevice::get_instance().upnp_get_string(event, "CurrentURI");

    if (uri == NULL) {
        return -1;
    }

    service_lock();
    const char* meta = UpnpDevice::get_instance().upnp_get_string(event, "CurrentURIMetaData");
    int requires_meta_update = replace_transport_uri_and_meta(uri, meta);

    if (transport_state_ == TRANSPORT_PLAYING) {
        replace_current_uri_and_meta(uri, meta);
    }

    Output::get_instance().output_set_uri(uri, (requires_meta_update
                                          ? update_meta_from_stream
                                          : NULL));
    service_unlock();

    return 0;
}

int UpnpTransport::set_next_avtransport_uri(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    const char* next_uri = UpnpDevice::get_instance().upnp_get_string(event, "NextURI");

    if (next_uri == NULL) {
        return -1;
    }

    int rc = 0;
    service_lock();

    Output::get_instance().output_set_next_uri(next_uri);
    replace_var(TRANSPORT_VAR_NEXT_AV_URI, next_uri);

    const char
    * next_uri_meta = UpnpDevice::get_instance().upnp_get_string(event, "NextURIMetaData");

    if (next_uri_meta == NULL) {
        rc = -1;
    } else {
        replace_var(TRANSPORT_VAR_NEXT_AV_URI_META, next_uri_meta);
    }

    service_unlock();

    return rc;
}

int UpnpTransport::get_transport_info(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_TRANSPORT_STATE,
            "CurrentTransportState");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_TRANSPORT_STATUS,
            "CurrentTransportStatus");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_TRANSPORT_PLAY_SPEED,
            "CurrentSpeed");
    return 0;
}

int UpnpTransport::get_current_transportactions(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS,
            "Actions");
    return 0;
}

int UpnpTransport::get_transport_settings(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    // TODO: what variables to add ?
    return 0;
}

int UpnpTransport::divide_leave_remainder(long* val, long divisor) {
    int result = *val / divisor;
    *val %= divisor;
    return result;
}

void UpnpTransport::print_upnp_time(char* result, size_t size, long t) {
    const long one_sec = 1000000000LL;  // units are in nanoseconds.
    const int hour = divide_leave_remainder(&t, 3600LL * one_sec);
    const int minute = divide_leave_remainder(&t, 60LL * one_sec);
    const int second = divide_leave_remainder(&t, one_sec);
    snprintf(result, size, "%d:%02d:%02d", hour, minute, second);
}

long UpnpTransport::parse_upnp_time(const char* time_string) {
    int hour = 0;
    int minute = 0;
    int second = 0;
    sscanf(time_string, "%d:%02d:%02d", &hour, &minute, &second);
    const long seconds = (hour * 3600 + minute * 60 + second);
    const long one_sec_unit = 1000000000LL;
    //  return seconds;
    return one_sec_unit * seconds;
}

void* UpnpTransport::thread_update_track_time(void* userdata) {
    const long one_sec_unit = 1000000000LL;
    char tbuf[32];
    long last_duration = -1; 
    long last_position = -1;

    for (;;) {
        usleep(1000000);  // 500ms
        service_lock();
        long duration = 0; 
        long position = 0;
        const int pos_result = Output::get_instance().output_get_position(&duration, &position);

        if (pos_result == 0) {
            //Logging::get_instance().Log_info("transport", "duration:%ld, position:%ld, last_position%ld",
            //                           duration, position, last_position);
            if (duration != last_duration) {
                print_upnp_time(tbuf, sizeof(tbuf), duration);
                replace_var(TRANSPORT_VAR_CUR_TRACK_DUR, tbuf);
                last_duration = duration;
            }

            if (position / one_sec_unit != last_position) {
                print_upnp_time(tbuf, sizeof(tbuf), position);
                replace_var(TRANSPORT_VAR_REL_TIME_POS, tbuf);
                last_position = position / one_sec_unit;
            }
        }

        service_unlock();
    }

    return NULL;  // not reached.
}

int UpnpTransport::get_position_info(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_CUR_TRACK, "Track");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_CUR_TRACK_DUR,
            "TrackDuration");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_CUR_TRACK_META,
            "TrackMetaData");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_CUR_TRACK_URI, "TrackURI");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_REL_TIME_POS, "RelTime");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_ABS_TIME_POS, "AbsTime");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_REL_CTR_POS, "RelCount");
    UpnpDevice::get_instance().upnp_append_variable(event, TRANSPORT_VAR_ABS_CTR_POS, "AbsCount");

    return 0;
}

int UpnpTransport::get_device_caps(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    // TODO: implement ?
    return 0;
}

int UpnpTransport::stop(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    service_lock();

    switch (transport_state_) {
    case TRANSPORT_STOPPED:
        // nothing to change.
        break;

    case TRANSPORT_PLAYING:
    case TRANSPORT_TRANSITIONING:
    case TRANSPORT_PAUSED_RECORDING:
    case TRANSPORT_RECORDING:
    case TRANSPORT_PAUSED_PLAYBACK:
        Output::get_instance().output_stop();
        change_transport_state(TRANSPORT_STOPPED);
        break;

    case TRANSPORT_NO_MEDIA_PRESENT:
        /* action not allowed in these states - error 701 */
        UpnpDevice::get_instance().upnp_set_error(event, UPNP_TRANSPORT_E_TRANSITION_NA,
                "Transition to STOP not allowed; allowed=%s",
                get_var(TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS));

        break;
    }

    service_unlock();

    return 0;
}

void UpnpTransport::inform_play_transition_from_output(enum PlayFeedback fb) {
    service_lock();

    switch (fb) {
    case PLAY_STOPPED:
        replace_transport_uri_and_meta("", "");
        replace_current_uri_and_meta("", "");
        change_transport_state(TRANSPORT_STOPPED);
        break;

    case PLAY_PAUSED_PLAYBACK:
        replace_transport_uri_and_meta("", "");
        replace_current_uri_and_meta("", "");
        change_transport_state(TRANSPORT_PAUSED_PLAYBACK);
        break;

    case PLAY_PLAYING:
        replace_transport_uri_and_meta("", "");
        replace_current_uri_and_meta("", "");
        change_transport_state(TRANSPORT_PLAYING);
        break;

    case PLAY_STARTED_NEXT_STREAM: {
        const char* av_uri = get_var(TRANSPORT_VAR_NEXT_AV_URI);
        const char* av_meta = get_var(TRANSPORT_VAR_NEXT_AV_URI_META);
        replace_transport_uri_and_meta(av_uri, av_meta);
        replace_current_uri_and_meta(av_uri, av_meta);
        replace_var(TRANSPORT_VAR_NEXT_AV_URI, "");
        replace_var(TRANSPORT_VAR_NEXT_AV_URI_META, "");
        break;
    }

    case PLAY_NO_MEDIA_PRESENT:
        replace_transport_uri_and_meta("", "");
        replace_current_uri_and_meta("", "");
        change_transport_state(TRANSPORT_NO_MEDIA_PRESENT);
        break;

    }

    service_unlock();
}

int UpnpTransport::play(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    int rc = 0;
    service_lock();

    switch (transport_state_) {
    case TRANSPORT_PLAYING:
        break;

    case TRANSPORT_STOPPED:
    case TRANSPORT_NO_MEDIA_PRESENT:
        replace_var(TRANSPORT_VAR_REL_TIME_POS, kZeroTime);

    case TRANSPORT_PAUSED_PLAYBACK:
        if (Output::get_instance().output_play(&inform_play_transition_from_output)) {
            UpnpDevice::get_instance().upnp_set_error(event, 704, "Playing failed");
            rc = -1;
        } else {
            change_transport_state(TRANSPORT_PLAYING);
            const char* av_uri = get_var(TRANSPORT_VAR_AV_URI);
            const char* av_meta = get_var(TRANSPORT_VAR_AV_URI_META);
            replace_current_uri_and_meta(av_uri, av_meta);
        }

        break;

    case TRANSPORT_TRANSITIONING:
    case TRANSPORT_PAUSED_RECORDING:
    case TRANSPORT_RECORDING:
        UpnpDevice::get_instance().upnp_set_error(event, UPNP_TRANSPORT_E_TRANSITION_NA,
                "Transition to PLAY not allowed; allowed=%s",
                get_var(TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS));
        rc = -1;
        break;
    }

    service_unlock();

    return rc;
}

int UpnpTransport::pause_stream(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    int rc = 0;
    service_lock();

    switch (transport_state_) {
    case TRANSPORT_PAUSED_PLAYBACK:
        break;

    case TRANSPORT_PLAYING:
        if (Output::get_instance().output_pause()) {
            UpnpDevice::get_instance().upnp_set_error(event, 704, "Pause failed");
            rc = -1;
        } else {
            change_transport_state(TRANSPORT_PAUSED_PLAYBACK);
        }

        break;

    default:
        UpnpDevice::get_instance().upnp_set_error(event, UPNP_TRANSPORT_E_TRANSITION_NA,
                "Transition to PAUSE not allowed; allowed=%s",
                get_var(TRANSPORT_VAR_CUR_TRANSPORT_ACTIONS));
        rc = -1;
    }

    service_unlock();

    return rc;
}

int UpnpTransport::seek(struct action_event* event) {
    if (obtain_instanceid(event, NULL) < 0) {
        return -1;
    }

    const char* unit = UpnpDevice::get_instance().upnp_get_string(event, "Unit");

    if (strcmp(unit, "REL_TIME") == 0) {
        const char* target = UpnpDevice::get_instance().upnp_get_string(event, "Target");
        long nanos = parse_upnp_time(target);
        service_lock();

        if (Output::get_instance().output_seek(nanos) == 0) {
            replace_var(TRANSPORT_VAR_REL_TIME_POS, target);
        }

        service_unlock();
    }

    return 0;
}


}
}















