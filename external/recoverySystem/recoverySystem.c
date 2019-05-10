/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * RecoverySystem contains methods for interacting with the Android
 * recovery system (the separate partition that can be used to install
 * system updates, wipe user data, etc.)
 */

#include <linux/reboot.h>
#include <stdio.h>
#include <string.h>
#include <sys/reboot.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "bootloader.h"

/**
 * Reboot into the recovery system with the supplied argument.
 * @param arg to pass to the recovery utility.
 */
static void bootCommand(struct bootloader_message *msg){
    set_bootloader_message(msg);
	printf("update: reboot!\n");
	//reboot(LINUX_REBOOT_MAGIC1, LINUX_REBOOT_MAGIC2,
	//       LINUX_REBOOT_CMD_RESTART2, "recovery");
	sync();
	reboot(RB_AUTOBOOT);
	return;
}


 /**
 * Reboots the device in order to install the given update
 * package.
 * Requires the {@link android.Manifest.permission#REBOOT} permission.
 *
 * @param packageFile  the update package to install.  Must be on
 * a partition mountable by recovery.  (The set of partitions
 * known to recovery may vary from device to device.  Generally,
 * /cache and /data are safe.)
 */
void installPackage(char *update_file){
    struct bootloader_message msg;
    memset(&msg, 0, sizeof(msg));
    strlcpy(msg.command, "boot-recovery", sizeof(msg.command));
    strlcpy(msg.recovery, "recovery\n--update_package=", sizeof(msg.recovery));
    strlcat(msg.recovery, update_file, sizeof(msg.recovery));
    strlcat(msg.recovery, "\n", sizeof(msg.recovery));
    strlcpy(msg.systemFlag, "false", sizeof(msg.systemFlag));

    bootCommand(&msg);
}


int lastUpdateStatus(char *result){
    struct bootloader_message msg;
    memset(&msg, 0, sizeof(msg));
    get_bootloader_message(&msg);
    printf("get message from recovery is %s.\n", msg.systemFlag);
    if(strncmp(msg.systemFlag, "false", 5) == 0){
        printf("lastUpdateStatus failed.\n");
        return -1;
    }else if(strncmp(msg.systemFlag, "true", 4) == 0){
        printf("lastUpdateStatus Success.\n");
        return 0;
    }else {
        printf("lastUpdateStatus Success.\n");
        strcpy(result, msg.systemFlag);
        return 1;
    }
}

void fastoryDataReset(){
    struct bootloader_message msg;
    memset(&msg, 0, sizeof(msg));
    strlcpy(msg.command, "boot-recovery", sizeof(msg.command));
    strlcpy(msg.recovery, "recovery\n--wipe_data", sizeof(msg.recovery));
    strlcat(msg.recovery, "\n", sizeof(msg.recovery));
    strlcpy(msg.systemFlag, "false", sizeof(msg.systemFlag));

    bootCommand(&msg);
}
