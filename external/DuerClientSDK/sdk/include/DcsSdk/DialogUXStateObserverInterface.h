/*
 * DialogUXStateObserverInterface.h
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
 * @file DialogUXStateObserverInterface.h
 * \if english
 *     @brief Pre defined interface, should implemented by APP.
 * \else
 *     @brief SDK定义一系列接口，需要由APP层实现，并在APP生存期间由SDK调用。
 * \endif
 */

#ifndef DUEROS_DCS_SDK_DCS_COMMON_SDK_INTERFACES_INCLUDE_DCS_COMMON_SDK_INTERFACES_DIALOG_UX_OBSERVER_INTERFACE_H_
#define DUEROS_DCS_SDK_DCS_COMMON_SDK_INTERFACES_INCLUDE_DCS_COMMON_SDK_INTERFACES_DIALOG_UX_OBSERVER_INTERFACE_H_

#include <string>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
* \if english
*     @brief A dueros state observer.
* \else
*     @brief DuerOS状态观察接口。
* \endif
*/
class DialogUXStateObserverInterface {
public:
    /**
    * \if english
    *     @brief An enum class to represent the different dialog specific DCS UX states.
    * \else
    *     @brief DCS UX状态枚举。
    * \endif
    */
    enum class DialogUXState {
        /**
        * \if english
        *     @brief DuerOS is idle and ready for an interaction.
        * \else
        *     @brief DuerOS处于空闲状态，可以进行交互。
        * \endif
        */
        IDLE,

        /**
        * \if english
        *     @brief Alexa is currently listening.
        * \else
        *     @brief DuerOS正在聆听。
        * \endif
        */
        LISTENING,

        /**
        * \if english
        *     @brief A customer request has been completed and no more input is accepted. In this
        *     state, duerOS is waiting for a response from DCS.
        * \else
        *     @brief 聆听结束，等待DCS返回内容。
        * \endif
        */
        THINKING,

        /**
        * \if english
        *     @brief DuerOS is responding to a request with speech.
        * \else
        *     @brief DuerOS正在播放TTS。
        * \endif
        */
        SPEAKING,

        /**
        * \if english
        *     @brief DuerOS has finished processing a SPEAK directive.
        * \else
        *     @brief DuerOS播放TTS结束。
        * \endif
        */
        FINISHED,

        /**
        * \if english
        *     @brief DuerOS is playing a music resources.
        * \else
        *     @brief DuerOS正在播放音乐。
        * \endif
        */
        MEDIA_PLAYING,

        /**
        * \if english
        *     @brief DuerOS has stopped playing a music resources.
        * \else
        *     @brief DuerOS停止音乐播放。
        * \endif
        */
        MEDIA_STOPPED,

        /**
        * \if english
        *     @brief DuerOS has finished playing a music resources.
        * \else
        *     @brief DuerOS完成音乐播放。
        * \endif
        */
        MEDIA_FINISHED,
    };

    /**
     * \if english
     *     @brief An enum class to represent the states an alert can be in.
     * \else
     *     @brief 闹钟状态枚举。
     * \endif
     */
    enum class AlertState {
        /**
         * \if english
         *     @brief The alert is ready to start, and is waiting for channel focus.
         * \else
         *     @brief 闹钟时间到，等待音频焦点。
         * \endif
         */
        READY,

        /**
          * \if english
          *     @brief The alert has started.
          * \else
          *     @brief 闹钟已经开始播放。
          * \endif
          */
        STARTED,

        /**
          * \if english
          *     @brief The alert has stopped due to user or system intervention.
          * \else
          *     @brief 系统或者用户打断闹钟，导致闹钟结束播放。
          * \endif
          */
        STOPPED,

        /**
          * \if english
          *     @brief The alert has snoozed.
          * \else
          *     @brief 小睡一会儿。
          * \endif
          */
        SNOOZED,

        /**
          * \if english
          *     @brief The alert has completed on its own.
          * \else
          *     @brief 闹钟响铃超时(15分钟)。
          * \endif
          */
        COMPLETED,

        /**
          * \if english
          *     @brief The alert has been determined to be past-due, and will not be rendered.
          * \else
          *     @brief 闹钟已经过期太久(30分钟)，不会再响铃。
          * \endif
          */
        PAST_DUE,

