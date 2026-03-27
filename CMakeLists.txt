cmake_minimum_required(VERSION 3.10)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

option(BUILD_FOR_HOST "Build for host system" OFF)

include("${CMAKE_SOURCE_DIR}/cmake/ndk.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/ogg-vorbis.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/squish.cmake")

if (BUILD_FOR_HOST)  
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector -msse2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector -msse2")

    if (NOT CMAKE_HOST_WIN32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -march=i686 -fsanitize=address")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -march=i686 -fsanitize=address")
    endif()
else()
    get_ndk()

    message(STATUS "Using Android NDK toolchain: ${NDK_TOOLCHAIN}")

    set(CMAKE_SYSTEM_NAME Android-Custom)
    
    set(CMAKE_SYSROOT "${NDK_SYSROOT}")
    set(CMAKE_C_COMPILER "${NDK_TOOLCHAIN}/bin/i686-linux-android-gcc")
    set(CMAKE_CXX_COMPILER "${NDK_TOOLCHAIN}/bin/i686-linux-android-g++")
    
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-function-sections -fno-data-sections")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-function-sections -fno-data-sections")
    
    if (CMAKE_HOST_WIN32)
        set(CMAKE_SYSTEM_PROCESSOR i686)
        set(CMAKE_EXECUTABLE_SUFFIX "")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type -static-libgcc -static-libstdc++")
    endif()
endif()

message(STATUS "Building for ${CMAKE_HOST_SYSTEM_NAME} on ${CMAKE_HOST_SYSTEM_PROCESSOR} with ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")

project(saga)

add_subdirectory(src libsaga)
if(BUILD_FOR_HOST)
    add_subdirectory(host-tests)
endif()
