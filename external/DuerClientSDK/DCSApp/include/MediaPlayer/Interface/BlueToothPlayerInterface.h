//
// Created by zhangtuanqing on 18-4-10.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_BLUETOOTHPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_BLUETOOTHPLAYERINTERFACE_H_
#include "LocalAudioPlayerListener.h"
#include "DCSApp/DeviceIoWrapper.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
class BlueToothPlayerInterface : public
        duerOSDcsApp::application::DeviceIoWrapperObserverInterface {
public:
    BlueToothPlayerInterface() {}

    ~BlueToothPlayerInterface() { }

    void registerListener(LocalAudioPlayerListener* notify) {};

    virtual void play() = 0;

    virtual void stop() = 0;

    virtual void pause() = 0;

    virtual void resume() = 0;

    virtual void playNext() = 0;

    virtual void playPrevious() = 0;

private:
    BlueToothPlayerInterface(const BlueToothPlayerInterface&);

    BlueToothPlayerInterface& operator=(const BlueToothPlayerInterface&);
};

}
}

#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_BLUETOOTHPLAYERINTERFACE_H_
