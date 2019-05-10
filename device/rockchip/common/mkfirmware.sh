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

source $TOP_DIR/device/rockchip/.BoardConfig.mk
ROCKDEV=$TOP_DIR/rockdev
PARAMETER=$TOP_DIR/device/rockchip/$RK_TARGET_PRODUCT/$RK_PARAMETER
OEM_DIR=$TOP_DIR/device/rockchip/oem/$RK_OEM_DIR
USER_DATA_DIR=$TOP_DIR/device/rockchip/userdata/$RK_USERDATA_DIR
MISC_IMG=$TOP_DIR/device/rockchip/rockimg/wipe_all-misc.img
ROOTFS_IMG=$TOP_DIR/$RK_ROOTFS_IMG
RECOVERY_IMG=$TOP_DIR/buildroot/output/$RK_CFG_RECOVERY/images/recovery.img
TRUST_IMG=$TOP_DIR/u-boot/trust.img
UBOOT_IMG=$TOP_DIR/u-boot/uboot.img
BOOT_IMG=$TOP_DIR/kernel/$RK_BOOT_IMG
LOADER=$TOP_DIR/u-boot/*_loader_v*.bin
#SPINOR_LOADER=$TOP_DIR/u-boot/*_loader_spinor_v*.bin
MKOEM=$TOP_DIR/device/rockchip/common/mk-oem.sh
MKUSERDATA=$TOP_DIR/device/rockchip/common/mk-userdata.sh
mkdir -p $ROCKDEV

if [ -f $ROOTFS_IMG ]
then
	echo -n "create rootfs.img..."
	ln -s -f $ROOTFS_IMG $ROCKDEV/rootfs.img
	echo "done."
else
	echo -e "\e[31m error: $ROOTFS_IMG not found! \e[0m"
fi

if [ -f $PARAMETER ]
then
	echo -n "create parameter..."
	ln -s -f $PARAMETER $ROCKDEV/parameter.txt
	echo "done."
else
	echo -e "\e[31m error: $PARAMETER not found! \e[0m"
fi

if [ -f $RECOVERY_IMG ]
then
	echo -n "create recovery.img..."
	ln -s -f $RECOVERY_IMG $ROCKDEV/recovery.img
	echo "done."
else
	echo -e "\e[31m error: $RECOVERY_IMG not found! \e[0m"
fi

if [ -f $MISC_IMG ]
then
	echo -n "create misc.img..."
	ln -s -f $MISC_IMG $ROCKDEV/misc.img
	echo "done."
else
	echo -e "\e[31m error: $MISC_IMG not found! \e[0m"
fi

if [ -d $OEM_DIR ]
then
	echo -n "create oem.img..."
	$MKOEM $OEM_DIR $ROCKDEV/oem.img $RK_OEM_FS_TYPE
	echo "done."
else
	echo -e "\e[31m error: create oem image fail! \e[0m"
fi

if [ -d $USER_DATA_DIR ]
then
	echo -n "create userdata.img..."
	$MKUSERDATA $USER_DATA_DIR $ROCKDEV/userdata.img $RK_USERDATA_FS_TYPE
	echo "done."
else
	echo -e "\e[31m error: $USER_DATA_DIR not found! \e[0m"
fi

if [ -f $UBOOT_IMG ]
then
        echo -n "create uboot.img..."
        ln -s -f $UBOOT_IMG $ROCKDEV/uboot.img
        echo "done."
else
        echo -e "\e[31m error: $UBOOT_IMG not found! \e[0m"
fi

if [ -f $TRUST_IMG ]
then
        echo -n "create trust.img..."
        ln -s -f $TRUST_IMG $ROCKDEV/trust.img
        echo "done."
else
        echo -e "\e[31m error: $TRUST_IMG not found! \e[0m"
fi

if [ -f $LOADER ]
then
        echo -n "create loader..."
        ln -s -f $LOADER $ROCKDEV/MiniLoaderAll.bin
        echo "done."
else
	echo -e "\e[31m error: $LOADER not found,or there are multiple loaders! \e[0m"
	rm $LOADER
fi

#if [ -f $SPINOR_LOADER ]
#then
#        echo -n "create spinor loader..."
#        ln -s -f $SPINOR_LOADER $ROCKDEV/MiniLoaderAll_SpiNor.bin
#        echo "done."
#else
#	rm $SPINOR_LOADER_PATH 2>/dev/null
#fi

if [ -f $BOOT_PATH ]
then
	echo -n "create boot.img..."
	ln -s -f $BOOT_IMG $ROCKDEV/boot.img
	echo "done."
else
	echo -e "\e[31m error: $BOOT_IMG not found! \e[0m"
fi

echo -e "\e[36m Image: image in rockdev is ready \e[0m"