        /**
          * \if english
          *     @brief The alert has entered the foreground.
          * \else
          *     @brief 闹钟进入前台。
          * \endif
          */
        FOCUS_ENTERED_FOREGROUND,

        /**
          * \if english
          *     @brief The alert has entered the background.
          * \else
          *     @brief 闹钟进入后台。
          * \endif
          */
        FOCUS_ENTERED_BACKGROUND,

        /**
          * \if english
          *     @brief The alert has encountered an error.
          * \else
          *     @brief 闹钟发生错误，播放错误时发生。
          * \endif
          */
        ERROR
    };

    /**
     * \if english
     *     @brief Destructor.
     * \else
     *     @brief 析构函数。
     * \endif
     */
    virtual ~DialogUXStateObserverInterface() = default;

    /**
     * \if english
     *     @brief This function is called whenever the DCS UX dialog state of the system changes.
     *     This function will block processing of other state changes, so any implementation of
     *     this should return quickly.
     *     @param[in] newState The new dialog specific DCS UX state.
     * \else
     *     @brief 回调函数，用于通知DCS UX系统状态变化。由于此回调函数回阻塞其他状态，所以实现方需要尽快返回。
     *     @param[in] newState DCS UX状态，参考DialogUXState枚举。
     * \endif
     */
    virtual void onDialogUXStateChanged(DialogUXState newState) = 0;

    /**
     * \if english
     *     @brief send listenStarted event(upload speech binary stream for speech Recognize)
     *     complete
     *     @param[in] success: true, send event success, else failed
     * \else
     *     @brief 发送语音流(做语音识别)是否成功
     *     @param[in] success: true, 成功; false,失败
     * \endif
     */
    virtual void onMessageSendComplete(bool success) = 0;

    /**
     * \if english
     *     @brief notify http2 stream link not connected
     *     @param[in] void
     * \else
     *     @brief 通知http2 stream连接断开了
     *     @param[in] void
     * \endif
     */
    virtual void onHttp2NotConnected() {}

    /**
     * \if english
     *     @brief notify start to upload speech binary stream for speech Recognize
     *     @param[in] success, it is always true now.
     * \else
     *     @brief 通知开始上传语音流数据做语音识别
     *     @param[in] success,目前总是为true
     * \endif
     */
    virtual void onListenStarted(bool success) {}

    /**
     * \if english
     *     @brief notify has stopped to upload speech binary stream for speech Recognize
     *     @param[in] success, it is always true now.
     * \else
     *     @brief 通知已结束上传语音流数据
     *     @param[in] succcess,目前总是为true
     * \endif
     */
    virtual void onStopListen(bool success) {}

    /**
     * \if english
     *     @brief notify received directive from dcs server
     *     @param[in] contextId: context id from dcs server
     *     @param[in] message: directive (json text format) infomation from dcs server
     * \else
     *     @brief 通知已结束上传语音流数据
     *     @param[in] contextId: 上下文id
     *     @param[in] message: server端下发的指令信息
     * \endif
     */
    virtual void onReceivedDirective(const std::string& contextId, const std::string& message) = 0;

    /**
     * \if english
     *     @brief A callback function to notify an object that an alert has updated its state.
     *     @param[in] alertToken The DCS token of the alert.
     *     @param[in] state The state of the alert.
     *     @param[in] reason The reason for the state change.
     * \else
     *     @brief 回调函数，用于通知闹钟状态变化。
     *     @param[in] alertToken DCS协议里的闹钟token，标识不同的闹钟。
     *     @param[in] state 闹钟状态。
     *     @param[in] reason 触发状态变化的原因。
     * \endif
     */
    virtual void onAlertStateChange(const std::string & alertToken,
                                    AlertState state,
                                    const std::string &reason="") = 0;

    /**
     * \if english
     *     @brief notify received directive from dcs server
     *     @param[in] success: it is always true now.
     * \else
     *     @brief 通知已进入多轮识别状态
     *     @param[in] success: 目前总是为true
     * \endif
     */
    virtual void onExpectSpeech(bool success) = 0;

