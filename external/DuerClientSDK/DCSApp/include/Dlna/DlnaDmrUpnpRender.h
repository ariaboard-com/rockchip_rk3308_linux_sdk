////
// Created by eddy on 18-1-29.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRUPNPRENDER_H
#define DUEROS_DCS_APP_DLNA_DLNADMRUPNPRENDER_H

namespace duerOSDcsApp {
namespace dueros_dlna {
class UpnpRender {
public:
    static UpnpRender& get_instance() {
        static UpnpRender m_instance;
        return m_instance;
    }

    void upnp_renderer_dump_connmgr_scpd(void);

    void upnp_renderer_dump_control_scpd(void);

    void upnp_renderer_dump_transport_scpd(void);

    struct upnp_device_descriptor* upnp_renderer_descriptor(const char* name,
            const char* uuid);

private:
    UpnpRender() {

    }

};
}
}
#endif //DLNA_DMR_UPNP_RENDER_H
