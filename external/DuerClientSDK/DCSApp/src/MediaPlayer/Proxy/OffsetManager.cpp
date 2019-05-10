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

#include "OffsetManager.h"
#include "LoggerUtils/DcsSdkLogger.h"

namespace duerOSDcsApp {
namespace mediaPlayer {

OffsetManager::OffsetManager() {
    clear();
}

void OffsetManager::setIsSeekable(bool seekable) {
    m_isSeekable = seekable;
}

void OffsetManager::setSeekPoint(std::chrono::milliseconds seekPoint) {
    m_isSeekPointSet = true;
    m_seekPoint = seekPoint;
}

std::chrono::milliseconds OffsetManager::getSeekPoint() {
    return m_seekPoint;
}

bool OffsetManager::isSeekable() {
    return m_isSeekable;
}

bool OffsetManager::isSeekPointSet() {
    return m_isSeekPointSet;
}

void OffsetManager::clear() {
    m_seekPoint = std::chrono::milliseconds::zero();
    m_isSeekable = false;
    m_isSeekPointSet = false;
}

}  // namespace mediaPlayer
}  // namespace duerOSDcsApp
