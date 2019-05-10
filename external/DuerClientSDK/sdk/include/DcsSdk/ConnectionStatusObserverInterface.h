/*
 * ConnectionStatusObserverInterface.h
 *
 * Copyright 2016-2017 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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

#ifndef DUEROS_DCS_SDK_INCLUDE_DCS_SDK_CONNECTION_STATUS_OBSERVER_INTERFACE_H_
#define DUEROS_DCS_SDK_INCLUDE_DCS_SDK_CONNECTION_STATUS_OBSERVER_INTERFACE_H_

#include <iostream>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief This class allows a client to be notified of changes to connection status to DCS.
 * \else
 *     @brief stream连接状态通知类
 * \endif
 */
class ConnectionStatusObserverInterface {
public:
    /**
     * \if english
     *     @brief This enum expresses the states that a logical DCS connection can be in.
     * \else
     *     @brief 连接状态的枚举值
     * \endif
     */
    enum class Status {
        /**
         * \if english
         *     @brief ACL is not connected to DCS
         * \else
         *     @brief 连接断开
         * \endif
         */
        DISCONNECTED,

        /**
         * \if english
         *     @brief ACL is attempting to establish a connection to DCS
         * \else
         *     @brief 正在建立连接
         * \endif
         */
        PENDING,

        /**
         * \if english
         *     @brief ACL is connected to DCS
         * \else
         *     @brief 连接成功
         * \endif
         */
        CONNECTED,

        /// ACL is connected and has done any necessary post-connection actions.
        ///POST_CONNECTED
    };

    /**
     * \if english
     *     @brief This enum expresses the reasons a connection status may change.
     * \else
     *     @brief 连接状态变化原因
     * \endif
     */
    enum class ChangedReason {

        /**
         * \if english
         *     @brief The connection status changed due to the client interacting with the Connection public api.
         * \else
         *     @brief 开始新的连接请求
         * \endif
         */
        ACL_CLIENT_REQUEST,

        /**
         * \if english
         *     @brief The connection attempt failed due to the Connection object being disabled.
         * \else
         *     @brief 连接请求无效
         * \endif
         */
        ACL_DISABLED,

        /**
         * \if english
         *     @brief The connection attempt failed due to DNS resolution timeout.
         * \else
         *     @brief DNS超时
         * \endif
         */
        DNS_TIMEDOUT,

        /**
         * \if english
         *     @brief The connection attempt failed due to timeout.
         * \else
         *     @brief 连接超时
         * \endif
         */
        CONNECTION_TIMEDOUT,

        /**
         * \if english
         *     @brief The connection attempt failed due to excessive load on the server.
         * \else
         *     @brief 连接受限
         * \endif
         */
        CONNECTION_THROTTLED,

        /**
         * \if english
         *     @brief The access credentials provided to ACL were invalid.
         * \else
         *     @brief access_token无效或过期
         * \endif
         */
        INVALID_AUTH,

        /**
         * \if english
         *     @brief There was a timeout sending a ping request.
         * \else
         *     @brief ping超时
         * \endif
         */
        PING_TIMEDOUT,

        /**
         * \if english
         *     @brief There was a timeout writing to AVS.
         * \else
         *     @brief 网路IO写数据超时
         * \endif
         */
        WRITE_TIMEDOUT,
        /**
         * \if english
         *     @brief There was a timeout reading from AVS.
         * \else
         *     @brief 网路IO读数据超时
         * \endif
         */
        READ_TIMEDOUT,

        /**
         * \if english
         *     @brief There was an underlying protocol error.
         * \else
         *     @brief protocol协议错误
         * \endif
         */
        FAILURE_PROTOCOL_ERROR,

        /**
         * \if english
         *     @brief There was an internal error within ACL.
         * \else
         *     @brief SDK内部错误
         * \endif
         */
        INTERNAL_ERROR,

        /**
         * \if english
         *     @brief There was an internal error on the server.
         * \else
         *     @brief Server内部错误
         * \endif
         */
        SERVER_INTERNAL_ERROR,

        /**
         * \if english
         *     @brief The server asked the client to reconnect.
         * \else
         *     @brief Server主动断开连接
         * \endif
         */
        SERVER_SIDE_DISCONNECT,

        /**
        * \if english
        *     @brief The server endpoint has changed.
        * \else
        *     @brief Server请求地址发生变化
        * \endif
        */
        SERVER_ENDPOINT_CHANGED
    };

