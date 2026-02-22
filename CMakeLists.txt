cmake_minimum_required(VERSION 3.10)

project(saga)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# add option to compile for host
option(BUILD_FOR_HOST "Build for host system" OFF)

function(get_ndk)
    if (CMAKE_HOST_WIN32)
        # Download Android NDK if not present
        set(NDK_DIR "${CMAKE_SOURCE_DIR}/ndk")
        set(NDK_EXTRACT_DIR "${NDK_DIR}/android-ndk-r8e")
        set(NDK_ARCHIVE "${NDK_DIR}/android-ndk-r8e-windows-x86_64.zip")
        set(NDK_URL "https://dl.google.com/android/ndk/android-ndk-r8e-windows-x86_64.zip")

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
                COMMAND ${CMAKE_COMMAND} -E tar xf "${NDK_ARCHIVE}"
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
    else()
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
    endif()
endfunction()

if (BUILD_FOR_HOST)
    message(STATUS "Building for host system")

    set(CMAKE_C_COMPILER "gcc")
    set(CMAKE_CXX_COMPILER "g++")
    set(CMAKE_SYSROOT "")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -fno-stack-protector -march=i686 -msse2 -fsanitize=address")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -fno-stack-protector -march=i686 -msse2 -fsanitize=address")

    add_compile_definitions(HOST_BUILD)
else()
    message(STATUS "Building for Android x86")

    get_ndk()

    if (CMAKE_HOST_WIN32)
        set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/windows-x86_64/bin/i686-linux-android-gcc")
        set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/windows-x86_64/bin/i686-linux-android-g++")
        set(CMAKE_SYSROOT "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/platforms/android-9/arch-x86")
        include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/sources/cxx-stl/system/include/")

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-function-sections -fno-data-sections")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-function-sections -fno-data-sections")

        set(CMAKE_SYSTEM_NAME Linux)
        set(CMAKE_SYSTEM_PROCESSOR i686)
        set(CMAKE_EXECUTABLE_SUFFIX "")
        set(CMAKE_CROSSCOMPILING TRUE)
    else()
        set(CMAKE_C_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-gcc")
        set(CMAKE_CXX_COMPILER "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/toolchains/x86-4.7/prebuilt/linux-x86_64/bin/i686-linux-android-g++")
        set(CMAKE_SYSROOT "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/platforms/android-9/arch-x86")
        include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/sources/cxx-stl/system/include/")

        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-function-sections -fno-data-sections")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-function-sections -fno-data-sections")
    endif()
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-rtti -Wno-write-strings -DANDROID")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DANDROID")

set(CMAKE_BUILD_TYPE Debug)

# disable --dependency-file argument on ld which is too old to support it
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)
set(SAGA_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/batman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/globals.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/aiscript.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/aistate.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edfile.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/gui/apimenu.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameframework/saveload.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/crc/crc.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/nuwind/nuwind.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/java/jni_stub.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/area.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/character.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cheat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/collection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/level.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/mission.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/qrand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/store.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/ai.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizaimessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/attractos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/edgizshadowmachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizbombgen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizbuildits.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizforce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizpanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizmopickups.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizobstacles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizportal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizrandom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizspecial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/giztimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/giztorpmachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizturrets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/grapples.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/guidelines.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/hatmachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/ledges.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/lever.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/minicut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/newblowup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/plugs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/push.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/securitydoors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/shards.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/signals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/spinner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/technos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/teleport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/tightropes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/tubes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/zipups.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/startup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/target_android.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/NuRenderDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nucamera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nucamvu0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nugscn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/numtl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuocclusion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuportal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuprim.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuqfnt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nurndr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nushader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nutexanm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuvport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuwater.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nugscn_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nuqfnt_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nurenderthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nurndr_android.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nutex_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nutex_ios_ex.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/generic/nucamera_gen.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuandroid/ios_graphics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuandroid/nuphoneos.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuInputDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuInputManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuMemoryManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuMemoryPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuThreadManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuVirtualTouchDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/deflate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/implode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nucore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nunew.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nukeyboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nulist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nulst.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/numem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/numemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/numouse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nupad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nupad_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuptrblock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nurdpf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nurdpi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nustring.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nustring_c.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nutime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuvideo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/NuInputDevice_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/NuThread_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/bgproc_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nuapi_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/numemory_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nupad_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nutime_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nuvideo_android.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufilebase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufilepak.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufpar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/numc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nudatfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/tmclient.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/android/NuFileAndroidDeviceAPK.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/android/nufile_android.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nufloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/numtx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuquat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuplane.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nurand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nutrig.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nutrig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nutrig_gen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuvec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuvec4.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numusic/numusic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numusic/sfx.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuplatform/nuplatform.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound3_include.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_loader_ogg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_source.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_streamer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_memorymanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_sample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_voice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_weakptr.cpp
)

