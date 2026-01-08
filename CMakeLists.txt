cmake_minimum_required(VERSION 3.10)

project(saga)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-gcc")
set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-g++")
set(CMAKE_SYSROOT "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/platforms/android-9/arch-x86")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# disable --dependency-file argument on ld which is too old to support it
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)

set(SAGA_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/batman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nuvec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nufloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nufile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nufile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nuthread/nuthread.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/numtx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nutrig_gen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nustring_c.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nutime.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nutrig.cpp
)

set(SAGA_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(saga ${SAGA_SOURCE_FILES})

set_target_properties(saga PROPERTIES ENABLE_EXPORTS TRUE)

target_include_directories(saga PUBLIC ${SAGA_INCLUDE_DIRS})
