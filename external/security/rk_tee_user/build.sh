#!/bin/bash
WORK_DIR_TOP=$(cd `dirname $0` ; pwd)

#./build.sh or ./build.sh 3232 to compile CA TA with 32 bits
#./build.sh 3264 to compile CA with 32 bits and TA with 64 bits
#./build.sh 6464 to compile CA TA with 64 bits
#./build.sh 6432 to compile CA with 64 bits and TA with 32 bits

AARCH64_TOOLCHAIN=$WORK_DIR_TOP/../../../prebuilts/gcc/linux-x86/aarch64/gcc-linaro-6.3.1-2017.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-
ARM32_TOOLCHAIN=$WORK_DIR_TOP/../../../prebuilts/gcc/linux-x86/arm/gcc-linaro-arm-linux-gnueabihf-4.9-2014.05_linux/bin/arm-linux-gnueabihf-

make TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-user_ta clean
export BUILD_CA=y
BUILD_CATA_BITS="$1"
if [ "$BUILD_CATA_BITS" == "3232" ]; then
	make CROSS_COMPILE_HOST=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_TA=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$ARM32_TOOLCHAIN \
	CROSS_COMPILE=$ARM32_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 \
	COMPILE_NS_USER=32 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "3264" ]; then
	make CROSS_COMPILE_HOST=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_TA=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE=$AARCH64_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm64 \
	COMPILE_NS_USER=32 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "6464" ]; then
	make CROSS_COMPILE_HOST=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_TA=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE=$AARCH64_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm64 \
	COMPILE_NS_USER=64 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "6432" ]; then
	make CROSS_COMPILE_HOST=$AARCH64_TOOLCHAIN \
	CROSS_COMPILE_TA=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$ARM32_TOOLCHAIN \
	CROSS_COMPILE=$ARM32_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 \
	COMPILE_NS_USER=64 \
	O=$WORK_DIR_TOP/out
fi

if [ "$BUILD_CATA_BITS" == "" ]; then
	make CROSS_COMPILE_HOST=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_TA=$ARM32_TOOLCHAIN \
	CROSS_COMPILE_user_ta=$ARM32_TOOLCHAIN \
	CROSS_COMPILE=$ARM32_TOOLCHAIN \
	TA_DEV_KIT_DIR=$WORK_DIR_TOP/export-ta_arm32 \
	COMPILE_NS_USER=32 \
	O=$WORK_DIR_TOP/out
fi
