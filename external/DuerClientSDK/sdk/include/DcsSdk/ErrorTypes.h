/*
 * ErrorTypes.h
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
 * @file ErrorTypes.h
 * \if english
 *     @brief Media player error code defitions.
 * \else
 *     @brief 播放器的错误代码定义。。
 * \endif
 */

#ifndef DUEROS_DCS_SDK_DCS_SDK_INCLUDE_ERROR_TYPES_H
#define DUEROS_DCS_SDK_DCS_SDK_INCLUDE_ERROR_TYPES_H

#include <ostream>

namespace duerOSDcsSDK {
namespace sdkInterfaces {

/**
 * \if english
 *     @brief Identifies the specific type of error in a @c PlaybackFailed event.
 * \else
 *     @brief 错误类型。
 * \endif
 */
enum class ErrorType {
    /**
     * \if english
     *     @brief An unknown error occurred.
     * \else
     *     @brief 未知错误。
     * \endif
     */
    MEDIA_ERROR_UNKNOWN,

    /**
     * \if english
     *     @brief The server recognized the request as being malformed (bad request, unauthorized, forbidden, not found, etc).
     * \else
     *     @brief 错误的请求。
     * \endif
     */
    MEDIA_ERROR_INVALID_REQUEST,

    /**
     * \if english
     *     @brief The client was unable to reach the service.
     * \else
     *     @brief 服务不可用。
     * \endif
     */
    MEDIA_ERROR_SERVICE_UNAVAILABLE,

    /**
     * \if english
     *     @brief The server accepted the request, but was unable to process the request as expected.
     * \else
     *     @brief 服务器错误。
     * \endif
     */
    MEDIA_ERROR_INTERNAL_SERVER_ERROR,

    /**
     * \if english
     *     @brief There was an internal error on the client.
     * \else
     *     @brief 设备错误。
     * \endif
     */
    MEDIA_ERROR_INTERNAL_DEVICE_ERROR
};

/**
 * \if english
 *     @brief Convert an @c ErrorType to an AVS-compliant @c std::string.
 *     @param[in] errorType The @c ErrorType to convert.
 *     @return The AVS-compliant string representation of @c errorType.
 * \else
 *     @brief 错误类型对应的字符串。
 *     @param[in] errorType 错误类型。
 *     @return 错误类型对应的字符串。
 * \endif
 */
inline std::string errorTypeToString(ErrorType errorType) {
    switch (errorType) {
        case ErrorType::MEDIA_ERROR_UNKNOWN:
            return "MEDIA_ERROR_UNKNOWN";
        case ErrorType::MEDIA_ERROR_INVALID_REQUEST:
            return "MEDIA_ERROR_INVALID_REQUEST";
        case ErrorType::MEDIA_ERROR_SERVICE_UNAVAILABLE:
            return "MEDIA_ERROR_SERVICE_UNAVAILABLE";
        case ErrorType::MEDIA_ERROR_INTERNAL_SERVER_ERROR:
            return "MEDIA_ERROR_INTERNAL_SERVER_ERROR";
        case ErrorType::MEDIA_ERROR_INTERNAL_DEVICE_ERROR:
            return "MEDIA_ERROR_INTERNAL_DEVICE_ERROR";
    }
    return "unknown ErrorType";
}

/**
 * \if english
 *     @brief Write an @c ErrorType value to an @c ostream.
 *     @param[in,out] stream The stream to write the value to.
 *     @param[in] errorType The @c ErrorType to convert.
 *     @return The @c ostream that was passed in and written to.
 * \else
 *     @brief 写入错误类型到ostream中。
 *     @param[in,out] stream ostream。
 *     @param[in] errorType 错误类型。
 *     @return 写入的ostream。
 * \endif
 */
inline std::ostream& operator<<(std::ostream& stream, const ErrorType& errorType) {
    return stream << errorTypeToString(errorType);
}

}  // namespace sdkInterfaces
}  // namespace duerOSDcsSDK

#endif  // DUEROS_DCS_SDK_DCS_SDK_INCLUDE_ERROR_TYPES_H
