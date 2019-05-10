//
// Created by zhangtuanqing on 18-4-12.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3URLPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3URLPLAYERINTERFACE_H_

#include <string>
#include <memory>
#include "AudioPlayerListener.h"


namespace duerOSDcsApp {
namespace mediaPlayer {

class Mp3UrlPlayerInterface {
public:
    Mp3UrlPlayerInterface() { }

    virtual ~Mp3UrlPlayerInterface() { }

    virtual void registerListener(std::shared_ptr<AudioPlayerListener> notify) = 0;

    virtual void audioPlay(const std::string& url,
                           unsigned int offset,
                           unsigned int report_interval) = 0;

    virtual void audioStop() = 0;

    virtual void audioPause() = 0;

    virtual void audioResume() = 0;

    virtual unsigned long getProgress() = 0;

    virtual unsigned long getDuration() = 0;

    virtual float getFactor() = 0;

    virtual void setFactor(float factor) = 0;

    virtual bool seekBy(unsigned int offsetMs) = 0;

    virtual bool seekTo(unsigned int positionMs) = 0;

private:
    Mp3UrlPlayerInterface(const Mp3UrlPlayerInterface&);

    Mp3UrlPlayerInterface& operator=(const Mp3UrlPlayerInterface&);
};

}  // mediaPlayer
}  // duerOSDcsApp

#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3URLPLAYERINTERFACE_H_
