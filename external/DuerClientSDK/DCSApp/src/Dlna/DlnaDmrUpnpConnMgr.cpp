////
// Created by eddy on 18-1-30.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "Dlna/upnp/upnp.h"
#include "Dlna/upnp/ithread.h"
#include "Dlna/DlnaDmrUpnpConnMgr.h"
#include "Dlna/DlnaDmrLogging.h"
#include "Dlna/DlnaDmrUpnpDevice.h"

const char* CONNMGR_TYPE = "urn:schemas-upnp-org:service:ConnectionManager:1";
const char* CONNMGR_SERVICE_ID = "urn:upnp-org:serviceId:ConnectionManager";
const char* CONNMGR_SCPD_URL = "/upnp/renderconnmgrSCPD.xml";
const char* CONNMGR_CONTROL_URL = "/upnp/control/renderconnmgr1";
const char* CONNMGR_EVENT_URL = "/upnp/event/renderconnmgr1";

namespace duerOSDcsApp {
namespace dueros_dlna {

static struct argument* arguments_getprotocolinfo[] = {
    new argument{"Source", PARAM_DIR_OUT, CONNMGR_VAR_SRC_PROTO_INFO},
    new argument{"Sink", PARAM_DIR_OUT, CONNMGR_VAR_SINK_PROTO_INFO},
    NULL
};

static struct argument* arguments_getcurrentconnectionids[] = {
    new argument{"ConnectionIDs", PARAM_DIR_OUT, CONNMGR_VAR_CUR_CONN_IDS},
    NULL
};
static struct argument* arguments_setcurrentconnectioninfo[] = {
    new argument{"ConnectionID", PARAM_DIR_IN, CONNMGR_VAR_AAT_CONN_ID},
    new argument{"RcsID", PARAM_DIR_OUT, CONNMGR_VAR_AAT_RCS_ID},
    new argument{"AVTransportID", PARAM_DIR_OUT, CONNMGR_VAR_AAT_AVT_ID},
    new argument{"ProtocolInfo", PARAM_DIR_OUT, CONNMGR_VAR_AAT_PROTO_INFO},
    new argument{"PeerConnectionManager", PARAM_DIR_OUT, CONNMGR_VAR_AAT_CONN_MGR},
    new argument{"PeerConnectionID", PARAM_DIR_OUT, CONNMGR_VAR_AAT_CONN_ID},
    new argument{"Direction", PARAM_DIR_OUT, CONNMGR_VAR_AAT_DIR},
    new argument{"Status", PARAM_DIR_OUT, CONNMGR_VAR_AAT_CONN_STATUS},
    NULL
};
static struct argument* arguments_prepareforconnection[] = {
    new argument{"RemoteProtocolInfo", PARAM_DIR_IN, CONNMGR_VAR_AAT_PROTO_INFO},
    new argument{"PeerConnectionManager", PARAM_DIR_IN, CONNMGR_VAR_AAT_CONN_MGR},
    new argument{"PeerConnectionID", PARAM_DIR_IN, CONNMGR_VAR_AAT_CONN_ID},
    new argument{"Direction", PARAM_DIR_IN, CONNMGR_VAR_AAT_DIR},
    new argument{"ConnectionID", PARAM_DIR_OUT, CONNMGR_VAR_AAT_CONN_ID},
    new argument{"AVTransportID", PARAM_DIR_OUT, CONNMGR_VAR_AAT_AVT_ID},
    new argument{"RcsID", PARAM_DIR_OUT, CONNMGR_VAR_AAT_RCS_ID},
    NULL
};

static const char* connstatus_values[] = {
    "OK",
    "ContentFormatMismatch",
    "InsufficientBandwidth",
    "UnreliableChannel",
    "Unknown",
    NULL
};
static const char* direction_values[] = {
    "Input",
    "Output",
    NULL
};

static ithread_mutex_t connmgr_mutex;

static struct argument** argument_list[CONNMGR_CMD_MAX_NUM];
static const char* connmgr_variable_names[CONNMGR_VARIABLE_MAX_NUM];
static const char* connmgr_default_values[CONNMGR_VARIABLE_MAX_NUM];
static struct var_meta connmgr_var_meta[CONNMGR_VARIABLE_MAX_NUM];
static struct action connmgr_actions[CONNMGR_CMD_MAX_NUM];
struct service connmgr_service_;

UpnpConnMgr::UpnpConnMgr() {
    //argument_list
    argument_list[CONNMGR_CMD_GETPROTOCOLINFO] = arguments_getprotocolinfo;
    argument_list[CONNMGR_CMD_GETCURRENTCONNECTIONIDS] = arguments_getcurrentconnectionids;
    argument_list[CONNMGR_CMD_SETCURRENTCONNECTIONINFO] = arguments_setcurrentconnectioninfo;
    argument_list[CONNMGR_CMD_PREPAREFORCONNECTION] = arguments_prepareforconnection;
    argument_list[CONNMGR_CMD_UNKNOWN] = nullptr;


    //connmgr_variable_names
    connmgr_variable_names[CONNMGR_VAR_SRC_PROTO_INFO] = "SourceProtocolInfo";
    connmgr_variable_names[CONNMGR_VAR_SINK_PROTO_INFO] = "SinkProtocolInfo";
    connmgr_variable_names[CONNMGR_VAR_CUR_CONN_IDS] = "CurrentConnectionIDs";
    connmgr_variable_names[CONNMGR_VAR_AAT_CONN_STATUS] = "A_ARG_TYPE_ConnectionStatus";
    connmgr_variable_names[CONNMGR_VAR_AAT_CONN_MGR] = "A_ARG_TYPE_ConnectionManager";
    connmgr_variable_names[CONNMGR_VAR_AAT_DIR] = "A_ARG_TYPE_Direction";
    connmgr_variable_names[CONNMGR_VAR_AAT_PROTO_INFO] = "A_ARG_TYPE_ProtocolInfo";
    connmgr_variable_names[CONNMGR_VAR_AAT_CONN_ID] = "A_ARG_TYPE_ConnectionID";
    connmgr_variable_names[CONNMGR_VAR_AAT_AVT_ID] = "A_ARG_TYPE_AVTransportID";
    connmgr_variable_names[CONNMGR_VAR_AAT_RCS_ID] = "A_ARG_TYPE_RcsID";
    connmgr_variable_names[CONNMGR_VAR_UNKNOWN] = nullptr;

    //connmgr_default_values
    connmgr_default_values[CONNMGR_VAR_SRC_PROTO_INFO] = "";
    connmgr_default_values[CONNMGR_VAR_SINK_PROTO_INFO] = "http-get:*:audio/mpeg:*";
    connmgr_default_values[CONNMGR_VAR_CUR_CONN_IDS] = "0";
    connmgr_default_values[CONNMGR_VAR_AAT_CONN_STATUS] = "Unknown";
    connmgr_default_values[CONNMGR_VAR_AAT_CONN_MGR] = "/";
    connmgr_default_values[CONNMGR_VAR_AAT_DIR] = "Input";
    connmgr_default_values[CONNMGR_VAR_AAT_PROTO_INFO] = ":::";
    connmgr_default_values[CONNMGR_VAR_AAT_CONN_ID] = "-1";
    connmgr_default_values[CONNMGR_VAR_AAT_AVT_ID] = "0";
    connmgr_default_values[CONNMGR_VAR_AAT_RCS_ID] = "0";
    connmgr_default_values[CONNMGR_VAR_UNKNOWN] = nullptr;

    //connmgr_var_meta
    connmgr_var_meta[CONNMGR_VAR_SRC_PROTO_INFO] =
    {SENDEVENT_YES, DATATYPE_STRING, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_SINK_PROTO_INFO] =
    {SENDEVENT_YES, DATATYPE_STRING, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_CUR_CONN_IDS] = {SENDEVENT_YES, DATATYPE_STRING, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_AAT_CONN_STATUS] =
    {SENDEVENT_NO, DATATYPE_STRING, connstatus_values, nullptr};
    connmgr_var_meta[CONNMGR_VAR_AAT_CONN_MGR] = {SENDEVENT_NO, DATATYPE_STRING, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_AAT_DIR] =
    {SENDEVENT_NO, DATATYPE_STRING, direction_values, nullptr};
    connmgr_var_meta[CONNMGR_VAR_AAT_PROTO_INFO] =
    {SENDEVENT_NO, DATATYPE_STRING, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_AAT_CONN_ID] = {SENDEVENT_NO, DATATYPE_I4, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_AAT_AVT_ID] = {SENDEVENT_NO, DATATYPE_I4, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_AAT_RCS_ID] = {SENDEVENT_NO, DATATYPE_I4, nullptr, nullptr};
    connmgr_var_meta[CONNMGR_VAR_UNKNOWN] = {SENDEVENT_NO, DATATYPE_UNKNOWN, nullptr, nullptr};

    //connmgr_actions
    connmgr_actions[CONNMGR_CMD_GETPROTOCOLINFO] =
    {"GetProtocolInfo", UpnpConnMgr::get_protocol_info};
    connmgr_actions[CONNMGR_CMD_GETCURRENTCONNECTIONIDS] =
    {"GetCurrentConnectionIDs", UpnpConnMgr::get_current_conn_ids};
    connmgr_actions[CONNMGR_CMD_SETCURRENTCONNECTIONINFO] =
    {"GetCurrentConnectionInfo", UpnpConnMgr::get_current_conn_info};
    connmgr_actions[CONNMGR_CMD_PREPAREFORCONNECTION] =
    {"PrepareForConnection", UpnpConnMgr::prepare_for_connection}; /** optional */
    connmgr_actions[CONNMGR_CMD_UNKNOWN] = {nullptr, nullptr};

    //connmgr_service_
    connmgr_service_.service_id = CONNMGR_SERVICE_ID;
    connmgr_service_.service_type = CONNMGR_TYPE;
    connmgr_service_.scpd_url = CONNMGR_SCPD_URL;
    connmgr_service_.control_url = CONNMGR_CONTROL_URL;
    connmgr_service_.event_url = CONNMGR_EVENT_URL;
    connmgr_service_.actions = connmgr_actions;
    connmgr_service_.action_arguments = argument_list;
    connmgr_service_.variable_names = connmgr_variable_names;
    connmgr_service_.variable_container = NULL; // set later.
    connmgr_service_.last_change = NULL;
    connmgr_service_.variable_meta = connmgr_var_meta;
    connmgr_service_.variable_count = CONNMGR_VAR_UNKNOWN;
    connmgr_service_.command_count = CONNMGR_CMD_UNKNOWN;
    connmgr_service_.service_mutex = &connmgr_mutex;
}


static struct mime_type {
    const char* mime_type;
    struct mime_type* next;
}* supported_types = nullptr;

void UpnpConnMgr::register_mime_type(const char* mime_type) {
    register_mime_type_internal(mime_type);

    if (strcmp("audio/mpeg", mime_type) == 0) {
        register_mime_type_internal("audio/x-mpeg");

        register_mime_type_internal("audio/x-scpls");

        register_mime_type("audio/L16;rate=44100;channels=2");
    }

    if (strcmp("audio/x-alac", mime_type) == 0) {
        register_mime_type_internal("audio/alac");
    }

    if (strcmp("audio/x-aiff", mime_type) == 0) {
        register_mime_type_internal("audio/aiff");
    }

    if (strcmp("audio/x-m4a", mime_type) == 0) {
        register_mime_type_internal("audio/m4a");
        register_mime_type_internal("audio/mp4");
    }
}

int UpnpConnMgr::connmgr_init(void) {
    struct mime_type* entry;
    char* buf = NULL;
    char* p = NULL;
    int offset = 0;
    int bufsize = 0;

    struct service* srv = upnp_connmgr_get_service();

    buf = (char*) malloc(bufsize);
    p = buf;
    assert(buf);

    if (buf == NULL) {
        fprintf(stderr, "%s: initial malloc failed\n",
                __FUNCTION__);
        return -1;
    }

    for (entry = supported_types; entry; entry = entry->next) {
        bufsize += strlen(entry->mime_type) + 1 + 8 + 3 + 2;
        offset = p - buf;
        buf = (char*) realloc(buf, bufsize);

        if (buf == NULL) {
            fprintf(stderr, "%s: realloc failed\n",
                    __FUNCTION__);
            return -1;
        }

        p = buf;
        p += offset;
        strncpy(p, "http-get:*:", 11);
        p += 11;
        strncpy(p, entry->mime_type, strlen(entry->mime_type));
        p += strlen(entry->mime_type);
        strncpy(p, ":*,", 3);
        p += 3;
    }

    if (p > buf) {
        p--;
        *p = '\0';
    }

    *p = '\0';

    VariableContainer::get_instance().variablecontainer_change(srv->variable_container,
            CONNMGR_VAR_SINK_PROTO_INFO, buf);
    free(buf);

    return 0;
}

struct service* UpnpConnMgr::upnp_connmgr_get_service(void) {
    if (connmgr_service_.variable_container == NULL) {
        connmgr_service_.variable_container =
            VariableContainer::get_instance().variablecontainer_new(CONNMGR_VAR_COUNT,
                    connmgr_variable_names,
                    connmgr_default_values);
        // no changes expected; no collector.
    }

