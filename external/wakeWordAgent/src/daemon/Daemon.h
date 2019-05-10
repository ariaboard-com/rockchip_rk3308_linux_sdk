#ifndef __RK_WAKEWORD_AGENT_H__
#define  __RK_WAKEWORD_AGENT_H__

#include <stdio.h>
#ifdef __cplusplus
extern "C" {
#endif

static int mode = 0;

int init_daemon();

int get_pid_cmd(char* cmd, char* res);

int get_javaclient_id(char* res);

int kill_pid(char* pid);

int kill_javaclient();

int is_javaclient_running();

int is_wifi_completed();

int start_javaclient();

int restart_javaclient();

void set_mode(int m);
#ifdef __cplusplus
}
#endif
#endif
