//
// Created by yuyaohui on 17-06-27.
//

#ifndef DUER_LINK_SDK_H
#define DUER_LINK_SDK_H

#include <string>
#include <vector>
#include "network_define_public.h"
#include "network_status_observer.h"
#include "duer_link_received_data_observer.h"
#include "duer_link_device_connection_status_observer.h"
#include "duer_link_tcp_connection_observer.h"
#include "duer_link_dlp_cloud_received_observer.h"
#include "duer_link_http2_connection_observer.h"
#include "duer_link_http2_con_factory_interface.h"

namespace duerLink {

//配网
class duerLinkSdk {
public:
    /**
    * 获取实例
    **/
    static duerLinkSdk* get_instance();

    /**
    * 销毁实例
    **/
    static void destroy();

    /**
    * 设置配置文件路径，MTK默认路径./appresources/duerLink_config.json；
    * 第三方默认当前路径./duerLink_config.json
    * @param config_file 配置文件路径
    **/
    void set_config_json_file(std::string config_file);

    /**
    * 初始化配网参数
    * @param speaker_type 设备类型
    * @param autoType 配网方式
    * @param devicedID 设备ID
    * @param interface AP网卡名称
    **/
    void init_config_network_parameter(platform_type speaker_type,
                                       auto_config_network_type autoType,
                                       std::string devicedID,
                                       std::string interface = "",
                                       std::string bdussfile = "/data/duer/budss.txt",
                                       std::string appID = "Bd9rtoM39XmYIesLpaAVX19jBAcF4gWq",
                                       std::string snID = "91201/16000xxxx");

    /**
    * 设置配网回调
    * @param config_listener 回调类 
    **/
    void set_networkConfig_observer(NetworkConfigStatusObserver* config_listener);

    /**
    * 初始化SoftAp准备，销毁环境接口
    * @param sp_prepare_func SoftAp环境准备 
    * @param sp_destroy_func Soft环境销毁
    **/
    void init_platform_softAp_control(softap_prepare_env_cb_t sp_prepare_func,
                                      softap_destroy_env_cb_t sp_destroy_func);

    /**
    * 初始化BLE操作接口
    * @param ble_prepare_func BLE准备操作 
    * @param ble_destroy_func BLE销毁操作
    * @param ble_send_func    BLE发送数据
    **/
    void init_platform_ble_control(ble_prepare_env_cb_t ble_prepare_func,
                                   ble_destroy_env_cb_t ble_destroy_func,
                                   ble_send_data_cb_t ble_send_func);

    /**
    * 初始化 网络连接接口
    * @param platform_connect_wifi_func 网络连接函数  
    **/
    void init_platform_connect_control(platform_connect_wifi_cb_t platform_connect_wifi_func);

    /**
    * 启动配网
    **/
    bool start_network_config();

    /**
    * 停止配网
    **/
    void stop_network_config();

    /**
    * 初始化设备参数
    * @param devicedID 设备ID  
    * @param appId APPID
    * @param interface STA网卡名称
    * @param bdussFile BDUSS存储路径
    **/
    void init_discovery_parameter(std::string devicedID,
                                  std::string appId,
                                  std::string interface,
                                  std::string bdussFile);

    /**
    * 开机网络状态检测
    **/    
    bool check_recovery_network_status();

    /**
    * BLE 连接成功
    **/    
    void ble_client_connected();
    
    /**
    * BLE 断开连接
    **/
    void ble_client_disconnected();

    /**
    * BLE接收函数
    * @param data  数据指针
    * @param len   数据长度
    **/
    bool ble_recv_data(void *data, int len);

    /**
    * 启动发现，配对操作
    **/    
    void start_device_discover_and_bound();

    /**
    * 设置日志输出接口，默认输出至控制台
    * @param duer_link_log 日志接口函数
    **/
    void init_duer_link_log(duer_link_log_cb_t duer_link_log);
    
    /**
    * set dlp data observer
    * @param observer
    **/
    void set_dlp_data_observer(DuerLinkReceivedDataObserver* observer);

    /**
    * send msg to client
    * @param msg
    **/
    bool send_dlp_msg_to_all_clients(const std::string& sendBuffer);

    /**
    * send msg to one client
    * @param msg
    **/
    bool send_dlp_msg_to_client_by_specific_id(const std::string& sendBuffer,unsigned short id);

    bool get_all_connection_id(std::vector<int>& id_array);

    //group interface::loudspeaker control devices
    /**
    * set access token observer
    * @param config_listener observer
    **/
    void set_access_token_observer(AccessTokenObserver* token_listener);

    /**
    * refresh access token
    **/
    void refresh_access_token();

    /**
    * refresh dumiId
    **/
    void refresh_dumiId();

    /*
     *The following interface is the speaker control TV interface, until End of them, if there is a need,
     *Please add macros in the compilation script, if there is no relevant requirements,
     *Please do not call them
     * */
    bool set_device_connection_status_observer(DuerLinkDeviceConnectionStatusObserver* observer);

    /**
    * start loudspeaker control dueros devices service
    * @param
    **/
    bool start_loudspeaker_ctrl_devices_service(device_type device_type_value, 
                                                const std::string& client_id = "",
                                                const std::string& link_buffer = "",
                                                const std::string& stored_file_path = "/data/duer/");

    /**
    * start loudspeaker control dueros device by device id
    * @param
    **/
    bool start_loudspeaker_ctrl_devices_service_by_device_id(const std::string& device_id);

    /**
    * send msg to deivices by spec type
    *@param msg_buf msg buffer
    * @param device_type devices type
    **/
    bool send_msg_to_devices_by_spec_type(const std::string& msg_buf, device_type device_type_value);

    /**
    * disconnect devices connection by spec type
    * @param devices type
    **/
    bool disconnect_devices_connections_by_spe_type(device_type device_type_value,const std::string& unlink_buffer = "");

    /**
    * get current connect device information
    * @param client_id client id of current connected device
    * @param device_id device id of current connected device
    * @param device_type_value device type 
    **/
    bool get_curret_connect_device_info(std::string& client_id,
                                        std::string& device_id,
                                        device_type device_type_value);
    /**
    * get operation type.
    **/
    void get_operation_type();
    
    /**
    * set dlp connection object.
    **/
    void set_dlp_http2_connection_object(DuerLinkHttp2ConnectionObserver* observer);

    /**
    * set wirtable file path.
    **/
    void set_writable_file_path(std::string& file_path);

    /**
    * set http2 connection factory interface.
    **/
    void set_dlp_http2_con_factory_interface(DuerLinkHttpConFactoryInterface* observer);

    /**
     * set local tcp socket op function
     * */
    void set_tcp_connection_observer(DuerLinkTcpConnectionObserver* observer);
    /*
     *  End of the speaker control TV interface.
     *  */
private:
    duerLinkSdk();
    virtual ~duerLinkSdk();

private:
    static duerLinkSdk* m_auto_network_config_ctrl;
    std::string m_device_ssid;
};

}

#endif //DUER_LINK_SDK_H