    /**
     * \if english
     *     @brief In this state, the @c SpeechSynthesizer is idle and not playing speech.
     * \else
     *     @brief TTS播放已结束。
     * \endif
     */
    virtual void onSpeechFinished() {};

    /**
     * \if english
     *     @brief Convert a @c State to a @c std::string.
     *     @param[in] state The @c DialogUXState to convert to a string.
     *     @return The string representation of @c state.
     * \else
     *     @brief 枚举状态转换成字符串。
     *     @param[in] state 需要转换的状态枚举值。
     *     @return 状态枚举值对应的字符串。
     * \endif
     */
    static std::string stateToString(DialogUXState state);

    /**
     * \if english
     *     @brief Convert a @c State to a @c std::string.
     *     @param[in] state The @c State to convert.
     *     @return The string representation of @c state.
     * \else
     *     @brief 枚举状态转换成字符串。
     *     @param[in] state 需要转换的状态枚举值。
     *     @return 状态枚举值对应的字符串。
     * \endif
     */
    static std::string stateToString(AlertState state);
};

inline std::string DialogUXStateObserverInterface::stateToString(DialogUXState state) {
    switch (state) {
        case DialogUXState::IDLE:
            return "IDLE";
        case DialogUXState::LISTENING:
            return "LISTENING";
        case DialogUXState::THINKING:
            return "THINKING";
        case DialogUXState::SPEAKING:
            return "SPEAKING";
        case DialogUXState::FINISHED:
            return "FINISHED";
        case DialogUXState::MEDIA_PLAYING:
            return "MEDIA PLAYING";
        case DialogUXState::MEDIA_STOPPED:
            return "MEDIA STOPPED";
        case DialogUXState::MEDIA_FINISHED:
            return "MEDIA FINISHED";
    }
    return "Unknown State";
}
/**
 * \if english
 *     @brief Write a @c DialogUXStateObserverInterface::DialogUXState value to an @c ostream as a string.
 *     @param[in] stream The stream to write the value to.
 *     @param[in] state The @c DialogUXStateObserverInterface::DialogUXState value to write to the @c ostream as a string.
 *     @return The @c ostream that was passed in and written to.
 * \else
 *     @brief 将DialogUX状态输出到ostream。
 *     @param[in] stream ostream输出文件流
 *     @param[in] state DialogUX状态。
 *     @return ostream输出文件流
 * \endif
 */
inline std::ostream& operator<<(std::ostream& stream, const DialogUXStateObserverInterface::DialogUXState& state) {
    return stream << DialogUXStateObserverInterface::stateToString(state);
}

inline std::string DialogUXStateObserverInterface::stateToString(AlertState state) {
    switch (state) {
    case AlertState::READY:
        return "READY";
    case AlertState::STARTED:
        return "STARTED";
    case AlertState::STOPPED:
        return "STOPPED";
    case AlertState::SNOOZED:
        return "SNOOZED";
    case AlertState::COMPLETED:
        return "COMPLETED";
    case AlertState::PAST_DUE:
        return "PAST_DUE";
    case AlertState::FOCUS_ENTERED_FOREGROUND:
        return "FOCUS_ENTERED_FOREGROUND";
    case AlertState::FOCUS_ENTERED_BACKGROUND:
        return "FOCUS_ENTERED_BACKGROUND";
    case AlertState::ERROR:
        return "ERROR";
    }
    return "unknown State";
}

/**
 * \if english
 *     @brief Write a @c State value to an @c ostream.
 *     @param[in] stream The stream to write the value to.
 *     @param[in] state The @c State value to write to the @c ostream as a string.
 *     @return The @c ostream that was passed in and written to.
 * \else
 *     @brief 将闹钟状态输出到ostream。
 *     @param[in] stream ostream输出文件流
 *     @param[in] state 闹钟状态。
 *     @return ostream输出文件流
 * \endif
 */
inline std::ostream& operator<<(std::ostream& stream, const DialogUXStateObserverInterface::AlertState& state) {
    return stream << DialogUXStateObserverInterface::stateToString(state);
}

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_DCS_COMMON_SDK_INTERFACES_INCLUDE_DCS_COMMON_SDK_INTERFACES_DIALOG_UX_OBSERVER_INTERFACE_H_
