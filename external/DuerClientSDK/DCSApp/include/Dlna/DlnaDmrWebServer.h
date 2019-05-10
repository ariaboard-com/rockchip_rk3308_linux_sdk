////
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRWEBSERVER_H
#define DUEROS_DCS_APP_DLNA_DLNADMRWEBSERVER_H

#include "DlnaDmrLogging.h"

namespace duerOSDcsApp {
namespace dueros_dlna {
typedef struct {
    off_t pos;
    const char* contents;
    size_t len;
} WebServerFile;

struct virtual_file {
    const char* virtual_fname;
    const char* contents;
    const char* content_type;
    size_t len;
    struct virtual_file* next;
};

class WebServer {
public:
    bool webserver_register_callbacks(void);

    int webserver_register_buf(const char* path, const char* contents,
                               const char* content_type);

    int webserver_register_file(const char* path,
                                const char* content_type);

};
}
}
#endif //DLNA_DMR_WEB_SERVER_H
