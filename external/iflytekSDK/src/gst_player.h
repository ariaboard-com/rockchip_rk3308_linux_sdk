#ifndef __RK_GST_H__
#define __RK_GST_H__

#include <gst/gst.h>
#ifdef __cplusplus
extern "C" {
#endif
#include <gst/gst.h>

void init_gst_player();
GstElement * play_pcm(char* pcm);
void play_music_url(const char* url,gboolean wait);
void pause_music_playing(GstElement *playbin);
void resume_music_playing(GstElement *playbin);
void stop_music_playing(GstElement *playbin);
GstState get_music_state(GstElement *playbin);
void play_alert_file(const char* path,gboolean wait);

extern GstElement *gst_player_url;
extern GstElement *gst_player_pcm;
extern GstElement *gst_player_alert;

#ifdef __cplusplus
} /* extern "C" */
#endif /* C++ */

#endif

