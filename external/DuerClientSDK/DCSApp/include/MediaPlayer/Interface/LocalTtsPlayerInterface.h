//
// Created by zhangtuanqing on 18-4-13.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_LOCALTTSPLAYERINTERFACE_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_LOCALTTSPLAYERINTERFACE_H_


namespace duerOSDcsApp {
namespace mediaPlayer {

enum TtsPlayerStatus {
    TtsPlayerStatusIdle,
    TtsPlayerStatusPlay,
    TtsPlayerStatusStop,
};

class LocalTtsPlayerListener {
public:
    virtual ~LocalTtsPlayerListener() {}

    virtual void speechStarted() = 0;

    virtual void speechFinished() = 0;
};

class LocalTtsPlayerInterface {
public:
    LocalTtsPlayerInterface() {}

    virtual ~LocalTtsPlayerInterface() {}

    virtual void ttsPlay(const std::string& content) = 0;

    virtual void ttsStop() = 0;

    virtual void registerListener(std::shared_ptr<LocalTtsPlayerListener> notify) = 0;

private:

    LocalTtsPlayerInterface(const LocalTtsPlayerInterface&);

    LocalTtsPlayerInterface& operator=(const LocalTtsPlayerInterface&);
};

}  // mediaPlayer
}  // duerOSDcsApp

#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_LOCALTTSPLAYERINTERFACE_H_
