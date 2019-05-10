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

#ifndef DUEROS_DCS_APP_SAMPLEAPP_CONFIGURATION_H_
#define DUEROS_DCS_APP_SAMPLEAPP_CONFIGURATION_H_

#include <string>
#include <fstream>
#include <sstream>
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace duerOSDcsApp {
namespace application {

class Configuration {
public:
    static Configuration* getInstance();

    static void release();

    bool readConfig();

    std::string getRecordAudioPath() const;

    std::string getSsidPrefix() const;

    std::string getDeviceId() const;

    std::string getInfoPlaybackDevice() const;

    std::string getTtsPlaybackDevice() const;

    std::string getMusicPlaybackDevice() const;

    std::string getAlertPlaybackDevice() const;

    std::string getRapidPlaybackDevice() const;

    std::string getNattsPlaybackDevice() const;

    std::string getRecordDevice() const;

    unsigned int getCommVol() const;

    void setCommVol(unsigned int volume);

    unsigned int getAlertsVol() const;

    void setAlertsVol(unsigned int volume);

    bool getDebug() const;

    std::string getAudioLibName() const;

private:
    Configuration();

    ~Configuration();

    Configuration(const Configuration&);

    Configuration& operator=(const Configuration&);

    void addPairToDoc(rapidjson::Document& document,
                      const std::string& key,
                      const std::string& value);

    void addPairToDoc(rapidjson::Document& document,
                      const std::string& key,
                      bool value);

    static Configuration *s_instance;
    std::string m_appConfigFile;
    std::string m_duerosConfigFile;
    unsigned int m_commonVolume;
    unsigned int m_alertsVolume;
    std::string m_infoPlaybackDevice;
    std::string m_ttsPlaybackDevice;
    std::string m_musicPlaybackDevice;
    std::string m_alertPlaybackDevice;
    std::string m_rapidPlaybackDevice;
    std::string m_nattsPlaybackDevice;
    std::string m_deviceId;
    std::string m_ssidPrefix;
    std::string m_recordAudioPath;
    std::string m_audioLibName;
    bool        m_debug;
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_SAMPLEAPP_CONFIGURATION_H_
