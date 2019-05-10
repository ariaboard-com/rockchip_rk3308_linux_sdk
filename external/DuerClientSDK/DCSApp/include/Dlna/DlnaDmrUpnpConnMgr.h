////
// Created by eddy on 18-1-30.
//
#ifndef DUEROS_DCS_APP_DLNA_DLNADMRUPNPCONNMGR_H
#define DUEROS_DCS_APP_DLNA_DLNADMRUPNPCONNMGR_H

namespace duerOSDcsApp {
namespace dueros_dlna {

const int CONNMGR_VARIABLE_MAX_NUM = 12;
typedef enum {
    CONNMGR_VAR_AAT_CONN_MGR,
    CONNMGR_VAR_SINK_PROTO_INFO,
    CONNMGR_VAR_AAT_CONN_STATUS,
    CONNMGR_VAR_AAT_AVT_ID,
    CONNMGR_VAR_AAT_DIR,
    CONNMGR_VAR_AAT_RCS_ID,
    CONNMGR_VAR_AAT_PROTO_INFO,
    CONNMGR_VAR_AAT_CONN_ID,
    CONNMGR_VAR_SRC_PROTO_INFO,
    CONNMGR_VAR_CUR_CONN_IDS,
    CONNMGR_VAR_UNKNOWN,
    CONNMGR_VAR_COUNT
} connmgr_variable;

const int CONNMGR_CMD_MAX_NUM = 7;
typedef enum {
    CONNMGR_CMD_GETCURRENTCONNECTIONIDS,
    CONNMGR_CMD_SETCURRENTCONNECTIONINFO,
    CONNMGR_CMD_GETPROTOCOLINFO,
    CONNMGR_CMD_PREPAREFORCONNECTION,
    //CONNMGR_CMD_CONNECTIONCOMPLETE,
    CONNMGR_CMD_UNKNOWN,
    CONNMGR_CMD_COUNT
} connmgr_cmd;

class UpnpConnMgr {
public:
    static UpnpConnMgr get_instance() {
        static UpnpConnMgr m_instance;
        return m_instance;
    }

    struct service* upnp_connmgr_get_service(void);

    int connmgr_init(void);

    void register_mime_type(const char* mime_type);

private:
    UpnpConnMgr();

    void register_mime_type_internal(const char* mime_type);

    static int get_protocol_info(struct action_event* event);

    static int get_current_conn_ids(struct action_event* event);

    static int prepare_for_connection(struct action_event* event);

    static int get_current_conn_info(struct action_event* event);
};
}
}
#endif //DLNA_DMR_UPNP_CONN_MGR_H
