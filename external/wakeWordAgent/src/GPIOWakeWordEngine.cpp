#include "GPIOWakeWordEngine.h"
#include "Logger.h"
#include "WakeWordUtils.h"
#include "WakeWordException.h"
#include "get_event.h"
//#include "rk-leds.h"
//#include <wiringPi.h>
#include "daemon/Daemon.h"
#include <unistd.h>
#include <string.h>
#include "rk-wifi.h"
#include "rk-bt.h"
#include "GPIOIPCSocket.h"
namespace AlexaWakeWord {

std::unique_ptr<InfoLed> infoled;

GPIOWakeWordEngine::GPIOWakeWordEngine(WakeWordDetectedInterface* interface) :
    WakeWordEngine(interface), m_isRunning { false }, m_curBootState {BOOT_STATE::STATE_BOOTED} {

  try {
    init();
  } catch (WakeWordException& e) {
    log(Logger::ERROR,
        std::string("GPIOWakeWordEngine: Initialization error:") + e.what());
    throw;
  }

}

void GPIOWakeWordEngine::pause() {
    log(Logger::DEBUG, "GPIOWakeWordEngine: handling pause");
}

void GPIOWakeWordEngine::resume() {
  kittAiReady = true;
  log(Logger::DEBUG, "GPIOWakeWordEngine: handling resume");
}

void GPIOWakeWordEngine::setVolume(int vol) {
  log(Logger::INFO, "GPIOWakeWordEngine: set volume: " + std::to_string(vol));
  if(vol == 0)
      alexa_volume_set_mute();
  else
      alexa_volume_set_step(vol);
}

void GPIOWakeWordEngine::setInitState() {
  log(Logger::INFO, "GPIOWakeWordEngine: set init state");
  mJavaIsReady = true;
}
void GPIOWakeWordEngine::startAmazonRecord() {
  log(Logger::DEBUG, "GPIOWakeWordEngine:startAmazonRecord ");
  if(infoled && kittAiReady && (alexa_get_mode() == ALEXA_NORMAL_MODE))
      infoled->led_open(MODE_VP_WAKEUP, 0);
}
void GPIOWakeWordEngine::stopAmazonRecord() {
  if(infoled && kittAiReady && (alexa_get_mode() == ALEXA_NORMAL_MODE))
      infoled->led_open(MODE_VP, 0);
  log(Logger::DEBUG, "GPIOWakeWordEngine:stopAmazonRecord");
}
void GPIOWakeWordEngine::startAmazonPlay() {
  if(infoled && kittAiReady && (alexa_get_mode() == ALEXA_NORMAL_MODE))
      infoled->led_open(MODE_OFF, 0);
  log(Logger::DEBUG, "GPIOWakeWordEngine:startAmazonPlay");
}

void GPIOWakeWordEngine::setAuthorizedState(bool state) {
  log(Logger::INFO, "GPIOWakeWordEngine:setAuthorizedState");
  if(!state && infoled && kittAiReady && gpioReady) {
      infoled->led_open(MODE_WIFI_ERR,0); 
  } else {
      log(Logger::INFO, "GPIOWakeWordEngine:setAuthorizedState booted");
      infoled->led_open(MODE_BOOTED,0);
      sleep(2);
      infoled->led_open(MODE_OFF,0);
  }
}

void GPIOWakeWordEngine::init() {
  log(Logger::DEBUG, "GPIOWakeWordEngine: initializing");
  m_isRunning = true;
  kittAiReady = false ;
  mJavaIsReady = false ;
  gpioReady = false;
  m_gpio_ipc_handle = make_unique<GPIOIPCSocket>();
  m_thread = make_unique<std::thread>(&GPIOWakeWordEngine::mainLoop, this);
}

int GPIOWakeWordEngine::my_exec(const char *cmdstring)
{
	FILE *fp;
	char buf[1024]; 

	if (!cmdstring) {
		log(Logger::INFO, "Invalid argument, parameter can't be NULL");
		return -1;
	}
	fp = popen(cmdstring,"r");
	if (!fp) {
		fprintf(stderr, "popen failed, the command is : %s", cmdstring);
		return -2;
	}
	fprintf(stderr, "execute command:%s successfully \n",cmdstring);
//	while (fgets(buf, sizeof(buf), fp) != NULL) {
//		fprintf(stderr, "%s" ,buf);
//	}
	pclose(fp);

	return 0;
}
void gpio_getrnd(char* buf,int n) {
    char metachar[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    srand(time(NULL));
    for (int i = 0; i < n - 1; i++)
    {
        buf[i] = metachar[rand() % 36];
    }
    buf[n - 1] = '\0';
}
static int cx20921_upgrade()
{
	int ret = -1;
	char buf[256] = {0};
	char name[256] = "/data/cx20921/i2c_flash -c -d /dev/i2c-2 -g 28 -f /data/cx20921/evk-nebula-generic.sfs /data/cx20921/iflash.bin";
	FILE *pFile = NULL;

	if (!name)
		return -1;
	printf("%s, cmd = %s\n",__func__,name);
	pFile = popen(name, "r");
	if (pFile != NULL)  {
		while (fgets(buf, sizeof(buf), pFile)) {
			if (strstr(buf, "Firmware Downloaded Successfully")){
				printf("cx20921 fw downloading is finished\n");
				ret = 0;
			}
			if (strstr(buf, "same with the downloading fw version")){
				printf("the current cx20921 fw is the latest version, don't need to upgrade.\n");
				ret = 1;
			}
//			printf("%s\n",buf);
		}
		pclose(pFile);
	}
	return ret;
}

static int VOLUME_STEP[12]={1,3,5,7,9,11,12,14,15,16,17,18};
void GPIOWakeWordEngine::mainLoop() {
  int ret;
  char cmdbuf[128]={0};
  FILE *devInfo;
  char tmpbuf[128]={0};
  char *kitt_env; 
  devInfo = fopen("/data/cfg/device_info.txt","rb");
  if(devInfo != NULL) {
      fscanf(devInfo,"sequence:%s volume:%d\n",m_defseq,&m_defvol);
      fprintf(stderr,"defsequence:%s\n",m_defseq);
      fprintf(stderr,"m_defvol:%d\n",m_defvol);
      fclose(devInfo);
  } else {
      devInfo = fopen("/data/cfg/device_info","rb");
      fprintf(stderr,"can't open /data/cfg/device_info.txt,try open device_info\n");
      if(devInfo!=NULL) {
          fscanf(devInfo,"sequence:%s volume:%d\n",m_defseq,&m_defvol);
          fprintf(stderr,"defsequence:%s\n",m_defseq);
          fprintf(stderr,"m_defvol:%d\n",m_defvol);
      } else {
          log(Logger::INFO, "can't open device file /data/cfg/device_info\n");
          log(Logger::INFO, "please check it\n");
      }
  }
  std::unique_ptr<rkSoftAP> rksoftap;
  std::unique_ptr<rkBlueTooth> rkbt;
  rkbt = make_unique<rkBlueTooth>();//download bt firmware
  while (m_isRunning) {
	switch (m_curBootState) {
		case BOOT_STATE::STATE_BOOTED:
			log(Logger::INFO, "Starting info led");
			//check cx20921 firmware version is updated
			ret = cx20921_upgrade();
			if (ret == 1) {
				fprintf(stderr, "don't need to download cx20921 firmware\n");
				usleep(100*1000);
			} else if(ret == 0) {
				fprintf(stderr, "download cx20921 firmware successfully, take time to emulated usb audio\n");
				usleep(3*1000*1000);
			} else {
				fprintf(stderr, "[ERROR]download cx20921 firmware failed!\n");
			}
			infoled = make_unique<InfoLed>(); 
			infoled->init(); 
			rksoftap = make_unique<rkSoftAP>(infoled.get(),this);
//			infoled->led_open(MODE_BOOTED,0);
			fprintf(stderr, "--------------STATE_BOOTED------------------\n");
		//	my_exec("resize2fs /dev/mmcblk0p7");
			m_curBootState = BOOT_STATE::STATE_NETWORK_STARTED;
			break;
		case BOOT_STATE::STATE_NETWORK_STARTED:
			fprintf(stderr, "--------------STATE_NETWORK_STARTED------------------\n");
			//infoled->led_open(MODE_NETWORK_STARTED,0);
            system("wpa_supplicant -i wlan0 -c /data/cfg/wpa_supplicant.conf &");
			log(Logger::INFO,"wpa_supplicant ret = " + std::to_string(ret));
			ret = system("udhcpc -n -t 2 -i wlan0");
			log(Logger::INFO,"udhcp ret = " + std::to_string(ret));
			m_curBootState = BOOT_STATE::STATE_SENSORY_STARTED;
			infoled->led_open(MODE_NETWORK_STARTED,0);
			break;
		case BOOT_STATE::STATE_SENSORY_STARTED:
			fprintf(stderr, "--------------STATE_SENSORY_STARTED------------------\n");
			system("cd /data/");
			kitt_env = getenv("KITT_AI_OPEN");
			if(kitt_env != NULL) {
				if(!strncmp(kitt_env,"true",4)) {
					log(Logger::INFO, "wakeword engine use kitt_ai ");
					system("./wakeWordAgent -e kitt_ai &");
				} else {
					log(Logger::INFO, "wakeword engine use sensory ");
					system("./wakeWordAgent -e sensory &");
				} 
			} else {
				log(Logger::INFO, "wakeword engine use sensory ");
				system("./wakeWordAgent -e sensory &");
			}

			if(!ret) {
				char dlna_buf[128];
				sprintf(dlna_buf,"gmediarender -f RK-DLNA-%s -u %s &",m_defseq,m_defseq);
				system(dlna_buf);
				log(Logger::INFO,"wifi is connect,start dlna ret = " + std::to_string(ret));
			}
			m_curBootState = BOOT_STATE::STATE_JAVACLIENT_STARTED;
			break;
		case BOOT_STATE::STATE_JAVACLIENT_STARTED:
			fprintf(stderr, "--------------STATE_JAVACLIENT_STARTED------------------\n");
			system("cd /data/");
			ret = system("./javacmd &");
			log(Logger::INFO,"javacmd ret = " + std::to_string(ret));
			while(!mJavaIsReady || !kittAiReady) {
				usleep(500*1000);
			}
			infoled->led_open(MODE_SYSTEM_OK,0);
			init_daemon();
			m_curBootState = BOOT_STATE::STATE_SYSTEM_OK;
			break;
		case BOOT_STATE::STATE_SYSTEM_OK:
			fprintf(stderr, "--------------STATE_SYSTEM_OK------------------\n");
			start_event_thread();
			usleep(1000*1000);
			m_isRunning = false;
			break;
		default:
			fprintf(stderr, "--------------STATE_DEFALT------------------\n");
			break;
	}
  }
  m_isRunning = true;
  alexa_set_mode(ALEXA_NORMAL_MODE);
  infoled->led_open(MODE_START,0);
  gpioReady = true;
  sprintf(cmdbuf,"amixer cset numid=1 %d",VOLUME_STEP[m_defvol-1]);//12 step,1 ~ 12,amixer:1 ~ 18
  system(cmdbuf);
  system("aplay /data/mode_sound/hello.wav");
  while (m_isRunning) {
      log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. event_process start");
      int evt_type = event_process();
      log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. event_process end. evt_type:" + evt_type);
      int ret, volume_step;
      switch(evt_type) {
          case FUNC_KEY_WAKEUP:
              log(Logger::INFO,"get key KEY_FUNC_WAKEUP");
              if(alexa_get_mode() == ALEXA_NORMAL_MODE) {
                  infoled->led_open(MODE_VP_WAKEUP, 0);
                  wakeWordDetected();
              }
              break;
          case FUNC_KEY_WIFI_MODE:
		fprintf(stderr, "event processing FUNC_KEY_WIFI_MODE\n");
		log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. enter wifi mode");
		set_mode(1);
		infoled->led_open(MODE_WIFI_CONNECT, 0);
		if(!strncmp(m_defseq,"12345",5)) {
			devInfo = fopen("/data/cfg/device_info.txt","rb+");
			if(devInfo) {
				memset(m_defseq,0,sizeof(m_defseq));
				gpio_getrnd(m_defseq,6);
				fprintf(stderr,"wifi name :%s\n",m_defseq);
				fseek(devInfo,9,SEEK_SET);//need optimize
				sprintf(tmpbuf,"%s",m_defseq); 
				fwrite(tmpbuf,1,strlen(tmpbuf)+1,devInfo);
				fprintf(stderr,"tmpbuf:%s\n",tmpbuf);
				fflush(devInfo);
				fsync(fileno(devInfo));
				fclose(devInfo);
			}
			system("cp /data/cfg/device_info.txt /data/cfg/device_info"); 
		}
		rksoftap->setWifi(true,m_defseq);
		system("aplay /data/mode_sound/wifi_mode.wav");
		break;
	case FUNC_KEY_BT_MODE:
		fprintf(stderr, "event processing FUNC_KEY_BT_MODE\n");
              log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. LED_MODE::BT");
              m_gpio_ipc_handle->sendCommand(IPCInterface::Command::PAUSE_WAKE_WORD_ENGINE);
              rksoftap->setWifi(false,m_defseq);
              set_mode(2);
              infoled->led_open(MODE_BLUETOOTH, 0);
              system("aplay /data/mode_sound/bluetooth_mode.wav");
		rkbt->btPlAudioCtl(true,m_defseq);
              break;
	case FUNC_KEY_NORMAL_MODE:
		log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. LED_MODE::BLUETOOTH");
		rksoftap->setWifi(false,m_defseq);
		set_mode(0);
		rkbt->btPlAudioCtl(false,m_defseq);
		m_gpio_ipc_handle->sendCommand(IPCInterface::Command::RESUME_WAKE_WORD_ENGINE);
		//rkbt->KittAiControl(true);//restart kitt_ai
		infoled->led_open(MODE_NORMAL, 0);
		ret = system("aplay /data/mode_sound/wakeup_mode.wav");
		if(ret != 0) {
			usleep(500*1000);
			ret = system("aplay /data/mode_sound/wakeup_mode.wav");
			log(Logger::INFO, "aplay try again");
		}
		usleep(200*1000);
		rkbt->DLNAControl(true,m_defseq);
		break;
         case FUNC_KEY_MIC_MUTE:
		 m_gpio_ipc_handle->sendCommand(IPCInterface::Command::PAUSE_WAKE_WORD_ENGINE);
		 infoled->led_open(MODE_MIC_MUTE, 0);
		break;
          case FUNC_KEY_MIC_UNMUTE:
		 m_gpio_ipc_handle->sendCommand(IPCInterface::Command::RESUME_WAKE_WORD_ENGINE);
		 infoled->led_open(MODE_NORMAL, 0);
		break;
        case FUNC_VOLUME_CHANGE:
              volume_step = alexa_volume_get_step();
              infoled->led_open(MODE_VOLUME, volume_step);
              log(Logger::INFO,"get volume changed event, current volume step is : " + std::to_string(volume_step));
              break;
          default:
              log(Logger::INFO,"evt_type: " + std::to_string(evt_type));
              break;
      }
      log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. loop next");
  }
  log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. led_open");
  infoled->led_open(MODE_OFF, 0);
  log(Logger::INFO, "GPIOWakeWordEngine::mainLoop. The end!");
}


} // namespace AlexaWakeWord
