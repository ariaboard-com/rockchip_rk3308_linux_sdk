#include <stdio.h>
#include <string.h>

#include "shell.h"

int exec(const char* cmd, char* result)
{
    FILE *stream = NULL;
    char buff[1024] = {0};

    if((stream = popen(cmd, "r")) != NULL) {
        while(fgets(buff,1024, stream)) {
            strcat(result, buff);
        }
    } else {
        printf("open cmdline failed!\n");
        return -1;
    }

    pclose(stream);

    return 0;
}

int isWpaCompleted()
{
    char ret_buff[MSG_BUFF_LEN] = {0};

    int result = exec("wpa_cli -iwlan0 status", ret_buff);
    if (result == 0) {
        if (strstr(ret_buff, "wpa_state=COMPLETED")) {
            return 0;
        }
    }

    return -1;
}

int isWifiConnected()
{
    char ret_buff[MSG_BUFF_LEN] = {0};
    exec("wpa_cli -iwlan0 status", ret_buff);

    if (strstr(ret_buff, "wpa_state=COMPLETED") && strstr(ret_buff, "ip_address=") && !strstr(ret_buff, "ip_address=127.0.0.1")) {
        return 0;
    }

    return -1;
}

int _kill(char* process)
{
    char reg[1];
    int i, pid;
    char cmd[32];
    int multi = 0;

    if (!process || strlen(process) <= 0) {
        return -1;
    }

    for (i=0; process[i]; i++) {
        if ((process[i]) < '0' || (process[i]) > '9') {
            reg[0] = process[i];
            multi = 1;
            break;
        }
    }

    if (multi) {
        char *s = strtok(process, reg);
        while(s) {
            pid = atoi(s);

            memset(cmd, 0, sizeof(cmd));
            snprintf(cmd, sizeof(cmd), "kill -9 %d", pid);
            system(cmd);

            s = strtok(NULL, " ");
        }
    } else {
        pid = atoi(process);

        memset(cmd, 0, sizeof(cmd));
        snprintf(cmd, sizeof(cmd), "kill -9 %d", pid);
        system(cmd);
    }

    return 0;
}
