//
// Created by eddy on 18-1-30.
//

#ifndef DUEROS_DCS_APP_DLNA_DLNADMROUTPUTFFMPEG_H
#define DUEROS_DCS_APP_DLNA_DLNADMROUTPUTFFMPEG_H

//#include <gst/gst.h>
#include <string>
#include "DlnaDmrInterfaceOutput.h"
#include "DlnaDmrSongMetaData.h"
#include <DcsSdk/MediaPlayerInterface.h>
#include "MediaPlayerProxy.h"
#include <DcsSdk/ErrorTypes.h>

namespace duerOSDcsApp {
namespace dueros_dlna {

enum MediaPlayerState {
    MEDIAPLAYER_STOPPED,
    MEDIAPLAYER_PLAYING,
    MEDIAPLAYER_PAUSED,
    MEDIAPLAYER_FINISHED
};

struct track_time_info {
    long duration;
    long position;
};

class OutputFfmpeg : public IOutput,
    public duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface {
public:
    int init(void);

    void set_next_uri(const std::string uri);

    void set_uri(const std::string uri,
                 output_update_meta_cb_t meta_cb);

    int play(output_transition_cb_t callback);

    int stop(void);

    int pause(void);

    int seek(long position_nanos);

    int get_position(long* track_duration,
                     long* track_pos);

    int get_volume(float* v);

    int set_volume(float value);

    int get_mute(int* m);

    int set_mute(int m);

    int get_state(void);

    int resume(void);

    void onPlaybackStarted();

    void onPlaybackFinished();

    void onPlaybackStopped();

    void onPlaybackNearlyfinished();

    void onPlaybackError(const duerOSDcsSDK::sdkInterfaces::ErrorType& type, std::string error);

    void onPlaybackPaused();

    void onPlaybackResumed();

    void onBufferUnderrun();

    void onBufferRefilled();

    void onRecvFirstpacket();

private:

    std::shared_ptr<duerOSDcsApp::mediaPlayer::MediaPlayerProxy> m_mediaPlayer;
};
}
}
#endif //DLNADEMO_OUTPUTGSTREAMER_H
