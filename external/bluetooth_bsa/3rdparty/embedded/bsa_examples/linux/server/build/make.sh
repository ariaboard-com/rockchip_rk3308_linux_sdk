#!/bin/sh

#export LD_LIBRARY_PATH=/home/wtong/Desktop/Projects/WE_HUAWEI_PEGATRON_ARM_BSA/ToolChain/gcc-linaro-5.4.1-2017.05-x86_64_arm-linux-gnueabihf/lib:$LD_LIBRARY_PATH
ARMGCC=/home/wtong/Desktop/Projects/WE_ROCKCHIP_ARM_BSA/ToolChain/rk3308-arm64-glibc-2018.03-toolschain/bin/aarch64-linux-gcc
ARMGCCLIBPATH=/home/wtong/Desktop/Projects/WE_ROCKCHIP_ARM_BSA/ToolChain/rk3308-arm64-glibc-2018.03-toolschain/lib

make -f Makefile.all CPU=arm ARMGCC=$ARMGCC ARMCCFLAGS="$ARMCCFLAGS" ARMGCCLIBPATH=$ARMGCCLIBPATH ENABLE_BTHID=TRUE clean
make -f Makefile.all CPU=arm ARMGCC=$ARMGCC  ARMGCCLIBPATH=$ARMGCCLIBPATH ENABLE_BTHID=TRUE all
make -f Makefile.all CPU=x86 clean
make -f Makefile.all CPU=x86 all
make -f Makefile.all CPU=x86_64 clean
make -f Makefile.all CPU=x86_64 all
make -f Makefile.all binarydelivery
