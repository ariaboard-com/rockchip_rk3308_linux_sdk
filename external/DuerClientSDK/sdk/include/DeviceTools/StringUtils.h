/*
 * StringUtils.h
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

#ifndef DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_STRING_UTILS_H_
#define DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_STRING_UTILS_H_

#include <string>
#include <vector>

namespace deviceCommonLib {
namespace deviceTools {

/**
 * A utility function to convert a std::string to an integer.
 * If the string is successfully parsed, then the out parameter will be updated.
 *
 * @param str The string input.
 * @param[out] result The resulting integer, if successfully parsed from the string.
 * @return @c true If the string was parsed as an integer, otherwise @c false.
 */
bool stringToInt(const std::string& str, int* result);

/**
 * A utility function to convert a c-string to an integer.
 * If the string is successfully parsed, then the out parameter will be updated.
 *
 * @param str The C-string input.
 * @param[out] result The resulting integer, if successfully parsed from the string.
 * @return @c true If the string was parsed as an integer, otherwise @c false.
 */
bool stringToInt(const char* str, int* result);

/**
 * A utility function to convert int type to string
 * @param i the int value.
 * @return the string which has been converted.
 */
std::string convertToString(int i);

bool stringToDouble(const std::string& str, double* result);

std::string convertDoubleToString(double d);

/// @split string str with delimiter
/// @return fields
void splitString(const std::string &str, const std::string &delimiter,
                 std::vector<std::string> &fields);

void split_string_only_first_delimiter(const std::string &str, const std::string &delimiter,
                                       std::vector<std::string> &fields);

std::string randomUID();

}  // namespace deviceTools
}  // namespace deviceCommonLib

#endif  // DEVICE_COMMON_LIB_DEVICE_TOOLS_INCLUDE_STRING_UTILS_H_
