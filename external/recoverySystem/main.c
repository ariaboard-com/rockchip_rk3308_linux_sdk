//#include <direct.h>
#include <linux/reboot.h>
//#include <io.h>
#include <stdio.h>
#include <string.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "recoverySystem.h"
#include "update_recv/update_recv.h"

#if 0
#define RECOVERY_PATH "/tmp/recovery"
#define LOG_FILE_PATH "/tmp/recovery/log"
#define COMMAND_FILE_PATH "/tmp/recovery/command"
#else
#define RECOVERY_PATH "/userdata/recovery"
#define LOG_FILE_PATH "/userdata/recovery/log"
#define COMMAND_FILE_PATH "/userdata/recovery/command"
#endif
#define SD_UPDATE_FILE "/mnt/sdcard/update.img"
#define USB_UPDATE_FILE "/mnt/udisk/update.img"
#define DATA_UPDATE_FILE "/userdata/update.img"


int rebootUpdate(char *path){

    if(path){
        printf("find %s\n", path);
        installPackage(path);
    }

    if(access(DATA_UPDATE_FILE, F_OK) == -1){
        printf("%s does not exist! try to use %s to update\n",
            DATA_UPDATE_FILE, SD_UPDATE_FILE);
        if(access(SD_UPDATE_FILE, F_OK) == -1){
            printf("%s does not exist!\n", SD_UPDATE_FILE);
            printf("try to %s.\n", USB_UPDATE_FILE);
            if(access(USB_UPDATE_FILE, F_OK) == -1){
                printf("%s does not exist!\n", USB_UPDATE_FILE);
                return -1;
            }
            printf("find %s\n", USB_UPDATE_FILE);
            installPackage(USB_UPDATE_FILE);
            return 0;
        }
        printf("find %s\n", SD_UPDATE_FILE);
        installPackage(SD_UPDATE_FILE);
        return 0;
    }

    printf("find %s\n", DATA_UPDATE_FILE);
    installPackage(DATA_UPDATE_FILE);
    return 0;
}

int main(int argc, char** argv){
    char* partition_name = "recovery";

    printf("update: Rockchip Update Tool\n");
    if(argc == 1) {
        fastoryDataReset();
    } else if(argc == 2){
        if(!strcmp(argv[1], "ota") || !strcmp(argv[1], "update"))
            rebootUpdate(0);
        else if(!strcmp(argv[1], "factory") || !strcmp(argv[1], "reset"))
            fastoryDataReset();
        else  return -1;

        return 0;

    } else if(argc == 3){
        if(!strcmp(argv[1], "ota") || !strcmp(argv[1], "update"))
            if(argv[2]) {
                int ret;
                ret = WriteFwData(argv[2], partition_name);
                if (ret < 0) {
                    printf(" Update partition %s fail \n", partition_name);
                    return -1;
                } else {
                    if (!CheckFwData(argv[2], partition_name)){
                        printf(" Check partition %s fail \n", partition_name);
                        return -1;
                    }
                }

                return rebootUpdate(argv[2]);
            }
    }

    return -1;
}
