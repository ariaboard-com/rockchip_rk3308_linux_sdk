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

#ifndef DUEROS_DCS_SDK_TTS_MANAGER_INCLUDE_TTS_MANAGER_TTS_MANAGER_H
#define DUEROS_DCS_SDK_TTS_MANAGER_INCLUDE_TTS_MANAGER_TTS_MANAGER_H

#include <BDSSDKMessage.hpp>
#include <BDSpeechSDK.hpp>

#include <pthread.h>
#include <string>
#include <memory>

namespace duerOSDcsSDK {
namespace ttsManager {

class TTSSynthesizeResultObserver {
public:
    virtual ~TTSSynthesizeResultObserver() {}

    virtual void ttsSynthesizeBegin(const std::string& strIndex) = 0;

    virtual void ttsSynthesizeAudioData(const char *stream, unsigned int length,
                                        unsigned int charIndex, const std::string& strIndex="") = 0;

    virtual void ttsSynthesizeFinish(const std::string& strIndex = "") = 0;

    virtual void ttsSynthesizeFailed(int errCode, const std::string &errDesc,
                                     const std::string& strIndex = "") = 0;

};

enum TTSEngineToneType {
    TTSEngineToneType_female = 0,
    TTSEngineToneType_male_normal = 1,
    TTSEngineToneType_male_special = 2,
    TTSEngineToneType_male_emotion = 3,
    TTSEngineToneType_children_emotion = 4,
    TTSEngineToneType_gezi = 100
};

class TTSManager {
public:

    static void loadTtsEngine(const std::string& resourcePath);
    static void unloadTtsEngine();

    //static std::unique_ptr<TTSManager> create(const std::string& resourcePath);

    static TTSManager* getTtsManagerInstance();

    virtual ~TTSManager();

    void setTtsSynthesizeResultObserver(TTSSynthesizeResultObserver* observer);

    TTSSynthesizeResultObserver* getResultObserver();

    void synthesizeTextToTts(const std::string& text, const std::string& strIndex="");

    void stopSynthesize();

    void pauseSynthesize();

    void resumeSynthesize();

    void switchTtsSynthesizeEngineToneType(TTSEngineToneType type);
    void setTtsSynthesizeEnginePitch(int value);
    void setTtsSynthesizeEngineSpeed(int value);
    void setTtsSynthesizeEngineVolume(int value);

protected:
    //static std::unique_ptr<TTSManager> getTtsManagerInstance();

    static void createTtsManagerInstance(const std::string& resourcePath);

private:
    TTSManager();

    void loadTtsSynthesizeEngine(const std::string& resourcePath);
    void setTtsSynthesizeEngineToneType(int type);
    void reloadTtsSynthesizeEngineData(bool online, TTSEngineToneType toneType);

    TTSEngineToneType getCurrTtsEngineToneType();
    bool saveTtsEngineToneType(TTSEngineToneType type);

    static void ttsSynthesizeListener(bds::BDSSDKMessage &message, void *myParam);

private:
    TTSSynthesizeResultObserver *m_resultObserver;
    pthread_mutex_t m_mutex;
    std::string m_resourcePath;
    static TTSManager* m_sInstance;
};

} // namespace ttsManager
} // namespace duerOSDcsSDK

#endif // DUEROS_DCS_SDK_TTS_MANAGER_INCLUDE_TTS_MANAGER_TTS_MANAGER_H