    /**
     * \if english
     *     @brief Destructor.
     * \else
     *     @brief 析构函数
     * \endif
     */
    virtual ~ConnectionStatusObserverInterface() = default;

    /**
     * \if english
     *     @brief Called when the AVS connection state changes.
     *     @param status The current connection status.
     *     @param reason The reason the status change occurred.
     * \else
     *     @brief 通知连接状态变化
     *     @param status 当前连接状态
     *     @param reason 连接状态发生变化的原因
     * \endif
     */
    virtual void onConnectionStatusChanged(const Status status, const ChangedReason reason) = 0;

    /**
     * \if english
     *     @brief Called when cancel the asr request.
     * \else
     *     @brief 通知语音识别请求已经取消
     * \endif
     */
    virtual void onSpeechAsrCanceled() {}
};

/**
 * \if english
 *     @brief Write a @c ConnectionStatusObserverInterface::Status value to an @c ostream as a string.
 *     @param stream The stream to write the value to.
 *     @param status The ConnectionStatusObserverInterface::Status value to write to the @c ostream as a string.
 *     @return The @c ostream that was passed in and written to.
 * \else
 *     @brief 将连接状态枚举值转化为文本输出到ostream
 *     @param stream ostream输出文件流
 *     @param status 连接状态枚举值
 *     @return ostream输出文件流
 * \endif
 */
inline std::ostream& operator<<(std::ostream& stream, ConnectionStatusObserverInterface::Status status) {
    switch (status) {
        case ConnectionStatusObserverInterface::Status::DISCONNECTED:
            stream << "DISCONNECTED";
            break;
        case ConnectionStatusObserverInterface::Status::PENDING:
            stream << "PENDING";
            break;
        case ConnectionStatusObserverInterface::Status::CONNECTED:
            stream << "CONNECTED";
            break;
        /*case ConnectionStatusObserverInterface::Status::POST_CONNECTED:
            stream << "POST_CONNECTED";
            break;
            */
    }
    return stream;
}

/**
 * \if english
 *     @brief Write a @c ConnectionStatusObserverInterface::ChangeReason value to an @c ostream as a string.
 *     @param stream The stream to write the value to.
 *     @param reason The ConnectionStatusObserverInterface::ChangeReason value to write to the @c ostream as a string.
 *     @return The @c ostream that was passed in and written to.
 * \else
 *     @brief 将连接状态变化原因枚举值转化为文本输出到ostream
 *     @param stream ostream输出文件流
 *     @param reason 连接状态变化原因枚举值
 *     @return ostream输出文件流
 * \endif
 */
inline std::ostream& operator<<(std::ostream& stream, ConnectionStatusObserverInterface::ChangedReason reason) {
    switch (reason) {
        case ConnectionStatusObserverInterface::ChangedReason::ACL_CLIENT_REQUEST:
            stream << "ACL_CLIENT_REQUEST";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::ACL_DISABLED:
            stream << "ACL_DISABLED";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::DNS_TIMEDOUT:
            stream << "DNS_TIMEDOUT";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::CONNECTION_TIMEDOUT:
            stream << "CONNECTION_TIMEDOUT";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::CONNECTION_THROTTLED:
            stream << "CONNECTION_THROTTLED";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::INVALID_AUTH:
            stream << "INVALID_AUTH";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::PING_TIMEDOUT:
            stream << "PING_TIMEDOUT";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::WRITE_TIMEDOUT:
            stream << "WRITE_TIMEDOUT";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::READ_TIMEDOUT:
            stream << "READ_TIMEDOUT";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::FAILURE_PROTOCOL_ERROR:
            stream << "FAILURE_PROTOCOL_ERROR";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::INTERNAL_ERROR:
            stream << "INTERNAL_ERROR";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::SERVER_INTERNAL_ERROR:
            stream << "SERVER_INTERNAL_ERROR";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::SERVER_SIDE_DISCONNECT:
            stream << "SERVER_SIDE_DISCONNECT";
            break;
        case ConnectionStatusObserverInterface::ChangedReason::SERVER_ENDPOINT_CHANGED:
            stream << "SERVER_ENDPOINT_CHANGED";
            break;
    }
    return stream;
}

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_INCLUDE_DCS_SDK_CONNECTION_STATUS_OBSERVER_INTERFACE_H_
