/*
 * Copyright (c) 2017 Baidu, Inc. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef DUEROS_DCS_APP_MEDIAPLAYER_LOCALTTSPROXY_H_
#define DUEROS_DCS_APP_MEDIAPLAYER_LOCALTTSPROXY_H_

#include <DcsSdk/MediaPlayerInterface.h>
#include "LocalTtsPlayerInterface.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

using duerOSDcsSDK::sdkInterfaces::AttachmentReader;

class LocalTtsProxy : public duerOSDcsSDK::sdkInterfaces::LocalSourcePlayerInterface,
    public duerOSDcsSDK::sdkInterfaces::MediaPlayerInterface,
    public LocalTtsPlayerListener {
public:
    static std::shared_ptr<LocalTtsProxy> create(const std::string& audio_device = "default");

    ~LocalTtsProxy();

    void playLocalSource(const std::string& source,
                         bool needFocus,
                         void (*start_callback)(void* arg),
                         void* start_cb_arg,
                         void (*finish_callback)()) override;

    void playTts(const std::string& content, bool needFocus, void (*callback)()) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(
        std::shared_ptr<AttachmentReader> attachmentReader) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(const std::string& url) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setSource(const std::string& tts_content,
            bool repeat) override;

    void setObserver(
        std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> playerObserver) override;

    void setLocalPlayObserver(
        std::shared_ptr<duerOSDcsSDK::sdkInterfaces::LocalSourcePlayerInterface> playerObserver);

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus setOffset(std::chrono::milliseconds offset) override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus play() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus stop() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus pause() override;

    duerOSDcsSDK::sdkInterfaces::MediaPlayerStatus resume() override;

    std::chrono::milliseconds getOffset() override;

    void setImpl(std::shared_ptr<LocalTtsPlayerInterface> impl);

private:
    LocalTtsProxy(const std::string& audio_device);

    bool init();

    void speechStarted() override;

    void speechFinished() override;

private:
    std::string m_tts_content;

    std::shared_ptr<LocalTtsPlayerInterface> m_impl;

    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::MediaPlayerObserverInterface> m_playerObserver;

    std::shared_ptr<duerOSDcsSDK::sdkInterfaces::LocalSourcePlayerInterface> m_local_source_player;
};

}  // mediaPlayer
}  // duerOSDcsApp

#endif  // DUEROS_DCS_APP_MEDIAPLAYER_LOCALTTSPROXY_H_
