////
// Created by eddy on 18-1-30.
//
#include "Dlna/DlnaDmrOutput.h"

namespace duerOSDcsApp {
namespace dueros_dlna {

void Output::add_output_module(std::shared_ptr<IOutput> output_module) {
    m_output_module = output_module;
}

bool Output::is_output_module_added() {
    return !(m_output_module == nullptr);
}

int Output::output_init() {
    return m_output_module->init();
}

void Output::output_set_uri(const char* uri, output_update_meta_cb_t meta_cb) {
    m_output_module->set_uri(uri, meta_cb);
}

void Output::output_set_next_uri(const char* uri) {
    m_output_module->set_next_uri(uri);
}

int Output::output_play(output_transition_cb_t transition_callback) {
    return m_output_module->play(transition_callback);
}

int Output::output_pause(void) {
    return m_output_module->pause();
}

int Output::output_stop(void) {
    return m_output_module->stop();
}

int Output::output_resume(void) {
    return m_output_module->resume();
}

int Output::output_seek(long position_nanos) {
    return m_output_module->seek(position_nanos);
}

int Output::output_get_position(long* track_dur, long* track_pos) {
    return m_output_module->get_position(track_dur, track_pos);
}

int Output::output_get_volume(float* value) {
    return m_output_module->get_volume(value);
}

int Output::output_set_volume(float value) {
    return m_output_module->set_volume(value);
}

int Output::output_get_mute(int* value) {
    return m_output_module->get_mute(value);
}

int Output::output_set_mute(int value) {
    return m_output_module->set_mute(value);
}

bool Output::dlna_is_playing(void) {
    return (m_output_module->get_state() == MEDIAPLAYER_PLAYING);
}

bool Output::dlna_is_paused(void) {
    return (m_output_module->get_state() == MEDIAPLAYER_PAUSED);
}

}
}




















