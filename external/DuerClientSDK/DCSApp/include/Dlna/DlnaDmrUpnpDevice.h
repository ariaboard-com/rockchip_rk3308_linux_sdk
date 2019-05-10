////
// Created by eddy on 18-1-29.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRUPNPDEVICE_H
#define DUEROS_DCS_APP_DLNA_DLNADMRUPNPDEVICE_H

#include "DlnaDmrUpnp.h"
#include "DlnaDmrWebServer.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
struct upnp_device_descriptor {
    int (* init_function)(void);

    const char* device_type;
    const char* friendly_name;
    const char* manufacturer;
    const char* manufacturer_url;
    const char* model_description;
    const char* model_name;
    const char* model_number;
    const char* model_url;
    const char* serial_number;
    const char* udn;
    const char* upc;
    const char* presentation_url;
    struct icon** icons;
    struct service** services;
};

struct upnp_device {
    struct upnp_device_descriptor* upnp_device_descriptor;
    ithread_mutex_t device_mutex;
    UpnpDevice_Handle device_handle;
};

class UpnpDevice {
public:
    static UpnpDevice& get_instance() {
        static UpnpDevice m_instance;

        return m_instance;
    }

    struct upnp_device* upnp_device_init(struct upnp_device_descriptor* device_def,
                                         const char* ip_address,
                                         unsigned short port);

    void upnp_device_shutdown(struct upnp_device* device);

    int upnp_add_response(struct action_event* event,
                          const char* key, const char* value);

    void upnp_set_error(struct action_event* event, int error_code,
                        const char* format, ...);


    const char* upnp_get_string(struct action_event* event, const char* key);

    void upnp_append_variable(struct action_event* event,
                              int varnum, const char* paramname);

    int upnp_device_notify(struct upnp_device* device,
                           const char* serviceID,
                           const char** varnames,
                           const char** varvalues,
                           int varcount);

    static struct service* find_service(struct upnp_device_descriptor* device_def,
                                        const char* service_name);

    char* upnp_create_device_desc(struct upnp_device_descriptor* device_def);

private:
    static int handle_subscription_request(struct upnp_device* priv,
                                           struct Upnp_Subscription_Request
                                           * sr_event);

    static int handle_var_request(struct upnp_device* priv,
                                  struct Upnp_State_Var_Request* var_event);

    static int handle_action_request(struct upnp_device* priv,
                                     struct Upnp_Action_Request* ar_event);

    static int event_handler(Upnp_EventType EventType, void* event, void* userdata);

    bool initialize_device(struct upnp_device_descriptor* device_def,
                           struct upnp_device* result_device,
                           const char* ip_address,
                           unsigned short port);

    struct xmlelement* gen_specversion(struct xmldoc* doc,
                                       int major, int minor);

    struct xmlelement* gen_desc_iconlist(struct xmldoc* doc,
                                         struct icon** icons);

    struct xmlelement* gen_desc_servicelist(struct upnp_device_descriptor* device_def,
                                            struct xmldoc* doc);

    struct xmldoc* generate_desc(struct upnp_device_descriptor* device_def);

private:
    WebServer _web_server;

};
}
}
#endif //DLNA_DMR_UPNP_DEVICE_H
