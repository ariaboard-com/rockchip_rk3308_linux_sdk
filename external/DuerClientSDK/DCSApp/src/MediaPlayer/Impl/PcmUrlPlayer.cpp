#include <memory>
#include "PcmUrlPlayer.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

PcmTtsPlayerImpl* PcmUrlPlayer::s_pcmUrlPlayer = NULL;

PcmUrlPlayer::PcmUrlPlayer(const std::string& dev) : m_listener(NULL) {
    if (s_pcmUrlPlayer) {
        delete s_pcmUrlPlayer;
        s_pcmUrlPlayer = NULL;
    }

    s_pcmUrlPlayer = new PcmTtsPlayerImpl(dev);
    s_pcmUrlPlayer->registerListener(std::shared_ptr<PcmUrlPlayer>(this));
}

PcmUrlPlayer::~PcmUrlPlayer() {
    if (s_pcmUrlPlayer) {
        delete s_pcmUrlPlayer;
        s_pcmUrlPlayer = NULL;
    }
}

void PcmUrlPlayer::registerListener(AudioPlayerListener* listener) {
    m_listener = listener;
}

size_t PcmUrlPlayer::recvData(void* ptr, size_t size, size_t nmemb, void* stream) {
    s_pcmUrlPlayer->pushData((char*)ptr, size * nmemb);
    return size * nmemb;
}

void PcmUrlPlayer::play(const char* url) {
    s_pcmUrlPlayer->ttsPlay();
    downloadFunc(url);
    s_pcmUrlPlayer->ttsEnd();
}

void PcmUrlPlayer::stop() {
    s_pcmUrlPlayer->ttsStop();
}

void PcmUrlPlayer::downloadFunc(const char* url) {
    CURL* curl_handle = NULL;
    curl_global_init(CURL_GLOBAL_ALL);
    /* init the curl session */
    curl_handle = curl_easy_init();
    /* set URL to get here */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    /* Switch on full protocol/debug output while testing */
    curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
    /* disable progress meter, set to 0L to enable and disable debug output */
    curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L);
    /* send all data to this function  */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, &PcmUrlPlayer::recvData);
    /* write the page body to this file handle */
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, NULL);
    /* get it! */
    curl_easy_perform(curl_handle);
    /* close the header file */
    curl_easy_cleanup(curl_handle);
}

void PcmUrlPlayer::playbackStarted() {
    if (m_listener) {
        m_listener->playbackStarted(0);
    }
}

void PcmUrlPlayer::playbackStopped(StopStatus stopStatus) {
    if (m_listener) {
        m_listener->playbackFinished(0, AUDIOPLAYER_FINISHSTATUS_END);
    }
}

}
}