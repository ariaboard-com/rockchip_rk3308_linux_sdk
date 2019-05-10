#include <string.h>  
#include <errno.h>  
#include <string>
#include <time.h>
#include "service_ipc.h"
char msg[128];
int shm_id;

void sighandler(int signum, siginfo_t *info,void *ctx)
{
  char *p_data = (char *)shmat(shm_id,NULL,0);
  char str[128];
  printf("mem start\n");
  memcpy(str,p_data,128);
  shmdt(p_data);
  printf("shmdt over\n");
  printf("message:%s\n",str);
}

int main(int argc, char** argv)  
{  
    shm_id = service_ipc_init(sighandler);
    while(1) {
        sleep(2);
    }
}



