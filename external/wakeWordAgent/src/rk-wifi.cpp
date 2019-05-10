#include "rk-wifi.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "WakeWordUtils.h"
#include <string.h>
#include <sys/time.h>
#include "Logger.h"
#include "get_event.h"
#define THREAD_SLEEP_TIME 200000//200ms
#define RAND_STRING_LENGTH 6 
#define RAND_STRING_TOTAL_LENGTH 36 

std::unique_ptr<std::thread> m_softap_thread;
namespace AlexaWakeWord {

rkSoftAP::rkSoftAP(InfoLed* infoled,GPIOWakeWordEngine* gpiowakeword) :
    m_infoled(infoled),
    m_gpiowakword(gpiowakeword),
    m_isRunning{false} {
    log(Logger::INFO,"rk soft ap init");
    m_isRunning = true;
    m_cwifistate = WifiState::IDLE;
    m_softap_thread = make_unique<std::thread>(&rkSoftAP::mainloop,this);
}
void rkSoftAP::getrnd(char* buf,int n) {
    char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    srand(time(NULL));
    for (int i = 0; i < n - 1; i++)
    {
        buf[i] = metachar[rand() % RAND_STRING_TOTAL_LENGTH];
    }
    buf[n - 1] = '\0';
}
void rkSoftAP::showWifiState(InfoLed* infoled,int mode) {
    int led_blink_time,end_mode;
    if(infoled) {
        if(mode == MODE_WIFI_ERR) {
            end_mode = MODE_WIFI_CONNECT;
            infoled->led_open(mode, 0);
            sleep(led_blink_time);
            infoled->led_open(end_mode, 0);
            system("aplay /data/mode_sound/wifi_mode.wav");//back to wakeup mode
        } else {
            led_blink_time = 9;
            end_mode = MODE_NORMAL;
            sleep(led_blink_time);
            infoled->led_open(end_mode, 0);
            system("aplay /data/mode_sound/wakeup_mode.wav");//back to wakeup mode

        }
    }
}
void rkSoftAP::startDLNA() {
    char buf[80];
    FILE *cmd_fp;
    cmd_fp = popen("pidof gmediarender","r");
    fgets(buf,sizeof(buf),cmd_fp);
    pclose(cmd_fp);
    fprintf(stderr,"gmediarender restart pid:%s\n",buf);
    int pid = atoi(buf);
    if(pid > 0) {
        sprintf(buf,"kill %d",pid);
        system(buf);
    }
    char dlna_buf[128];
    sprintf(dlna_buf,"gmediarender -f RK-DLNA-%s -u %s &",m_wifiName,m_wifiName);
    system(dlna_buf);
}
void rkSoftAP::setWifi(bool state,char* wifiName) {
    if(state) {
        char APcmd[128];
        sprintf(APcmd,"softapDemo Rockchip-Echo-%s",wifiName);
        system("updater -g -d &");
        memcpy(m_wifiName,wifiName,6);
        system(APcmd);
        system("rm /data/cfg/softap");
        m_cwifistate = WifiState::SET_MODE;
        std::lock_guard<std::mutex> lock(m_mtx);
        m_wifiState.notify_one();
    } else {
        system("rm /data/cfg/softap");
        system("updater stop");
        system("softapDemo stop");
        m_cwifistate = WifiState::IDLE;
        std::lock_guard<std::mutex> lock(m_mtx);
        m_wifiState.notify_one();
    }
}
void rkSoftAP::mainloop() {
    int ret; 
    char ap_buf[128];
    
    std::unique_lock<std::mutex> lck(m_mtx);
    auto checkstate = [this] {
        return m_cwifistate == WifiState::SET_MODE;
    };
    log(Logger::INFO,"wifi is begin");
    while(m_isRunning) {
        m_wifiState.wait(lck,checkstate);
        log(Logger::DEBUG,"wifi is connect");
        FILE* file = fopen("/data/cfg/softap","r");
        if(file == NULL) {
            ret = -1;
            log(Logger::DEBUG,"/data/cfg/softap not open");
        } else {
            fgets(ap_buf,sizeof(ap_buf),file);
            fclose(file);
        }
        if(!strncmp(ap_buf,"true",4)){
            ret = 1;
            log(Logger::INFO,"softap wifi connect success");
            system("rm /data/cfg/softap");
            memset(ap_buf,0,sizeof(ap_buf));
            showWifiState(m_infoled,MODE_WIFI_CONNECT);
            startDLNA();
            alexa_set_mode(ALEXA_NORMAL_MODE);
            m_cwifistate = WifiState::IDLE;
            system("softapDemo stop");
        } else if(!strncmp(ap_buf,"false",5)) {
            log(Logger::ERROR,"softap wifi connect failed");
            ret = -1;
            system("rm /data/cfg/softap");
            memset(ap_buf,0,sizeof(ap_buf));
            showWifiState(m_infoled,MODE_WIFI_ERR);
            //m_gpiowakword->setLedMode(GPIOWakeWordEngine::LED_MODE::NORMAL);
            //m_cwifistate = WifiState::IDLE;
        } else {
            log(Logger::DEBUG,"open other value");
        }
        usleep(THREAD_SLEEP_TIME);
    }
    /*if(ret < 0) {
        log(Logger::INFO,"wifi connect failed retry");
        infoled->led_open(MODE_WIFI_ERR, 0);
    }else {
        infoled->led_all_on(0);
        usleep(1000*1000);
        infoled->led_open(MODE_OFF, 0);
    }*/
}
rkSoftAP::~rkSoftAP(){
    m_isRunning = false;
    m_softap_thread->join();
    log(Logger::INFO,"rk soft ap deinit");
}

}
