//
// Created by zhangtuanqing on 18-4-12.
//

#ifndef DCSDEVICEAPP_LOCALAUDIOPLAYERLISTENER_H
#define DCSDEVICEAPP_LOCALAUDIOPLAYERLISTENER_H

namespace duerOSDcsApp {
namespace mediaPlayer {

class LocalAudioPlayerListener {
public:
    virtual ~LocalAudioPlayerListener() {}

    virtual void btStartPlay() = 0;

    virtual void btPausePlay() {}

    virtual void btStopPlay() {}

    virtual void btDisconnect() = 0;

    virtual void btPowerOff() = 0;

    virtual void dlnaStartPlay() = 0;

    virtual void dlnaStopPlay() = 0;

    virtual void dlnaPausePlay() = 0;
};

}  // mediaPlayer
}  // duerOSDcsApp
#endif //DCSDEVICEAPP_LOCALAUDIOPLAYERLISTENER_H
