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

#include "DCSApp/Configuration.h"
#include "LoggerUtils/DcsSdkLogger.h"
#include "DCSApp/DeviceIoWrapper.h"
#include "DeviceTools/FileUtils.h"
#include "DeviceTools/StringUtils.h"
#include <rapidjson/istreamwrapper.h>

namespace duerOSDcsApp {
namespace application {

#ifdef MTK8516
#define MUSIC_DEFAULT_PLAYBACK_DEVICE  "duer_plug"
#define INFO_DEFAULT_PLAYBACK_DEVICE   "track:1"
#define TTS_DEFAULT_PLAYBACK_DEVICE    "track:2"
#define ALERT_DEFAULT_PLAYBACK_DEVICE  "track:3"
#define RAPID_DEFAULT_PLAYBACK_DEVICE  "track:7"
#define NATTS_DEFAULT_PLAYBACK_DEVICE  "track:8"
#else
#define MUSIC_DEFAULT_PLAYBACK_DEVICE  "default"
#define INFO_DEFAULT_PLAYBACK_DEVICE   "default"
#define TTS_DEFAULT_PLAYBACK_DEVICE    "default"
#define ALERT_DEFAULT_PLAYBACK_DEVICE  "default"
#define RAPID_DEFAULT_PLAYBACK_DEVICE  "default"
#define NATTS_DEFAULT_PLAYBACK_DEVICE  "default"
#endif

#define NETWORK_HODOR_SSID_PREFIX       "Hodor_"
#define NETWORK_KUKE_SSID_PREFIX        "KUKE_"
#define NETWORK_DOT_SSID_PREFIX         "Dot_"
#define NETWORK_STANDARD_SSID_PREFIX    "Standard_"
#define NETWORK_OTHER_SSID_PREFIX       "DuerOS_"

#define DEFAULT_COMMON_VOLUME 50
#define DEFAULT_ALERTS_VOLUME 60

Configuration* Configuration::s_instance = NULL;

Configuration* Configuration::getInstance() {
    if (NULL == s_instance) {
        s_instance = new Configuration();
    }
    return s_instance;
}

void Configuration::release() {
    if (NULL != s_instance) {
        delete s_instance;
        s_instance = NULL;
    }
}

Configuration::Configuration():m_appConfigFile("./appresources/app_config.json"),
                               m_duerosConfigFile("/data/duer/dueros_config.json"),
                               m_commonVolume(DEFAULT_COMMON_VOLUME),
                               m_alertsVolume(DEFAULT_ALERTS_VOLUME),
                               m_infoPlaybackDevice(INFO_DEFAULT_PLAYBACK_DEVICE),
                               m_ttsPlaybackDevice(TTS_DEFAULT_PLAYBACK_DEVICE),
                               m_musicPlaybackDevice(MUSIC_DEFAULT_PLAYBACK_DEVICE),
                               m_alertPlaybackDevice(ALERT_DEFAULT_PLAYBACK_DEVICE),
                               m_rapidPlaybackDevice(RAPID_DEFAULT_PLAYBACK_DEVICE),
                               m_nattsPlaybackDevice(NATTS_DEFAULT_PLAYBACK_DEVICE),
                               m_deviceId(""),
                               m_ssidPrefix(""),
                               m_recordAudioPath(""),
                               m_debug(false) {
}

Configuration::~Configuration() {

}

bool Configuration::readConfig() {
    const std::string appConfigFile = "";
    std::string duerosConfigFile = "";

    duerosConfigFile = "/data/duer/dueros_config.json";

    if (!duerosConfigFile.empty()) {
        m_duerosConfigFile = duerosConfigFile;
    }
    if (!appConfigFile.empty()) {
        m_appConfigFile = appConfigFile;
    }
    //handle app config file.
    std::string appConfigDir = deviceCommonLib::deviceTools::parseDir(m_appConfigFile);
    if (!deviceCommonLib::deviceTools::isDirExist(appConfigDir)) {
        if (!deviceCommonLib::deviceTools::mkdirR(appConfigDir.c_str())) {
            APP_ERROR("Please verify %s is a valid path!", appConfigDir.c_str());
            return false;
        }
    }

    bool isRecreateAppConfigFile = false;

    if (access(m_appConfigFile.c_str(), F_OK) == 0) {
        std::ifstream appConfigStream(m_appConfigFile);
        if (appConfigStream.good()) {
            rapidjson::IStreamWrapper appConfigStreamWrapper(appConfigStream);
            rapidjson::Document readDocument;
            readDocument.ParseStream(appConfigStreamWrapper);

            if (readDocument.HasParseError()) {
                isRecreateAppConfigFile = true;
            } else {
                if (readDocument.HasMember("record_audio_path")) {
                    m_recordAudioPath = readDocument["record_audio_path"].GetString();
                } else {
                    m_recordAudioPath = "";
                }

                if (readDocument.HasMember("ssid_prefix")) {
                    m_ssidPrefix = readDocument["ssid_prefix"].GetString();
                } else {
#if (defined Hodor)
                m_ssidPrefix = NETWORK_HODOR_SSID_PREFIX;
#elif (defined Dot)
                m_ssidPrefix = NETWORK_DOT_SSID_PREFIX;
#elif (defined Kuke)
                m_ssidPrefix = NETWORK_KUKE_SSID_PREFIX;
#elif (defined MTK)
                m_ssidPrefix = NETWORK_STANDARD_SSID_PREFIX;
#else
                m_ssidPrefix = NETWORK_OTHER_SSID_PREFIX;
#endif
                }

                if (readDocument.HasMember("info_playback_device")) {
                    m_infoPlaybackDevice = readDocument["info_playback_device"].GetString();
                    if (m_infoPlaybackDevice.empty()) {
                        m_infoPlaybackDevice = INFO_DEFAULT_PLAYBACK_DEVICE;
                    }
                } else {
                    m_infoPlaybackDevice = INFO_DEFAULT_PLAYBACK_DEVICE;
                }

                if (readDocument.HasMember("tts_playback_device")) {
                    m_ttsPlaybackDevice = readDocument["tts_playback_device"].GetString();
                    if (m_ttsPlaybackDevice.empty()) {
                        m_ttsPlaybackDevice = TTS_DEFAULT_PLAYBACK_DEVICE;
                    }
                } else {
                    m_ttsPlaybackDevice = TTS_DEFAULT_PLAYBACK_DEVICE;
                }

                if (readDocument.HasMember("music_playback_device")) {
                    m_musicPlaybackDevice = readDocument["music_playback_device"].GetString();
                    if (m_musicPlaybackDevice.empty()) {
                        m_musicPlaybackDevice = MUSIC_DEFAULT_PLAYBACK_DEVICE;
                    }
                } else {
                    m_musicPlaybackDevice = MUSIC_DEFAULT_PLAYBACK_DEVICE;
                }

                if (readDocument.HasMember("alert_playback_device")) {
                    m_alertPlaybackDevice = readDocument["alert_playback_device"].GetString();
                    if (m_alertPlaybackDevice.empty()) {
                        m_alertPlaybackDevice = ALERT_DEFAULT_PLAYBACK_DEVICE;
                    }
                } else {
                    m_alertPlaybackDevice = ALERT_DEFAULT_PLAYBACK_DEVICE;
                }

                if (readDocument.HasMember("rapid_playback_device")) {
                    m_rapidPlaybackDevice = readDocument["rapid_playback_device"].GetString();
                    if (m_rapidPlaybackDevice.empty()) {
                        m_rapidPlaybackDevice = RAPID_DEFAULT_PLAYBACK_DEVICE;
                    }
                } else {
                    m_rapidPlaybackDevice = RAPID_DEFAULT_PLAYBACK_DEVICE;
                }

                if (readDocument.HasMember("natts_playback_device")) {
                    m_nattsPlaybackDevice = readDocument["natts_playback_device"].GetString();
                    if (m_nattsPlaybackDevice.empty()) {
                        m_nattsPlaybackDevice = NATTS_DEFAULT_PLAYBACK_DEVICE;
                    }
                } else {
                    m_nattsPlaybackDevice = NATTS_DEFAULT_PLAYBACK_DEVICE;
                }

                if (readDocument.HasMember("debug")) {
                    m_debug = readDocument["debug"].GetBool();
                }

                if (readDocument.HasMember("audio_lib_name")) {
                    m_audioLibName = readDocument["audio_lib_name"].GetString();
                }

                rapidjson::Document document;
                document.SetObject();
                addPairToDoc(document, "record_audio_path", m_recordAudioPath);
                addPairToDoc(document, "ssid_prefix", m_ssidPrefix);
                addPairToDoc(document, "info_playback_device", m_infoPlaybackDevice);
                addPairToDoc(document, "tts_playback_device", m_ttsPlaybackDevice);
                addPairToDoc(document, "music_playback_device", m_musicPlaybackDevice);
                addPairToDoc(document, "alert_playback_device", m_alertPlaybackDevice);
                addPairToDoc(document, "rapid_playback_device", m_rapidPlaybackDevice);
                addPairToDoc(document, "natts_playback_device", m_nattsPlaybackDevice);
                addPairToDoc(document, "audio_lib_name", m_audioLibName);
                if (m_debug) {
                    addPairToDoc(document, "debug", m_debug);
                }

                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                document.Accept(writer);
                const char* str = buffer.GetString();
                FILE *fp = fopen(m_appConfigFile.c_str(), "w");
                if (fp) {
                    fwrite(str, sizeof(char), strlen(str), fp);
                    fclose(fp);
                    sync();
                } else {
                    printf("Configuration can't open appConfigFile\n");
                }
            }
        } else {
            isRecreateAppConfigFile = true;
        }
    } else {
        isRecreateAppConfigFile = true;
    }

    if (isRecreateAppConfigFile) {
        //配置文件自动生成时，根据不同设备自动填充，也可以手动修改配置文件，进行设置
#if (defined Hodor)
        m_ssidPrefix = NETWORK_HODOR_SSID_PREFIX;
#elif (defined Dot)
        m_ssidPrefix = NETWORK_DOT_SSID_PREFIX;
#elif (defined Kuke)
        m_ssidPrefix = NETWORK_KUKE_SSID_PREFIX;
#elif (defined MTK)
        m_ssidPrefix = NETWORK_STANDARD_SSID_PREFIX;
#else
        m_ssidPrefix = NETWORK_OTHER_SSID_PREFIX;
#endif
        m_infoPlaybackDevice = INFO_DEFAULT_PLAYBACK_DEVICE;
        m_ttsPlaybackDevice = TTS_DEFAULT_PLAYBACK_DEVICE;
        m_musicPlaybackDevice = MUSIC_DEFAULT_PLAYBACK_DEVICE;
        m_alertPlaybackDevice = ALERT_DEFAULT_PLAYBACK_DEVICE;
        m_rapidPlaybackDevice = RAPID_DEFAULT_PLAYBACK_DEVICE;
        m_nattsPlaybackDevice = NATTS_DEFAULT_PLAYBACK_DEVICE;

        rapidjson::Document document;
        document.SetObject();
        addPairToDoc(document, "record_audio_path", m_recordAudioPath);
        addPairToDoc(document, "ssid_prefix", m_ssidPrefix);
        addPairToDoc(document, "info_playback_device", m_infoPlaybackDevice);
        addPairToDoc(document, "tts_playback_device", m_ttsPlaybackDevice);
        addPairToDoc(document, "music_playback_device", m_musicPlaybackDevice);
        addPairToDoc(document, "alert_playback_device", m_alertPlaybackDevice);
        addPairToDoc(document, "rapid_playback_device", m_rapidPlaybackDevice);
        addPairToDoc(document, "natts_playback_device", m_nattsPlaybackDevice);

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        const char* str = buffer.GetString();
        FILE *fp = fopen(m_appConfigFile.c_str() , "w");
        if (fp) {
            fwrite(str, sizeof(char), strlen(str), fp);
            fclose(fp);
            sync();
        } else {
            printf("Configuration isRecreateAppConfigFile can't open appConfigFile\n");
        }
    }

    //handle dueros config file.
    std::string duerosConfigDir = deviceCommonLib::deviceTools::parseDir(m_duerosConfigFile);
    if (!deviceCommonLib::deviceTools::isDirExist(duerosConfigDir)) {
        if (!deviceCommonLib::deviceTools::mkdirR(duerosConfigDir.c_str())) {
            APP_ERROR("Please verify %s is a valid path!", duerosConfigDir.c_str());
            return false;
        }
    }

    bool isRecreateDuerosConfigFile = false;

    if (access(m_duerosConfigFile.c_str(), F_OK) == 0) {
        std::ifstream duerosConfigStream(m_duerosConfigFile);
        if (duerosConfigStream.good()) {
            rapidjson::IStreamWrapper duerosConfigStreamWrapper(duerosConfigStream);
            rapidjson::Document readDocument;
            readDocument.ParseStream(duerosConfigStreamWrapper);

            if (readDocument.HasParseError()) {
                isRecreateDuerosConfigFile = true;
            } else {
#if (defined RaspberryPi) || (defined Hodor) || (defined Kuke) || (defined Dot) || (defined Box86)
                m_deviceId = DeviceIoWrapper::getInstance()->getDeviceId();
#else
                if (readDocument.HasMember("device_id")) {
                    m_deviceId = readDocument["device_id"].GetString();
                    if (m_deviceId.empty()) {
                        m_deviceId = deviceCommonLib::deviceTools::randomUID();
                    }
                } else {
                    m_deviceId = deviceCommonLib::deviceTools::randomUID();
                }
#endif

                if (readDocument.HasMember("com_volume")) {
                    std::string volumeStr = readDocument["com_volume"].GetString();
                    if (volumeStr.empty()) {
                        m_commonVolume = DEFAULT_COMMON_VOLUME;
                    } else {
                        std::stringstream stream(volumeStr);
                        stream >> m_commonVolume;
                    }
                } else {
                    m_commonVolume = DEFAULT_COMMON_VOLUME;
                }

                if (readDocument.HasMember("alert_volume")) {
                    std::string volumeStr = readDocument["alert_volume"].GetString();
                    if (volumeStr.empty()) {
                        m_alertsVolume = DEFAULT_ALERTS_VOLUME;
                    } else {
                        std::stringstream stream(volumeStr);
                        stream >> m_alertsVolume;
                    }
                } else {
                    m_alertsVolume = DEFAULT_ALERTS_VOLUME;
                }

                rapidjson::Document document;
                document.SetObject();
                addPairToDoc(document, "device_id", m_deviceId);
                std::stringstream streamCmv;
                streamCmv << m_commonVolume;
                std::stringstream streamAlv;
                streamAlv << m_alertsVolume;
                addPairToDoc(document, "com_volume", streamCmv.str());
                addPairToDoc(document, "alert_volume", streamAlv.str());

                rapidjson::StringBuffer buffer;
                rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
                document.Accept(writer);
                const char* str = buffer.GetString();
                FILE *fp = fopen(m_duerosConfigFile.c_str(), "w");
                if (fp) {
                    fwrite(str, sizeof(char), strlen(str), fp);
                    fclose(fp);
                    sync();
                } else {
                    printf("Configuration can't open duerosConfigFile\n");
                }
            }
        } else {
            isRecreateDuerosConfigFile = true;
        }
    } else {
        isRecreateDuerosConfigFile = true;
    }

    if (isRecreateDuerosConfigFile) {
#if (defined RaspberryPi) || (defined Hodor) || (defined Kuke) || (defined Dot) || (defined Box86)
        m_deviceId = DeviceIoWrapper::getInstance()->getDeviceId();
#else
        m_deviceId = deviceCommonLib::deviceTools::randomUID();
#endif
        m_commonVolume = DEFAULT_COMMON_VOLUME;
        m_alertsVolume = DEFAULT_ALERTS_VOLUME;

        rapidjson::Document document;
        document.SetObject();
        addPairToDoc(document, "device_id", m_deviceId);
        std::stringstream streamCmv;
        streamCmv << m_commonVolume;
        std::stringstream streamAlv;
        streamAlv << m_alertsVolume;
        addPairToDoc(document, "com_volume", streamCmv.str());
        addPairToDoc(document, "alert_volume", streamAlv.str());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        const char* str = buffer.GetString();
        FILE *fp = fopen(m_duerosConfigFile.c_str() , "w");
        if (fp) {
            fwrite(str, sizeof(char), strlen(str), fp);
            fclose(fp);
            sync();
        } else {
            printf("Configuration isRecreateDuerosConfigFile can't open duerosConfigFile\n");
        }
    }

    return true;
}

void Configuration::addPairToDoc(rapidjson::Document& document, const std::string& key, const std::string& value) {
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value pKey(rapidjson::kStringType);
    pKey.SetString(key, allocator);
    rapidjson::Value pValue(rapidjson::kStringType);
    pValue.SetString(value, allocator);
    document.AddMember(pKey, pValue, allocator);
}

void Configuration::addPairToDoc(rapidjson::Document &document, const std::string &key, bool value) {
    rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
    rapidjson::Value pKey(rapidjson::kStringType);
    pKey.SetString(key, allocator);
    rapidjson::Value pValue;
    pValue.SetBool(value);
    document.AddMember(pKey, pValue, allocator);
}

std::string Configuration::getRecordAudioPath() const {
    return m_recordAudioPath;
}

std::string Configuration::getSsidPrefix() const {
    return m_ssidPrefix;
}

std::string Configuration::getDeviceId() const {
    return m_deviceId;
}

std::string Configuration::getInfoPlaybackDevice() const {
    return m_infoPlaybackDevice;
}

std::string Configuration::getTtsPlaybackDevice() const {
    return m_ttsPlaybackDevice;
}

std::string Configuration::getMusicPlaybackDevice() const {
    return m_musicPlaybackDevice;
}

std::string Configuration::getAlertPlaybackDevice() const {
    return m_alertPlaybackDevice;
}

std::string Configuration::getRapidPlaybackDevice() const {
    return m_rapidPlaybackDevice;
}

std::string Configuration::getNattsPlaybackDevice() const {
    return m_nattsPlaybackDevice;
}

bool Configuration::getDebug() const {
    return m_debug;
}

std::string Configuration::getAudioLibName() const {
    return m_audioLibName;
}

unsigned int Configuration::getCommVol() const {
    return m_commonVolume;
}

void Configuration::setCommVol(unsigned int volume) {
    m_commonVolume = volume;
    if (access(m_duerosConfigFile.c_str(), F_OK) == 0) {
        rapidjson::Document document;
        document.SetObject();
        addPairToDoc(document, "device_id", m_deviceId);
        std::stringstream streamCmv;
        streamCmv << m_commonVolume;
        addPairToDoc(document, "com_volume", streamCmv.str());
        std::stringstream streamAlv;
        streamAlv << m_alertsVolume;
        addPairToDoc(document, "alert_volume", streamAlv.str());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        const char* str = buffer.GetString();
        FILE *fp = fopen(m_duerosConfigFile.c_str() , "w");
        if (!fp) {
            return;
        }
        fwrite(str, sizeof(char), strlen(str), fp);
        fclose(fp);
        sync();
    }
}

unsigned int Configuration::getAlertsVol() const {
    return m_alertsVolume;
}

void Configuration::setAlertsVol(unsigned int volume) {
    m_alertsVolume = volume;
    if (access(m_duerosConfigFile.c_str(), F_OK) == 0) {
        rapidjson::Document document;
        document.SetObject();
        addPairToDoc(document, "device_id", m_deviceId);
        std::stringstream streamCmv;
        streamCmv << m_commonVolume;
        addPairToDoc(document, "com_volume", streamCmv.str());
        std::stringstream streamAlv;
        streamAlv << m_alertsVolume;
        addPairToDoc(document, "alert_volume", streamAlv.str());

        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        document.Accept(writer);
        const char* str = buffer.GetString();
        FILE *fp = fopen(m_duerosConfigFile.c_str() , "w");
        if (!fp) {
            return;
        }
        fwrite(str, sizeof(char), strlen(str), fp);
        fclose(fp);
        sync();
    }
}

}  // namespace application
}  // namespace duerOSDcsApp
