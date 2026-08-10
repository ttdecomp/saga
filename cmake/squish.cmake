include(ExternalProject)

function(add_squish)
    find_program(SAGA_MAKE_EXECUTABLE NAMES make gmake)
    if(NOT SAGA_MAKE_EXECUTABLE)
        message(FATAL_ERROR "Building Squish requires make")
    endif()

    set(prefix "${CMAKE_SOURCE_DIR}/libs/squishlib")
    set(stl_include "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/sources/cxx-stl/gnu-libstdc++/4.7/include")
    set(stl_arch_include "${CMAKE_SOURCE_DIR}/ndk/android-ndk-r8e/sources/cxx-stl/gnu-libstdc++/4.7/libs/x86/include")
    set(cppflags
        "--sysroot=${CMAKE_SYSROOT} -I${stl_include} -I${stl_arch_include} -I${prefix}/include -include climits -fno-rtti -fno-exceptions"
    )

    file(MAKE_DIRECTORY "${prefix}/include" "${prefix}/lib")

    ExternalProject_Add(squish_external
        PREFIX "${CMAKE_BINARY_DIR}/external/squish"
        URL "https://storage.googleapis.com/google-code-archive-downloads/v2/code.google.com/libsquish/squish-1.10.tar.gz"
        URL_HASH "SHA256=aec3428badafdbb3b0ed1ea3a385a8029e65694282459f61b4a538963e8cada3"
        DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/libs"
        BUILD_IN_SOURCE TRUE
        UPDATE_COMMAND ""
        PATCH_COMMAND
            "${CMAKE_COMMAND}"
            "-DMAKEFILE=<SOURCE_DIR>/Makefile"
            -P "${CMAKE_SOURCE_DIR}/cmake/patch-squish.cmake"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND
            "${SAGA_MAKE_EXECUTABLE}" -j
            "CXX=${CMAKE_CXX_COMPILER}"
            "CPPFLAGS=${cppflags}"
            "AR=${CMAKE_AR}"
            "RANLIB=${CMAKE_RANLIB}"
        INSTALL_COMMAND
            "${CMAKE_COMMAND}" -E copy <SOURCE_DIR>/squish.h "${prefix}/include/squish.h"
            COMMAND "${CMAKE_COMMAND}" -E copy <SOURCE_DIR>/libsquish.a "${prefix}/lib/libsquish.a"
        BUILD_BYPRODUCTS "${prefix}/lib/libsquish.a"
    )

    add_custom_target(build_squish ALL DEPENDS squish_external)

    add_library(custom_squish INTERFACE)
    add_dependencies(custom_squish build_squish)
    target_link_libraries(custom_squish INTERFACE "stdc++")
    target_include_directories(custom_squish SYSTEM INTERFACE "${prefix}/include")
    target_link_directories(custom_squish INTERFACE "${prefix}/lib")
    target_link_libraries(custom_squish INTERFACE "squish")
endfunction()
