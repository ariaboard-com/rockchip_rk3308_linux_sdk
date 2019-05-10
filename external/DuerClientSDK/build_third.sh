
compile_openssl() {
    cd ${WORK_PATH}/common/openssl/    
    tar xzf openssl-1.0.2g.tar.gz
    cd openssl-1.0.2g/
    
    CONFINURE_CMD="no-ssl3 shared no-asm --prefix=${THIRD_PATH} --openssldir=${THIRD_PATH}/build/ssl"
    
    if [ ! -z ${SYSROOT_PATH} ]; then
        CONFINURE_CMD="${CONFINURE_CMD} --sysroot=${SYSROOT_PATH}"
    fi

    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CONFINURE_CMD="${CONFINURE_CMD} -mfloat-abi=hard"
    fi
    
    if [ "$CPU_ARCH" = "arm" ]; then
        CONFINURE_CMD="${CONFINURE_CMD} linux-generic32"
    elif [ "$CPU_ARCH" = "mips" ]; then
        CONFINURE_CMD="${CONFINURE_CMD} linux-mips32 -mips32 -mabi=32"
    elif [ "$CPU_ARCH" = "aarch64" ]; then
        CONFINURE_CMD="${CONFINURE_CMD} linux-generic64"
    elif [ "$CPU_ARCH" = "x86_64" ]; then
        CONFINURE_CMD="${CONFINURE_CMD} linux-x86_64"
    fi

    if [ "$Platform" = "Elina" ]; then
        CC="gcc" \
        AR="${CROSS_TOOLS}ar" \
        LD="${CROSS_TOOLS}ld" \
        RANLIB="${CROSS_TOOLS}ranlib" \
        ./Configure $CONFINURE_CMD
    else
        CC="${CROSS_TOOLS}gcc" \
        AR="${CROSS_TOOLS}ar" \
        LD="${CROSS_TOOLS}ld" \
        RANLIB="${CROSS_TOOLS}ranlib" \
        ./Configure $CONFINURE_CMD
    fi

    make clean
    make -j ${THREAD_COUNT}
    make install

    unset CONFINURE_CMD
}

compile_nghttp2() {
    cd ${WORK_PATH}/common/nghttp2new/
    tar zxf nghttp2-1.24.0.tar.gz
    cd nghttp2-1.24.0
    
    CONFINURE_CMD="--prefix=${THIRD_PATH} --enable-lib-only --host=${CROSS_PLATFORM} --disable-assert --disable-largefile"
    
    if [ ! -z ${SYSROOT_PATH} ]; then
        CONFINURE_CMD="${CONFINURE_CMD} --with-sysroot=${SYSROOT_PATH}"
        CFLAGS="--sysroot=${SYSROOT_PATH}"
    fi

    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    CC="${CROSS_TOOLS}gcc" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS} -nostdlib -nostartfiles -nodefaultlibs -ffreestanding -Os" \
    ./configure ${CONFINURE_CMD}

    make clean
    make -j ${THREAD_COUNT}
    make install

    unset CONFINURE_CMD
}

compile_zlib() {
    cd ${WORK_PATH}/common/zlib/
    CONFINURE_CMD="--shared --prefix=${THIRD_PATH} --sharedlibdir=${THIRD_PATH}/lib --libdir=${THIRD_PATH}/lib --includedir=${THIRD_PATH}/include/"
    
    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    if [ ! -z ${SYSROOT_PATH} ]; then
        CFLAGS="${CFLAGS} --sysroot=${SYSROOT_PATH}"
    fi
    
    CC="${CROSS_TOOLS}gcc" \
    CFLAGS="${CFLAGS}" \
    ./configure $CONFINURE_CMD
    
    make clean
    make -j ${THREAD_COUNT}
    make install

    unset CONFINURE_CMD
}

compile_sqlite3() {
    cd ${WORK_PATH}/common/sqlite
    rm -rf sqlite-autoconf-3200100
    tar zxvf sqlite-autoconf-3200100.tar.gz
    cd sqlite-autoconf-3200100
    
    CONFINURE_CMD="--prefix=${THIRD_PATH} --host=${CROSS_PLATFORM}"

    if [ ! -z ${SYSROOT_PATH} ]; then
        CFLAGS="--sysroot=${SYSROOT_PATH}"
    fi
    
    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    CC="${CROSS_TOOLS}gcc" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS}" \
    ./configure ${CONFINURE_CMD}

    make clean
    make -j ${THREAD_COUNT}
    make install

    unset CONFINURE_CMD
}

