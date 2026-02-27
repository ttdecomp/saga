#!/bin/bash

set -xeu

TOOLCHAIN=$(realpath ../ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/windows-x86_64)
SYSROOT=$(realpath ../ndk/android-ndk-r8e/platforms/android-9/arch-x86)
TARGET=i686-linux-android
CC="${TOOLCHAIN}/bin/i686-linux-android-gcc.exe"
CXX="${TOOLCHAIN}/bin/i686-linux-android-g++.exe"
PREFIX="${PWD}/ogg-vorbis"
BUILD=$(gcc -dumpmachine)
CFLAGS="--sysroot=${SYSROOT} -I${PREFIX}/include"
CPPFLAGS="--sysroot=${SYSROOT} -I${PREFIX}/include"
LDFLAGS="--sysroot=${SYSROOT} -L${PREFIX}/lib -lm"


if [ -f "${PREFIX}/lib/libvorbis.a" ] && [ -f "${PREFIX}/lib/libogg.a" ]; then
    echo "Libraries already built and cached. Skipping build."
    exit 0
fi




mkdir -p "${PREFIX}"

# build libogg

if [ ! -f "libogg-1.2.1.tar.xz" ]; then
    wget https://ftp.osuosl.org/pub/xiph/releases/ogg/libogg-1.2.1.tar.xz
fi

tar -xf libogg-1.2.1.tar.xz

cd libogg-1.2.1

wget https://raw.githubusercontent.com/gcc-mirror/gcc/refs/heads/master/config.sub -O config.sub
wget https://raw.githubusercontent.com/gcc-mirror/gcc/refs/heads/master/config.guess -O config.guess
chmod +x config.sub config.guess

./configure --host="${TARGET}" --build="${BUILD}" --prefix="${PREFIX}" --enable-shared=no \
    CC="${CC}" CXX="${CXX}" CFLAGS="${CFLAGS}" CPPFLAGS="${CPPFLAGS}" LDFLAGS="${LDFLAGS}"

make -j

make install

cd ..

# build libvorbis

if [ ! -f "libvorbis-1.3.2.tar.xz" ]; then
    wget https://ftp.osuosl.org/pub/xiph/releases/vorbis/libvorbis-1.3.2.tar.xz
fi

tar -xf libvorbis-1.3.2.tar.xz

cd libvorbis-1.3.2

wget https://raw.githubusercontent.com/gcc-mirror/gcc/refs/heads/master/config.sub -O config.sub
wget https://raw.githubusercontent.com/gcc-mirror/gcc/refs/heads/master/config.guess -O config.guess
chmod +x config.sub config.guess

./configure --host="${TARGET}" --build="${BUILD}" --prefix="${PREFIX}" --enable-shared=no \
    CC="${CC}" CXX="${CXX}" CFLAGS="${CFLAGS}" CPPFLAGS="${CPPFLAGS}" LDFLAGS="${LDFLAGS}"

make -j

make install

cd ..

echo $(date) > "${PREFIX}/build.txt"
