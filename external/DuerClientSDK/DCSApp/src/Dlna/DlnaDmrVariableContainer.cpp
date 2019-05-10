////
// Created by eddy on 18-1-30.
//

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "Dlna/DlnaDmrUpnpDevice.h"
#include "Dlna/DlnaDmrVariableContainer.h"

namespace duerOSDcsApp {
namespace dueros_dlna {

variable_container_t* VariableContainer::variablecontainer_new(int variable_num,
        const char** variable_names,
        const char** variable_init_values) {
    assert(variable_num > 0);
    variable_container_t* result
        = (variable_container_t*) malloc(sizeof(variable_container_t));
    result->variable_num = variable_num;
    result->variable_names = variable_names;
    result->values = (char**) malloc(variable_num * sizeof(char*));
    result->callbacks = NULL;

    for (int i = 0; i < variable_num; ++i) {
        if (variable_init_values[i]) {
            result->values[i] = strndup(variable_init_values[i], strlen(variable_init_values[i]));
        } else {
            result->values[i] = {0};
        }
    }

    return result;
}

void VariableContainer::variablecontainer_delete(variable_container_t* object) {
    for (int i = 0; i < object->variable_num; ++i) {
        free(object->values[i]);
    }

    free(object->values);

    for (struct cb_list* list = object->callbacks; list; /**/) {
        struct cb_list* next = list->next;
        free(list);
        list = next;
    }

    free(object);
}

// Get number of variables.
int VariableContainer::variableContainer_get_num_vars(variable_container_t* object) {
    return object->variable_num;
}

const char* VariableContainer::variablecontainer_get(variable_container_t* object,
        int var, const char** name) {
    if (var < 0 || var >= object->variable_num) {
        return NULL;
    }

    const char* varname = object->variable_names[var];

    if (name) {
        *name = varname;
    }

    // Names of not used variables are set to NULL.
    return varname ? object->values[var] : NULL;
}

// Change content of variable with given number to NUL terminated content.
int VariableContainer::variablecontainer_change(variable_container_t* object,
        int var_num, const char* value) {
    assert(var_num >= 0 && var_num < object->variable_num);

    if (value == NULL) {
        value = "";
    }

    if (strcmp(value, object->values[var_num]) == 0) {
        return 0;    // no change.
    }

    char* old_value = object->values[var_num];
    char* new_value = strdup(value);
    object->values[var_num] = new_value;

    for (struct cb_list* it = object->callbacks; it; it = it->next) {
        it->callback(it->userdata,
                     var_num, object->variable_names[var_num],
                     old_value, new_value);
    }

    free(old_value);
    return 1;
}

void VariableContainer::variablecontainer_register_callback(variable_container_t* object,
        variable_change_listener_t callback,
        void* userdata) {
    // Order is not guaranteed, so we just register it at the front.
    struct cb_list* item = (struct cb_list*) malloc(sizeof(struct cb_list));
    item->next = object->callbacks;
    item->userdata = userdata;
    item->callback = callback;
    object->callbacks = item;
}

upnp_last_change_builder_t*
VariableContainer::upnp_last_change_builder_new(const char* xml_namespace) {
    upnp_last_change_builder_t* result = (upnp_last_change_builder_t*)
                                         malloc(sizeof(upnp_last_change_builder_t));
    result->xml_namespace = xml_namespace;
    result->change_event_doc = NULL;
    result->instance_element = NULL;
    return result;
}

void VariableContainer::upnp_last_change_builder_delete(upnp_last_change_builder_t* builder) {
    if (builder->change_event_doc != NULL) {
        XMLDoc::get_instance().xmldoc_free(builder->change_event_doc);
    }

    free(builder);
}

void VariableContainer::upnp_last_change_builder_add(upnp_last_change_builder_t* builder,
        const char* name, const char* value) {
    assert(name != NULL);
    assert(value != NULL);

    if (builder->change_event_doc == NULL) {
        builder->change_event_doc = XMLDoc::get_instance().xmldoc_new();
        struct xmlelement* toplevel =
            XMLDoc::get_instance().xmldoc_new_topelement(builder->change_event_doc, "Event",
                    builder->xml_namespace);
        builder->instance_element =
            XMLDoc::get_instance().add_attributevalue_element(builder->change_event_doc,
                    toplevel,
                    "InstanceID", "val", "0");
    }

    struct xmlelement* xml_value;

    xml_value = XMLDoc::get_instance().add_attributevalue_element(builder->change_event_doc,
                builder->instance_element,
                name, "val", value);

    if (strcmp(name, "Volume") == 0
            || strcmp(name, "VolumeDB") == 0
            || strcmp(name, "Mute") == 0
            || strcmp(name, "Loudness") == 0) {
        XMLDoc::get_instance().xmlelement_set_attribute(builder->change_event_doc,
                xml_value, "channel", "Master");
    }
}

char* VariableContainer::upnp_last_change_builder_to_xml(upnp_last_change_builder_t* builder) {
    if (builder->change_event_doc == NULL) {
        return NULL;
    }

    char* xml_doc_string = XMLDoc::get_instance().xmldoc_tostring(builder->change_event_doc);
    XMLDoc::get_instance().xmldoc_free(builder->change_event_doc);
    builder->change_event_doc = NULL;
    builder->instance_element = NULL;
    return xml_doc_string;
}


void VariableContainer::upnp_last_change_collector_add_ignore(upnp_last_change_collector_t* object,
        int variable_num) {
    object->not_eventable_variables |= (1 << variable_num);
}

void VariableContainer::upnp_last_change_collector_start(upnp_last_change_collector_t* object) {
    object->open_transactions += 1;
}

void VariableContainer::upnp_last_change_collector_finish(upnp_last_change_collector_t* object) {
    assert(object->open_transactions >= 1);
    object->open_transactions -= 1;
    upnp_last_change_collector_notify(object);
}

upnp_last_change_collector_t*
VariableContainer::upnp_last_change_collector_new(variable_container_t* variable_container,
        const char* event_xml_namespace,
        struct upnp_device* upnp_device,
        const char* service_id) {
    upnp_last_change_collector_t* result = (upnp_last_change_collector_t*)
                                           malloc(sizeof(upnp_last_change_collector_t));
    result->variable_container = variable_container;
    result->last_change_variable_num = -1;
    result->not_eventable_variables = 0;
    result->upnp_device = upnp_device;
    result->service_id = service_id;
    result->open_transactions = 0;
    result->builder = upnp_last_change_builder_new(event_xml_namespace);

    const int var_count = variableContainer_get_num_vars(variable_container);
    assert(var_count < 32);

    for (int i = 0; i < var_count; ++i) {
        const char* name;
        const char* value = variablecontainer_get(variable_container, i, &name);

        if (!value) {
            continue;
        }

        if (strcmp("LastChange", name) == 0) {
            result->last_change_variable_num = i;
            continue;
        }

        upnp_last_change_builder_add(result->builder, name, value);
    }

    assert(result->last_change_variable_num >= 0); // we expect to have one.
    upnp_last_change_collector_add_ignore(result,
                                       result->last_change_variable_num);
    upnp_last_change_collector_notify(result);

    variablecontainer_register_callback(variable_container,
                                        upnp_last_change_collector_callback,
                                        result);
    return result;
}

void VariableContainer::upnp_last_change_collector_notify(upnp_last_change_collector_t* obj) {
    if (obj->open_transactions != 0) {
        return;
    }

    char* xml_doc_string = upnp_last_change_builder_to_xml(obj->builder);

    if (xml_doc_string == NULL) {
        return;
    }

    if (variablecontainer_change(obj->variable_container,
                                 obj->last_change_variable_num,
                                 xml_doc_string)) {
        const char* varnames[] = {
            "LastChange",
            NULL
        };
        const char* varvalues[] = {
            NULL, NULL
        };

        varvalues[0] = XMLEscape::get_instance().xmlescape(xml_doc_string, 0);
        UpnpDevice::get_instance().upnp_device_notify(obj->upnp_device,
                obj->service_id,
                varnames, varvalues, 1);
        free((char*) varvalues[0]);
    }

    free(xml_doc_string);
}

void VariableContainer::upnp_last_change_collector_callback(void* userdata,
        int var_num, const char* var_name,
        const char* old_value,
        const char* new_value) {
    upnp_last_change_collector_t* object =
        (upnp_last_change_collector_t*) userdata;

    if (object->not_eventable_variables & (1 << var_num)) {
        return;
    }

    upnp_last_change_builder_add(object->builder, var_name, new_value);
    upnp_last_change_collector_notify(object);
}

}
}