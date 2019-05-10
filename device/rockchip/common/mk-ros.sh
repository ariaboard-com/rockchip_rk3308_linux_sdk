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
CONF=$TOP_DIR/device/rockchip/$RK_TARGET_PRODUCT/yocto/build/conf

cd $TOP_DIR/kernel && git branch master 1>/dev/null 2>/dev/null && cd -
cd $TOP_DIR/yocto && . ./oe-init-build-env $TOP_DIR/yocto/build && cp $CONF/* $TOP_DIR/yocto/build/conf/ && bitbake core-image-ros-roscore -c do_image_ext4 && cd -
if [ $? -ne 0 ]; then
	exit 1
fi
rm -f $TOP_DIR/yocto/rootfs.img
ln -s $TOP_DIR/yocto/build/tmp/work/$YOCTO_MACHINE-poky-linux/core-image-ros-roscore/1.0-r0/deploy-core-image-ros-roscore-image-complete/core-image-ros-roscore-$YOCTO_MACHINE.ext4 $TOP_DIR/yocto/rootfs.img