compile_curl() {
    cd ${WORK_PATH}/common/curlnew
    rm -rf curl-7.54.1
    tar zxf curl-7.54.1.tar.gz
    cd curl-7.54.1
    
    CONFINURE_CMD="--enable-shared=yes --enable-static=no --prefix=${THIRD_PATH} --with-ssl=${THIRD_PATH} --host=${CROSS_PLATFORM} --with-zlib=${THIRD_PATH} --with-nghttp2=${THIRD_PATH} --enable-threaded-resolver --disable-largefile --disable-ftp --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smb --disable-smtp --disable-ipv6"
    
    if [ ! -z ${SYSROOT_PATH} ]; then
        CONFINURE_CMD="${CONFINURE_CMD} --with-sysroot=${SYSROOT_PATH}"
        CFLAGS="--sysroot=${SYSROOT_PATH}"
    fi

    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    PKG_CONFIG_PATH="${THIRD_PATH}/lib/pkgconfig" \
    CC="${CROSS_TOOLS}gcc" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS} -Os" \
    ./configure ${CONFINURE_CMD}

    make clean
    make -j ${THREAD_COUNT}
    make install

    unset CONFINURE_CMD
}

compile_portaudio() {
    cd ${WORK_PATH}/common/portaudio
    tar xzf portaudio_v190600_20161030.tar.gz
    cd portaudio

    CONFINURE_CMD="--host=${CROSS_PLATFORM} --prefix=${THIRD_PATH} --without-jack --without-oss --with-alsa"

    if [ ! -z ${SYSROOT_PATH} ]; then
        CONFINURE_CMD="${CONFINURE_CMD} --with-sysroot=${SYSROOT_PATH}"
        CFLAGS="--sysroot=${SYSROOT_PATH}"
    fi

    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    CC="${CROSS_TOOLS}gcc" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS}" \
    ./configure ${CONFINURE_CMD}

    make clean
    make -j4
    make install

    unset CONFINURE_CMD
}

compile_ffmpeg() {
    cd ${WORK_PATH}/common/ffmpeg/3.2.7
    tar zxvf ffmpeg-3.2.7.tar.gz
    cd ffmpeg-3.2.7

    if [ ! -z ${SYSROOT_PATH} ]; then
        CFLAGS="--sysroot=${SYSROOT_PATH}"
        CONFINURE_CMD="${CONFINURE_CMD}  --sysroot=${SYSROOT_PATH}"
    fi

    CONFINURE_CMD="${CONFINURE_CMD} --enable-openssl --disable-static --enable-shared --disable-programs --target-os=linux --incdir=${THIRD_PATH}/include/ --libdir=${THIRD_PATH}/lib --disable-doc"

    if [ ! -z ${CROSS_PLATFORM} ]; then
        CONFINURE_CMD="${CONFINURE_CMD} --enable-cross-compile --cross-prefix=${CROSS_TOOLS}  --host-os=${CROSS_PLATFORM}"
    fi

    CONFINURE_CMD="${CONFINURE_CMD} --arch=${CPU_ARCH}"
    if [ "${Platform}" = "Ingenic" ]; then
        patch < ${WORK_PATH}/patchs/ffmpeg_ingenic.patch
        CONFINURE_CMD="${CONFINURE_CMD} --cpu=xburst"
    else
        CONFINURE_CMD="${CONFINURE_CMD} --cpu=generic"
    fi

    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    CC="${CROSS_TOOLS}gcc" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS} -nostdlib -nostartfiles -nodefaultlibs -ffreestanding" \
    ./configure ${CONFINURE_CMD}

    make clean
    make -j4
    make install

   unset CONFINURE_CMD
}

