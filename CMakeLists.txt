cmake_minimum_required(VERSION 3.10)

project(saga)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# add option to compile for host
option(BUILD_FOR_HOST "Build for host system" OFF)

function(get_ndk)
    # Download Android NDK if not present
    set(NDK_DIR "${CMAKE_SOURCE_DIR}/ndk")
    set(NDK_EXTRACT_DIR "${NDK_DIR}/android-ndk-r8e")
    set(NDK_ARCHIVE "${NDK_DIR}/android-ndk-r8e-linux-x86_64.tar.bz2")
    set(NDK_URL "https://dl.google.com/android/ndk/android-ndk-r8e-linux-x86_64.tar.bz2")

    if(NOT EXISTS "${NDK_EXTRACT_DIR}")
        file(MAKE_DIRECTORY "${NDK_DIR}")

        if(NOT EXISTS "${NDK_ARCHIVE}")
            message(STATUS "Downloading Android NDK r8e...")
            file(DOWNLOAD "${NDK_URL}" "${NDK_ARCHIVE}"
                SHOW_PROGRESS
                STATUS download_status)
            list(GET download_status 0 download_code)
            if(NOT download_code EQUAL 0)
                message(FATAL_ERROR "Failed to download NDK: ${download_status}")
            endif()
        endif()

        message(STATUS "Extracting Android NDK r8e...")
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E tar xjf "${NDK_ARCHIVE}"
            WORKING_DIRECTORY "${NDK_DIR}"
            RESULT_VARIABLE extract_result)
        if(NOT extract_result EQUAL 0)
            message(FATAL_ERROR "Failed to extract NDK")
        endif()

        file(REMOVE "${NDK_ARCHIVE}")
        message(STATUS "Android NDK r8e ready")
    else()
        message(STATUS "Android NDK r8e already present")
    endif()
endfunction()

if (BUILD_FOR_HOST)
    message(STATUS "Building for host system")

    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_SYSROOT "")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -fno-stack-protector -march=i686 -msse2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -fno-stack-protector -march=i686 -msse2")

    add_compile_definitions(HOST_BUILD)
else()
    message(STATUS "Building for Android x86")

    get_ndk()

    set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-gcc")
    set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-g++")
    set(CMAKE_SYSROOT "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/platforms/android-9/arch-x86")
    include_directories("${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/sources/cxx-stl/stlport/stlport/")
endif()

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -Wwrite-strings")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wwrite-strings")

set(CMAKE_BUILD_TYPE Debug)

# disable --dependency-file argument on ld which is too old to support it
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)

set(SAGA_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/batman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/globals.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/deflate/deflate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/deflate/deflate_noopt.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/area.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/character.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/cheat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/collection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/episode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/level.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/mission.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/store.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game/world.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/nuwind/nuwind.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi.saga/ai/aisys/aiscript.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi.saga/edtools/edfile.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/init/init.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nu3d/nutex.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nuandroid/nuios.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nuapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nucore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nulist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nupad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nurdpf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nurdpi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nustring_c.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nutime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nucore/nuvideo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nufile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nufile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nufilepak.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nufpar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nupsfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/numc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nudatfile.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nufloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/numtx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nutrig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nutrig_gen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nuvec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numath/nuvec4.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numemory/numemory.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nuplatform/nuplatform.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nuthread/nuthread.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/saveload/saveload.cpp
)

set(SAGA_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(saga ${SAGA_SOURCE_FILES})
set_target_properties(saga PROPERTIES ENABLE_EXPORTS TRUE)
target_include_directories(saga PUBLIC ${SAGA_INCLUDE_DIRS})


# set __FILENAME__ macro to relative path from src/
function(set_filename_macro f)
    get_filename_component(FILENAME_ABS ${f} ABSOLUTE)
    file(RELATIVE_PATH FILENAME_REL ${CMAKE_CURRENT_SOURCE_DIR}/src ${FILENAME_ABS})
    set_source_files_properties(${f} PROPERTIES COMPILE_DEFINITIONS "__FILENAME__=\"src/${FILENAME_REL}\"")
endfunction()

if(BUILD_FOR_HOST)
    add_library(saga_shared SHARED ${SAGA_SOURCE_FILES})
    target_include_directories(saga_shared PUBLIC ${SAGA_INCLUDE_DIRS})

    include(CTest)
    enable_testing()

    function(add_saga_test file test_description)
        get_filename_component(test_name ${file} NAME_WE)

        add_executable(${test_name} ${file})
        target_include_directories(${test_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_link_libraries(${test_name} saga_shared)

        add_test(
            NAME ${test_name}
            COMMAND ${test_name}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        )

        set_target_properties(${test_name} PROPERTIES CXX_STANDARD 23)
        set_target_properties(${test_name} PROPERTIES CXX_STANDARD_REQUIRED ON)
        set_tests_properties(${test_name} PROPERTIES LABELS "${test_description}")

        set_filename_macro(${file})
    endfunction()

    add_saga_test(test/dat_file_read.cpp "Open DAT file and read badwords.txt")
else()
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi.saga/ai/aisys/aiscript.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi.saga/edtools/edfile.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nufile.c PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nufile/nupsfile.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/numemory/numemory.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api.saga/nuandroid/nuios.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/game/world.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/game/level.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/game/area.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/game/timer.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/nuwind.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/deflate/deflate.cpp PROPERTIES COMPILE_OPTIONS "-O3")
endif()

foreach(source_file IN LISTS SAGA_SOURCE_FILES)
    set_filename_macro(${source_file})
endforeach()
