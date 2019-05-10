#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>  
#include <dirent.h>  
#include <sys/ipc.h>
#include <sys/shm.h>
#define BUF_SIZE 1024 
void getPidByName(pid_t *pid, char *task_name);

int get_ipc_share_memory_id() {
  //共享内存
  int shm_id;
  char pathname[] = "/tmp";
  key_t key = ftok(pathname,0x03);
  if(key==-1)
  {
      perror("ftok error");
      return -1;
  }
  printf("key=%d\n",key) ;
  shm_id=shmget(key, 0, IPC_CREAT|0600); 
  if(shm_id==-1)
  {
      perror("shmget error");
      return -1;
  } else {
      return shm_id;
  }
}

int main(int argc, char** argv)  
{  
  if (argc < 2){
    printf("Usages: %s pid)\n",argv[0]);
    printf("example: %s ipc_service\n",argv[0]);
    return 1;
  }
  
  int pid = 0;
  char *p_data;
  int shm_id;
  getPidByName(&pid,argv[1]);
  if(pid == 0){
      printf("cannot find ipc_service pid!\n");
      return -1;
  }

  shm_id = get_ipc_share_memory_id(); 
  if(shm_id < 0) {
      return -1;
  }
  
  while(1) {
      p_data=(char *)shmat(shm_id,NULL,0);
      const char* ipc_message = "hello ipc!";
      memcpy(p_data,ipc_message,strlen(ipc_message));
      printf("send message:%s",p_data);
      shmdt(p_data);
      union sigval mysigval;
      if(sigqueue(pid,SIGUSR1,mysigval)==-1){
          printf("send error:%d\n",errno);
      }
      sleep(1);
  }
}  

void getPidByName(pid_t *pid, char *task_name)  
 {  
     DIR *dir;  
     struct dirent *ptr;  
     FILE *fp;  
     char filepath[50];  
     char cur_task_name[50];  
     char buf[BUF_SIZE];  
   
     dir = opendir("/proc");   
     if (NULL != dir)  
     {  
         while ((ptr = readdir(dir)) != NULL) 
         {  
              
             if ((strcmp(ptr->d_name, ".") == 0) || (strcmp(ptr->d_name, "..") == 0))  
                 continue;  
             if (DT_DIR != ptr->d_type)  
                 continue;  
              
             sprintf(filepath, "/proc/%s/status", ptr->d_name);
             fp = fopen(filepath, "r");  
             if (NULL != fp)  
             {  
                 if( fgets(buf, BUF_SIZE-1, fp)== NULL ){  
                     fclose(fp);  
                     continue;  
                 }  
                 sscanf(buf, "%*s %s", cur_task_name);  
           
                
                 if (!strcmp(task_name, cur_task_name)){  
                     sscanf(ptr->d_name, "%d", pid);  
                 }  
                 fclose(fp);  
             }  
         }  
         closedir(dir);  
     }  
 }  