compile_iconv() {
    cd ${WORK_PATH}/common/iconv/1.14

    if [ "$DUER_USE_STATIC_ICONV" = "yes" ]; then
        CONFINURE_CMD="--enable-static=yes --enable-shared=no --prefix=${THIRD_PATH} --libdir=${THIRD_PATH}/lib --includedir=${THIRD_PATH}/include/"
    else
        CONFINURE_CMD="--enable-static=no --enable-shared --prefix=${THIRD_PATH} --libdir=${THIRD_PATH}/lib --includedir=${THIRD_PATH}/include/"
    fi
 
    if [ ! -z ${SYSROOT_PATH} ]; then
        CFLAGS="--sysroot=${SYSROOT_PATH}"
        CONFINURE_CMD="${CONFINURE_CMD} --with-sysroot=${SYSROOT_PATH}"
    fi

    if [ ! -z ${CROSS_PLATFORM} ]; then
        CONFINURE_CMD="${CONFINURE_CMD} --host=${CROSS_PLATFORM}"
    fi

    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    CC="${CROSS_TOOLS}gcc" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS}" \
    ./configure ${CONFINURE_CMD}

    make clean
    make -j4
    make install

    unset CONFINURE_CMD
}

compile_breakpad() {
    cd ${WORK_PATH}/common/google-breakpad/breakpad

    if [ ! -z ${CROSS_PLATFORM} ]; then
        CONFINURE_CMD="--host=${CROSS_PLATFORM} --prefix=${THIRD_PATH}"
    fi
    if [ "${SUPPORT_HARD_FLOAT}" = "yes" ]; then
        CFLAGS="${CFLAGS} -mfloat-abi=hard"
    fi

    if [ ! -z ${SYSROOT_PATH} ]; then
        CFLAGS="${CFLAGS} --sysroot=${SYSROOT_PATH}"
        CXXFLAGS="-std=c++11 --sysroot=${SYSROOT_PATH}"
    fi

    CC="${CROSS_TOOLS}gcc" \
    CXX="${CROSS_TOOLS}g++" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS}" \
    CXXFLAGS="${CXXFLAGS}" \
    ./configure ${CONFINURE_CMD}

    make clean
    make -j4
    make install

    unset CONFINURE_CMD
}

compile_libupnp(){
    cd ${WORK_PATH}/common/pupnp   
    unzip pupnp-branch-1.6.x.zip
    cd pupnp-branch-1.6.x
 
    ./bootstrap
 
    CONFINURE_CMD="--prefix=${THIRD_PATH} --disable-static --enable-shared --includedir=${THIRD_PATH}/include/ --libdir=${THIRD_PATH}/lib --disable-samples"
 
    if [ ! -z ${SYSROOT_PATH} ]; then
         CFLAGS="--sysroot=${SYSROOT_PATH}"
         CONFINURE_CMD="${CONFINURE_CMD}  --with-sysroot=${SYSROOT_PATH}"
    fi
   
    if [ ! -z ${CROSS_PLATFORM} ]; then
         CONFINURE_CMD="${CONFINURE_CMD} --host=${CROSS_PLATFORM}"
    fi 
 
    CC="${CROSS_TOOLS}gcc" \
    AR="${CROSS_TOOLS}ar" \
    LD="${CROSS_TOOLS}ld" \
    RANLIB="${CROSS_TOOLS}ranlib" \
    CFLAGS="${CFLAGS}" \
    ./configure $CONFINURE_CMD
 
    make clean
    make -j4
    make install

    unset CONFINURE_CMD
}

compile_third_party() {
    export PATH="${TOOLCHAIN_PATH}:${PATH}"

    # openssl
    compile_openssl

    # nghttp2
    compile_nghttp2

    # zlib
    compile_zlib

    # curl
    compile_curl

    # sqlite3
    # compile if BUILD_WITH_SQLITE3 != OFF
    if [ "${BUILD_WITH_SQLITE3}" != "OFF" ]; then
        compile_sqlite3
    fi

    # portaudio
    # compile if BUILD_USE_RECORDER != ALSA
    if [ "${BUILD_USE_RECORDER}" != "ALSA" ]; then
        compile_portaudio
    fi

    # ffmpeg
    compile_ffmpeg

    # iconv
    # compile if BUILD_TTS_SDK=ON
    if [ "${BUILD_TTS_SDK}" = "ON" ]; then
        compile_iconv
    fi

    # breakpad
    # compile if BUILD_CRAB_SDK=ON
    if [ "${BUILD_CRAB_SDK}" = "ON" ]; then
        compile_breakpad
    fi
    
    # libupnp
    # compile if BUILD_DLNA=ON
    if [ "${BUILD_DLNA}" = "ON" ]; then
        compile_libupnp
    fi
}

