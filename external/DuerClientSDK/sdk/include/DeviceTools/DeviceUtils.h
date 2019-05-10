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
 * @file DeviceUtils.h
 * \if english
 *  @brief device utils interface
 * \else
 *  @brief device 常用工具接口
 * \endif
 *
 */

#ifndef DEVICE_COMMON_LIB_DEVICEUTILS_INCLUDE_DEVICEUTILS_H_
#define DEVICE_COMMON_LIB_DEVICEUTILS_INCLUDE_DEVICEUTILS_H_
namespace deviceCommonLib {
namespace deviceTools {

class DeviceUtils {
public:
    static void clear_voice_inactive_frames();
    static void set_pkg_index(int pkg_index);
    static int get_pkg_index();

private:
    static int m_pkg_index;
};

}
};
#endif //DEVICE_COMMON_LIB_DEVICEUTILS_INCLUDE_DEVICEUTILS_H_
