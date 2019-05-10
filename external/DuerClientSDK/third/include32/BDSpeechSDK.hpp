/***************************************************************************
 *
 * Copyright (c) 2016 Baidu.com, Inc. All Rights Reserved
 *
 **************************************************************************/

/**
 * @file BDSpeechSDK.hpp
 * @author Vaino(lappivaeinoe@baidu.com)
 * @date 2016/10/26 17:52:07
 * @brief Main interface for speech SDK for Linux.
 *
 **/

#ifndef _BDS_SPEECH_SDK_HPP_
#define _BDS_SPEECH_SDK_HPP_

#include <string>
#include "BDSSDKMessage.hpp"

namespace bds {

    /** @defgroup COMMON 通用消息 */
    /* @{ */
    /** @defgroup post-common post */
    /* @{ */
    /**
     * @brief    <b>设置系统文件路径</b>
     * @n
     * 详细描述参考：
     * @brief    参数列表
     *    <table>
     *        <tr><td>参数值</td><td>说明</td><td>类型</td><td>默认值</td></tr>
     *        <tr><td>BDS_PARAM_KEY_WRITABLE_LIBRARY_DATA_PATH</td><td>系统路径</td><td>std::string</td><td></td></tr>
     *    </table>
     *    @n
     *    @brief   回调列表
     *    <table>
     *        <tr><td>回调事件</td><td>回调事件解释</td><td>回调参数</td><td>参数说明</td><td>回调参数值</td><td>回调参数值说明</td></tr>
     *        <tr><td rowspan="5">asr_callback_name</td><td rowspan="5">asr模块回调事件</td><td>CALLBACK_ASR_STATUS</td><td>识别回调状态</td><td>EVoiceRecognitionClientWorkStatusError</td><td>发生错误</td></tr>
     *        <tr><td>CALLBACK_ERROR_DESC</td><td>错误描述</td><td>std::string</td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_CODE</td><td>错误码</td><td></td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_DOMAIN</td><td>错误域</td><td></td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_SERIAL_NUM</td><td>sn号</td><td></td><td></td></tr>
     *        <tr><td rowspan="5">asr_callback_name</td><td rowspan="5">asr模块回调事件</td><td>CALLBACK_ASR_STATUS</td><td>识别回调状态</td><td>EVoiceRecognitionClientWorkStatusError</td><td>发生错误</td></tr>
     *        <tr><td>CALLBACK_ERROR_DESC</td><td>错误描述</td><td>std::string</td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_CODE</td><td>错误码</td><td></td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_DOMAIN</td><td>错误域</td><td></td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_SERIAL_NUM</td><td>sn号</td><td></td><td></td></tr>
     *    </table>
     */
    extern const std::string BDS_COMMAND_SET_WRITABLE_LIBRARY_DATA_PATH;
    /**
     * @brief    <b>设置用户指定文件路径</b>
     * @n
     * 详细描述参考：
     * @brief    参数列表
     *    <table>
     *        <tr><td>参数值</td><td>说明</td><td>类型</td><td>默认值</td></tr>
     *        <tr><td>BDS_PARAM_KEY_WRITABLE_USER_DATA_PATH</td><td>用户路径</td><td>std::string</td><td></td></tr>
     *    </table>
     *    @n
     *    @brief   回调列表
     *    <table>
     *        <tr><td>回调事件</td><td>回调事件解释</td><td>回调参数</td><td>参数说明</td><td>回调参数值</td><td>回调参数值说明</td></tr>
     *        <tr><td rowspan="5">asr_callback_name</td><td rowspan="5">asr模块回调事件</td><td>CALLBACK_ASR_STATUS</td><td>识别回调状态</td><td>EVoiceRecognitionClientWorkStatusError</td><td>发生错误</td></tr>
     *        <tr><td>CALLBACK_ERROR_DESC</td><td>错误描述</td><td>std::string</td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_CODE</td><td>错误码</td><td></td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_DOMAIN</td><td>错误域</td><td></td><td></td></tr>
     *        <tr><td>CALLBACK_ERROR_SERIAL_NUM</td><td>sn号</td><td></td><td></td></tr>
     *    </table>
     */
    extern const std::string BDS_COMMAND_SET_WRITABLE_USER_DATA_PATH;
    /**
     * @brief    <b>获取manager版本（asr模块不支持）</b>
     * @n
     * 详细描述参考：
     *    @brief   回调列表
     *    <table>
     *        <tr><td>回调事件</td><td>回调事件解释</td><td>回调参数</td><td>参数说明</td></tr>
     *        <tr><td>BDS_TTS_RESULT_UNLOAD_OFFLINE_ENGINE_DOMAIN</td><td>卸载定制离线引擎回调</td><td></td><td></td></tr>
     *    </table>
     */
    extern const std::string BDS_COMMAND_GET_EVENT_MANAGER_VERSION;
    /* @} */
    /* @} */

    class BDSpeechSDK{
    public:
/**
 * @brief    获取sdk实例
 * @param   SDKType     需要获取的实例类型，一次只能获取一种类型。可以传入的值有{SDK_TYPE_ASR,SDK_TYPE_WAKEUP,SDK_TYPE_TTS,SDK_TYPE_AUDIO_PROCESSOR,SDK_TYPE_MDL,SDK_TYPE_UPLOADER,SDK_TYPE_SPEECH,SDK_TYPE_DCS}
 * @param   outErrorDescription     错误描述
 * @return  返回sdk实例
 * @n
 */
        static BDSpeechSDK* get_instance(const std::string &SDKType, std::string &outErrorDescription);
/**
 * @brief    设置监听器
 * @param   listener     回调函数（能接收的回调事件和回调参数请参考<a class="el" href="modules.html">参数说明</a>中的回调事件和回调参数）
 * @param   userParam    用户参数
 * @n
 */
        virtual void set_event_listener(void(*listener)(BDSSDKMessage&,void*), void* userParam) = 0;
/**
 * @brief    释放sdk实例，在程序退出前调用
 * @param   instance     sdk实例
 * @n
 */
        static void release_instance(BDSpeechSDK* instance);
/**
 * @brief    发送消息
 * @param   message     BDSSDKMessage类型消息对象，用法请参考接口说明中<a class="el" href="classbds_1_1_b_d_s_s_d_k_message.html">BDSSDKMessage</a>
 * @param   outErrorDescription    错误描述
 * @n
 */
        virtual bool post(BDSSDKMessage &message, std::string &outErrorDescription) = 0;
/**
 * @brief    清理（已废弃）
 * @n
 */
        static void do_cleanup();
/**
 * @brief    打开日志
 * @param   logFileName     文件名
 * @param   fileSize    文件大小（单位MB）
 * @n
 */
        static void open_log_file(const char *logFileName, int fileSize = 0);
/**
 * @brief    关闭日志
 * @n
 */
        static void close_log_file();
/**
 * @brief    打开耗时统计
 * @param   recordFileName     文件名
 * @param   fileSize    文件大小（单位MB）
 * @n
 */
        static void open_time_record(const char *recordFileName, int fileSize = 0);
/**
 * @brief    关闭耗时统计
 * @n
 */
        static void close_time_record();
/**
 * @brief    获取sdk版本号
 * @return  sdk版本号
 * @n
 */
        static std::string get_sdk_version();
    protected:
        BDSpeechSDK();
        virtual ~BDSpeechSDK();
    };
}

#endif
