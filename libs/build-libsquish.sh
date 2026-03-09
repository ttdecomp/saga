#!/bin/bash

set -xeu

# 1. Base paths dynamically located
NDK_DIR=$(realpath ../ndk/android-ndk-r8e)
TOOLCHAIN="${NDK_DIR}/toolchains/x86-4.7/prebuilt/linux-x86_64"
SYSROOT="${NDK_DIR}/platforms/android-9/arch-x86"
TARGET=i686-linux-android

# 2. Toolchain definitions
CC="${TOOLCHAIN}/bin/i686-linux-android-gcc"
CXX="${TOOLCHAIN}/bin/i686-linux-android-g++"
AR="${TOOLCHAIN}/bin/i686-linux-android-ar"
RANLIB="${TOOLCHAIN}/bin/i686-linux-android-ranlib"

# --- THE UPDATED PREFIX ---
PREFIX="${PWD}/squishlib"
mkdir -p "${PREFIX}"
mkdir -p "${PREFIX}/include"
mkdir -p "${PREFIX}/lib"
# 3. Locate C++ STL headers
STL_INC1="${NDK_DIR}/sources/cxx-stl/gnu-libstdc++/4.7/include"
STL_INC2="${NDK_DIR}/sources/cxx-stl/gnu-libstdc++/4.7/libs/x86/include"

# SAFEGUARD: Fail early if the NDK doesn't contain these STL folders
if [ ! -d "${STL_INC1}" ]; then
    echo "ERROR: STL include path not found! Make sure it exists: ${STL_INC1}"
    exit 1
fi
if [ ! -d "${STL_INC2}" ]; then
    echo "ERROR: Arch-specific STL path not found! Make sure it exists: ${STL_INC2}"
    exit 1
fi

# 4. Set compilation flags
CXX_INCLUDES="-I${STL_INC1} -I${STL_INC2}"
CFLAGS="--sysroot=${SYSROOT} -I${PREFIX}/include"
CPPFLAGS="--sysroot=${SYSROOT} ${CXX_INCLUDES} -I${PREFIX}/include -include climits"
LDFLAGS="--sysroot=${SYSROOT} -L${PREFIX}/lib"

# 5. Download and extract
if [ ! -f "squish-1.10.tar.gz" ]; then
    wget https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/libsquish/squish-1.10.tar.gz
fi
tar -xf squish-1.10.tar.gz
cd squish-1.10

# 6. Build
echo ">>> Verifying CPPFLAGS: ${CPPFLAGS}"
make -j CXX="${CXX}" CPPFLAGS="${CPPFLAGS}" AR="${AR}" RANLIB="${RANLIB}"

# 7. Install
make install INSTALL_DIR="${PREFIX}"
cd ..

echo $(date) > "${PREFIX}/build.txt"