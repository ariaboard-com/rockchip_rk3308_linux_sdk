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

/**
 * @file LocalSourcePlayerInterface.h
 * \if english
 *     @brief Local source player interface.
 * \else
 *     @brief Local source player interface。
 * \endif
 */

#ifndef DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_SOURCE_PLAYER_INTERFACE_H
#define DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_SOURCE_PLAYER_INTERFACE_H

#include <memory>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief Interface of local source player.
 * \else
 *     @brief Interface of local source player.
 * \endif
 */
class LocalSourcePlayerInterface {
public:
    /**
     * \if english
     *     @brief Play local source.
     *     @param[in] source source uri.
     *     @param[in] needFocus True/False.
     *     @param[in] start_callback callback function when start play.
     *     @param[in] start_cb_arg callback function arguments of start play.
     *     @param[in] finish_callback callback function when finish play.
     * \else
     *     @brief 播放local资源.
     *     @param[in] source source uri。
     *     @param[in] needFocus True/False。
     *     @param[in] start_callback 开始播放时的回调函数。
     *     @param[in] start_cb_arg 开始播放时回调函数的参数列表。
     *     @param[in] finish_callback 结束播放时的回调函数。
     * \endif
     */
    virtual void playLocalSource(const std::string& source,
                                 bool needFocus,
                                 void (*start_callback)(void *arg),
                                 void *start_cb_arg,
                                 void (*finish_callback)()) = 0;

    /**
     * \if english
     *     @brief Play tts.
     *     @param[in] content content.
     *     @param[in] needFocus True/False.
     *     @param[in] callback callback function when start play.
     * \else
     *     @brief 播放tts.
     *     @param[in] content content。
     *     @param[in] needFocus True/False。
     *     @param[in] callback 播放时的回调函数。
     * \endif
     */
    virtual void playTts(const std::string& content, bool needFocus, void (*callback)()) = 0;
};

} // namespace sdkInterfaces
} // namespace duerOSDcsSDK

#endif // DUEROS_DCS_SDK_DCS_SDK_INCLUDE_LOCAL_SOURCE_PLAYER_INTERFACE_H
