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
 * @file Configuration.h
 * \if english
 *  @brief the configuration node can be obtained by this interface
 * \else
 *  @brief 应用层可以通过这个接口获取配置节点信息
 * \endif
 *
 */

#ifndef DEVICE_COMMON_LIB_CONFIGURATION_INCLUDE_CONFIGURATION_H_
#define DEVICE_COMMON_LIB_CONFIGURATION_INCLUDE_CONFIGURATION_H_

#include <map>
#include <rapidjson/document.h>

#include "Configuration/ConfigurationNode.h"

namespace deviceCommonLib {
namespace configuration {

/**
 * @file RuntimeConfiguration.h
 * \if english
 *  @brief A class that provides functionality to obtain RuntimeConfigurationNode
 * \else
 *  @brief 应用层可以通过这个接口获取配置节点信息
 * \endif
 *
 */
class Configuration {
public:
    /**
     * \if english
     *  @brief load configuration
     *  @param key the index string.
     *  @param jsonFileName the file name which is loaded.
     *  @param secretKey key used to encrypt.
     *  @param keyLen length of key.
     *  @return True/False load configuration succeed or failed.
     * \else
     *  @brief 加载配置方法
     *  @param key 使用的访问索引字符串
     *  @param jsonFileName 要加载的json文件名
     *  @param secretKey 用于加密的key
     *  @param keyLen key长度
     *  @return True/False 加载配置成功/失败
     * \endif
     */
    static bool loadConfiguration(const std::string &key, const std::string &jsonFileName,
                                  unsigned char* secretKey = nullptr, size_t keyLen = 0);

    /**
     * \if english
     *  @brief unload configuration
     *  @param key the index string
     *  @return True/False Whether unload configuration was successful.
     * \else
     *  @brief 卸载配置方法
     *  @param key 使用的索引字符串
     *  @retun True/False 卸载配置成功或失败
     * \endif
     */
    static bool unloadConfiguration(const std::string &key);

    /**
     * \if english
     *  @brief get @c RuntimeConfigurationNode value for @c key from this @c RuntimeConfgiuration
     *  @param key  The key of the @c ConfigurationNode value to get.
     *  @return The @c RuntimeConfigurationNode value, or an empty node if this @c RuntimeConfigurationNode does not have
     * \else
     *  @brief 根据key值从RuntimeConfgiuration找到RuntimeConfigurationNode
     *  @param key 键值字符串
     *  @return RuntimeConfigurationNode（如果找不到返回空RuntimeConfigurationNode）
     * \endif
     */
    ConfigurationNode operator[](const std::string& key) const;

    /**
     * \if english
     *  @brief get the instance of @c RuntimeConfiguration
     *  @return the instance of RuntimeConfiguration
     * \else
     *  @brief 获取单例RuntimeConfiguration
     *  @return 单例RuntimeConfiguration
     * \endif
     */
    static Configuration getInstance();

private:
    /**
     * \if english
     *  @brief static RuntimeConfiguration
     * \else
     *  @brief 静态RuntimeConfiguration类
     * \endif
     */
    static Configuration m_Configuration;

    /**
     * \if english
     *  @brief the intenal runtime config map
     * \else
     *  @brief 内部配置map表
     * \endif
     */
    static std::map<std::string,ConfigurationNode> m_CfgMap;

protected:
    /**
     * \if english
     *  @brief Once this method has been called, the item will be added in the cfgMap
     *  @param key the index key
     *  @param jsonFileName the file name which contains json strings
     *  @param secretKey key used to encrypt.
     *  @param keyLen length of key.
     *  @return True/False Whether addItemToMap was successful.
     * \else
     *  @brief 添加item到configMap中
     *  @param key key字符串
     *  @param jsonFileName 包含json字符串的文件名
     *  @param secretKey 用于加密的key
     *  @param keyLen key长度
     *  @return True/False 添加成功或失败
     * \endif
     */
    static bool addItemToMap(const std::string &key, const std::string & jsonFileName,
                             unsigned char* secretKey = nullptr, size_t keyLen = 0);

    /**
     * \if english
     *  @brief the intenal document map
     * \else
     *  @brief 内部document map表
     * \endif
     */
    static std::map<std::string,rapidjson::Document*> m_documentMap;
};

}  // namespace configuration
}  // namespace deviceCommonLib

#endif // DEVICE_COMMON_LIB_CONFIGURATION_INCLUDE_CONFIGURATION_H_
