#!/bin/bash

SRC=$1
DST=$2
SIZE=`du -sk --apparent-size $SRC | cut --fields=1`
inode_counti=`find $SRC | wc -l`
inode_counti=$[inode_counti+512]
EXTRA_SIZE=$[inode_counti*4]
SIZE=$[SIZE+EXTRA_SIZE]

if [ $SIZE -lt "65536" ]; then
    SIZE="65536"
fi

echo "genext2fs -b $SIZE -N $inode_counti -d $SRC $DST"
genext2fs -b $SIZE -N $inode_counti -d $SRC $DST
tune2fs -C 1 -j -O extents,large_file,huge_file $DST
resize2fs -M $DST
e2fsck -fy $DST
