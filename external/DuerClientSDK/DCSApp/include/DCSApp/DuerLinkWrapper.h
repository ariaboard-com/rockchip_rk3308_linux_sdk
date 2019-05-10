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

#ifndef DUEROS_DCS_APP_SAMPLEAPP_DUERLINKWRAPPER_H_
#define DUEROS_DCS_APP_SAMPLEAPP_DUERLINKWRAPPER_H_

#include "DCSApp/DuerLinkMtkInstance.h"
#include "network_observer/network_status_observer.h"
#include "device_ctrl_observer/duer_link_received_data_observer.h"
#include <mutex>

namespace duerOSDcsApp {
namespace application {

class IDuerLinkWrapperCallback {
public:
    virtual void networkReady() = 0;

    virtual void duerlinkNotifyReceivedData(const std::string& jsonPackageData, int sessionId = 0) = 0;

    virtual void duerlink_notify_received_bduss(const std::string& bdussValue) = 0;

    virtual void duerlinkNetworkOnlineStatus(bool status) = 0;
};

enum DuerLinkNetworkStatus {
    DUERLINK_NETWORK_CONFIG_STARTED,
    DUERLINK_NETWORK_CONFIGING,
    DUERLINK_NETWORK_CONFIG_SUCCEEDED,
    DUERLINK_NETWORK_CONFIG_FAILED,
    DUERLINK_NETWORK_SUCCEEDED,
    DUERLINK_NETWORK_FAILED,
    DUERLINK_NETWORK_RECOVERY_START,
    DUERLINK_NETWORK_RECOVERY_SUCCEEDED,
    DUERLINK_NETWORK_RECOVERY_FAILED,
    DUERLINK_WAIT_LOGIN
};

class DuerLinkWrapper: public duerLink::NetworkConfigStatusObserver,
                       public NetWorkPingStatusObserver,
                       public DuerLinkReceivedDataObserver {
public:
    static DuerLinkWrapper *getInstance();

    void setCallback(IDuerLinkWrapperCallback *callback);

    void release();

    void notify_network_config_status(duerLink::notify_network_status_type notify_type) override;

    void network_status_changed(InternetConnectivity current_status, bool wakeupTrigger) override;

    DuerLinkNetworkStatus getNetworkStatus() const;

    void setNetworkStatus(DuerLinkNetworkStatus networkStatus);

    void wakeupDuerLinkNetworkStatus();
#ifdef DUERLINK_V2
    void initDuerLink();
    void startDiscoverAndBound(const std::string& client_id);
#else
    void initDuerLink(const std::string& bdussFilePath, const std::string& clientId);
    void startDiscoverAndBound(const std::string& client_id, const std::string& bdussfile);
#endif
    void startNetworkRecovery();

    void stopNetworkRecovery();

    bool startNetworkConfig();

    void stopNetworkConfig();

    void verifyNetworkStatus(bool wakeupTrigger);

    void bleClientConnected();

    void bleClientDisconnected();

    bool bleRecvData(void *data, int len);


    std::string NotifyReceivedData(const std::string& jsonPackageData, int iSessionID = 0) override;
#ifdef DUERLINK_V2
    std::string notify_received_bduss(const std::string& bdussValue);
#else
    std::string notify_received_bduss(const std::string& bdussValue) override;
#endif
    bool sendDlpMsgToAllClients(const std::string& sendBuffer);

    /**
     * 发送DLP消息到指定的session id。
     * @param message 要发送的消息。
     * @param sessionId session id。
     * @return True/False 操作成功失败
     */
    bool sendDlpMessageBySessionId(const std::string& message, unsigned short sessionId);

    bool isNetworkOnline() const;

    void setNetworkOnline(bool isNetworkOnline);

    bool startLoudspeakerCtrlDevicesService(duerLink::device_type deviceTypeValue,
                                            const std::string& clientId,
                                            const std::string& message);

    bool sendMsgToDevicesBySpecType(const std::string& msgBuf, duerLink::device_type deviceTypeValue);

    bool disconnectDevicesConnectionsBySpeType(
            duerLink::device_type deviceTypeValue, const std::string& message);

    void networkLinkOrRecoverySuccess();

    /**
     * 根据设备类型获取client id和device id
     * @param[in] deviceType 设备类型
     * @param[out] clientId duer link取到的client id
     * @param[out] deviceId duer link取到的device id
     * @return
     */
    bool getCurrentConnectedDeviceInfo(duerLink::device_type deviceType,
                                       std::string& clientId, std::string& deviceId);

    bool isFirstNetworkReady() const;

    void setFirstNetworkReady(bool isFirstNetworkReady);

    bool isFromConfigNetwork() const;

    void setFromConfigNetwork(bool isFromConfigNetwork);

    void waitLogin();

    static void logFunction(const char* msg, ...);

    void OnNetworkReady();

private:
    DuerLinkWrapper();

    virtual ~DuerLinkWrapper();

    static void init();

    static void destroy();

    IDuerLinkWrapperCallback *m_callback;

    static DuerLinkWrapper *s_duerLinkWrapper;

    static pthread_once_t s_initOnce;

    static pthread_once_t s_destroyOnce;

    DuerLinkNetworkStatus m_networkStatus;

    static void networkLinkSuccess();

    static void networkLinkFailed();

    /// Mutex used to serialize access to @_network_status.
    std::mutex m_mutex;

    /// is loop network config
    bool m_isLoopNetworkConfig;

    /// is network online
    bool m_isNetworkOnline;

    /// network is first ready
    bool m_isFirstNetworkReady;

    /// network is from config
    bool m_isFromConfigNetwork;
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif  // DUEROS_DCS_APP_SAMPLEAPP_DUERLINKWRAPPER_H_
