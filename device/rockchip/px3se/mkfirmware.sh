#!/bin/bash

DEVICE_DIR=$(cd `dirname $0`; pwd)
if [ -h $0 ]
then
        CMD=$(readlink $0)
        DEVICE_DIR=$(dirname $CMD)
fi
cd $DEVICE_DIR
cd ../../..
TOP_DIR=$(pwd)

source $TOP_DIR/device/rockchip/.BoardConfig.mk
ROCKDEV=$TOP_DIR/rockdev
ROOTFS_PATH=$(pwd)/rootfs
PARAMETER_DEBIAN=$TOP_DIR/$DEVICE_DIR/parameter-debian.txt
PARAMETER_BUILDROOT=$TOP_DIR/$DEVICE_DIR/parameter-buildroot.txt
OEM_DIR=$TOP_DIR/device/rockchip/oem/$RK_OEM_DIR
USER_DATA_DIR=$TOP_DIR/device/rockchip/userdata/$RK_USERDATA_DIR
MISC_IMG=$TOP_DIR/device/rockchip/rockimg/wipe_all-misc.img
BUILDROOT_IMG=$TOP_DIR/buildroot/output/$RK_CFG_BUILDROOT/images/rootfs.$RK_ROOTFS_TYPE
RECOVERY_IMG=$TOP_DIR/buildroot/output/$RK_CFG_RECOVERY/images/recovery.img
TRUST_IMG=$TOP_DIR/u-boot/trust.img
UBOOT_IMG=$TOP_DIR/u-boot/uboot.img
BOOT_IMG=$TOP_DIR/kernel/$RK_BOOT_IMG
LOADER=$TOP_DIR/u-boot/*_loader_v*.bin
#SPINOR_LOADER=$TOP_DIR/u-boot/*_loader_spinor_v*.bin
MKOEM=$TOP_DIR/device/rockchip/common/mk-oem.sh
MKUSERDATA=$TOP_DIR/device/rockchip/common/mk-userdata.sh
ROOTFS_TYPE=
mkdir -p $ROCKDEV
if [ ! -n "$1" ]
then
echo "build buildroot type rootfs as default"
ROOTFS_TYPE=buildroot
else
ROOTFS_TYPE="$1"
fi

if [ $ROOTFS_TYPE = debian ]
then
	echo -n "create rootfs.img and parameter.txt..."
	ln -s -f $ROOTFS_PATH/linaro-rootfs.img $ROCKDEV/rootfs.img
	ln -s -f $PARAMETER_DEBIAN $ROCKDEV/parameter.txt
	echo "done."
else
	echo -n "create rootfs.img and parameter.txt..."
	ln -s -f $BUILDROOT_IMG $ROCKDEV/rootfs.img
	ln -s -f $PARAMETER_BUILDROOT $ROCKDEV/parameter.txt
	echo "done"
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
	$MKOEM $OEM_DIR $ROCKDEV/oem.img ext2
	echo "done."
else
	echo -e "\e[31m error: create oem image fail! \e[0m"
fi

if [ -d $USER_DATA_DIR ]
then
	echo -n "create userdata.img..."
	$MKUSERDATA $USER_DATA_DIR $ROCKDEV/userdata.img fat
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
