#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "gst_player.h"
GstElement *gst_player_url;// play network music
GstElement *gst_player_pcm;//play pcm
GstElement *gst_player_alert;//play local audio file

void init_gst_player()  //初始化只要调用一次
{
    gst_init(NULL, NULL);
    gst_player_url = gst_element_factory_make("playbin", "playbin");
    gst_player_alert = gst_element_factory_make("playbin", "playbin");
}


void play_music_url(const char* url, gboolean wait)
{
    printf("---play_music_url = %s--\n", url);
    if(!gst_player_url)
    {
        printf("----play_music_url,gst_element_factory_make fail----\n");
        return;
    }
    if(url == NULL)
    {
        printf("---play_music_url,url is NULL---\n");
        return;
    }
    gst_element_set_state(gst_player_url, GST_STATE_READY);
    gst_element_set_state(gst_player_url, GST_STATE_NULL);
    g_object_set(gst_player_url, "uri", url, NULL);
    /* Start playing */
    gst_element_set_state(gst_player_url, GST_STATE_PLAYING);
    printf("---play_music_url succeed--\n");

    if(wait)
    {
        printf("---wait start!---\n");
        GstBus *bus = gst_element_get_bus(gst_player_url);
        int terminate = 0;
        do
        {
            GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                              GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
            switch(GST_MESSAGE_TYPE(msg))
            {
                case GST_MESSAGE_EOS:
                    printf("MUSIC EOS!\n");
                    terminate = 1;
                    break;
                case GST_MESSAGE_ERROR:
                    terminate = 1;
                    GError *err = NULL;
                    gst_message_parse_error (msg, &err, NULL);
                    printf ("MUSIC ERROR: %s\n", err->message);
                    g_clear_error (&err);
                    break;
            }
            gst_message_unref(msg);
            if(get_music_state(gst_player_url) == GST_STATE_NULL)  // being stop
            {
                printf("PLAYING BEING STOPED!\n");
                break;
            }
        }
        while(!terminate);
        gst_object_unref(bus);
        printf("---wait end!---\n");
    }
}

void play_alert_file(const char* path, gboolean wait)
{
    printf("-----play_alert_file %s--\n", path);
    if(!gst_player_alert)
    {
        printf("----play_alert_file,gst_element_factory_make fail----\n");
        return;
    }
    if(path == NULL)
    {
        printf("---play_alert_file,path is NULL---\n");
        return;
    }
    gst_element_set_state(gst_player_alert, GST_STATE_READY);
    gst_element_set_state(gst_player_alert, GST_STATE_NULL);

    g_object_set(gst_player_alert, "uri", path, NULL);
    /* Start playing */
    gst_element_set_state(gst_player_alert, GST_STATE_PLAYING);
    printf("---play_alert_file succeed--\n");
    if(wait)
    {
        printf("---wait start!---\n");
        GstBus *bus = gst_element_get_bus(gst_player_alert);
        int terminate = 0;
        do
        {
            GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                              GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
            switch(GST_MESSAGE_TYPE(msg))
            {
                case GST_MESSAGE_EOS:
                    printf("ALERT EOS!\n");
                    terminate = 1;
                    break;
                case GST_MESSAGE_ERROR:
                    terminate = 1;
                    GError *err = NULL;
                    gst_message_parse_error (msg, &err, NULL);
                    printf ("ALERT ERROR: %s\n", err->message);
                    g_clear_error (&err);
                    break;
            }
            gst_message_unref(msg);
        }
        while(!terminate);
        gst_object_unref(bus);
        printf("---wait end!---\n");
    }
}

GstElement * play_pcm(char* pcm)
{
    //gst_init (NULL,NULL);
    GstElement *playbin, *source, *sink;
    GstCaps *caps;

    playbin = gst_pipeline_new("playbin");
    source = gst_element_factory_make("filesrc", "source");
    sink = gst_element_factory_make("alsasink", "sink");
    caps = gst_caps_new_simple("audio/x-raw",
                               "format", G_TYPE_STRING, "S16LE",
                               "rate", G_TYPE_INT, 16000,
                               "channels", G_TYPE_INT, 8,
                               NULL);

    gst_bin_add_many(GST_BIN(playbin), source, sink, NULL);
    gst_element_link_filtered(source, sink, caps);
    gst_caps_unref(caps);
    g_object_set(source, "location", pcm, NULL);
    gst_element_set_state(playbin, GST_STATE_PLAYING);

    return playbin;
}


void pause_music_playing(GstElement *playbin)
{
    if(playbin != NULL)
    {
        gst_element_set_state(playbin, GST_STATE_PAUSED);
    }
}

void resume_music_playing(GstElement *playbin)
{
    if(playbin != NULL)
    {
        gst_element_set_state(playbin, GST_STATE_PLAYING);
    }
}

void stop_music_playing(GstElement *playbin)
{
    if(playbin != NULL)
    {
        gst_element_set_state(playbin, GST_STATE_READY);
        gst_element_set_state(playbin, GST_STATE_NULL);
        //g_object_unref(playbin);
    }
}


GstState get_music_state(GstElement *playbin)
{
    if(playbin != NULL)
    {
        GstState curState = GST_STATE_NULL;
        gst_element_get_state(playbin, &curState, NULL, GST_CLOCK_TIME_NONE);
        return curState;
    }
    return  GST_STATE_NULL;
}

