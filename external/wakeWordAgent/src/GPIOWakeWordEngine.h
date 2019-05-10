
#ifndef GPIO_WAKE_WORD_ENGINE_H
#define GPIO_WAKE_WORD_ENGINE_H

#include "WakeWordEngine.h"
#include <thread>
#include <memory>
#include <atomic>
#include "GPIOIPCSocket.h"
namespace AlexaWakeWord {

// A specialization of a WakeWordEngine, where a trigger comes from GPIO
class GPIOWakeWordEngine: public WakeWordEngine {
public:


  GPIOWakeWordEngine(WakeWordDetectedInterface* interface);
  ~GPIOWakeWordEngine() = default;
  void pause();
  void resume();
  void setVolume(int vol); 
  void setInitState(); 
  void startAmazonRecord();
  void stopAmazonRecord();
  void startAmazonPlay();
  void setAuthorizedState(bool state);


private:

  enum class BOOT_STATE {
      STATE_BOOTED = 1,
      STATE_NETWORK_STARTED,
      STATE_JAVACLIENT_STARTED,
      STATE_SENSORY_STARTED,
      STATE_SYSTEM_OK,
  };
  void init();
  void mainLoop();
  int my_exec(const char *cmdstring);
  bool gpioReady ;
  bool kittAiReady ;
  bool mJavaIsReady;
  // GPIO is monitored in this thread
  std::unique_ptr<std::thread> m_thread;
  std::atomic<bool> m_isRunning;
  BOOT_STATE m_curBootState;
  char m_defseq[128];
  int m_defvol; 
  std::unique_ptr<GPIOIPCSocket> m_gpio_ipc_handle;
};

}
#endif // GPIO_WAKE_WORD_ENGINE_H
