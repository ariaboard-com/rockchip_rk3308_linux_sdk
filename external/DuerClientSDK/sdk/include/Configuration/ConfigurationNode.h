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
 * @file ConfigurationNode.h
 * \if english
 *  @brief the json value can be read by this interface
 * \else
 *  @brief 应用层可以通过这个接口读取json值
 * \endif
 *
 */

#ifndef DEVICE_COMMON_LIB_CONFIGURATION_INCLUDE_CONFIGURATION_NODE_H_
#define DEVICE_COMMON_LIB_CONFIGURATION_INCLUDE_CONFIGURATION_NODE_H_

#include <chrono>
#include <memory>
#include <mutex>
#include <string>

namespace deviceCommonLib {
namespace configuration {

/**
 * 通过用户给定的key值，生成变换后加密、解密用的key。
 * @param key 输入的key值。
 * @param keyLen 输入key的长度。
 * @param out 转换后的key值（长度为AES_BLOCK_SIZE）
 * @return True/False 成功/失败
 */
bool genKey(const unsigned char* key, size_t keyLen, unsigned char* out);

/**
 * 使用CFB方式，基于传入的key值对字符串进行加密。
 * @param key 输入的key值。
 * @param keyLen key长度。
 * @param src 要加密的字符串。
 * @param srcLen 字符串长度。
 * @param out 加密后的字符串。
 * @param outLen 加密后字符串的长度。
 * @return True/False 成功/失败。
 *
 * @note 如果返回为True，调用者使用后需要调用free函数释放out指针。
 */
bool aesCfbEncrypt(const unsigned char* key, size_t keyLen, const unsigned char* src,
                          size_t srcLen, unsigned char*& out, int& outLen);

/**
 * 使用CFB方式，基于传入的key值对字符串进行解密。
 * @param key 输入的key值。
 * @param keyLen key长度。
 * @param src 要解密的字符串。
 * @param srcLen 字符串长度。
 * @param out 解密后的字符串。
 * @param outLen 解密后字符串的长度。
 * @return True/False 成功/失败。
 *
 * @note 如果返回为True，调用者使用后需要调用free函数释放out指针。
 */
bool aesCfbDecrypt(const unsigned char* key, size_t keyLen, const unsigned char* src,
                          size_t srcLen, unsigned char*& out, int& outLen);

/**
 * 依据输入的key值解密文件
 * @param filename 文件名
 * @param key key值
 * @param keyLen key的长度
 * @param out 解密后的字符串
 * @param outLen 解密后字符串的长度
 * @return True/False 成功/失败
 *
 * @note 如果返回为True，调用者使用后需要调用free函数释放out指针
 */
bool decryptFile(const std::string& filename, const unsigned char* key, size_t keyLen,
                        unsigned char*& out, int& outLen);

class ConfigurationNode{
public:
    /**
     * \if english
     *  @brief construct the configuration node
     *  @param fileName file name that contains json string
     * \else
     *  @brief 构造configuration node
     *  @param fileName 包含json字符串的文件名
     * \endif
     */
    ConfigurationNode(const std::string& fileName);

    /**
     * \if english
     *  @brief Get @c bool value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c bool value to get.
     *  @param[out] out Pointer to receive the returned value.
     *  @param defaultValue Default value to use if this @c RuntimeConfigurationNode does not have a @c bool value for @c key.
     * @c false if not specified.
     *  @return Whether this @c RuntimeConfigurationNode has a @c bool value for @c key.
     * \else
     *  @brief 从RuntimeConfigurationNode根据key得到bool值
     *  @param key 索引字符串
     *  @param[out] out 返回值的指针
     *  @param defaultValue 如果没有找到对应的bool值，使用默认值false
     *  @return 是否从RuntimeConfigurationNode中找到bool值
     * \endif
     */
    bool getBool(const std::string& key, bool* out = nullptr, bool defaultValue = false) const;

    /**
     * \if english
     *  @brief Get @c int value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c int value to get.
     *  @param[out] out Pointer to receive the returned value.
     *  @param defaultValue Default value to use if this @c RuntimeConfigurationNode does not have a @c int value for @c key.
     * Zero if not specified.
     *  @return Whether this @c RuntimeConfigurationNode has a @c int value for @c key.
     * \else
     *  @brief 从RuntimeConfigurationNode根据key得到int值
     *  @param key 索引字符串
     *  @param[out] out 返回值的指针
     *  @param defaultValue 如果没有找到对应的int值，使用默认值 0
     *  @return 是否从RuntimeConfigurationNode中找到int值
     * \endif
     */
    bool getInt(const std::string& key, int* out = nullptr, int defaultValue = 0) const;

