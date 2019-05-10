#!/bin/bash

COMMON_DIR=$(cd `dirname $0`; pwd)
if [ -h $0 ]
then
        CMD=$(readlink $0)
        COMMON_DIR=$(dirname $CMD)
fi

if [ -n "$1" ]
then
	OEM_DIR="$1"
else
	exit 1
fi

if [ -n "$2" ]
then
	OEM_IMG="$2"
else
	exit 1
fi

if [ -n "$3" ]
then
        FS_TYPE="$3"
else
	exit 1
fi
#echo "OEM_IMG:$OEM_IMG"
#echo "FS_TYPE:$FS_TYPE"
if [ $FS_TYPE = ext2 ]
then
	$COMMON_DIR/mke2img.sh $OEM_DIR $OEM_IMG

elif [ $FS_TYPE = squashfs ]
then
	mksquashfs $OEM_DIR $OEM_IMG -noappend -comp gzip
else
	echo "file system: $FS_TYPE not support."
	exit 1
fi
