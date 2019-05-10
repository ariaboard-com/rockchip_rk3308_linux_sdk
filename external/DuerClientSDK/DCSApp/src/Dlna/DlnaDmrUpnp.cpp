////
// Created by eddy on 18-1-30.
//
#include <stdlib.h>
#include <string.h>
#include "Dlna/DlnaDmrXmlDoc.h"
#include "Dlna/DlnaDmrUpnp.h"

namespace duerOSDcsApp {
namespace dueros_dlna {

static const char* param_datatype_names[PARAM_DATATYPE_ELEMENTS_NUM];

Upnp::Upnp() {
    char** p = const_cast<char**>(param_datatype_names);
    p[DATATYPE_STRING] = (char*) "string";
    p[DATATYPE_BOOLEAN] = (char*) "boolean";
    p[DATATYPE_I2] = (char*) "i2";
    p[DATATYPE_I4] = (char*) "i4";
    p[DATATYPE_UI2] = (char*) "ui2";
    p[DATATYPE_UI4] = (char*) "ui4";
    p[DATATYPE_UNKNOWN] = nullptr;

}

struct action* Upnp::find_action(struct service* event_service,
                                 char* action_name) {
    struct action* event_action;
    int action_num = 0;

    if (event_service == NULL) {
        return NULL;
    }

    while (event_action =
                &(event_service->actions[action_num]),
            event_action->action_name != NULL) {
        if (strcmp(event_action->action_name, action_name) == 0) {
            return event_action;
        }

        action_num++;
    }

    return NULL;
}

char* Upnp::upnp_get_scpd(struct service* srv) {
    char* result = NULL;
    struct xmldoc* doc;

    doc = generate_scpd(srv);

    if (doc != NULL) {
        result = XMLDoc::get_instance().xmldoc_tostring(doc);
        XMLDoc::get_instance().xmldoc_free(doc);
    }

    return result;
}

struct xmlelement* Upnp::gen_specversion(struct xmldoc* doc,
        int major, int minor) {
    struct xmlelement* top;

    top = XMLDoc::get_instance().xmlelement_new(doc, "specVersion");

    XMLDoc::get_instance().add_value_element_int(doc, top, "major", major);
    XMLDoc::get_instance().add_value_element_int(doc, top, "minor", minor);

    return top;
}

struct xmlelement* Upnp::gen_scpd_action(struct xmldoc* doc,
        struct action* act,
        struct argument** arglist,
        const char** varnames) {
    struct xmlelement* top;
    struct xmlelement* parent; 
    struct xmlelement* child;

    top = XMLDoc::get_instance().xmlelement_new(doc, "action");

    XMLDoc::get_instance().add_value_element(doc, top, "name", act->action_name);

    if (arglist) {
        struct argument* arg;
        int j = 0;
        parent = XMLDoc::get_instance().xmlelement_new(doc, "argumentList");
        XMLDoc::get_instance().xmlelement_add_element(doc, top, parent);

        for (j = 0; (arg = arglist[j]); j++) {
            child = XMLDoc::get_instance().xmlelement_new(doc, "argument");
            XMLDoc::get_instance().add_value_element(doc, child, "name", arg->name);
            XMLDoc::get_instance().add_value_element(doc, child, "direction",
                    (arg->direction == PARAM_DIR_IN)
                    ? "in" : "out");
            XMLDoc::get_instance().add_value_element(doc, child, "relatedStateVariable",
                    varnames[arg->statevar]);
            XMLDoc::get_instance().xmlelement_add_element(doc, parent, child);
        }
    }

    return top;
}

struct xmlelement* Upnp::gen_scpd_actionlist(struct xmldoc* doc,
        struct service* srv) {
    struct xmlelement* top;
    struct xmlelement* child;
    int i = 0;

    top = XMLDoc::get_instance().xmlelement_new(doc, "actionList");

    for (i = 0; i < srv->command_count; i++) {
        struct action* act;
        struct argument** arglist;
        const char** varnames;
        act = &(srv->actions[i]);
        arglist = srv->action_arguments[i];
        varnames = srv->variable_names;

        if (act) {
            child = gen_scpd_action(doc, act, arglist, varnames);
            XMLDoc::get_instance().xmlelement_add_element(doc, top, child);
        }
    }

    return top;
}

struct xmlelement* Upnp::gen_scpd_statevar(struct xmldoc* doc,
        const char* name,
        struct var_meta* meta) {
    struct xmlelement* top; 
    struct xmlelement* parent;
    const char** valuelist;
    const char* default_value;
    struct param_range* range;

    valuelist = meta->allowed_values;
    range = meta->allowed_range;
    default_value = meta->default_value;

    top = XMLDoc::get_instance().xmlelement_new(doc, "stateVariable");

    XMLDoc::get_instance().xmlelement_set_attribute(doc, top, "sendEvents",
            (meta->sendevents == SENDEVENT_YES) ? "yes"
            : "no");
    XMLDoc::get_instance().add_value_element(doc, top, "name", name);
    XMLDoc::get_instance()
    .add_value_element(doc, top, "dataType", param_datatype_names[meta->datatype]);

    if (valuelist) {
        const char* allowed_value;
        int i = 0;
        parent = XMLDoc::get_instance().xmlelement_new(doc, "allowedValueList");
        XMLDoc::get_instance().xmlelement_add_element(doc, top, parent);

        for (i = 0; (allowed_value = valuelist[i]); i++) {
            XMLDoc::get_instance().add_value_element(doc, parent, "allowedValue", allowed_value);
        }
    }

    if (range) {
        parent = XMLDoc::get_instance().xmlelement_new(doc, "allowedValueRange");
        XMLDoc::get_instance().xmlelement_add_element(doc, top, parent);
        XMLDoc::get_instance().add_value_element_long(doc, parent, "minimum", range->min);
        XMLDoc::get_instance().add_value_element_long(doc, parent, "maximum", range->max);

        if (range->step != 0L) {
            XMLDoc::get_instance().add_value_element_long(doc, parent, "step", range->step);
        }
    }

    if (default_value) {
        XMLDoc::get_instance().add_value_element(doc, top, "defaultValue", default_value);
    }

    return top;
}

struct xmlelement* Upnp::gen_scpd_servicestatetable(struct xmldoc* doc, struct service* srv) {
    struct xmlelement* top;
    struct xmlelement* child;
    int i = 0;

    top = XMLDoc::get_instance().xmlelement_new(doc, "serviceStateTable");

    for (i = 0; i < srv->variable_count; i++) {
        struct var_meta* meta = &(srv->variable_meta[i]);
        const char* name = srv->variable_names[i];
        child = gen_scpd_statevar(doc, name, meta);
        XMLDoc::get_instance().xmlelement_add_element(doc, top, child);
    }

    return top;
}

struct xmldoc* Upnp::generate_scpd(struct service* srv) {
    struct xmldoc* doc;
    struct xmlelement* root;
    struct xmlelement* child;

    doc = XMLDoc::get_instance().xmldoc_new();

    root = XMLDoc::get_instance()
           .xmldoc_new_topelement(doc, "scpd", "urn:schemas-upnp-org:service-1-0");
    child = gen_specversion(doc, 1, 0);
    XMLDoc::get_instance().xmlelement_add_element(doc, root, child);

    child = gen_scpd_actionlist(doc, srv);
    XMLDoc::get_instance().xmlelement_add_element(doc, root, child);

    child = gen_scpd_servicestatetable(doc, srv);
    XMLDoc::get_instance().xmlelement_add_element(doc, root, child);

    return doc;
}

}
}