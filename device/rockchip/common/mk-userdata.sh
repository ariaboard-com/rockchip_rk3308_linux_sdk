#!/bin/bash

COMMON_DIR=$(cd `dirname $0`; pwd)
if [ -h $0 ]
then
        CMD=$(readlink $0)
        COMMON_DIR=$(dirname $CMD)
fi

if [ -n "$1" ]
then
        USERDATA_DIR="$1"
else
        exit 1
fi

if [ -n "$2" ]
then
        USERDATA_IMG="$2"
else
        exit 1
fi

if [ -n "$3" ]
then
        FS_TYPE="$3"
else
        exit 1
fi

if [ $FS_TYPE = ext2 ]
then
        $COMMON_DIR/mke2img.sh $USERDATA_DIR $USERDATA_IMG

elif [ $FS_TYPE = ext4 ]
then
        $COMMON_DIR/mke4img.sh $USERDATA_DIR $USERDATA_IMG

elif [ $FS_TYPE = fat ]
then
	SIZE=$(du -h -BM --max-depth=1 $USERDATA_DIR | awk '{print int($1)}')
	# echo "create image size=${SIZE}M"
	dd if=/dev/zero of=$USERDATA_IMG bs=1M count=$SIZE >/dev/null 2>&1
	mkfs.vfat $USERDATA_IMG >/dev/null 2>&1
	mcopy -i $USERDATA_IMG $USERDATA_DIR/* ::/ >/dev/null 2>&1
else
        echo "file system: $FS_TYPE not support."
        exit 1
fi
