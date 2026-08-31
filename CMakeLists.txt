cmake_minimum_required(VERSION 3.10)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(BUILD_FOR_HOST "Build for host system" OFF)
option(BUILD_FOR_HOST64 "Build a native 64-bit host executable" OFF)
option(HOST_ENABLE_ASAN "Enable AddressSanitizer in non-Windows host builds" ON)

if(BUILD_FOR_HOST64)
    set(BUILD_FOR_HOST ON)
endif()

include("${CMAKE_SOURCE_DIR}/cmake/ndk.cmake")

if(NOT BUILD_FOR_HOST)
    get_ndk()

    if(CMAKE_HOST_WIN32)
        set(NDK_TOOL_SUFFIX ".exe")
    else()
        set(NDK_TOOL_SUFFIX "")
    endif()

    set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR i686)
    set(CMAKE_SYSROOT "${NDK_SYSROOT}")
    set(CMAKE_C_COMPILER "${NDK_TOOLCHAIN}/bin/i686-linux-android-gcc${NDK_TOOL_SUFFIX}")
    set(CMAKE_CXX_COMPILER "${NDK_TOOLCHAIN}/bin/i686-linux-android-g++${NDK_TOOL_SUFFIX}")
endif()

project(saga)

if(CMAKE_VERSION VERSION_LESS 3.12)
    find_package(PythonInterp REQUIRED)
    set(Python3_EXECUTABLE ${PYTHON_EXECUTABLE})
else()
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
endif()

add_custom_target(check_host_boundary ALL
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_host_boundary.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Checking host/shared source boundary"
)

if(BUILD_FOR_HOST64)
    add_custom_target(check_host64_safety ALL
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_host64_safety.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Checking host64 pointer safety"
    )
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include("${CMAKE_SOURCE_DIR}/cmake/ogg-vorbis.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/squish.cmake")

if (BUILD_FOR_HOST)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector -msse2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector -msse2")

    if(NOT CMAKE_HOST_WIN32)
        if(NOT BUILD_FOR_HOST64)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -march=i686")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -march=i686")
        endif()

        if(HOST_ENABLE_ASAN)
            set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
        endif()
    endif()
else()
    message(STATUS "Using Android NDK toolchain: ${NDK_TOOLCHAIN}")

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-function-sections -fno-data-sections")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-function-sections -fno-data-sections")
    
    if (CMAKE_HOST_WIN32)
        set(CMAKE_EXECUTABLE_SUFFIX "")

        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-type -static-libgcc -static-libstdc++")
    endif()
endif()

# set C(++) standard to C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_IMPLICIT_LINK_LIBRARIES "")
set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES "")

