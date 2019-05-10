#! /bin/bash

TOOLCHAIN_PATH="$(pwd)/../../host/usr/bin"

CXX="CXX="$TOOLCHAIN_PATH"/arm-rockchip-linux-gnueabihf-g++"
CC="CC="$TOOLCHAIN_PATH"/arm-rockchip-linux-gnueabihf-gcc"
CPP="CPP="$TOOLCHAIN_PATH"/arm-rockchip-linux-gnueabihf-cpp"

./configure '--target=arm-rockchip-linux-gnueabihf' '--host=arm-rockchip-linux-gnueabihf' '--build=x86_64-pc-linux-gnu' '--prefix=/usr' '--exec-prefix=/usr' '--sysconfdir=/etc' '--localstatedir=/var' '--program-prefix=' '--disable-gtk-doc' '--disable-gtk-doc-html' '--disable-doc' '--disable-docs' '--disable-documentation' '--with-xmlto=no' '--with-fop=no' '--disable-dependency-tracking' '--enable-ipv6' '--disable-static' '--enable-shared' 'build_alias=x86_64-pc-linux-gnu' 'host_alias=arm-rockchip-linux-gnueabihf' 'target_alias=arm-rockchip-linux-gnueabihf' $CXX 'CXXFLAGS=-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64  -Os ' 'LDFLAGS=' 'CPPFLAGS=-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64' $CC 'CFLAGS=-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64  -Os ' $CPP

