////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRXMLESCAPE_H
#define DUEROS_DCS_APP_DLNA_DLNADMRXMLESCAPE_H

namespace duerOSDcsApp {
namespace dueros_dlna {
class XMLEscape {
public:
    static XMLEscape& get_instance() {
        static XMLEscape m_instance;
        return m_instance;
    }

    char* xmlescape(const char* str, int attribute);

private:
    void xmlescape_real(const char* str, char* target, int* length,
                        int attribute);
};
}
}
#endif //DLNA_DMR_XML_ESCAPE_H
