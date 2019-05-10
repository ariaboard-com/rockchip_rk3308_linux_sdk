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

#ifndef DEVICE_COMMON_LIB_RUMTIMECONFIGURATION_INCLUDE_RUMTIME_CONFIGURATIONNODE_H_
#define DEVICE_COMMON_LIB_RUMTIMECONFIGURATION_INCLUDE_RUMTIME_CONFIGURATIONNODE_H_

#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "Configuration/ConfigurationNode.h"

namespace deviceCommonLib {
namespace configuration {

class RuntimeConfigurationNode: public ConfigurationNode {
public:
    /**
     * \if english
     *  @brief get array size
     *  @return the size of json array
     * \else
     *  @brief 获取array的大小
     *  @return 数目
     * \endif
     */
    int get_array_size();

    /**
     * \if english
     *  @brief construct the configuration node
     *  @param tmutex1 mutex pointer which used by RuntimeConfigurationNode
     *  @param fileName1 file name that contains json string
     * \else
     *  @brief 构造configuration node
     *  @param tmutex1 RuntimeConfigurationNode使用的mutex指针
     *  @param fileName1 包含json字符串的文件名
     * \endif
     */
    RuntimeConfigurationNode(std::mutex *tmutex1, std::string fileName1);

    /**
     * \if english
     *  @brief construct the configuration node
     *  @param object1 rapidjson::Value pointer which passed into node
     *  @param tmutex1 mutex pointer which used by RuntimeConfigurationNode
     *  @param fileName1 file name that contains json string
     *  @param doc1 rapidjson::Document pointer
     *  @param[in] secretKey key used to encrypt
     *  @param[in] keyLen key length
     * \else
     *  @brief 构造configuration node
     *  @param object1 rapidjson::Value 指针
     *  @param tmutex1 RuntimeConfigurationNode使用的mutex指针
     *  @param fileName1 包含json字符串的文件名
     *  @param doc1 rapidjson::Document 指针
     *  @param[in] secretKey 加密用的key
     *  @param[in] keyLen key的长度
     * \endif
     */
    RuntimeConfigurationNode(rapidjson::Value *object1, std::mutex *tmutex1,
                             const std::string& fileName1, rapidjson::Document *doc1,
                             unsigned char* secretKey, int keyLen);

    /**
     *\if english
     *  @brief Set @c bool value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c bool value to set.
     *  @param value @c bool value
     *  @param saveFlag whether the value will be saved in the file through the saveFlag.@c true if not specified.
     *  @return
     * \else
     *  @brief 从RuntimeConfigurationNode根据key设置bool值
     *  @param key 索引字符串
     *  @param value 要设置的bool值
     *  @param saveFlag 是否保存到文件中;如果没有指定saveFlag，使用true。
     *  @return 是否设置bool成功
     * \endif
     */
    bool setBool(const std::string& key, bool value, bool saveFlag = true);

    /**
     *\if english
     *  @brief Set @c int value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c int value to set.
     *  @param value @c int value
     *  @param saveFlag whether the value will be saved in the file through the saveFlag.@c true if not specified.
     *  @return
     * \else
     *  @brief 从RuntimeConfigurationNode根据key设置int值
     *  @param key 索引字符串
     *  @param value 要设置的int值
     *  @param saveFlag 是否保存到文件中;如果没有指定saveFlag，使用true。
     *  @return 是否设置int成功
     * \endif
     */
    bool setInt(const std::string& key, int value, bool saveFlag = true);

    /**
     *\if english
     * @brief get @c RuntimeConfigurationNode value for @c key from this @c RuntimeConfigurationNode.
     * @param key The key of the @c ConfigurationNode value to get.
     * @return The @c RuntimeConfigurationNode value, or an empty node if this @c RuntimeConfigurationNode does not have
     * a @c RuntimeConfigurationNode value for @c key.
     *\else
     * @brief 获取RuntimeCongigurationNode
     * @param key 索引字符串
     * @return RuntimeCongigurationNode
     *\endif
     */
    RuntimeConfigurationNode operator[](const std::string& key) const;

    /**
     *\if english
     *  @brief Set @c string value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c string value to set.
     *  @param value @c string value
     *  @param saveFlag whether the value will be saved in the file through the saveFlag.@c true if not specified.
     *  @return
     * \else
     *  @brief 从RuntimeConfigurationNode根据key设置string值
     *  @param key 索引字符串
     *  @param value 要设置的string值
     *  @param saveFlag 是否保存到文件中;如果没有指定saveFlag，使用true。
     *  @return 是否设置string成功
     * \endif
     */
    bool setString(const std::string& key, const std::string& value, bool saveFlag = true);

    /**
     *\if english
     *  @brief Set @c double value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c double value to set.
     *  @param value @c double value
     *  @param saveFlag whether the value will be saved in the file through the saveFlag.@c true if not specified.
     *  @return
     * \else
     *  @brief 从RuntimeConfigurationNode根据key设置double值
     *  @param key 索引字符串
     *  @param value 要设置的double值
     *  @param saveFlag 是否保存到文件中;如果没有指定saveFlag，使用true。
     *  @return 是否设置double成功
     * \endif
     */
    bool setDouble(const std::string& key, double value, bool saveFlag = true);

