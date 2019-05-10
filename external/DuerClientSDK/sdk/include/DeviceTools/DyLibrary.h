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
 * @file DyLibrary.h
 * \if english
 *  @brief the library invoke interface
 * \else
 *  @brief 动态库使用接口
 * \endif
 *
 */

#ifndef DEVICE_COMMON_LIB_DYLIBRARY_INCLUDE_DYLIBRARY_H_
#define DEVICE_COMMON_LIB_DYLIBRARY_INCLUDE_DYLIBRARY_H_
#include <dlfcn.h>

namespace deviceCommonLib {
namespace deviceTools {

class DyLibrary {
public:
    /**
     * \if english
     *  @brief load library interface
     *  @param libraryName the library name which will be loaded.
     *  @param mode the mode of opening the library.
     *  @return True/False load library succeed or failed.
     * \else
     *  @brief 加载库接口
     *  @param libraryName 要加载的库名
     *  @param mode 打开库的模式
     *  @return True/False 加载库成功/失败
     * \endif
     */
    bool load(const std::string &libraryName, int mode = RTLD_NOW);

    /**
     * \if english
     *  @brief get the library symbol
     *  @param symbol the function symbol which will be gotten.
     *  @return (void *) function pointer
     * \else
     *  @brief 获取库里的符号
     *  @param symbol 要获取的函数符号
     *  @return (void *) 函数指针
     * \endif
     */
    void * getSym(const std::string symbol);

    /**
     * \if english
     *  @brief unload the library
     * \else
     *  @brief 卸载库
     * \endif
     */
    void unload();

    virtual ~DyLibrary();

private:
    void * m_handle;
};

}
}
#endif //DEVICE_COMMON_LIB_DYLIBRARY_INCLUDE_DYLIBRARY_H_
