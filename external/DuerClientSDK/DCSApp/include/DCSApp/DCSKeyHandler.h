#ifndef __DCS_KEY_HANDLER__
#define __DCS_KEY_HANDLER__

#include <memory>
#include <thread>
#include "DCSApp/get_event.h"
#include "DCSApp/DeviceIoWrapper.h"
#include "DCSApp/SoundController.h"
#include "DCSApp/Configuration.h"
#include "DCSApp/DuerLinkWrapper.h"
#include <LoggerUtils/DcsSdkLogger.h>
#include <DeviceIo/DeviceIo.h>
#include "ApplicationManager.h"

namespace duerOSDcsApp {
namespace application {

class DCSKeyHandler
{
 public:
  DCSKeyHandler(DeviceIoWrapper* devIoWrapper);
  ~DCSKeyHandler();

  static std::unique_ptr<DCSKeyHandler> create(DeviceIoWrapper* devIoWrapper);

  void keyEventLoop(void);

 private:
  DeviceIoWrapper* m_devIoWrapper;
  std::atomic_bool m_isRuning;
  std::unique_ptr<std::thread> m_thread;
};

}  // namespace application
}  // namespace duerOSDcsApp

#endif
