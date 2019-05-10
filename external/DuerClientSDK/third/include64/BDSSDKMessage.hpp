/***************************************************************************
 *
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file BDSSDKMessage.hpp
 * @author Vaino(lappivaeinoe@baidu.com)
 * @date 2016/10/26 17:51:32
 * @brief Message container to pass commands to SDK and receive events/callbacks from the SDK
 *
 **/

#ifndef _BDS_SDK_MESSAGE_HPP_
#define _BDS_SDK_MESSAGE_HPP_
#include <map>
#include <vector>
#include <string>

namespace bds {
/**
 * @brief	chunk数据
 * @n
 */
    extern const std::string DATA_CHUNK;

    class BDSSDKMessageImpl;
    class BDSSDKMessage{
    public:
        BDSSDKMessage(const std::string &p_name);
        BDSSDKMessage();
        BDSSDKMessage(const BDSSDKMessage &m);
        virtual BDSSDKMessage& operator=(const BDSSDKMessage &m);
/**
 * @brief	发送命令名，给name赋不同的命令值，来实现不同的命令
 * @n
 */
        std::string name;
/**
 * @brief	发送命令类型，在使用speech模式时用到，来区分是唤醒和识别
 * @n
 */
        std::string type;
        /*
        char* data;
        unsigned int length;
        */
        virtual ~BDSSDKMessage();

/**
 * @brief	设置参数
 * @param   key         键
 * @param   value       值（const std::string）
 * @n
 */
        void set_parameter(const std::string &key, const std::string &value);
/**
 * @brief	设置参数
 * @param   key         键
 * @param   value       值（int）
 * @n
 */
        void set_parameter(const std::string &key, int value);
/**
 * @brief	设置参数
 * @param   key         键
 * @param   value       值（float）
 * @n
 */
        void set_parameter(const std::string &key, float value);
/**
 * @brief	设置参数
 * @param   key         键
 * @param   value       值（const char*）
 * @param   valueLen    值长度（int）
 * @n
 */
        void set_parameter(const std::string &key, const char* value, int valueLen);
/**
 * @brief	设置参数
 * @param   key         键
 * @param   value       值（const std::vector<std::string>）
 * @n
 */
        void set_parameter(const std::string &key, const std::vector<std::string> &value);
/**
 * @brief	设置参数
 * @param   key         键
 * @param   value       值（void*）
 * @n
 */
        void set_parameter(const std::string &key, void *value);

/**
 * @brief	获取所有string类型参数key
 * @n
 */
        std::vector<std::string> string_param_keys();
/**
 * @brief	获取所有int类型参数key
 * @n
 */
        std::vector<std::string> int_param_keys();
/**
 * @brief	获取所有float类型参数key
 * @n
 */
        std::vector<std::string> float_param_keys();
/**
 * @brief	获取所有char类型参数key
 * @n
 */
        std::vector<std::string> char_param_keys();
/**
 * @brief	获取所有vector类型参数key
 * @n
 */
        std::vector<std::string> vector_param_keys();

/**
 * @brief	通过key获取value
 * @param   key            键
 * @param   outValue       值（string）
 * @n
 */
        bool get_parameter(const std::string &key, std::string &outValue);
/**
 * @brief	通过key获取value
 * @param   key            键
 * @param   outValue       值（int）
 * @n
 */
        bool get_parameter(const std::string &key, int &outValue);
/**
 * @brief	通过key获取value
 * @param   key            键
 * @param   outValue       值（float）
 * @n
 */
        bool get_parameter(const std::string &key, float &outValue);
/**
 * @brief	通过key获取value
 * @param   key            键
 * @param   outValue       值（const char*）
 * @param   outValueLen    值长度（int）
 * @n
 */
        bool get_parameter(const std::string &key, const char* &outValue, int &outValueLen);
/**
 * @brief	通过key获取value
 * @param   key            键
 * @param   outValue       值（std::vector<std::string>）
 * @n
 */
        bool get_parameter(const std::string &key, std::vector<std::string> &outValue);
/**
 * @brief	通过key获取value
 * @param   key            键
 * @param   outValue       值（void*）
 * @n
 */
        bool get_parameter(const std::string &key, void* &outValue);
    private:
        BDSSDKMessageImpl* _impl;
        friend class BDSSDKMessageImpl;
    };
}

#endif

