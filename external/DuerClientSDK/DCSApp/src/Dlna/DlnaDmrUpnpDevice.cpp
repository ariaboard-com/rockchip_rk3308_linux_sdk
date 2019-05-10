////
// Created by eddy on 18-1-30.
//
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "Dlna/upnp/ithread.h"
#include "Dlna/upnp/upnptools.h"
#include "Dlna/DlnaDmrLogging.h"
#include "Dlna/DlnaDmrXmlEscape.h"
#include "Dlna/DlnaDmrXmlDoc.h"
#include "Dlna/DlnaDmrUpnpDevice.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
int UpnpDevice::upnp_add_response(struct action_event* event,
                                  const char* key, const char* value) {
    assert(event != NULL);
    assert(key != NULL);
    assert(value != NULL);

    if (event->status) {
        return -1;
    }

    int rc = 0;
    rc = UpnpAddToActionResponse(&event->request->ActionResult,
                                 event->request->ActionName,
                                 event->service->service_type, key, value);

    if (rc != UPNP_E_SUCCESS) {
        /* report custom error */
        event->request->ActionResult = NULL;
        event->request->ErrCode = UPNP_SOAP_E_ACTION_FAILED;
        strncpy(event->request->ErrStr, UpnpGetErrorMessage(rc), strlen(UpnpGetErrorMessage(rc)));
        return -1;
    }

    return 0;
}

void UpnpDevice::upnp_append_variable(struct action_event* event,
                                      int varnum, const char* paramname) {
    const char* value;
    struct service* service = event->service;

    assert(event != NULL);
    assert(paramname != NULL);

    ithread_mutex_lock(service->service_mutex);

    value = VariableContainer::get_instance().variablecontainer_get(service->variable_container,
            varnum, NULL);
    assert(value != NULL);   // triggers on invalid variable.
    upnp_add_response(event, paramname, value);

    ithread_mutex_unlock(service->service_mutex);
}

void UpnpDevice::upnp_set_error(struct action_event* event, int error_code,
                                const char* format, ...) {
    event->status = -1;

    va_list ap;
    va_start(ap, format);
    event->request->ActionResult = NULL;
    event->request->ErrCode = UPNP_SOAP_E_ACTION_FAILED;
    vsnprintf(event->request->ErrStr, sizeof(event->request->ErrStr),
              format, ap);

    va_end(ap);
    err("%s: %s\n", __FUNCTION__, event->request->ErrStr);
}

const char* UpnpDevice::upnp_get_string(struct action_event* event, const char* key) {
    IXML_Node* node = NULL;

    node = (IXML_Node*) event->request->ActionRequest;

    if (node == NULL) {
        upnp_set_error(event, UPNP_SOAP_E_INVALID_ARGS,
                       "Invalid action request document");
        return NULL;
    }

    node = ixmlNode_getFirstChild(node);

    if (node == NULL) {
        upnp_set_error(event, UPNP_SOAP_E_INVALID_ARGS,
                       "Invalid action request document");
        return NULL;
    }

    node = ixmlNode_getFirstChild(node);

    for (/**/; node != NULL; node = ixmlNode_getNextSibling(node)) {
        if (strcmp(ixmlNode_getNodeName(node), key) == 0) {
            node = ixmlNode_getFirstChild(node);
            const char* node_value = (node != NULL
                                      ? ixmlNode_getNodeValue(node)
                                      : NULL);
            return node_value != NULL ? node_value : "";
        }
    }

    upnp_set_error(event, UPNP_SOAP_E_INVALID_ARGS,
                   "Missing action request argument (%s)", key);
    return NULL;
}

int UpnpDevice::upnp_device_notify(struct upnp_device* device,
                                   const char* serviceID,
                                   const char** varnames,
                                   const char** varvalues, int varcount) {
    UpnpNotify(device->device_handle,
               device->upnp_device_descriptor->udn, serviceID,
               varnames, varvalues, varcount);

    return 0;
}

