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

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/time.h>
#include <cerrno>
#include <cstring>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include "LoggerUtils/DcsSdkLogger.h"
#include "DCSApp/DuerLinkWrapper.h"
#include "DCSApp/SystemUpdateRevWrapper.h"
#include "DCSApp/ActivityMonitorSingleton.h"
#include "DCSApp/SoundController.h"
#include "DCSApp/DeviceIoWrapper.h"

static const std::string TTS_STR_UPDATING       = "正在升级中,请稍等";
static const std::string TTS_STR_UPDATE_SUCCESS = "音箱即将重启，请稍等";
static const std::string TTS_STR_UPDATE_FAIL    = "升级失败,请检查网络或重启音箱后重试";

namespace duerOSDcsApp {
namespace application {

#define MSGKEY 1008
#define MSGSIZE 1024
#define MSG_ID_LENTH 32
#define VERSION_STRING_LENTH 64

struct msgstruct {
    long msgtype;
    int msgcase;
    char msgtext[MSGSIZE] = {0};
};
//type = 2;

enum SdkMsgCase {
    MSG_CASE_UPDATEABLE = 0,
    MSG_CASE_NOT_UPDATEABLE,
    MSG_CASE_UPDATING,
    MSG_CASE_UPDATE_FAIL,
    MSG_CASE_UPDATE_SUCCESS,
    MSG_CASE_GET_STATUS,
};

std::unique_ptr<SystemUpdateRevWrapper> SystemUpdateRevWrapper::create() {
    std::unique_ptr<SystemUpdateRevWrapper> systemUpdateRevWrapper(new SystemUpdateRevWrapper());
    if (!systemUpdateRevWrapper) {
        APP_ERROR("Failed to initialize SystemUpdateRevWrapper");
        return nullptr;
    }
    return systemUpdateRevWrapper;
}

SystemUpdateRevWrapper::SystemUpdateRevWrapper() : m_isRunning{false} {
    m_isRunning = true;
    m_thread = std::unique_ptr<std::thread>(new std::thread(&SystemUpdateRevWrapper::mainLoop, this));
}

SystemUpdateRevWrapper::~SystemUpdateRevWrapper() {
    APP_ERROR("joining on thread.");

    m_isRunning = false;

    m_thread->join();
}

static long currentTimeMs() {
    timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

static std::string Json(const std::string &updateStatus, const std::string &targetVersionStr,
                        const std::string &targetVersionDscStr, const std::string &msgStr) {
    char msgId[MSG_ID_LENTH];
    rapidjson::Document jsonDoc(rapidjson::kObjectType);
    rapidjson::Document::AllocatorType &alloc = jsonDoc.GetAllocator();
    jsonDoc.SetObject();

    rapidjson::Value headerStr(rapidjson::kObjectType);
    if(snprintf(msgId, MSG_ID_LENTH, "%s%ld", "msg", currentTimeMs())) {
        APP_ERROR("json msgId err\n");
    }
    headerStr.AddMember("messageId", std::string(msgId), alloc);
    headerStr.AddMember("namespace", "dlp.system_update", alloc);
    headerStr.AddMember("name", "Status", alloc);

    rapidjson::Value payloadStr(rapidjson::kObjectType);
    payloadStr.AddMember("currentVersion", "1.0.1", alloc);
    payloadStr.AddMember("currentVersionDesc", "", alloc);
    payloadStr.AddMember("targetVersion", targetVersionStr, alloc);
    payloadStr.AddMember("targetVersionDesc", targetVersionDscStr, alloc);
    payloadStr.AddMember("state", updateStatus, alloc);
    payloadStr.AddMember("message", msgStr, alloc);

    rapidjson::Value clientObj(rapidjson::kObjectType);
    clientObj.AddMember("header", headerStr, alloc);
    clientObj.AddMember("payload", payloadStr, alloc);

    jsonDoc.AddMember("to_client", clientObj, alloc);

    rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    if (!jsonDoc.Accept(writer)) {
        APP_ERROR("json err\n");
        return "";
    }

    APP_INFO("\n\njson %s \n\n", buffer.GetString());
    return buffer.GetString();
}

static inline char fromHex(const char &x) {
    return isdigit(x) ? x - '0' : x - 'A' + 10;
}

static std::string URLDecode(const std::string &sIn) {
    std::string sOut;
    for( size_t ix = 0; ix < sIn.size(); ix++ ) { //for the last 2 ??, cut it
        char ch = 0;
        if(sIn[ix]=='%') {
            ch = (fromHex(sIn[ix+1])<<4);
            ch |= fromHex(sIn[ix+2]);
            ix += 2;
        } else if(sIn[ix] == '+') {
            ch = ' ';
        } else {
            ch = sIn[ix];
        }
        sOut += (char)ch;
    }
    return sOut;
}

static std::string getVerStr(char* msgStr, char splitChar){
    int cnt = 0;
    char verStr[VERSION_STRING_LENTH];
    char* verStrTmp = verStr;
    char* msgStrTmp = msgStr;

    if (!msgStr) {
        return "";
    }

    while((*msgStrTmp != '\0') && (*msgStrTmp != splitChar)) {
        *verStrTmp++ = *msgStrTmp++;
        if(cnt++ >= (VERSION_STRING_LENTH -1))
            break;
    }
    *verStrTmp = '\0';

    return std::string(verStr);
}

static std::string getVerDsc(char* msgStr, char splitChar){
    int cnt = 0;
    char verStrDsc[MSGSIZE];
    char* verStrDscTmp = msgStr;

    if (!msgStr) {
        return "";
    }

    while((*verStrDscTmp != '\0') && (*verStrDscTmp != splitChar)) {
        verStrDscTmp++;
        if(cnt++ >= (MSGSIZE -1))
            break;
    }

    if (strlen(verStrDscTmp) < MSGSIZE -1)
        memcpy(verStrDsc, (verStrDscTmp + 1), strlen(verStrDscTmp + 1));
    *(verStrDsc + strlen(verStrDscTmp + 1) + 1) = '\0';

    std::string verStrDscConv = URLDecode(std::string(verStrDsc));
    return verStrDscConv;
}

static void enterSleepModeAndLightLed() {
    DeviceIoWrapper::getInstance()->enterSleepMode(false);
    DeviceIoWrapper::getInstance()->ledOtaDoing(); //reuse net connect led effect
}

void SystemUpdateRevWrapper::mainLoop() {
    APP_INFO("SystemUpdateRevWrapper mainLoop thread started");
    int msgqid;
    struct msgstruct msgs;
    static struct msgstruct msgs_bak;
    std::string updatableStr, notUpdatableStr, updatingStr;
    std::string updateFailStr, updateSuccessStr;

    if ((msgqid = msgget(MSGKEY, IPC_EXCL)) < 0) {
        APP_INFO("SystemUpdateRevWrapper msg is not exist, creat it!");
        msgqid = msgget(MSGKEY, IPC_CREAT | 0666);
        if (msgqid < 0) {
            APP_ERROR("SystemUpdateRevWrapper faild to creat mesg queue, error no is %d [%s]\n", errno, strerror(errno));
            return;
        }
    }

    while (m_isRunning) {
        if (msgrcv(msgqid, &msgs, (sizeof(struct msgstruct) - sizeof(long)), 2, 0) < 0) {
            APP_ERROR("SystemUpdateRevWrapper msgrcv err!");
            return;
        }

        if (msgs.msgcase != MSG_CASE_UPDATING ){
            memset( (void*)&msgs_bak, 0, sizeof(msgs_bak));
        }

        switch (msgs.msgcase) {
        case MSG_CASE_UPDATEABLE:
            APP_INFO("UPDATABLE");
            updatableStr = Json(std::string("UPDATABLE"), getVerStr(msgs.msgtext, ':'), getVerDsc(msgs.msgtext, ':'),std::string(""));
            msgs_bak = msgs;
            DuerLinkWrapper::getInstance()->sendDlpMsgToAllClients(std::string(updatableStr));
            break;

        case MSG_CASE_NOT_UPDATEABLE:
            APP_INFO("NOT_UPDATABLE");
            notUpdatableStr = Json(std::string("NOT_UPDATABLE"), std::string(""), std::string(""),std::string(""));
            DuerLinkWrapper::getInstance()->sendDlpMsgToAllClients(std::string(notUpdatableStr));
            break;

        case MSG_CASE_UPDATING:
            APP_INFO("UPDATING");
            updatableStr = Json(std::string("UPDATING"), getVerStr(msgs_bak.msgtext, ':'), getVerDsc(msgs_bak.msgtext, ':'),std::string(TTS_STR_UPDATING));
            DuerLinkWrapper::getInstance()->sendDlpMsgToAllClients(std::string(updatingStr));
            DeviceIoWrapper::getInstance()->ledOtaDoing(); //reuse net connect led effect
            SoundController::getInstance()->playTts(TTS_STR_UPDATING, false, enterSleepModeAndLightLed);
            break;

        case MSG_CASE_UPDATE_FAIL:
            APP_INFO("UPDATE FAIL");
            DeviceIoWrapper::getInstance()->exitSleepMode();
            updateFailStr = Json(std::string("UPDATE_FAIL"), std::string(""), std::string(""),std::string(TTS_STR_UPDATE_FAIL));
            DuerLinkWrapper::getInstance()->sendDlpMsgToAllClients(std::string(updateFailStr));
            DeviceIoWrapper::getInstance()->ledNetConnectFailed(); //reuse net connect led effect
            DeviceIoWrapper::getInstance()->exitSleepMode();
            SoundController::getInstance()->playTts(TTS_STR_UPDATE_FAIL, false, nullptr);
            break;

        case MSG_CASE_UPDATE_SUCCESS:
            //DeviceIoWrapper::getInstance()->exitSleepMode();
            updateSuccessStr = Json(std::string("UPDATE_SUCCESS"), std::string(""), std::string(""),std::string(TTS_STR_UPDATE_SUCCESS));
            DuerLinkWrapper::getInstance()->sendDlpMsgToAllClients(std::string(updateSuccessStr));
            DeviceIoWrapper::getInstance()->ledNetConnectSuccess(); //reuse net connect led effect
            SoundController::getInstance()->playTts(TTS_STR_UPDATE_SUCCESS, false, nullptr);
            APP_INFO("UPDATE SUCCESS");
            break;

        case MSG_CASE_GET_STATUS: {
            APP_INFO("GET DEVICES STATUS");
            char cmd[100];
            long int activeTime = 0; //ActivityMonitorSingleton::getInstance()->getLastestActiveTimestamp();
            APP_INFO("SystemUpdateRevWrapper get lastest active timestamp is %ld", activeTime);
            sprintf(cmd, "upgrade_silent_client 4 %ld",
                    activeTime); //set active time to upgrade server
            system(cmd);
            break;
        }

        default:
            break;
        }
    }
}

}  // namespace application
}  // namespace duerOSDcsApp
