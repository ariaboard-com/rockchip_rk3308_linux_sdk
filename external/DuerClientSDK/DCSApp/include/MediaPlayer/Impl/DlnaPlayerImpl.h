//
// Created by zhangtuanqing on 18-4-12.
//

#ifndef DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_DLNAPLAYERIMPL_H_
#define DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_DLNAPLAYERIMPL_H_


#include "DlnaPlayerInterface.h"
#include "LocalAudioPlayerListener.h"

namespace duerOSDcsApp {
namespace mediaPlayer {
class DlnaPlayerImpl : public DlnaPlayerInterface {
public:
    DlnaPlayerImpl();

    ~DlnaPlayerImpl();

    void registerListener(std::shared_ptr<LocalAudioPlayerListener> notify);

    void play() override;

    void stop() override;

    void pause() override ;

    void resume() override;

    void playNext() override;

    void playPrevious() override;

    virtual void dlnaStartPlay() override;

    virtual void dlnaStopPlay() override;

    virtual void dlnaPausePlay() override;

    static std::shared_ptr<DlnaPlayerImpl> create();

private:
    DlnaPlayerImpl(const DlnaPlayerImpl&);

    DlnaPlayerImpl& operator=(const DlnaPlayerImpl&);

private:
    std::shared_ptr<LocalAudioPlayerListener> m_listener;
};

}
}


#endif //DUEROS_DCSAPP_INCLUDE_MEDIAPLAYER_DLNAPLAYERIMPL_H_
