#!/bin/bash

COMMON_DIR=$(cd `dirname $0`; pwd)
if [ -h $0 ]
then
        CMD=$(readlink $0)
        COMMON_DIR=$(dirname $CMD)
fi
cd $COMMON_DIR
cd ../../..
TOP_DIR=$(pwd)

BOARD_CONFIG=$1
source $BOARD_CONFIG
echo "recovery config: $CFG_RECOVERY"
if [ $RK_ARCH == arm64 ];then
KERNEL_IMAGE=$TOP_DIR/kernel/arch/arm64/boot/Image
    if [ $RK_TARGET_PRODUCT == rk3308 ];then
    KERNEL_IMAGE=$TOP_DIR/kernel/arch/arm64/boot/Image.lz4
    fi
elif [ $RK_ARCH == arm ];then
KERNEL_IMAGE=$TOP_DIR/kernel/arch/arm/boot/zImage
fi
KERNEL_DTB=$TOP_DIR/kernel/resource.img

# build kernel
if [ -f $KERNEL_IMAGE ]
then
	echo "found kernel image"
else
	echo "kernel image doesn't exist, now build kernel image"
	$TOP_DIR/build.sh kernel
	if [ $? -eq 0 ]; then
		echo "build kernel done"
	else
		exit 1
	fi
fi

source $TOP_DIR/buildroot/build/envsetup.sh $RK_CFG_RECOVERY
RAMDISK_IMAGE=$TOP_DIR/buildroot/output/$RK_CFG_RECOVERY/images/rootfs.cpio.gz
RECOVERY_IMAGE=$TOP_DIR/buildroot/output/$RK_CFG_RECOVERY/images/recovery.img

# build recovery
echo "====Start build recovery===="
make
if [ $? -eq 0 ]; then
    echo "====Build recovery ok!===="
else
    echo "====Build recovery failed!===="
    exit 1
fi

echo -n "pack recovery image..."
$TOP_DIR/kernel/scripts/mkbootimg --kernel $KERNEL_IMAGE --ramdisk $RAMDISK_IMAGE --second $KERNEL_DTB -o $RECOVERY_IMAGE
echo "done."