    /**
     *\if english
     * @brief  append object.
     * @param key object key string
     * @param saveFlag whether the object will be saved in the file through the saveFlag.@c true if not specified.
     * @return True/False append object succeed or failed.
     *\else
     * @brief 添加object
     * @param key object key字符串
     * @param saveFlag 根据saveFlag来决定是否保存object到文件中。如果没有指定，saveFlag为true
     * @return 添加成功或失败
     *\endif
     */
    bool appendObject(const std::string& key, bool saveFlag = true);

    /**
     *\if english
     * @brief  append json array string.
     * @param jsonArrayStr json array string
     * @param saveFlag whether the object will be saved in the file through the saveFlag.@c true if not specified.
     * @return True/False append succeed or failed.
     *\else
     * @brief 添加jsonArray字符串
     * @param jsonArrayStr json array字符串
     * @param saveFlag 根据saveFlag来决定是否保存到文件中。如果没有指定，saveFlag为true
     * @return 添加成功或失败
     *\endif
     */
    bool appendArray(const std::string& jsonArrayStr, bool saveFlag = true);

    /**
     *\if english
     * @brief  append json object to  document
     * @param arrayKey json array key
     * @param jsonKey json key
     * @param jsonStr json string
     * @param saveFlag whether the object will be saved in the file through the saveFlag.@c true if not specified.
     * @return True/False append object succeed or failed.
     *\else
     * @brief 添加jsonArray字符串
     * @param arrayKey json array key名字
     * @param jsonKey json字符串的主键
     * @param jsonStr json字符串
     * @param saveFlag 根据saveFlag来决定是否保存到文件中。如果没有指定，saveFlag为true
     * @return 添加成功或失败
     *\endif
     */
    bool appendObjectToDocument(const std::string& arrayKey, const std::string& jsonKey, const std::string &jsonStr,
                bool saveFlag);

    /**
     *\if english
     * @brief  obtain filed value from jsonArray by the key value.
     * @param key_name the key name
     * @param key_value the key value
     * @param filed_name the filed name
     * @param filed_value the filed value
     * @return True/False whether the value is obtained successfully.
     *\else
     * @brief  获取jsonArray中filed值.
     * @param key_name key名字
     * @param key_value key值
     * @param filed_name filed名称
     * @param filed_value filed 值
     * @return 获取成功或失败
     *\endif
     */
    bool getValueByIndex(std::string key_name, std::string key_value, std::string filed_name, std::string& filed_value);

    /**
     *\if english
     * @brief  save configuration.
     * @param rtNode RuntimeConfigurationNode
     * @return True/False save configuration succeed or failed.
     *\else
     * @brief 保存配置
     * @param rtNode 配置节点
     * @return 保存成功或失败
     *\endif
     */
    bool saveConfiguration();

    /**
     *\if english
     * @brief get @c RuntimeConfigurationNode value for @c arrayIndex from this @c RuntimeConfigurationNode.
     * @param arrayIndex The index of the @c ConfigurationNode value to get.
     * @return The @c RuntimeConfigurationNode value, or an empty node if this @c RuntimeConfigurationNode does not have
     * a @c RuntimeConfigurationNode value for @c key.
     *\else
     * @brief 通过数组小标获取RuntimeCongigurationNode
     * @param arrayIndex jsonArray数组中的小标
     * @return RuntimeCongigurationNode
     *\endif
     */
    RuntimeConfigurationNode operator[](const int & arrayIndex) const;


    /**
     *\if english
     * @brief  the mutex pointer which was used by RuntimeConfigurationNode
     *\else
     * @brief 配置节点使用的mutex指针
     *\endif
     */
    std::mutex *m_mutex;

    /**
     * \if english
     *  @brief internal document vetor
     * \else
     *  @brief 内部document向量
     * \endif
     */
    std::vector <rapidjson::Document*> _doc_vec;

private:
    /**
     * \if english
     *  @brief initial the configuration node
     *  @param rtNode RuntimeConfigurationNode
     *  @return True/False
     * \else
     *  @brief 初始化configuration node
     *  @param rtNode 配置节点
     *  @return 初始化成功或失败
     * \endif
     */
    friend bool initialize(RuntimeConfigurationNode& node);

    /**
     * \if english
     *  @brief uninitial the configuration node
     *  @param rtNode RuntimeConfigurationNode
     *  @return True/False
     * \else
     *  @brief 卸载初始化configuration node
     *  @param rtNode 配置节点
     *  @return 卸载初始化成功或失败
     * \endif
     */
    friend void uninitialize(RuntimeConfigurationNode& node);
};

}  // namespace configuration
}  // namespace deviceCommonLib

#endif // DEVICE_COMMON_LIB_RUMTIMECONFIGURATION_INCLUDE_RUMTIME_CONFIGURATIONNODE_H_
