#include "service_ipc.h"
int service_ipc_init(SIGNAL_HANDLER_FUNC sighandler) {
  char pathname[] = "/tmp";
  key_t key = ftok(pathname,0x03);
  if(key==-1)
  {
      perror("ftok error");
      return -1;
  }
  printf("key=%d\n",key) ;
  int shm_id=shmget(key, 128, IPC_CREAT|0600);
  if(shm_id == -1)
  {
      perror("shmget error");
      return -1;
  }

  struct sigaction act;
  act.sa_sigaction = sighandler;
  sigemptyset(&act.sa_mask);
  act.sa_flags = SA_SIGINFO;
  if(sigaction(SIGUSR1,&act,NULL) == -1){
      perror("sigaction error");
      return -1;
  }
  return shm_id;
}

void wait_device_mode_timeout_ms(pthread_mutex_t lock,pthread_cond_t cond,int microseconds)
{
    struct timeval tv;
    long long absmsec;
    struct timespec abstime;

    gettimeofday(&tv, NULL);
    absmsec = tv.tv_sec * 1000ll + tv.tv_usec / 1000ll;
    absmsec += microseconds;

    abstime.tv_sec = absmsec / 1000ll;
    abstime.tv_nsec = absmsec % 1000ll * 1000000ll;

    printf("#### public sleep mode ####");
    pthread_mutex_lock(&lock);
    pthread_cond_timedwait(&cond, &lock, &abstime);
    pthread_mutex_unlock(&lock);
    printf("#### return sleep mode succeed ####");
}

void signal_device(pthread_mutex_t lock,pthread_cond_t cond) {
    pthread_mutex_lock(&lock);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&lock);
}

