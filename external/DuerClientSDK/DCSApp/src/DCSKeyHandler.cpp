#include <string>
#include "DeviceIo/Memory.h"
#include "DCSApp/DCSKeyHandler.h"
#include <unistd.h>

namespace duerOSDcsApp {
namespace application {

using namespace framework;

DCSKeyHandler::DCSKeyHandler(DeviceIoWrapper* devIoWrapper):
    m_devIoWrapper(devIoWrapper)
{
  m_isRuning = true;
  start_event_thread();

  m_thread = make_unique<std::thread>(&DCSKeyHandler::keyEventLoop, this);
}

DCSKeyHandler::~DCSKeyHandler()
{
  m_isRuning = false;
}

std::unique_ptr<DCSKeyHandler> DCSKeyHandler::create(DeviceIoWrapper* devIoWrapper)
{
  if(!devIoWrapper) {
    return nullptr;
  }
  return std::unique_ptr<DCSKeyHandler>(new DCSKeyHandler(devIoWrapper));
}

void DCSKeyHandler::keyEventLoop()
{
  while(m_isRuning) {
    printf("DCSKeyHandler::keyEventLoop event_process before.\n");
    int event_typ = event_process();
    printf("DCSKeyHandler::keyEventLoop event_process end. event_typ:%d.\n", event_typ);
    switch(event_typ) {
      case FUNC_KEY_WAKEUP:
        {
          m_devIoWrapper->callback(DeviceInput::KEY_WAKE_UP, NULL, 0);
        }
        break;
      case FUNC_KEY_WIFI_MODE:
        {
          // m_infoled->led_open(MODE_WIFI_CONNECT, 0);
          m_devIoWrapper->callback(DeviceInput::KEY_ENTER_AP, NULL, 0);
        }
        break;
      case FUNC_KEY_NORMAL_MODE:
        {
          // m_infoled->led_open(MODE_NORMAL, 0);
          m_devIoWrapper->callback(DeviceInput::KEY_EXIT_AP, NULL, 0);
        }
        break;
      case FUNC_VOLUME_CHANGE:
        {
          static int volume_step = alexa_volume_get_step();
          m_devIoWrapper->callback(DeviceInput::VOLUME_CHANGED, &volume_step, 0);
        }
        break;
      case FUNC_KEY_VOL_UP:
        {
          static int volume_step = 10;//step:8,1~12
          m_devIoWrapper->callback(DeviceInput::KEY_VOLUME_UP, &volume_step, 0);
        }
        break;
      case FUNC_KEY_VOL_DOWN:
        {
          static int volume_step = 10;//step:8,1~12
          m_devIoWrapper->callback(DeviceInput::KEY_VOLUME_DOWN, &volume_step, 0);
        }
        break;
      case FUNC_KEY_PLAY_PAUSE:
       {
            m_devIoWrapper->callback(DeviceInput::KEY_PLAY_PAUSE, NULL, 0);
       }
       break;
      case FUNC_KEY_MIC_MUTE:
        {
            printf("FUNC_KEY_MIC_MUTE\n");
            m_devIoWrapper->callback(DeviceInput::KEY_MIC_MUTE, NULL, 0);
        }
        break;
      default:break;
    }
  }

  printf("DCSKeyHandler key event thread dead!\n");
}

} // application
} // duerOSDcsApp
