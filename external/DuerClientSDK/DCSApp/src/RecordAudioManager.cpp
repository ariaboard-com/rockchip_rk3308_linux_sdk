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

#include "DCSApp/RecordAudioManager.h"
#include "DCSApp/Configuration.h"

namespace duerOSDcsApp {
namespace application {

#define FILE_SPLIT_NUM 100000

using duerOSDcsApp::application::Configuration;

RecordAudioManager* RecordAudioManager::s_instance = nullptr;

pthread_once_t RecordAudioManager::s_initOnce = PTHREAD_ONCE_INIT;

pthread_once_t RecordAudioManager::s_destroyOnce = PTHREAD_ONCE_INIT;

RecordAudioManager* RecordAudioManager::getInstance() {
    pthread_once(&s_initOnce, &RecordAudioManager::init);
    return s_instance;
}

void RecordAudioManager::releaseInstance() {
    pthread_once(&s_destroyOnce, RecordAudioManager::release);
}

void RecordAudioManager::init() {
    if (s_instance == nullptr) {
        s_instance = new RecordAudioManager();
    }
}

void RecordAudioManager::release() {
    if (s_instance) {
        delete s_instance;
        s_instance = nullptr;
    }
}

RecordAudioManager::RecordAudioManager() : m_file(nullptr), m_writeCounter(0) {
    m_dirName = Configuration::getInstance()->getRecordAudioPath();
    if (!m_dirName.empty()) {
        if (nullptr == opendir(m_dirName.c_str())) {
            mkdir(m_dirName.c_str(), 0775);
        }
    }
}
RecordAudioManager::~RecordAudioManager() {
    m_dirName = "";
    if (m_file) {
        fflush(m_file);
        fclose(m_file);
        m_file = nullptr;
    }
}

int RecordAudioManager::getCurrentFileIndex() {
    DIR *dir = nullptr;
    struct dirent *ptr = nullptr;
    if ((dir = opendir(m_dirName.c_str())) == nullptr) {
        return -1;
    }
    int maxIndex = -1000;
    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        } else if (ptr->d_type == DT_REG) {
            std::string fileName(ptr->d_name);
            int index = getIndex(fileName, fileName.find('.'));
            if (index > 0 && index > maxIndex) {
                maxIndex = index;
            }
        }
    }
    closedir(dir);
    return maxIndex;
}

int RecordAudioManager::getIndex(const std::string& fileName, int endIndex) {
    size_t i = endIndex - 1;
    while (fileName[i] <= '9' && fileName[i] >= '0') {
        i--;
    }
    i++;
    if ((int)i < endIndex) {
        return atoi(fileName.substr(i, endIndex - i).c_str());
    } else {
        return -1;
    }
}

bool RecordAudioManager::chooseTargetFile() {
    int targetFileIndex = 0;
    int maxIndex = getCurrentFileIndex();
    if (maxIndex > 0) {
        targetFileIndex = maxIndex;
    }
    std::string targetFileName;
    bool isExist = false;
    do {
        std::stringstream ss;
        ss << targetFileIndex++;
        targetFileName = m_dirName + "/data" + ss.str() + ".pcm";
        isExist = access(targetFileName.c_str(), F_OK) == 0;
    } while (isExist);

    std::stringstream ss;
    ss << --targetFileIndex;
    targetFileName = m_dirName + "/data" + ss.str() + ".pcm";
    m_file = fopen(targetFileName.c_str(), "a+");
    if (m_file) {
        return true;
    } else {
        return false;
    }
}

void RecordAudioManager::transmitAudioRecordData(const void* buffer, unsigned long size) {
    if (m_dirName.empty()) {
        return;
    }
    if (m_file == nullptr) {
        if (!chooseTargetFile()) {
            return;
        }
    }

    if (m_file && buffer) {
        fwrite(buffer, 1, size, m_file);
        m_writeCounter++;
        if (m_writeCounter >= FILE_SPLIT_NUM) {
            fflush(m_file);
            fclose(m_file);
            m_file = nullptr;
            m_writeCounter = 0;
        }
    }
}

}  // namespace application
}  // namespace duerOSDcsApp