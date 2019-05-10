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

#include <fstream>
#include <algorithm>
#include <netinet/in.h>
#include "LoggerUtils/DcsSdkLogger.h"
#include "DeviceIo/DeviceIo.h"
#include "DCSApp/DuerLinkWrapper.h"
#include "DCSApp/DeviceIoWrapper.h"
#include "DCSApp/SoundController.h"
#include "DCSApp/Configuration.h"

namespace duerOSDcsApp {
namespace application {

using namespace duerLink;
using std::string;
using std::vector;

DuerLinkWrapper *DuerLinkWrapper::s_duerLinkWrapper = nullptr;
pthread_once_t  DuerLinkWrapper::s_initOnce = PTHREAD_ONCE_INIT;
pthread_once_t  DuerLinkWrapper::s_destroyOnce = PTHREAD_ONCE_INIT;

DuerLinkWrapper::DuerLinkWrapper() : m_networkStatus{DUERLINK_NETWORK_SUCCEEDED},
                                     m_isLoopNetworkConfig{false},
                                     m_isNetworkOnline{false},
                                     m_isFirstNetworkReady{true},
                                     m_isFromConfigNetwork{false} {
    s_destroyOnce = PTHREAD_ONCE_INIT;
}

DuerLinkWrapper::~DuerLinkWrapper() {
    s_initOnce = PTHREAD_ONCE_INIT;
}

DuerLinkWrapper* DuerLinkWrapper::getInstance() {
    pthread_once(&s_initOnce, &DuerLinkWrapper::init);
    return s_duerLinkWrapper;
}

void DuerLinkWrapper::init() {
    if (s_duerLinkWrapper == nullptr) {
        s_duerLinkWrapper = new DuerLinkWrapper();
    }
}

void DuerLinkWrapper::destroy() {
    delete s_duerLinkWrapper;
    s_duerLinkWrapper = nullptr;
}

void DuerLinkWrapper::release() {
    pthread_once(&s_destroyOnce, DuerLinkWrapper::destroy);
}

void DuerLinkWrapper::setCallback(IDuerLinkWrapperCallback *callback) {
    m_callback = callback;
}

void DuerLinkWrapper::logFunction(const char* msg, ...) {
    va_list arg_ptr;
    char msg_buff[30720] = { 0 };

    va_start(arg_ptr, msg);
    vsnprintf(msg_buff, 30720 , msg, arg_ptr);
    va_end(arg_ptr);

    LINK_INFO(msg_buff);
}

#ifdef DUERLINK_V2    
void DuerLinkWrapper::initDuerLink() {
/*
    DuerLinkMtkInstance::get_instance()->init_config_network_parameter(duerLink::platform_type::EHodor,
                                                                       duerLink::auto_config_network_type::EAll,
                                                                       DeviceIoWrapper::getInstance()->getDeviceId(),
                                                                       "ap0");
*/
#else
void DuerLinkWrapper::initDuerLink(const std::string& bdussFilePath, const std::string& clientId) {
/*
    DuerLinkMtkInstance::get_instance()->init_config_network_parameter(duerLink::platform_type::EHodor,
                                                                       duerLink::auto_config_network_type::EAll,
                                                                       DeviceIoWrapper::getInstance()->getDeviceId(),
                                                                       "ap0",
                                                                       bdussFilePath,
                                                                       clientId);
*/
#endif
    if (!Configuration::getInstance()->getDebug()) {
        DuerLinkMtkInstance::get_instance()->init_duer_link_log(logFunction);
    }
    DuerLinkMtkInstance::get_instance()->set_networkConfig_observer(this);

    DuerLinkMtkInstance::get_instance()->set_monitor_observer(this);

//    DuerLinkMtkInstance::get_instance()->init_softAp_env_cb();
//
//    DuerLinkMtkInstance::get_instance()->init_ble_env_cb();
//
//    DuerLinkMtkInstance::get_instance()->init_wifi_connect_cb();

}

void DuerLinkWrapper::startNetworkRecovery() {
    DuerLinkMtkInstance::get_instance()->start_network_recovery();
    DuerLinkMtkInstance::get_instance()->start_network_monitor();
}

bool DuerLinkWrapper::startNetworkConfig() {
    return DuerLinkMtkInstance::get_instance()->start_network_config(5*60);
}

void DuerLinkWrapper::stopNetworkConfig() {
    DuerLinkMtkInstance::get_instance()->stop_network_config();
}

void DuerLinkWrapper::bleClientConnected() {
    DuerLinkMtkInstance::get_instance()->ble_client_connected();
}

void DuerLinkWrapper::bleClientDisconnected() {
    DuerLinkMtkInstance::get_instance()->ble_client_disconnected();
}

bool DuerLinkWrapper::bleRecvData(void *data, int len) {
    return DuerLinkMtkInstance::get_instance()->ble_recv_data(data, len);
}

DuerLinkNetworkStatus DuerLinkWrapper::getNetworkStatus() const {
    return m_networkStatus;
}

void DuerLinkWrapper::setNetworkStatus(DuerLinkNetworkStatus networkStatus) {
    std::lock_guard<std::mutex> lock(m_mutex);
    DuerLinkWrapper::m_networkStatus = networkStatus;
}

void DuerLinkWrapper::verifyNetworkStatus(bool wakeupTrigger) {
    APP_INFO("verifyNetworkStatus wakeupTrigger:%d", wakeupTrigger);
    DuerLinkMtkInstance::get_instance()->start_verify_network();
    APP_INFO("verifyNetworkStatus wakeupTrigger end");
}

void DuerLinkWrapper::notify_network_config_status(duerLink::notify_network_status_type notify_type) {

    APP_INFO("notify_network_config_status: %d", DuerLinkMtkInstance::get_instance()->get_operation_type());

    switch (notify_type) {
//        case duerLink::ENetworkConfigStarted: {
//            APP_INFO("notify_network_config_status: ENetworkConfigStarted=====");
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(true);
//            setNetworkStatus(DUERLINK_NETWORK_CONFIG_STARTED);
//            DeviceIoWrapper::getInstance()->exitMute();
//            DeviceIoWrapper::getInstance()->ledNetWaitConnect();
//            if (!m_isLoopNetworkConfig) {
//                if (DuerLinkMtkInstance::get_instance()->get_operation_type() == operation_type::EAutoConfig) {
//                    SoundController::getInstance()->linkStartFirst();
//                } else if (DuerLinkMtkInstance::get_instance()->get_operation_type() == operation_type::EManualConfig) {
//                    SoundController::getInstance()->linkStart();
//                }
//                DeviceIoWrapper::getInstance()->cancelMusic();
//                m_isLoopNetworkConfig = true;
//            }
//            break;
//        }
//        case duerLink::ENetworkConfigIng: {
//            APP_INFO("notify_network_config_status: ENetworkConfigIng=====");
//            setNetworkStatus(DUERLINK_NETWORK_CONFIGING);
//            if (DuerLinkMtkInstance::get_instance()->get_operation_type() != operation_type::EAutoEnd) {
//                DeviceIoWrapper::getInstance()->ledNetDoConnect();
//                SoundController::getInstance()->linkConnecting();
//            }
//            break;
//        }
//        case duerLink::ENetworkLinkFailed: {
//            APP_INFO("notify_network_config_status: ENetworkLinkFailed=====");
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(false);
//            //Network config failed, reset wpa_supplicant.conf
//            DuerLinkMtkInstance::get_instance()->set_wpa_conf(false);
//
//            setNetworkStatus(DUERLINK_NETWORK_CONFIG_FAILED);
//            if (DuerLinkMtkInstance::get_instance()->get_operation_type() != operation_type::EAutoEnd) {
//                DeviceIoWrapper::getInstance()->ledNetConnectFailed();
//                SoundController::getInstance()->linkFailedPing(DuerLinkWrapper::networkLinkFailed);
//            }
//            break;
//        }
//        case duerLink::ENetworkConfigRouteFailed: {
//            APP_INFO("notify_network_config_status: ENetworkConfigRouteFailed=====");
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(false);
//            //Network config failed, reset wpa_supplicant.conf
//            DuerLinkMtkInstance::get_instance()->set_wpa_conf(false);
//
//            setNetworkStatus(DUERLINK_NETWORK_CONFIG_FAILED);
//            if (DuerLinkMtkInstance::get_instance()->get_operation_type() != operation_type::EAutoEnd) {
//                DeviceIoWrapper::getInstance()->ledNetConnectFailed();
//                SoundController::getInstance()->linkFailedIp(DuerLinkWrapper::networkLinkFailed);
//            }
//            break;
//        }
//        case duerLink::ENetworkConfigExited: {
//            APP_INFO("notify_network_config_status: ENetworkConfigExited=====");
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(false);
//            setNetworkStatus(DUERLINK_NETWORK_FAILED);
//            m_isLoopNetworkConfig = false;
//            DeviceIoWrapper::getInstance()->ledNetOff();
//            SoundController::getInstance()->linkExit(NULL);
//            APP_INFO("notify_network_config_status: ENetworkConfigExited=====End");
//            break;
//        }
//        case duerLink::ENetworkLinkSucceed: {
//
//            APP_INFO("notify_network_config_status: ENetworkLinkSucceed===== [%d]",
//                     DuerLinkMtkInstance::get_instance()->get_operation_type());
//            setFromConfigNetwork(false);
//            if (DeviceIoWrapper::getInstance()->isTouchStartNetworkConfig()) {
//                /// from networkConfig
//                setFromConfigNetwork(true);
//            }
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(false);
//            //Network config succed, update wpa_supplicant.conf
//            DuerLinkMtkInstance::get_instance()->stop_network_config_timeout_alarm();
//            DuerLinkMtkInstance::get_instance()->set_wpa_conf(true);
//
//            setNetworkStatus(DUERLINK_NETWORK_CONFIG_SUCCEEDED);
//            if (!isFromConfigNetwork()) {
//                DeviceIoWrapper::getInstance()->ledNetOff();
//            }
//
//            m_isLoopNetworkConfig = false;
//            if (m_callback) {
//                m_callback->networkReady();
//            }
//            break;
//        }
        case duerLink::ENetworkRecoveryStart: {
            APP_INFO("notify_network_config_status: ENetworkRecoveryStart=====");
//            DeviceIoWrapper::getInstance()->setRecoveryingNetwork(true);
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(true);
            setNetworkStatus(DUERLINK_NETWORK_RECOVERY_START);
//            DeviceIoWrapper::getInstance()->ledNetRecoveryConnect();
//            SoundController::getInstance()->reLink();
            break;
        }
        case duerLink::ENetworkRecoverySucceed: {
            APP_INFO("notify_network_config_status: ENetworkRecoverySucceed=====");
//            DeviceIoWrapper::getInstance()->setRecoveryingNetwork(false);
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(false);
//            setNetworkStatus(DUERLINK_NETWORK_RECOVERY_SUCCEEDED);

//            setFromConfigNetwork(false);
            if (m_callback) {
                m_callback->networkReady();
            }
            break;
        }
        case duerLink::ENetworkRecoveryFailed: {
            APP_INFO("notify_network_config_status: ENetworkRecoveryFailed=====");
//            DeviceIoWrapper::getInstance()->setRecoveryingNetwork(false);
//            DeviceIoWrapper::getInstance()->setTouchStartNetworkConfig(false);
            setNetworkStatus(DUERLINK_NETWORK_RECOVERY_FAILED);
            DeviceIoWrapper::getInstance()->ledNetConnectFailed();
            //SoundController::getInstance()->reLinkFailed();
            SoundController::getInstance()->networkConnectFailed();
            break;
        }
//        case duerLink::ENetworkDeviceConnected: {
//            DeviceIoWrapper::getInstance()->ledNetDoConnect();
//            SoundController::getInstance()->hotConnected();
//            break;
//        }
        default:
            break;
    }
}

void DuerLinkWrapper::network_status_changed(InternetConnectivity current_status, bool wakeupTrigger) {
    if (current_status == InternetConnectivity::AVAILABLE) {
        if (!isNetworkOnline()) {
            setNetworkStatus(DUERLINK_NETWORK_SUCCEEDED);
            setNetworkOnline(true);
            if (m_callback) {
                m_callback->duerlinkNetworkOnlineStatus(isNetworkOnline());
            }
        }
    } else {
        if (isNetworkOnline()) {
            if (!DeviceIoWrapper::getInstance()->isTouchStartNetworkConfig()) {
                setNetworkStatus(DUERLINK_NETWORK_FAILED);
                if (wakeupTrigger) {
                    wakeupDuerLinkNetworkStatus();
                }
            }
            setNetworkOnline(false);
            if (m_callback) {
                m_callback->duerlinkNetworkOnlineStatus(isNetworkOnline());
            }
        }
    }
}

void DuerLinkWrapper::wakeupDuerLinkNetworkStatus() {
    switch (m_networkStatus) {
//        case DUERLINK_NETWORK_CONFIG_STARTED: {
//            APP_INFO("wakeup_duer_link_network_status: DUERLINK_NETWORK_CONFIG_STARTED=====");
//            DeviceIoWrapper::getInstance()->ledNetWaitConnect();
//            SoundController::getInstance()->linkStart();
//            break;
//        }
//        case DUERLINK_NETWORK_CONFIGING: {
//            APP_INFO("wakeup_duer_link_network_status: DUERLINK_NETWORK_CONFIGING=====");
//            DeviceIoWrapper::getInstance()->ledNetDoConnect();
//            SoundController::getInstance()->linkConnecting();
//            break;
//        }
        case DUERLINK_NETWORK_RECOVERY_START: {
            APP_INFO("wakeup_duer_link_network_status: DUERLINK_NETWORK_RECOVERY_START=====");
            DeviceIoWrapper::getInstance()->ledNetRecoveryConnect();
            SoundController::getInstance()->reLink();
            break;
        }
        case DUERLINK_NETWORK_FAILED: {
            APP_INFO("wakeup_duer_link_network_status: DUERLINK_NETWORK_FAILED=====");
            DeviceIoWrapper::getInstance()->ledNetConnectFailed();
            SoundController::getInstance()->networkConnectFailed();
            break;
        }
        case DUERLINK_NETWORK_RECOVERY_FAILED: {
            APP_INFO("wakeup_duer_link_network_status: DUERLINK_NETWORK_RECOVERY_FAILED=====");
            DeviceIoWrapper::getInstance()->ledNetConnectFailed();
            //SoundController::getInstance()->reLinkFailed();
            SoundController::getInstance()->networkConnectFailed();
            break;
        }
        case DUERLINK_WAIT_LOGIN: {
            waitLogin();
            break;
        }
        default:
            break;
    }
}

void DuerLinkWrapper::networkLinkSuccess() {
    DeviceIoWrapper::getInstance()->ledNetOff();
    if (framework::DeviceIo::getInstance()->inOtaMode()) {
        DeviceIoWrapper::getInstance()->setMute(false);
        framework::DeviceIo::getInstance()->rmOtaFile();
    }
}

void DuerLinkWrapper::networkLinkFailed() {
    if (framework::DeviceIo::getInstance()->inOtaMode()) {
        DeviceIoWrapper::getInstance()->setMute(false);
        framework::DeviceIo::getInstance()->rmOtaFile();
    }
}
#ifdef DUERLINK_V2    
void DuerLinkWrapper::startDiscoverAndBound(const string& client_id) {
    DuerLinkMtkInstance::get_instance()->init_discovery_parameter(DeviceIoWrapper::getInstance()->getDeviceId()
                                                                         , client_id, "eth0");
    DuerLinkMtkInstance::get_instance()->set_dlp_data_observer(this);
    DuerLinkMtkInstance::get_instance()->start_discover_and_bound();
}
#else
void DuerLinkWrapper::startDiscoverAndBound(const string& client_id, const std::string& bdussfile) {
    DuerLinkMtkInstance::get_instance()->init_discovery_parameter(DeviceIoWrapper::getInstance()->getDeviceId(),
                                                                  client_id,
                                                                  "eth0",
                                                                  bdussfile);
    DuerLinkMtkInstance::get_instance()->set_dlp_data_observer(this);
    DuerLinkMtkInstance::get_instance()->start_discover_and_bound();
}
#endif

string DuerLinkWrapper::NotifyReceivedData(const string& jsonPackageData, int iSessionID) {
    if (m_callback) {
        m_callback->duerlinkNotifyReceivedData(jsonPackageData);
    }
    return "";
}

std::string DuerLinkWrapper::notify_received_bduss(const std::string &bdussValue) {
    if (m_callback) {
        m_callback->duerlink_notify_received_bduss(bdussValue);
    }
    return "";
}

bool DuerLinkWrapper::sendDlpMsgToAllClients(const string &sendBuffer) {
    return DuerLinkMtkInstance::get_instance()->send_dlp_msg_to_all_clients(sendBuffer);
}

bool DuerLinkWrapper::sendDlpMessageBySessionId(
    const string& message, unsigned short sessionId) {
    return DuerLinkMtkInstance::get_instance()->send_dlp_msg_by_specific_session_id(message,
                                                                                    sessionId);
}

bool DuerLinkWrapper::isNetworkOnline() const {
    return m_isNetworkOnline;
}

void DuerLinkWrapper::setNetworkOnline(bool isNetworkOnline) {
    DuerLinkWrapper::m_isNetworkOnline = isNetworkOnline;
}

bool DuerLinkWrapper::startLoudspeakerCtrlDevicesService(duerLink::device_type deviceTypeValue,
                                                         const string &clientId,
                                                         const string& message) {
    return DuerLinkMtkInstance::get_instance()->start_loudspeaker_ctrl_devices_service(deviceTypeValue,
                                                                                       clientId,
                                                                                       message);
}

bool DuerLinkWrapper::sendMsgToDevicesBySpecType(const string &msgBuf,
                                                 duerLink::device_type deviceTypeValue) {
        return DuerLinkMtkInstance::get_instance()->send_msg_to_devices_by_spec_type(msgBuf,
                                                                                     deviceTypeValue);
}

bool DuerLinkWrapper::disconnectDevicesConnectionsBySpeType(duerLink::device_type deviceTypeValue,
                                                            const string& message) {
    return DuerLinkMtkInstance::get_instance()->disconnect_devices_connections_by_spe_type(deviceTypeValue,
                                                                                           message);
}

void DuerLinkWrapper::networkLinkOrRecoverySuccess() {
    setNetworkStatus(DUERLINK_NETWORK_SUCCEEDED);
    DeviceIoWrapper::getInstance()->ledNetConnectSuccess();
    if (isFromConfigNetwork()) {
        SoundController::getInstance()->linkSuccess(DuerLinkWrapper::networkLinkSuccess);
    } else {
        SoundController::getInstance()->reLinkSuccess(DuerLinkWrapper::networkLinkSuccess);
    }
}

bool DuerLinkWrapper::getCurrentConnectedDeviceInfo(duerLink::device_type deviceType,
                                                    string& clientId,
                                                    string& deviceId) {
    return DuerLinkMtkInstance::get_instance()->get_curret_connect_device_info(clientId,
                                                                               deviceId,
                                                                               deviceType);
}

bool DuerLinkWrapper::isFirstNetworkReady() const {
    return m_isFirstNetworkReady;
}

void DuerLinkWrapper::setFirstNetworkReady(bool isFirstNetworkReady) {
    DuerLinkWrapper::m_isFirstNetworkReady = isFirstNetworkReady;
}

bool DuerLinkWrapper::isFromConfigNetwork() const {
    return m_isFromConfigNetwork;
}

void DuerLinkWrapper::setFromConfigNetwork(bool isFromConfigNetwork) {
    if (isFromConfigNetwork) {
        APP_INFO("setFromConfigNetwork: isFromConfigNetwork is true");
    } else {
        APP_INFO("setFromConfigNetwork: isFromConfigNetwork is false");
    }
    DuerLinkWrapper::m_isFromConfigNetwork = isFromConfigNetwork;
}

void DuerLinkWrapper::waitLogin() {
    setNetworkStatus(DUERLINK_WAIT_LOGIN);

    SoundController::getInstance()->waitLogin();
    DeviceIoWrapper::getInstance()->ledNetWaitLogin();
}

}  // namespace application
}  // namespace duerOSDcsApp
