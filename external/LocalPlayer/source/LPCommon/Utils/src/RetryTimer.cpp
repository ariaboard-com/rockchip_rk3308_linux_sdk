/*
 * RetryTimer.cpp
 *
 * Copyright 2018 Rockchip.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://www.rock-chips.com/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#include <chrono>
#include <random>

#include "LPCommon/Utils/RetryTimer.h"

namespace localPlayerSDK {
namespace rkpCommon {
namespace utils {

RetryTimer::RetryTimer(int* retryTable, int retrySize) :
        m_RetryTable{retryTable},
        m_RetrySize{retrySize},
        m_RetryDecreasePercentage{(100 * 100) / (100 + 50)},
        m_RetryIncreasePercentage{100 + 50} {
}

RetryTimer::RetryTimer(int* retryTable, int retrySize, int randomizationPercentage) :
        m_RetryTable{retryTable},
        m_RetrySize{retrySize},
        m_RetryDecreasePercentage{(100 * 100) / (100 + randomizationPercentage)},
        m_RetryIncreasePercentage{100 + randomizationPercentage} {
}

RetryTimer::RetryTimer(int* retryTable, int retrySize, int decreasePercentage, int increasePercentage) :
        m_RetryTable{retryTable},
        m_RetrySize{retrySize},
        m_RetryDecreasePercentage{decreasePercentage},
        m_RetryIncreasePercentage{increasePercentage} {
}

std::chrono::milliseconds RetryTimer::calculateTimeToRetry(int retryCount) {
    if (retryCount < 0) {
        retryCount = 0;
    } else if (retryCount >= m_RetrySize) {
        retryCount = m_RetrySize - 1;
    }

    std::mt19937 generator(static_cast<unsigned>(std::time(nullptr)));
    std::uniform_int_distribution<int> distribution(
        (m_RetryTable[retryCount] * m_RetryDecreasePercentage) / 100,
        (m_RetryTable[retryCount] * m_RetryIncreasePercentage) / 100);
    auto delayMs = std::chrono::milliseconds(distribution(generator));
    return delayMs;
}

}  // namespace utils
}  // namespace rkpCommon
}  // namespace localPlayerSDK
