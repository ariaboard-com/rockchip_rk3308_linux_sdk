#!/bin/bash

# Target arch
export RK_ARCH=arm64
# Uboot defconfig
export RK_UBOOT_DEFCONFIG=evb-rk3308-novotech
# Kernel defconfig
export RK_KERNEL_DEFCONFIG=rk3308-novotech-asclepius_linux_defconfig
# Kernel dts
export RK_KERNEL_DTS=rk3308-novotech-asclepius-emmc
# boot image type
export RK_BOOT_IMG=zboot.img
# parameter for GPT table
export RK_PARAMETER=parameter-64bit-emmc-asclepius2.txt
# Buildroot config
export RK_CFG_BUILDROOT=asclepius_rk3308_release
# Recovery config
export RK_CFG_RECOVERY=asclepius_rk3308_recovery
# Pcba config
export RK_CFG_PCBA=asclepius_rk3308_pcba
# Build jobs
export RK_JOBS=4
# target chip
export RK_TARGET_PRODUCT=rk3308
# Set rootfs type, including ext2(ext4) squashfs
export RK_ROOTFS_TYPE=ext2
# rootfs image path
export RK_ROOTFS_IMG=buildroot/output/$RK_CFG_BUILDROOT/images/rootfs.$RK_ROOTFS_TYPE
# Set oem partition type, including ext2 squashfs
export RK_OEM_FS_TYPE=ext4
# Set userdata partition type, including ext2, fat
export RK_USERDATA_FS_TYPE=ext4
# Set flash type. support <emmc, nand, spi_nand, spi_nor>
export RK_STORAGE_TYPE=emmc
export RK_OEM_DIR=oem
#userdata config
export RK_USERDATA_DIR=userdata_asclepius
MIC_NUM=6
