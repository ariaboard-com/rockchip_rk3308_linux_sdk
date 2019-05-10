//
// Created by zhangtuanqing on 18-4-12.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3STREAMPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3STREAMPLAYERINTERFACE_H_

#include "TtsPlayerListener.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

class Mp3TtsPlayerInterface {
public:
    Mp3TtsPlayerInterface() {};

    virtual ~Mp3TtsPlayerInterface() {};

    virtual void registerListener(std::shared_ptr<TtsPlayerListener> listener) = 0;

    virtual void ttsPlay() = 0;

    virtual void pushData(const char* data, unsigned int len) = 0;

    virtual void ttsEnd() = 0;

    virtual void ttsStop() = 0;

private:

    Mp3TtsPlayerInterface(const Mp3TtsPlayerInterface&);

    Mp3TtsPlayerInterface& operator=(const Mp3TtsPlayerInterface&);
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp
#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_MP3STREAMPLAYERINTERFACE_H_