    /**
     * \if english
     *  @brief Get @c string value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c string value to get.
     *  @param[out] out Pointer to receive the returned value.
     *  @param defaultValue Default value to use if this @c RuntimeConfigurationNode does not have a @c string value for @c key.
     *  The empty string if not specified.
     *  @return Whether this @c RuntimeConfigurationNode has a @c string value for @c key.
     * \else
     *  @brief 从RuntimeConfigurationNode根据key得到string
     *  @param key 索引字符串
     *  @param[out] out 返回值的指针
     *  @param defaultValue 如果没有找到对应的string值，使用空字符串默认值
     *  @return 是否从RuntimeConfigurationNode中找到string
     * \endif
     */
    bool getString(const std::string& key, std::string* out = nullptr, const std::string defaultValue = "") const;

    /**
     * \if english
     *  @brief Get @c double value for @c key from this @c RuntimeConfigurationNode.
     *  @param key The key of the @c double value to get.
     *  @param[out] out Pointer to receive the returned value.
     *  @param defaultValue Default value to use if this @c RuntimeConfigurationNode does not have a @c double value for @c key.
     *  The 0 if not specified.
     *  @return Whether this @c RuntimeConfigurationNode has a @c double value for @c key.
     * \else
     *  @brief 从RuntimeConfigurationNode根据key得到double值
     *  @param key 索引字符串
     *  @param[out] out 返回值的指针
     *  @param defaultValue 如果没有找到对应的double值，使用默认值0
     *  @return 是否从RuntimeConfigurationNode中找到double值
     * \endif
     */
    bool getDouble(const std::string& key, double* out = nullptr, double defaultValue = 0.0) const;

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
    ConfigurationNode operator[](const std::string& key) const;

    /**
     *\if english
     * @brief  Indicates of the @c RuntimeConfigurationNode references a valid object.
     * @return Whether the @c RuntimeConfigurationNode references a valid object.
     *\else
     * @brief RuntimeCongigurationNode是否有有效对象
     * @return 有有效对象或没有
     *\endif
     */
    operator bool() const;

    /**
     * \if english
     *  @brief construct the configuration node
     *  @param object rapidjson::Value pointer which passed into node
     *  @param fileName file name that contains json string
     *  @param doc rapidjson::Document pointer
     *  @param[in] secretKey key used to encrypt
     *  @param[in] keyLen key length
     * \else
     *  @brief 构造configuration node
     *  @param object rapidjson::Value 指针
     *  @param fileName 包含json字符串的文件名
     *  @param doc rapidjson::Document 指针
     *  @param[in] secretKey 加密用的key
     *  @param[in] keyLen key的长度
     * \endif
     */
    ConfigurationNode(rapidjson::Value* object, const std::string& fileName, rapidjson::Document* doc,
                      unsigned char* secretKey, int keyLen);

    /**
     *\if english
     * @brief Get a duration value derived from an integer value for @c key from this @c RuntimeConfigurationNode.
     * @tparam InputTypes td::chrono::duration type whose unit specifies how the integer value in the
     * JSON input stream is to be interpreted.
     * @tparam OutputType std::chrono::duration type specifying the type of the @c out parameter to this method.
     * @tparam DefaultType std::chrono::duration type specifying the type of the @c defaultValue to this method.
     * @param key The key of of the integer value to fetch and convert to @c OutputType.
     * @param out Pointer to receive the returned value.
     * @param defaultValue Default value to use if this @c RuntimeConfigurationNode does not have an integer value
     * value for @c key.  Zero if not specified.
     * @return Whether this @c RuntimeConfigurationNode has an integer value for @c key.
     *\else
     * @brief 从RuntimeConfigurationNode中根据key获取duration
     * @tparam InputTypes std::chrono::duration type 指定JSON输入流中的整数值如何被解析
     * @tparam OutputType std::chrono::duration type 指定输出参数
     * @tparam DefaultType 默认值类型
     * @param key 被使用并转成OutputType的索引值
     * @param out 返回值的指针
     * @param defaultValue 如果RuntimeConfigurationNode没有整数，使用默认值0
     * @return 是否能从RuntimeConfigurationNode找到对应的integer值
     *\endif
     */
    template <typename InputType, typename OutputType, typename DefaultType>
    bool getDuration(
            const std::string& key,
            OutputType* out = static_cast<std::chrono::seconds>(0),
            DefaultType defaultValue = std::chrono::seconds(0)) const;

