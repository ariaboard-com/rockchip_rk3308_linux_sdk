//
// Created by zhangtuanqing on 18-4-12.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_DLNAPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_DLNAPLAYERINTERFACE_H_

#include "LocalAudioPlayerListener.h"
#include "DCSApp/DeviceIoWrapper.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
class DlnaPlayerInterface : public duerOSDcsApp::application::DeviceIoWrapperObserverInterface {
public:
    DlnaPlayerInterface() {}

    ~DlnaPlayerInterface() { }

    void registerListener(LocalAudioPlayerListener* notify) {};

    virtual void play() = 0;

    virtual void stop() = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;

    virtual void playNext() = 0;

    virtual void playPrevious() = 0;

private:
    DlnaPlayerInterface(const DlnaPlayerInterface&);

    DlnaPlayerInterface& operator=(const DlnaPlayerInterface&);
};

}
}
#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_DLNAPLAYERINTERFACE_H_
