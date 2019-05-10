//
// Created by zhangtuanqing on 18-4-13.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMFILEPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMFILEPLAYERINTERFACE_H_


namespace duerOSDcsApp {
namespace mediaPlayer {

class PcmFilePlayerInterface {
public:
    PcmFilePlayerInterface() {};

    ~PcmFilePlayerInterface() {};

    virtual void play() = 0;

    virtual void stop() = 0;

private:

    PcmFilePlayerInterface(const PcmFilePlayerInterface&);

    PcmFilePlayerInterface& operator=(const PcmFilePlayerInterface&);
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp

#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMFILEPLAYERINTERFACE_H_
