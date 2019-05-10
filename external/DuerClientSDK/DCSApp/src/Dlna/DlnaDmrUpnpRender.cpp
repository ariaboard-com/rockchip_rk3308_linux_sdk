////
// Created by eddy on 18-1-30.
//
#include <stdio.h>
#include <assert.h>
#include "Dlna/upnp/upnp.h"
#include "Dlna/upnp/ithread.h"
#include "Dlna/DlnaDmrUpnpDevice.h"
#include "Dlna/DlnaDmrUpnpConnMgr.h"
#include "Dlna/DlnaDmrUpnpControl.h"
#include "Dlna/DlnaDmrUpnpTransport.h"
#include "Dlna/DlnaDmrUpnpRender.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
static struct icon icon1 = {
    .width =        64,
    .height =       64,
    .depth =        24,
    .url =          "/upnp/grender-64x64.png",
    .mimetype =     "image/png"
};
static struct icon icon2 = {
    .width =        128,
    .height =       128,
    .depth =        24,
    .url =          "/upnp/grender-128x128.png",
    .mimetype =     "image/png"
};

static struct icon* renderer_icon[] = {
    &icon1,
    &icon2,
    NULL
};

static int upnp_renderer_init(void);

static struct upnp_device_descriptor render_device = {
    .init_function          = upnp_renderer_init,
    .device_type            = "urn:schemas-upnp-org:device:MediaRenderer:1",
    .friendly_name          = "DuerOSRender",
    .manufacturer           = "DuerOS",
    .manufacturer_url       = "http://xxxxxxxxxx",
    .model_description      = "",
    .model_name             = "DuerOS-Render",
    .model_number           = "",
    .model_url              = "http://xxxxxxxx",
    .serial_number          = "1",
    .udn                    = "uuid:DuerOSRender-1_0-000-000-002",
    .upc                    = "",
    .presentation_url       = "",
    .icons                  = renderer_icon,
};

static int upnp_renderer_init(void) {
    static struct service* upnp_services[4];
    upnp_services[0] = UpnpTransport::get_instance().upnp_transport_get_service();
    upnp_services[1] = UpnpConnMgr::get_instance().upnp_connmgr_get_service();
    upnp_services[2] = UpnpControl::get_instance().upnp_control_get_service();
    upnp_services[3] = NULL;
    render_device.services = upnp_services;
    return UpnpConnMgr::get_instance().connmgr_init();
}

void UpnpRender::upnp_renderer_dump_connmgr_scpd(void) {
    char* buf = NULL;
    buf = Upnp::get_instance().upnp_get_scpd(UpnpConnMgr::get_instance().upnp_connmgr_get_service
            ());
    assert(buf != NULL);
    fputs(buf, stdout);
}

void UpnpRender::upnp_renderer_dump_control_scpd(void) {
    char* buf = NULL;
    buf = Upnp::get_instance().upnp_get_scpd(UpnpControl::get_instance().upnp_control_get_service
            ());
    assert(buf != NULL);
    fputs(buf, stdout);
}

void UpnpRender::upnp_renderer_dump_transport_scpd(void) {
    char* buf = NULL;
    buf = Upnp::get_instance().upnp_get_scpd(UpnpTransport::get_instance()
            .upnp_transport_get_service());
    assert(buf != NULL);
    fputs(buf, stdout);
}


struct upnp_device_descriptor*
UpnpRender::upnp_renderer_descriptor(const char* friendly_name,
                                     const char* uuid) {
    render_device.friendly_name = friendly_name;

    char* udn = NULL;

    if (asprintf(&udn, "uuid:%s", uuid) > 0) {
        render_device.udn = udn;
    }

    return &render_device;
}

}
}










