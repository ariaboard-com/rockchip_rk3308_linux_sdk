/*
 * KeyWordObserverInterface.h
 *
 * Copyright 2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

/**
 * @file KeyWordObserverInterface.h
 * \if english
 *     @brief KeyWord wakeup observer interface.
 * \else
 *     @brief 接口类,用于客户端监听唤醒的检测。
 * \endif
 */

#ifndef DUEROS_DCS_SDK_INCLUDE_DCS_SDK_KEY_WORD_OBSERVER_INTERFACE_H_
#define DUEROS_DCS_SDK_INCLUDE_DCS_SDK_KEY_WORD_OBSERVER_INTERFACE_H_

#include <limits>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief KeyWordObserverInterface is an interface class that clients can extend to listen for keyword detections.
 * \else
 *     @brief 接口类,用于客户端监听唤醒的检测。
 * \endif
 */
class KeyWordObserverInterface {
public:
    /**
    * \if english
    *     @brief This represents when an index passed into the onKeyWordDetected() call should be ignored.
    * \else
    *     @brief 这是默认的index,如果在onKeyWordDetected()参数里有设置,可以忽略它
    * \endif
    */
    static constexpr uint64_t UNSPECIFIED_INDEX =
        std::numeric_limits<uint64_t>::max();

    /**
     * \if english
     *     @brief Destructor function.
     * \else
     *     @brief 析构函数.
     * \endif
     */
    virtual ~KeyWordObserverInterface() = default;

    /**
     * \if english
     *     @brief Used to notify the observer of keyword detections. Once called, the client should return as soon as possible.
     *     Failure to do so might block the wake word engine from processing audio data, depending on which wake word
     *     engine is used. Any additional work that needs to be done should be done on a separate thread or after
     *     returning.
     *
     *     @param[in] keyword The keyword detected.
     *     @param[in] beginIndex The optional absolute begin index of the first part of the keyword found within the @c stream.
     *     If this is set to UNSPECIFIED_INDEX, then it should be ignored.
     *     @param[in] endIndex The optional absolute end index of the last part of the keyword within the stream of the @c
     *     stream. If this is set to UNSPECIFIED_INDEX, then it should be ignored.
     * \else
     *     @brief 用于通知唤醒词检测的observer, 一旦调用,客户端应该尽快返回. 如果不这样做,可能会阻塞唤醒引擎继续处理音频数据,
     *     任何额外的工作应该在分开的线程或唤醒后进行.
     *     @param[in] keyword 检测到的唤醒词.
     *     @param[in] beginIndex 检测到的唤醒词的开始位置,如果设置为UNSPECIFIED_INDEX,可以被忽略.
     *     @param[in] endIndex 检测到的唤醒词的结束位置,如果设置为UNSPECIFIED_INDEX,可以被忽略.
     * \endif
     */
    virtual void onKeyWordDetected(
        std::string keyword,
        uint64_t beginIndex,
        uint64_t endIndex) {} ;

    virtual void onLocationDetected(
            int location) {} ;

    virtual void printInfo() {};
};

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_INCLUDE_DCS_SDK_KEY_WORD_OBSERVER_INTERFACE_H_
