#include "wpa_manager.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static char WLAN_PID_NAME[] = "wpa_supplicant";

WPAManager *WPAManager::_instance = NULL;

WPAManager* WPAManager::getInstance()
{
    if(!_instance)
    {
        _instance = new WPAManager();
    }

    return _instance;
}

WPAManager::WPAManager()
{
}

int get_pid(char *processName)
{
    int len;
    char name[20] = {0};
    char cmdresult[256] = {0};
    char cmd[20] = {0};

    len = strlen(processName);
    strncpy(name, processName, len);
    name[len] = '\0';
    FILE *pFile = NULL;
    int pid = 0;

    sprintf(cmd, "pidof %s", name);
    pFile = popen(cmd, "r");
    if(pFile != NULL)
    {
        while(fgets(cmdresult, sizeof(cmdresult), pFile))
        {
            pid = atoi(cmdresult);
            break;
        }
    }
    pclose(pFile);

    return pid;
}

bool WPAManager::isNetConnected()
{
    int pid;

    pid = get_pid(WLAN_PID_NAME);

    return pid > 0;
}
