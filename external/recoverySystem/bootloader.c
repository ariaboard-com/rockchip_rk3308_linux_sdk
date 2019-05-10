/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include "bootloader.h"
//#include "common.h"
//#include "mtdutils/mtdutils.h"
//#include "roots.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>

// ------------------------------------
// for misc partitions on block devices
// ------------------------------------

int get_bootloader_message(struct bootloader_message *out) {
    FILE* f = fopen(MISC_NODE, "rb");
    if (f == NULL) {
        printf("Can't open %s\n(%s)\n", MISC_NODE, strerror(errno));
        return -1;
    }
    struct bootloader_message temp;
    fseek(f, BOOTLOADER_MESSAGE_OFFSET_IN_MISC, SEEK_SET);

    int count = fread(&temp, sizeof(temp), 1, f);
    if (count != 1) {
        printf("Failed reading %s\n(%s)\n", MISC_NODE, strerror(errno));
        return -1;
    }
    if (fclose(f) != 0) {
        printf("Failed closing %s\n(%s)\n", MISC_NODE, strerror(errno));
        return -1;
    }
    memcpy(out, &temp, sizeof(temp));
    return 0;
}

int set_bootloader_message(const struct bootloader_message *in) {
    FILE* f = fopen(MISC_NODE, "wb");
    if (f == NULL) {
        printf("Can't open %s\n(%s)\n", MISC_NODE, strerror(errno));
        return -1;
    }
    fseek(f, BOOTLOADER_MESSAGE_OFFSET_IN_MISC, SEEK_SET);
    int count = fwrite(in, sizeof(*in), 1, f);
    if (count != 1) {
        printf("Failed writing %s\n(%s)\n", MISC_NODE, strerror(errno));
        return -1;
    }
    fsync(fileno(f));
    if (fclose(f) != 0) {
        printf("Failed closing %s\n(%s)\n", MISC_NODE, strerror(errno));
        return -1;
    }
    return 0;
}