set(SAGA_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(saga ${SAGA_SOURCE_FILES})
if(CMAKE_HOST_WIN32)
    #Needed because msys adds incorrect flags to the linker
    set_target_properties(saga PROPERTIES ENABLE_EXPORTS TRUE)
    set_target_properties(saga PROPERTIES LINK_FLAGS "-rdynamic")
    set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
    set(CMAKE_C_LINK_EXECUTABLE   "<CMAKE_C_COMPILER>   <FLAGS> <CMAKE_C_LINK_FLAGS>   <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
    set(CMAKE_C_STANDARD_LIBRARIES "")
    set(CMAKE_CXX_STANDARD_LIBRARIES "")
endif()

target_include_directories(saga PUBLIC ${SAGA_INCLUDE_DIRS})

# find dependencies
if (CMAKE_HOST_WIN32)
    set(GLES2_LIBRARIES "GLESv2")
else()
    find_package(PkgConfig REQUIRED)

    if (BUILD_FOR_HOST)
        pkg_check_modules(VORBIS REQUIRED vorbis)
        pkg_check_modules(VORBISFILE REQUIRED vorbisfile)
        set(VORBIS_LIBRARIES ${VORBIS_LIBRARIES} ${VORBISFILE_LIBRARIES})
        set(VORBIS_INCLUDE_DIRS ${VORBIS_INCLUDE_DIRS} ${VORBISFILE_INCLUDE_DIRS})
        message(STATUS "Found Ogg Vorbis: ${VORBIS_LIBRARIES} (include: ${VORBIS_INCLUDE_DIRS})")

        pkg_check_modules(GLES2 REQUIRED glesv2)
    else()
        set(GLES2_LIBRARIES "GLESv2")

        # build Ogg Vorbis
        add_custom_command(
            OUTPUT "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/build.txt"
            COMMAND echo "Building Ogg Vorbis for Android x86..."
            COMMAND env bash -c "${CMAKE_SOURCE_DIR}/libs/build-ogg-vorbis.sh"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/libs"
            COMMENT "Building Ogg Vorbis library"
            VERBATIM
        )
        add_custom_target(build_ogg_vorbis ALL DEPENDS "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/build.txt")
        add_dependencies(saga build_ogg_vorbis)

        target_link_directories(saga PRIVATE "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/lib")
        set(VORBIS_LIBRARIES
            vorbisfile vorbis ogg
        )
        set(VORBIS_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/include")
    endif()
endif()

# include & link dependencies
target_link_libraries(saga PRIVATE ${GLES2_LIBRARIES})
target_include_directories(saga SYSTEM PRIVATE ${GLES2_INCLUDE_DIRS})

target_link_libraries(saga PRIVATE ${VORBIS_LIBRARIES})
target_include_directories(saga SYSTEM PRIVATE ${VORBIS_INCLUDE_DIRS})

# set __FILENAME__ macro to relative path from src/
function(set_filename_macro f)
    get_filename_component(FILENAME_ABS ${f} ABSOLUTE)
    file(RELATIVE_PATH FILENAME_REL ${CMAKE_CURRENT_SOURCE_DIR}/src ${FILENAME_ABS})
    set_source_files_properties(${f} PROPERTIES COMPILE_DEFINITIONS "__FILENAME__=\"src/${FILENAME_REL}\"")
endfunction()

if(BUILD_FOR_HOST)
    add_library(saga_shared SHARED ${SAGA_SOURCE_FILES})
    target_include_directories(saga_shared PUBLIC ${SAGA_INCLUDE_DIRS})
    target_link_libraries(saga_shared PUBLIC ${GLES2_LIBRARIES})
    target_link_libraries(saga_shared PUBLIC ${VORBIS_LIBRARIES})

    include(CTest)
    enable_testing()

    function(add_saga_test file test_description)
        get_filename_component(test_name ${file} NAME_WE)

        add_executable(${test_name} ${file})
        target_include_directories(${test_name} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_link_libraries(${test_name} PRIVATE saga_shared)

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
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/aiscript.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/aistate.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edfile.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/gui/apimenu.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/crc/crc.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/nuwind/nuwind.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/area.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/level.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/qrand.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/timer.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/ai.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizaimessage.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/attractos.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/edgizshadowmachine.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizbombgen.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizbuildits.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizforce.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizpanel.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizmopickups.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizobstacles.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizportal.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizrandom.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizspecial.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/giztimer.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/giztorpmachine.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizturrets.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/grapples.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/guidelines.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/hatmachine.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/ledges.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/lever.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/minicut.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/newblowup.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/plugs.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/push.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/securitydoors.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/shards.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/signals.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/spinner.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/technos.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/teleport.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/tightropes.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/tubes.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/zipups.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/NuRenderDevice.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nugscn.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuportal.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuqfnt.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nurndr.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nushader.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nutex.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuvport.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nuqfnt_android.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nurenderthread.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nutex_ios_ex.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/generic/nucamera_gen.cpp PROPERTIES COMPILE_OPTIONS "-O2")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuandroid/ios_graphics.cpp PROPERTIES COMPILE_OPTIONS "-O2")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuInputDevice.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuInputManager.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuMemoryManager.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuMemoryPool.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuThreadManager.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuVirtualTouchDevice.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/deflate.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nucore.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/numemory.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nupad_interface.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nunew.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nustring.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/NuInputDevice_android.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/NuThread_android.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/bgproc_android.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/numemory_android.cpp PROPERTIES COMPILE_OPTIONS "-O2")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufile.c PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufilebase.cpp PROPERTIES COMPILE_OPTIONS "-O2")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/tmclient.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/android/nufile_android.cpp PROPERTIES COMPILE_OPTIONS "-O2")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numusic/numusic.cpp PROPERTIES COMPILE_OPTIONS "-O2")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuplatform/nuplatform.cpp PROPERTIES COMPILE_OPTIONS "-O3")

    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_loader.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_loader_ogg.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_sample.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound3_include.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_buffer.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_streamer.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_memorymanager.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_system.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_voice.cpp PROPERTIES COMPILE_OPTIONS "-O3")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_weakptr.cpp PROPERTIES COMPILE_OPTIONS "-O3")
endif()

foreach(source_file IN LISTS SAGA_SOURCE_FILES)
    set_filename_macro(${source_file})
endforeach()

find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")

    if(BUILD_FOR_HOST)
        set_target_properties(saga PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        set_target_properties(saga PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-extra-arg=-nostdinc++")
    endif()

    add_custom_target(lint
        COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${SAGA_SOURCE_FILES}
        COMMENT "Running clang-tidy"
    )
else()
    message(STATUS "clang-tidy not found")
endif()
