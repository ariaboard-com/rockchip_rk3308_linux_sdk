//
// Created by zhangtuanqing on 18-4-13.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3FILEPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3FILEPLAYERINTERFACE_H_

#include <memory>
#include <string>
#include "AudioPlayerListener.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

enum PlayMode {
    PLAYMODE_NORMAL,
    PLAYMODE_LOOP
};

class Mp3FilePlayerInterface {
public:
    Mp3FilePlayerInterface() { }

    virtual ~Mp3FilePlayerInterface() { }

    virtual void registerListener(std::shared_ptr<AudioPlayerListener> notify) = 0;

    /**
     * set play mode of the player.
     *
     * @param mode the mode : PLAYMODE_NORMAL PLAYMODE_LOOP
     * @param val loop times, useful when the mode is PLAYMODE_LOOP; loop forever when 0.
     * @return void
     */
    virtual void setPlayMode(PlayMode mode, unsigned int val) = 0;

    virtual void audioPlay(const std::string& url,
                           unsigned int offset,
                           unsigned int report_interval) = 0;

    virtual void audioPause() = 0;

    virtual void audioResume() = 0;

    virtual void audioStop() = 0;

    virtual void setMute() = 0;

    virtual void setUnmute() = 0;

    virtual unsigned long getProgress() = 0;

    virtual unsigned long getDuration() = 0;

    virtual void setFadeIn(int timeSec) = 0;

private:
    Mp3FilePlayerInterface(const Mp3FilePlayerInterface&);

    Mp3FilePlayerInterface& operator=(const Mp3FilePlayerInterface&);
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3FILEPLAYERINTERFACE_H_
