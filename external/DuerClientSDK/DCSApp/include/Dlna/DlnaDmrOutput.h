////
// Created by eddy on 18-1-29.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMROUTPUT_H
#define DUEROS_DCS_APP_DLNA_DLNADMROUTPUT_H

#include <memory>
#include "DlnaDmrSongMetaData.h"
#include "DlnaDmrInterfaceOutput.h"
#include "DlnaDmrOutputFfmpeg.h"

namespace duerOSDcsApp {
namespace dueros_dlna {

class Output {
public:
    static Output& get_instance() {
        static Output m_instance;

        return m_instance;
    }

    bool is_output_module_added();

    void add_output_module(std::shared_ptr<IOutput>);

    int output_init();

    void output_dump_modules(void);

    void output_set_uri(const char* uri, output_update_meta_cb_t meta_info);

    void output_set_next_uri(const char* uri);

    int output_play(output_transition_cb_t done_callback);

    int output_stop(void);

    int output_pause(void);

    int output_get_position(long* track_dur_nanos, long* track_pos_nanos);

    int output_seek(long position_nanos);

    int output_get_volume(float* v);

    int output_set_volume(float v);

    int output_get_mute(int* m);

    int output_set_mute(int m);

    bool dlna_is_playing(void);

    bool dlna_is_paused(void);

    int output_resume(void);

private:
    Output() {

    };

private:
    std::shared_ptr<IOutput> m_output_module;
};
}
}
#endif //DLNA_DMR_OUTPUT_H
