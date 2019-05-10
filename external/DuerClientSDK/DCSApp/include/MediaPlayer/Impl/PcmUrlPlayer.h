#ifndef NETWORKCHECK_FILEDOWNLOADER_H
#define NETWORKCHECK_FILEDOWNLOADER_H
#include <string>
#include <unistd.h>
#include <curl/curl.h>
#include "PcmTtsPlayerImpl.h"
#include "TtsPlayerListener.h"
#include "AudioPlayerListener.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

class PcmUrlPlayer : public TtsPlayerListener {
public:
    PcmUrlPlayer(const std::string& dev);

    ~PcmUrlPlayer();

    void registerListener(AudioPlayerListener* listener);

    void play(const char* url);

    void stop();

protected:
    void playbackStarted();

    void playbackStopped(StopStatus stopStatus);

private:
    PcmUrlPlayer(const PcmUrlPlayer& ths);

    PcmUrlPlayer& operator=(const PcmUrlPlayer& ths);

    void downloadFunc(const char* url);

    static size_t recvData(void* ptr, size_t size, size_t nmemb, void* stream);

private:
    AudioPlayerListener* m_listener;
    static PcmTtsPlayerImpl* s_pcmUrlPlayer;
};

}
}
#endif //NETWORKCHECK_FILEDOWNLOADER_H
