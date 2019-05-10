#ifndef __SHELL_H__
#define __SHELL_H__

#define MSG_BUFF_LEN 8888

int exec(const char* cmd, char* result);
int isWpaCompleted();
int isWifiConnected();
int _kill(char* process);

#endif
