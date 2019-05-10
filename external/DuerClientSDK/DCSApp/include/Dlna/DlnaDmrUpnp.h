////
// Created by eddy on 18-1-29.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRUPNP_H
#define DUEROS_DCS_APP_DLNA_DLNADMRUPNP_H

#include "DlnaDmrVariableContainer.h"
#include "DlnaDmrXmlDoc.h"
#include "upnp/ithread.h"
#include "upnp/upnp.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
struct action {
    const char* action_name;

    int (* callback)(struct action_event*);
};

typedef enum {
    PARAM_DIR_IN,
    PARAM_DIR_OUT,
} param_dir;

struct argument {
    const char* name;
    param_dir direction;
    int statevar;
};

const int PARAM_DATATYPE_ELEMENTS_NUM = 8;
typedef enum {
    DATATYPE_STRING,
    DATATYPE_BOOLEAN,
    DATATYPE_I2,
    DATATYPE_I4,
    DATATYPE_UI2,
    DATATYPE_UI4,
    DATATYPE_UNKNOWN,
    DATATYPE_COUNT
} param_datatype;

typedef enum {
    SENDEVENT_NO,
    SENDEVENT_YES
} param_event;

struct param_range {
    long long min;
    long long max;
    long long step;
};

struct var_meta {
    param_event sendevents;
    param_datatype datatype;
    const char** allowed_values;
    struct param_range* allowed_range;
    const char* default_value;
};

struct icon {
    int width;
    int height;
    int depth;
    const char* url;
    const char* mimetype;
};

struct service {
    ithread_mutex_t* service_mutex;
    const char* service_id;
    const char* service_type;
    const char* scpd_url;
    const char* control_url;
    const char* event_url;
    const char* event_xml_ns;
    struct action* actions;
    struct argument*** action_arguments;
    const char** variable_names;
    struct variable_container* variable_container;
    struct upnp_last_change_collector* last_change;
    struct var_meta* variable_meta;
    int variable_count;
    int command_count;
    const char** eventvar_names;
    const char** eventvar_values;
};

struct action_event {
    struct Upnp_Action_Request* request;
    int status;
    struct service* service;
    struct upnp_device* device;
};

class Upnp {
public:
    static Upnp& get_instance() {
        static Upnp m_instance;
        return m_instance;
    }

    struct action* find_action(struct service* event_service,
                               char* action_name);

    char* upnp_get_scpd(struct service* srv);

private:
    struct xmlelement* gen_specversion(struct xmldoc* doc,
                                       int major, int minor);

    struct xmlelement* gen_scpd_action(struct xmldoc* doc,
                                       struct action* act,
                                       struct argument** arglist,
                                       const char** varnames);

    struct xmlelement* gen_scpd_actionlist(struct xmldoc* doc,
                                           struct service* srv);

    struct xmlelement* gen_scpd_statevar(struct xmldoc* doc,
                                         const char* name,
                                         struct var_meta* meta);

    struct xmlelement* gen_scpd_servicestatetable(struct xmldoc* doc, struct service* srv);

    struct xmldoc* generate_scpd(struct service* srv);

private:
    Upnp();

};
}
}
#endif //DLNA_DMR_UPNP_H
