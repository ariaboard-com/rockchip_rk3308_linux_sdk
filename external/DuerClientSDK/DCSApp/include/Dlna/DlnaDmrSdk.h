////
// Created by eddy on 18-2-2.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMRSDK_H
#define DUEROS_DCS_APP_DLNA_DLNADMRSDK_H

#include<string>
#include "DlnaDmrUpnpDevice.h"
#include "DlnaDmrInterfaceOutput.h"

namespace duerOSDcsApp {
namespace dueros_dlna {

class DlnaDmrSdk {
public:
    DlnaDmrSdk();

    void add_output_module(std::shared_ptr<IOutput> output_module);

    void set_uuid(std::string uuid);

    std::string get_uuid();

    void set_friendly_name(std::string name);

    std::string get_friendly_name();

    void set_listen_port(int port);

    int get_listen_port();

    int start();

    int stop();

private:
    void param_init();

    void param_reset();

    static void log_variable_change(void* userdata, int var_num,
                                    const char* variable_name,
                                    const char* old_value,
                                    const char* variable_value);

private:
    int _listen_port;
    std::string _uuid;
    std::string _friendly_name;
};
}
}
#endif //DLNA_DMR_SDK_H
