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
source $TOP_DIR/buildroot/build/envsetup.sh $RK_CFG_BUILDROOT

echo "Copy kernel modules..."
cp -r $TOP_DIR/kernel/deploy/lib $TOP_DIR/buildroot/board/rockchip/rk3308/fs-overlay/

make
if [ $? -ne 0 ]; then
    exit 1
fi
echo "pack buildroot image at: $TOP_DIR/buildroot/output/$RK_CFG_BUILDROOT/images/rootfs.$RK_ROOTFS_TYPE"