struct upnp_device* UpnpDevice::upnp_device_init(struct upnp_device_descriptor* device_def,
        const char* ip_address,
        unsigned short port) {
    int rc = 0;
    char* buf = NULL;
    struct service* srv;
    struct icon* icon_entry;

    assert(device_def != NULL);
    dbg("upnp_device_init");

    if (device_def->init_function) {
        rc = device_def->init_function();

        if (rc != 0) {
            return NULL;
        }
    }

    struct upnp_device* result_device = (struct upnp_device*) malloc(sizeof(*result_device));

    result_device->upnp_device_descriptor = device_def;

    ithread_mutex_init(&(result_device->device_mutex), NULL);

    for (int i = 0; (icon_entry = device_def->icons[i]); i++) {
        _web_server.webserver_register_file(icon_entry->url, "image/png");
    }

    for (int i = 0; (srv = device_def->services[i]); i++) {
        buf = Upnp::get_instance().upnp_get_scpd(srv);
        assert(buf != NULL);
        _web_server.webserver_register_buf(srv->scpd_url, buf, "text/xml");

    }

    if (!initialize_device(device_def, result_device, ip_address, port)) {
        UpnpFinish();
        free(result_device);
        return NULL;
    }

    return result_device;
}

void UpnpDevice::upnp_device_shutdown(struct upnp_device* device) {
    UpnpFinish();
    if (device != NULL) {
        free(device);
    }
}

struct service* UpnpDevice::find_service(struct upnp_device_descriptor* device_def,
        const char* service_id) {
    struct service* event_service;
    int service_num = 0;

    assert(device_def != NULL);
    assert(service_id != NULL);

    while (event_service =
                device_def->services[service_num], event_service != NULL) {
        if (strcmp(event_service->service_id, service_id) == 0) {
            return event_service;
        }

        service_num++;
    }

    return NULL;
}

char* UpnpDevice::upnp_create_device_desc(struct upnp_device_descriptor* device_def) {
    char* result = NULL;
    struct xmldoc* doc;

    doc = generate_desc(device_def);

    if (doc != NULL) {
        result = XMLDoc::get_instance().xmldoc_tostring(doc);
        XMLDoc::get_instance().xmldoc_free(doc);
    }

    return result;
}

int UpnpDevice::handle_subscription_request(struct upnp_device* priv,
        struct Upnp_Subscription_Request
        * sr_event) {
    struct service* srv;
    int rc = 0;

    assert(priv != NULL);

    dbg("Subscription request for %s (%s)",
                                     sr_event->ServiceId, sr_event->UDN);

    srv = find_service(priv->upnp_device_descriptor, sr_event->ServiceId);

    if (srv == NULL) {
        err("%s: Unknown service '%s'", __FUNCTION__,
                                          sr_event->ServiceId);
        return -1;
    }

    int result = -1;
    ithread_mutex_lock(&(priv->device_mutex));

    // There is really only one variable evented: LastChange
    const char* eventvar_names[] = {
        "LastChange",
        NULL
    };
    const char* eventvar_values[] = {
        NULL, NULL
    };

    // Build the current state of the variables as one gigantic initial
    // LastChange update.
    ithread_mutex_lock(srv->service_mutex);
    const int var_count =
        VariableContainer::get_instance().variableContainer_get_num_vars(srv->variable_container);
    // TODO(hzeller): maybe use srv->last_change directly ?
    upnp_last_change_builder_t* builder = VariableContainer::get_instance()
                                          .upnp_last_change_builder_new(srv->event_xml_ns);

    for (int i = 0; i < var_count; ++i) {
        const char* name;
        const char* value =
            VariableContainer::get_instance()
            .variablecontainer_get(srv->variable_container, i, &name);

        // Send over all variables except "LastChange" itself. Also all
        // A_ARG_TYPE variables are not evented.
        if (value && strcmp("LastChange", name) != 0
                && strncmp("A_ARG_TYPE_", name, strlen("A_ARG_TYPE_")) != 0) {
            VariableContainer::get_instance().upnp_last_change_builder_add(builder, name, value);
        }
    }

    ithread_mutex_unlock(srv->service_mutex);
    char* xml_value = VariableContainer::get_instance().upnp_last_change_builder_to_xml(builder);
    dbg("Initial variable sync: %s", xml_value);
    eventvar_values[0] = XMLEscape::get_instance().xmlescape(xml_value, 0);
    free(xml_value);
    VariableContainer::get_instance().upnp_last_change_builder_delete(builder);

    rc = UpnpAcceptSubscription(priv->device_handle,
                                sr_event->UDN, sr_event->ServiceId,
                                eventvar_names, eventvar_values, 1,
                                sr_event->Sid);

    if (rc == UPNP_E_SUCCESS) {
        result = 0;
    } else {
        err("Accept Subscription Error: %s (%d)",
                                          UpnpGetErrorMessage(rc), rc);
    }

    ithread_mutex_unlock(&(priv->device_mutex));

    free((char*) eventvar_values[0]);

    return result;
}