    return &connmgr_service_;
}

void UpnpConnMgr::register_mime_type_internal(const char* mime_type) {
    struct mime_type* entry;

    for (entry = supported_types; entry; entry = entry->next) {
        if (strcmp(entry->mime_type, mime_type) == 0) {
            return;
        }
    }

    dbg("Registering support for '%s'", mime_type);

    entry = (struct mime_type*) malloc(sizeof(struct mime_type));
    entry->mime_type = strdup(mime_type);
    entry->next = supported_types;
    supported_types = entry;
}

int UpnpConnMgr::get_protocol_info(struct action_event* event) {
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_SRC_PROTO_INFO, "Source");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_SINK_PROTO_INFO, "Sink");
    return event->status;
}

int UpnpConnMgr::get_current_conn_ids(struct action_event* event) {
    int rc = -1;
    UpnpDevice::get_instance().upnp_add_response(event, "ConnectionIDs", "0");
    ///rc = upnp_append_variable(event, CONNMGR_VAR_CUR_CONN_IDS,
    //              "ConnectionIDs");
    return rc;
}

int UpnpConnMgr::prepare_for_connection(struct action_event* event) {
    UpnpDevice::get_instance()
    .upnp_append_variable(event, CONNMGR_VAR_CUR_CONN_IDS, "ConnectionID");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_AVT_ID, "AVTransportID");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_RCS_ID, "RcsID");
    return 0;
}

int UpnpConnMgr::get_current_conn_info(struct action_event* event) {
    const char* value = UpnpDevice::get_instance().upnp_get_string(event, "ConnectionID");

    if (value == NULL) {
        return -1;
    }

    dbg("Query ConnectionID='%s'", value);

    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_RCS_ID, "RcsID");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_AVT_ID, "AVTransportID");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_PROTO_INFO,
            "ProtocolInfo");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_CONN_MGR,
            "PeerConnectionManager");
    UpnpDevice::get_instance()
    .upnp_append_variable(event, CONNMGR_VAR_AAT_CONN_ID, "PeerConnectionID");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_DIR, "Direction");
    UpnpDevice::get_instance().upnp_append_variable(event, CONNMGR_VAR_AAT_CONN_STATUS, "Status");
    return 0;
}

}
}
