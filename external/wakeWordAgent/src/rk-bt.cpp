#include "rk-bt.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "WakeWordUtils.h"
#include <string.h>
#include <sys/time.h>
#include "Logger.h"
std::unique_ptr<std::thread> m_bt_thread;
#define THREAD_SLEEP_TIME 200000//200ms
#define RAND_STRING_LENGTH 8
#define RAND_STRING_TOTAL_LENGTH 36

namespace AlexaWakeWord {

rkBlueTooth::rkBlueTooth() :
    m_isRunning{false} {
    log(Logger::INFO,"rk soft ap init");
    m_isRunning = true;
    m_bt_thread = make_unique<std::thread>(&rkBlueTooth::mainloop,this);
}
void rkBlueTooth::getrnd(char* buf,int n) {
    char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    srand(time(NULL));
    for (int i = 0; i < n - 1; i++)
    {
        buf[i] = metachar[rand() % RAND_STRING_TOTAL_LENGTH];
    }
    buf[n - 1] = '\0';
}

void rkBlueTooth::mainloop() {
    int ret;
    system("rfkill unblock all");
    system("brcm_patchram_plus1 --enable_hci --no2bytes --use_baudrate_for_download  --tosleep  200000 --baudrate 1500000 --patchram  /data/bcm43438a0.hcd /dev/ttyS0 &");
    usleep(25*1000*1000);
    system("/usr/libexec/bluetooth/bluetoothd --compat -n &");  
    system("sdptool add A2SNK");
    system("hciconfig hci0 up");
    system("hciconfig hci0 piscan");
    //system("hciconfig hci0 down");
}

void rkBlueTooth::DLNAControl(bool state,char *btName) {
    char buf[80];
    FILE *cmd_fp;
    if(!state) {
        cmd_fp = popen("pidof gmediarender","r");
        fgets(buf,sizeof(buf),cmd_fp);
        pclose(cmd_fp);
        fprintf(stderr,"gmediarender restart pid:%s\n",buf);
        int pid = atoi(buf);
        if(pid > 0) {
            sprintf(buf,"kill %d",pid);
            system(buf);
        }
    } else {
        char dlna_buf[128];
        sprintf(dlna_buf,"gmediarender -f RK-DLNA-%s -u %s &",btName,btName);
        system(dlna_buf);
    }
}

void rkBlueTooth::KittAiControl(bool state) {
    char buf[80];
    FILE *cmd_fp;
    if(!state) {
        cmd_fp = popen("pidof -s wakeWordAgent","r");//kill kitt ai
        fgets(buf,sizeof(buf),cmd_fp);
        pclose(cmd_fp);
        fprintf(stderr,"it need kill kitt ai in bt mode, pid:%s\n",buf);
        int pid = atoi(buf);
        if(pid > 0) {
            sprintf(buf,"kill %d",pid);
            system(buf);
        }
    } else {
        char kitt_buf[128];
        sprintf(kitt_buf,"/data/wakeWordAgent -e kitt_ai &");
        system(kitt_buf);
    }
}

void rkBlueTooth::btPlAudioCtl(int state,char *btName) {
    char buf[80];
    FILE *cmd_fp;
    if(state) {
        DLNAControl(false,btName);

        cmd_fp = popen("pidof bluetoothd","r");
        fgets(buf,sizeof(buf),cmd_fp);
        pclose(cmd_fp);
        int pid = atoi(buf);
        if(pid > 0) {

        } else {
            fprintf(stderr,"bluetoothd restart\n");
            system("/usr/libexec/bluetooth/bluetoothd --compat -n &");
        }

        system("hciconfig hci0 up");
        char bt_buf[128];
        char btcmd[128];
        sprintf(btcmd,"hciconfig hci0 name 'rk-bt-%s'",btName);
        system(btcmd);
        //system("hciconfig hci0 name 'rk-echo-bt'");
        system("hciconfig hci0 down");
        usleep(50*1000);
        system("hciconfig hci0 up");
        system("pulseaudio --start -v");
        usleep(2000*1000);
        system("pulseaudio --start -v");//try again,confirm pulseaudio is start
        system("/data/bt-auto-connect &");
    } else {
        system("pulseaudio -k");
        system("hciconfig hci0 down");
        system("busybox killall bt-auto-connect");
    }
}
rkBlueTooth::~rkBlueTooth(){
    m_isRunning = false;
    m_bt_thread->join();
    log(Logger::INFO,"rk soft ap deinit");
}

}