int UpnpDevice::handle_var_request(struct upnp_device* priv,
                                   struct Upnp_State_Var_Request* var_event) {
    struct service* srv = find_service(priv->upnp_device_descriptor,
                                       var_event->ServiceID);

    if (srv == NULL) {
        var_event->ErrCode = UPNP_SOAP_E_INVALID_ARGS;
        return -1;
    }

    ithread_mutex_lock(srv->service_mutex);

    char* result = NULL;
    const int var_count =
        VariableContainer::get_instance().variableContainer_get_num_vars(srv->variable_container);

    for (int i = 0; i < var_count; ++i) {
        const char* name;
        const char* value =
            VariableContainer::get_instance()
            .variablecontainer_get(srv->variable_container, i, &name);

        if (value && strcmp(var_event->StateVarName, name) == 0) {
            result = strndup(value, strlen(value));
            break;
        }
    }

    ithread_mutex_unlock(srv->service_mutex);

    var_event->CurrentVal = result;
    var_event->ErrCode = (result == NULL)
                         ? UPNP_SOAP_E_INVALID_VAR
                         : UPNP_E_SUCCESS;
    dbg("Variable request %s -> %s (%s)",
                                     var_event->StateVarName, result, var_event->ServiceID);
    return 0;
}

int UpnpDevice::handle_action_request(struct upnp_device* priv,
                                      struct Upnp_Action_Request* ar_event) {
    struct service* event_service;
    struct action* event_action;

    event_service = find_service(priv->upnp_device_descriptor,
                                 ar_event->ServiceID);

    event_action = Upnp::get_instance().find_action(event_service, ar_event->ActionName);

    if (event_action == NULL) {
        err("Unknown action '%s' for service '%s'",
                                          ar_event->ActionName, ar_event->ServiceID);
        ar_event->ActionResult = NULL;
        ar_event->ErrCode = 401;
        return -1;
    }

    if (event_service->last_change) {
        ithread_mutex_lock(event_service->service_mutex);
        VariableContainer::get_instance()
			.upnp_last_change_collector_start(event_service->last_change);
        ithread_mutex_unlock(event_service->service_mutex);
    }

    if (event_action->callback) {
        struct action_event event;
        int rc = 0;
        event.request = ar_event;
        event.status = 0;
        event.service = event_service;
        event.device = priv;

        rc = (event_action->callback)(&event);

        if (rc == 0) {
            ar_event->ErrCode = UPNP_E_SUCCESS;
        }

        if (ar_event->ActionResult == NULL) {
            ar_event->ActionResult =
                UpnpMakeActionResponse(ar_event->ActionName,
                                       event_service->service_type,
                                       0, NULL);
        }
    } else {
        err("Got a valid action, but no handler defined (!)\n"
                                          "  ErrCode:    %d\n"
                                          "  Socket:     %d\n"
                                          "  ErrStr:     '%s'\n"
                                          "  ActionName: '%s'\n"
                                          "  DevUDN:     '%s'\n"
                                          "  ServiceID:  '%s'\n",
                                          ar_event->ErrCode, ar_event->Socket, ar_event->ErrStr,
                                          ar_event->ActionName, ar_event->DevUDN,
                                          ar_event->ServiceID);
        ar_event->ErrCode = UPNP_E_SUCCESS;
    }

    if (event_service->last_change) {   // See comment above.
        ithread_mutex_lock(event_service->service_mutex);
        VariableContainer::get_instance()
        .upnp_last_change_collector_finish(event_service->last_change);
        ithread_mutex_unlock(event_service->service_mutex);
    }

    return 0;
}

