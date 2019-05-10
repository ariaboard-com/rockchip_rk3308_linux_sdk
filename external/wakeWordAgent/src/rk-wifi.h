#ifndef __RK_WIFI_H
#define __RK_WIFI_H
#include <memory>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <time.h>

#include "rk-leds.h"
#include "GPIOWakeWordEngine.h"
namespace AlexaWakeWord {


class rkSoftAP {
    public:
        rkSoftAP(InfoLed* infoled,GPIOWakeWordEngine*);
        ~rkSoftAP();
        void mainloop();
        void getrnd(char *buf,int n);
        void showWifiState(InfoLed* infoled,int mode);
        void startDLNA();
        void setWifi(bool state,char*);
    private:
        enum class WifiState {
            UNINITIALIZED = 1,            // agent is not initialized.
            IDLE,                         // initialized, but idle.
            SET_MODE,                     // set wifi
        };

        std::atomic<bool> m_isRunning;
        InfoLed* m_infoled;
        // Synchronization variables
        WifiState m_cwifistate;
        std::mutex m_mtx;
        std::condition_variable m_wifiState;
        GPIOWakeWordEngine* m_gpiowakword;
        char m_wifiName[6];
};

}
#endif