if(CMAKE_HOST_WIN32)
    # needed because msys adds incorrect flags to the linker
    set(CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_CXX_COMPILER> <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
    set(CMAKE_C_LINK_EXECUTABLE   "<CMAKE_C_COMPILER>   <FLAGS> <CMAKE_C_LINK_FLAGS>   <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
    set(CMAKE_C_STANDARD_LIBRARIES "")
    set(CMAKE_CXX_STANDARD_LIBRARIES "")
endif()

set(CMAKE_BUILD_TYPE Debug)

# disable --dependency-file argument on ld which is too old to support it
set(CMAKE_LINK_DEPENDS_NO_SHARED ON)

message(STATUS "Building for ${CMAKE_HOST_SYSTEM_NAME} on ${CMAKE_HOST_SYSTEM_PROCESSOR} with ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")

# source files for both host and target builds
file(GLOB_RECURSE SAGA_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/decomp.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/batman.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/globals.cpp"
    
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/gameframework/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gameframework/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/java/jni_stub.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/*.cpp"


    "${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/*.cpp"

    "${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/*.cpp"
)

# add platform-specific source files and build rules
if (BUILD_FOR_HOST)
    # Input is a true platform backend. All other reconstructed Android source
    # files stay in the host build so the harness exercises the same engine
    # implementation as the target build.
    list(REMOVE_ITEM SAGA_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/NuInputDevice_android.cpp
        # An executable-level operator new/delete override is safe on the
        # monolithic Android target, but interposes allocations made inside
        # host C++ graphics/audio drivers. Let the host C++ runtime own global
        # allocation; explicit NuMemory allocations still use the shared
        # reconstructed manager.
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nunew.cpp
    )

    list(APPEND SAGA_SOURCE_FILES
        # Command-line harness. These files may inspect the game, but never
        # provide symbols consumed by reconstructed game/engine code.
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/harness/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/harness/audio.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/harness/load.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/harness/window.cpp

        # Minimal host platform boundary: one selected input backend and
        # external API/filesystem adapters only.
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/platform/input.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/platform/compressed_texture.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/platform/documents.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/platform/egl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host/platform/opensl.cpp
    )

    add_executable(saga ${SAGA_SOURCE_FILES})
    add_dependencies(saga check_host_boundary)
    if(BUILD_FOR_HOST64)
        add_dependencies(saga check_host64_safety)
    endif()
    
    include(src/host.cmake)
else()
    list(APPEND SAGA_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nutex_android.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nutex_ios_ex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuandroid/ios_graphics.cpp
    )

    add_executable(saga ${SAGA_SOURCE_FILES})
    add_dependencies(saga check_host_boundary)

    include(src/target.cmake)
endif()

# set common compile options and definitions
target_include_directories(saga PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(saga PRIVATE
    -fno-exceptions
    -Wno-write-strings
    -DANDROID
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)
if(CMAKE_HOST_WIN32)
    set_target_properties(saga PROPERTIES ENABLE_EXPORTS TRUE)
    if(NOT BUILD_FOR_HOST)
        set_target_properties(saga PROPERTIES LINK_FLAGS "-rdynamic")
    endif()
endif()

# set __FILENAME__ macro to relative path from src/
function(set_filename_macro f)
    get_filename_component(FILENAME_ABS ${f} ABSOLUTE)
    file(RELATIVE_PATH FILENAME_REL ${CMAKE_CURRENT_SOURCE_DIR} ${FILENAME_ABS})
    set_source_files_properties(${f} PROPERTIES COMPILE_DEFINITIONS "__FILENAME__=\"${FILENAME_REL}\"")
endfunction()

foreach(source_file IN LISTS SAGA_SOURCE_FILES)
    set_filename_macro(${source_file})
endforeach()

# use clang-tidy if available
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
if(CLANG_TIDY_EXE)
    message(STATUS "Found clang-tidy: ${CLANG_TIDY_EXE}")

    if(BUILD_FOR_HOST)
        set_target_properties(saga PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    else()
        set_target_properties(saga PROPERTIES CXX_CLANG_TIDY "${CLANG_TIDY_EXE};-extra-arg=-nostdinc++")
    endif()

    # check_symbols.py needs the original ROM (res/libTTapp.so), which only
    # exists in local checkouts. Skip it in CI (where the ROM is absent) and
    # for developers who haven't checked out the ROM, so `lint` stays green
    # there.
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/res/libTTapp.so)
        set(CHECK_SYMBOLS_CMD echo "WARNING: res/libTTapp.so not present; skipping symbol check")
    else()
        set(CHECK_SYMBOLS_CMD python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_symbols.py)
    endif()

    add_custom_target(lint
        COMMAND ${CLANG_TIDY_EXE} -p=${CMAKE_BINARY_DIR} ${SAGA_SOURCE_FILES} --warnings-as-errors="*"
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_duplicate_definitions.py ${CMAKE_CURRENT_SOURCE_DIR}/src --build ${CMAKE_BINARY_DIR}
        COMMAND ${CHECK_SYMBOLS_CMD}
        COMMENT "Running clang-tidy, duplicate-definition check, and symbol baseline check"
    )
else()
    message(WARNING "clang-tidy not found")
endif()
