////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRVARIABLECONTAINER_H
#define DUEROS_DCS_APP_DLNA_DLNADMRVARIABLECONTAINER_H

#include <stdint.h>
#include "DlnaDmrXmlDoc.h"
#include "DlnaDmrXmlEscape.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
typedef void (* variable_change_listener_t)(void* userdata,
        int var_num, const char* var_name,
        const char* old_value,
        const char* new_value);

struct cb_list {
    variable_change_listener_t callback;
    void* userdata;
    struct cb_list* next;
};

struct variable_container {
    int variable_num;
    const char** variable_names;
    char** values;
    struct cb_list* callbacks;
};

typedef struct variable_container variable_container_t;


struct upnp_last_change_builder {
    const char* xml_namespace;
    struct xmldoc* change_event_doc;
    struct xmlelement* instance_element;
};
typedef struct upnp_last_change_builder upnp_last_change_builder_t;

struct upnp_last_change_collector {
    variable_container_t* variable_container;
    int last_change_variable_num;      // the variable we manipulate.
    uint32_t not_eventable_variables;  // variables not to event on.
    struct upnp_device* upnp_device;
    const char* service_id;
    int open_transactions;
    upnp_last_change_builder_t* builder;
};
typedef struct upnp_last_change_collector upnp_last_change_collector_t;


class VariableContainer {
public:
    static VariableContainer& get_instance() {
        static VariableContainer m_instance;

        return m_instance;
    }

    variable_container_t* variablecontainer_new(int variable_num,
            const char** variable_names,
            const char** variable_init_values);

    void variablecontainer_delete(variable_container_t* object);

    int variableContainer_get_num_vars(variable_container_t* object);

    const char* variablecontainer_get(variable_container_t* object, int var,
                                      const char** name);

    static int variablecontainer_change(variable_container_t* object,
                                        int variable_num, const char* value);

    void variablecontainer_register_callback(variable_container_t* object,
            variable_change_listener_t callback,
            void* userdata);

    upnp_last_change_builder_t* upnp_last_change_builder_new(const char* xml_namespace);

    void upnp_last_change_builder_delete(upnp_last_change_builder_t* builder);

    static void upnp_last_change_builder_add(upnp_last_change_builder_t* builder,
                                          const char* name, const char* value);

    static char* upnp_last_change_builder_to_xml(upnp_last_change_builder_t* builder);

    upnp_last_change_collector_t*
    upnp_last_change_collector_new(variable_container_t* variable_container,
                                const char* event_xml_namespac,
                                struct upnp_device* upnp_device,
                                const char* service_id);

    void upnp_last_change_collector_add_ignore(upnp_last_change_collector_t* object,
                                            int variable_num);

    void upnp_last_change_collector_start(upnp_last_change_collector_t* object);

    void upnp_last_change_collector_finish(upnp_last_change_collector_t* object);

private:
    VariableContainer() {

    }

private:
    static void upnp_last_change_collector_notify(upnp_last_change_collector_t* obj);

    static void upnp_last_change_collector_callback(void* userdata,
            int var_num, const char* var_name,
            const char* old_value,
            const char* new_value);

};
}
}
#endif //DLNA_DMR_VARIABLE_CONTAINER_H
