//
// Created by zhangtuanqing on 18-4-10.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_BLUETOOTHPLAYERIMPL_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_BLUETOOTHPLAYERIMPL_H_

#include "BlueToothPlayerInterface.h"
#include "LocalAudioPlayerListener.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
class BluetoothPlayerImpl : public BlueToothPlayerInterface {
public:
    BluetoothPlayerImpl();

    ~BluetoothPlayerImpl();

    void registerListener(std::shared_ptr<LocalAudioPlayerListener> notify);

    void play() override;

    void stop() override;

    void pause() override ;

    void resume() override;

    void playNext() override;

    void playPrevious() override;

    virtual void btStartPlay() override;

    virtual void btStopPlay() override;

    virtual void btDisconnect() override;

    virtual void btPowerOff() override;

    static std::shared_ptr<BluetoothPlayerImpl> create();

private:
    BluetoothPlayerImpl(const BluetoothPlayerImpl&);

    BluetoothPlayerImpl& operator=(const BluetoothPlayerImpl&);

private:
    std::shared_ptr<LocalAudioPlayerListener> m_listener;
};

}
}

#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_BLUETOOTHPLAYERIMPL_H_