    /**
     *\if english
     * @brief  convert document to string.
     * @param object rapidjson::Value
     * @param [out] out the output string which is converted by json::value
     * @return True/False convert succeed or failed.
     *\else
     * @brief 将document转化成字符串
     * @param object rapidjson::Value值
     * @param [out] out 转化后的字符串
     * @return 转化成功或失败
     *\endif
     */
    bool documentToString(rapidjson::Value *object, std::string &out);

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
    friend bool initialize(ConfigurationNode& node);

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
    friend void uninitialize(ConfigurationNode& node);

    /**
     *\if english
     * @brief Adapt between the public version of @c getString() (which fetches @c std::string) and the @c getValue()
     * template method (which fetches @c const @c char * from a @c rapidjson::Value).
     * @param key The key of the @c string value to get.
     * @param [out] out Pointer to receive the returned value.
     * @param defaultValue Default value to use if this @c RuntimeConfigurationNode does not have a @c string value for @c key.
     * @return Whether this @c RuntimeConfigurationNode has a @c string value for @c key.
     *\else
     * @brief 此方法是公共getString方法和getValue方法的适配层
     * @param key 索引值
     * @param [out] out 返回值的指针
     * @param defaultValue 如果RuntimeConfigurationNode没找到对应的字符串，使用空字符串。
     * @return 是否找到字符串
     *\endif
     */
    bool getString(const std::string& key, const char** out, const char* defaultValue) const;

    /**
     *\if english
     * @brief Common logic for getting a value of a specific type.
     * @tparam Type The type to be gotten.
     * @param key The key of the value to get.
     * @param out Pointer to receive the value. May be nullptr to just test for the presence of the value.
     * @param defaultValue A default output value if no value of the desired type for @c key is present.
     * @param isType rapidjson::Value member function to test for the desired type.
     * @param getType rapidjson::Value member function to get the desired type.
     * @return Whether a value of the specified @c Type is present for @c key.
     *\else
     * @brief 通过给定类型的key获取值的通用方法
     * @tparam Type 要得到值的类型
     * @param key 索引字符串
     * @param out 返回值的指针。如果测试值是否存在，所以可能是空指针
     * @param defaultValue 如果没有找到对应的值，使用默认值
     * @param isType rapidjson::Value成员函数
     * @param getType rapidjson::Value成员函数
     * @return 是否找到对应key的value值
     *\endif
     */
    template <typename Type>
    bool getValue(
            const std::string& key,
            Type* out,
            Type defaultValue,
            bool (rapidjson::Value::*isType)() const,
            Type (rapidjson::Value::*getType)() const) const;

protected:
    /**
    *\if english
    * @brief  the object value pointer within the RuntimeConfigurationNode
    *\else
    * @brief 配置节点中的object指针
    *\endif
    */
    rapidjson::Value* m_object;

    /**
     *\if english
     * @brief  file name which was used by RuntimeConfigurationNode
     *\else
     * @brief 配置节点使用的json文件名
     *\endif
     */
    std::string m_runtimeFilename;

    /**
     *\if english
     * @brief  the document pointer which was used by RuntimeConfigurationNode
     *\else
     * @brief 配置节点使用的document指针
     *\endif
     */
    rapidjson::Document * m_document;

    /**
     * \if english
     *     @brief key used to encrypt
     * \else
     *     @brief 加密使用的key值
     * \endif
     */
    unsigned char* m_secretKey;

    /**
     * \if english
     *     @brief length of key
     * \else
     *     @brief key长度
     * \endif
     */
    int m_keyLen;
};

template <typename InputType, typename OutputType, typename DefaultType>
bool ConfigurationNode::getDuration(const std::string& key, OutputType* out, DefaultType defaultValue) const {
    int temp;
    auto result = getInt(key, &temp);
    if (out) {
        *out = OutputType(result ? InputType(temp) : defaultValue);
    }
    return result;
}

}  // namespace configuration
}  // namespace deviceCommonLib

#endif // DEVICE_COMMON_LIB_CONFIGURATION_INCLUDE_CONFIGURATION_NODE_H_
