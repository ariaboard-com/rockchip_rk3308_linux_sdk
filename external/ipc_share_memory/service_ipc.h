#ifndef _SERVICE_IPC_H
#define _SERVICE_IPC_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include <thread>
#include <pthread.h>
#include <sys/time.h>

typedef void (*SIGNAL_HANDLER_FUNC)(int signo, siginfo_t *info,void *ctx);
int service_ipc_init(SIGNAL_HANDLER_FUNC sighandler); 
void signal_device(pthread_mutex_t lock,pthread_cond_t cond); 
void wait_device_mode_timeout_ms(pthread_mutex_t lock,pthread_cond_t cond,int microseconds);
#endif

