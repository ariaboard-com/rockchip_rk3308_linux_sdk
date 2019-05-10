#!/bin/sh

# check args
if [ $# -ne 1 ]; then
    echo "Usage: $0 <build configure file>"
    exit 1
fi

if [ ! -f $1 ]; then
    echo "Invalid filename $1"
    exit 2
fi

# load compile configure
. $1

# global
THREAD_COUNT=`cat /proc/cpuinfo| grep "processor"| wc -l`

WORK_PATH="${PWD}"
BUILD_PATH="${WORK_PATH}/${Platform}_buildout"
OUTPUT_PATH="${BUILD_PATH}/output"
THIRD_PATH="${WORK_PATH}/third"

compile_app() {
    cd ${BUILD_PATH}

    cmake .. \
        -DCMAKE_INSTALL_PREFIX=${OUTPUT_PATH} \
        -DCMAKE_INSTALL_RPATH="'\$ORIGIN/lib'" \
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} \
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} \
        -DCMAKE_C_FLAGS="${CMAKE_C_FLAGS}" \
        -DCMAKE_CXX_FLAGS="${CMAKE_CXX_FLAGS}" \
        -DCMAKE_C_COMPILE_OBJECT=${CMAKE_C_COMPILE_OBJECT} \
        \
        -DPlatform=${Platform} \
        \
        -DPORTAUDIO_LIB_PATH=${THIRD_PATH}/lib/libportaudio.a \
        -DPORTAUDIO_INCLUDE_DIR=${THIRD_PATH}/include \
        -DFFMPEG_LIB_PATH=${THIRD_PATH}/lib \
        -DFFMPEG_INCLUDE_DIR=${THIRD_PATH}/include \
        -DCURL_LIBRARY=${THIRD_PATH}/lib/libcurl.so \
        -DCURL_INCLUDE_DIR=${THIRD_PATH}/include \
        -DSQLITE_LDFLAGS=${THIRD_PATH}/lib/libsqlite3.so \
        -DSQLITE_INCLUDE_DIRS=${THIRD_PATH}/include \
        -DKITTAI_KEY_WORD_DETECTOR=${KITTAI_KEY_WORD_DETECTOR} \
        -DBUILD_TTS_SDK=${BUILD_TTS_SDK} \
        -DBUILD_CRAB_SDK=${BUILD_CRAB_SDK} \
        -DPORTAUDIO=ON \
        -DGSTREAMER_MEDIA_PLAYER=ON \
        -DBUILD_DOC=OFF \
        -DBUILD_TEST=OFF \
        -DBUILD_ONE_LIB=ON \
        -DOUTPUT_FOR_THIRD=ON \
        -DDEBUG_FLAG=${DEBUG_FLAG} \
        -DDUERLINK_V2=${DUERLINK_V2}
 
    make -j${THREAD_COUNT}
    make install
}

generate_output() {
    cp -r ${WORK_PATH}/resources  ${OUTPUT_PATH}
    cp -r ${WORK_PATH}/sdk/log.config  ${OUTPUT_PATH}
    cp -r ${WORK_PATH}/sdk/duerlinklog.config  ${OUTPUT_PATH}
    cp -r ${WORK_PATH}/sdk/lib/*  ${OUTPUT_PATH}/lib
    cp -r ${WORK_PATH}/third/lib/* ${OUTPUT_PATH}/lib
    ${STRIP} -s ${OUTPUT_PATH}/lib/*
    ${STRIP} -s ${OUTPUT_PATH}/duer_linux

    if [ "${Platform}" = "Sengled" ]; then
         cp -r ${WORK_PATH}/DCSApp/src/Vendor/Sengled/lib/libsai_preprocess.so  ${OUTPUT_PATH}/lib
         cp -r ${WORK_PATH}/DCSApp/src/Vendor/Sengled/sai_config ${OUTPUT_PATH}
    fi
    if [ "${Platform}" = "Ingenic" ]; then
         cp -r ${WORK_PATH}/DCSApp/src/Vendor/Ingenic/lib/*  ${OUTPUT_PATH}/lib
    fi
}

rm -rf ${BUILD_PATH}
mkdir -p ${BUILD_PATH}
mkdir -p ${OUTPUT_PATH}/lib

compile_app

generate_output
