#ifndef __RK_BT_H
#define __RK_BT_H
#include <memory>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <time.h>

namespace AlexaWakeWord {

class rkBlueTooth {
    public:
        rkBlueTooth();
        ~rkBlueTooth();
        void getrnd(char *buf,int n);
        void mainloop();
        void btPlAudioCtl(int state,char*);
        void DLNAControl(bool state,char *btName);
        void KittAiControl(bool state);
    private:
        std::atomic<bool> m_isRunning;
};

}
#endif
