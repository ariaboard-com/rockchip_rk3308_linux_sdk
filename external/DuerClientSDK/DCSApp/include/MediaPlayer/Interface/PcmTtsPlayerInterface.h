//
// Created by zhangtuanqing on 18-4-12.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMTTSPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMTTSPLAYERINTERFACE_H_

#include "TtsPlayerListener.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

class PcmTtsPlayerInterface {
public:
    PcmTtsPlayerInterface() {};

    virtual ~PcmTtsPlayerInterface() {};

    virtual void registerListener(std::shared_ptr<TtsPlayerListener> listener) = 0;

    virtual void ttsPlay() = 0;

    virtual void pushData(const char* data, unsigned int len) = 0;

    virtual void ttsEnd() = 0;

    virtual void ttsStop() = 0;

private:

    PcmTtsPlayerInterface(const PcmTtsPlayerInterface&);

    PcmTtsPlayerInterface& operator=(const PcmTtsPlayerInterface&);
};

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp
#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_PCMTTSPLAYERINTERFACE_H_