int UpnpDevice::event_handler(Upnp_EventType EventType, void* event, void* userdata) {
    struct upnp_device* priv = (struct upnp_device*) userdata;

    switch (EventType) {
    case UPNP_CONTROL_ACTION_REQUEST:
        dbg("Action request name:%s\n", ((struct Upnp_Action_Request*)event)->ActionName);
        handle_action_request(priv, (struct Upnp_Action_Request*) event);
        break;

    case UPNP_CONTROL_GET_VAR_REQUEST:
        dbg("Var request name:%s\n", ((struct Upnp_State_Var_Request*)event)->StateVarName);
        handle_var_request(priv, (struct Upnp_State_Var_Request*) event);
        break;

    case UPNP_EVENT_SUBSCRIPTION_REQUEST:
        dbg("Subscription requeset sid:%s\n", ((struct Upnp_Subscription_Request
        *) event)->Sid);
        handle_subscription_request(priv, (struct Upnp_Subscription_Request
                                           *) event);
        break;

    default:
        err("Unknown event type: %d", EventType);
        break;
    }

    return 0;
}

bool UpnpDevice::initialize_device(struct upnp_device_descriptor* device_def,
                                   struct upnp_device* result_device,
                                   const char* ip_address,
                                   unsigned short port) {
    int rc = 0;
    char* buf = NULL;

    rc = UpnpInit(ip_address, port);

    /*
        if (UPNP_E_SUCCESS != rc) {
            err("UpnpInit(ip=%s, port=%d) Error: %s (%d)",
                                              ip_address, port, UpnpGetErrorMessage(rc), rc);
            return FALSE;
        }
    */
    while (UPNP_E_SUCCESS != rc) {
        if (ip_address == NULL) {
            err("ip_address is NULL, waite a second");
            sleep(1);
            rc = UpnpInit(ip_address, port);
        } else {
            err("UpnpInit(ip=%s, port=%d) Error: %s (%d)",
                                              ip_address, port, UpnpGetErrorMessage(rc), rc);
            return FALSE;
        }
    }

    dbg("Registered IP=%s port=%d\n",
                                     UpnpGetServerIpAddress(), UpnpGetServerPort());

    rc = UpnpEnableWebserver(TRUE);

    if (UPNP_E_SUCCESS != rc) {
        err("UpnpEnableWebServer() Error: %s (%d)",
                                          UpnpGetErrorMessage(rc), rc);
        return FALSE;
    }

    if (!_web_server.webserver_register_callbacks()) {
        return FALSE;
    }

    rc = UpnpAddVirtualDir("/upnp");

    if (UPNP_E_SUCCESS != rc) {
        err("UpnpAddVirtualDir() Error: %s (%d)",
                                          UpnpGetErrorMessage(rc), rc);
        return FALSE;
    }

    buf = upnp_create_device_desc(device_def);
    rc = UpnpRegisterRootDevice2(UPNPREG_BUF_DESC,
                                 buf, strlen(buf), 1,
                                 &event_handler, result_device,
                                 &(result_device->device_handle));
    free(buf);

    if (UPNP_E_SUCCESS != rc) {
        err("UpnpRegisterRootDevice2() Error: %s (%d)",
                                          UpnpGetErrorMessage(rc), rc);
        return FALSE;
    }

    rc = UpnpSendAdvertisement(result_device->device_handle, 100);

    if (UPNP_E_SUCCESS != rc) {
        err("Error sending advertisements: %s (%d)", UpnpGetErrorMessage(rc), rc);
        return FALSE;
    }

    return TRUE;
}

struct xmlelement* UpnpDevice::gen_specversion(struct xmldoc* doc,
        int major, int minor) {
    struct xmlelement* top;

    top = XMLDoc::get_instance().xmlelement_new(doc, "specVersion");

    XMLDoc::get_instance().add_value_element_int(doc, top, "major", major);
    XMLDoc::get_instance().add_value_element_int(doc, top, "minor", minor);

    return top;
}

struct xmlelement* UpnpDevice::gen_desc_iconlist(struct xmldoc* doc,
        struct icon** icons) {
    struct xmlelement* top;
    struct xmlelement* parent;
    struct icon* icon_entry;

    top = XMLDoc::get_instance().xmlelement_new(doc, "iconList");

    for (int i = 0; (icon_entry = icons[i]); i++) {
        parent = XMLDoc::get_instance().xmlelement_new(doc, "icon");
        XMLDoc::get_instance().add_value_element(doc, parent, "mimetype", icon_entry->mimetype);
        XMLDoc::get_instance().add_value_element_int(doc, parent, "width", icon_entry->width);
        XMLDoc::get_instance().add_value_element_int(doc, parent, "height", icon_entry->height);
        XMLDoc::get_instance().add_value_element_int(doc, parent, "depth", icon_entry->depth);
        XMLDoc::get_instance().add_value_element(doc, parent, "url", icon_entry->url);
        XMLDoc::get_instance().xmlelement_add_element(doc, top, parent);
    }

    return top;
}

struct xmlelement* UpnpDevice::gen_desc_servicelist(struct upnp_device_descriptor* device_def,
        struct xmldoc* doc) {
    int i = 0;
    struct service* srv;
    struct xmlelement* top;
    struct xmlelement* parent;

    top = XMLDoc::get_instance().xmlelement_new(doc, "serviceList");

    for (i = 0; (srv = device_def->services[i]); i++) {
        parent = XMLDoc::get_instance().xmlelement_new(doc, "service");
        XMLDoc::get_instance().add_value_element(doc, parent, "serviceType", srv->service_type);
        XMLDoc::get_instance().add_value_element(doc, parent, "serviceId", srv->service_id);
        XMLDoc::get_instance().add_value_element(doc, parent, "SCPDURL", srv->scpd_url);
        XMLDoc::get_instance().add_value_element(doc, parent, "controlURL", srv->control_url);
        XMLDoc::get_instance().add_value_element(doc, parent, "eventSubURL", srv->event_url);
        XMLDoc::get_instance().xmlelement_add_element(doc, top, parent);
    }

    return top;
}

struct xmldoc* UpnpDevice::generate_desc(struct upnp_device_descriptor* device_def) {
    struct xmldoc* doc;
    struct xmlelement* root;
    struct xmlelement* child;
    struct xmlelement* parent;

    doc = XMLDoc::get_instance().xmldoc_new();

    root = XMLDoc::get_instance()
           .xmldoc_new_topelement(doc, "root", "urn:schemas-upnp-org:device-1-0");
    child = gen_specversion(doc, 1, 0);
    XMLDoc::get_instance().xmlelement_add_element(doc, root, child);
    parent = XMLDoc::get_instance().xmlelement_new(doc, "device");
    XMLDoc::get_instance().xmlelement_add_element(doc, root, parent);
    XMLDoc::get_instance().add_value_element(doc, parent, "deviceType", device_def->device_type);
    XMLDoc::get_instance()
    .add_value_element(doc, parent, "presentationURL", device_def->presentation_url);
    XMLDoc::get_instance()
    .add_value_element(doc, parent, "friendlyName", device_def->friendly_name);
    XMLDoc::get_instance().add_value_element(doc, parent, "manufacturer", device_def->manufacturer);
    XMLDoc::get_instance()
    .add_value_element(doc, parent, "manufacturerURL", device_def->manufacturer_url);
    XMLDoc::get_instance()
    .add_value_element(doc, parent, "modelDescription", device_def->model_description);
    XMLDoc::get_instance().add_value_element(doc, parent, "modelName", device_def->model_name);
    XMLDoc::get_instance().add_value_element(doc, parent, "modelNumber", device_def->model_number);
    XMLDoc::get_instance().add_value_element(doc, parent, "modelURL", device_def->model_url);
    XMLDoc::get_instance().add_value_element(doc, parent, "UDN", device_def->udn);

    //add_value_element(doc,parent,"serialNumber", device_def->serial_number);
    //add_value_element(doc,parent,"UPC", device_def->upc);
    if (device_def->icons) {
        child = gen_desc_iconlist(doc, device_def->icons);
        XMLDoc::get_instance().xmlelement_add_element(doc, parent, child);
    }

    child = gen_desc_servicelist(device_def, doc);
    XMLDoc::get_instance().xmlelement_add_element(doc, parent, child);

    return doc;
}
}
}
