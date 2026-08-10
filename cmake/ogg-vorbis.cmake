include(ExternalProject)

function(add_ogg_vorbis)
    find_program(SAGA_MAKE_EXECUTABLE NAMES make gmake)
    find_program(SAGA_SH_EXECUTABLE NAMES sh bash)
    if(NOT SAGA_MAKE_EXECUTABLE OR NOT SAGA_SH_EXECUTABLE)
        message(FATAL_ERROR "Building Ogg Vorbis requires make and a POSIX shell")
    endif()

    set(prefix "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis")
    set(cflags "--sysroot=${CMAKE_SYSROOT} -I${prefix}/include")
    set(ldflags "--sysroot=${CMAKE_SYSROOT} -L${prefix}/lib")
    if(CMAKE_HOST_WIN32)
        set(ldflags "${ldflags} -lm")
    endif()

    file(MAKE_DIRECTORY "${prefix}/include" "${prefix}/lib")

    ExternalProject_Add(ogg_external
        PREFIX "${CMAKE_BINARY_DIR}/external/ogg"
        URL "https://ftp.osuosl.org/pub/xiph/releases/ogg/libogg-1.2.1.tar.xz"
        URL_HASH "SHA256=21e0a61e15e9dd294587bcd39d81fbe1998b27b1c525e15ecfaba94344f921b4"
        DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/libs"
        BUILD_IN_SOURCE TRUE
        UPDATE_COMMAND ""
        PATCH_COMMAND
            "${CMAKE_COMMAND}"
            "-DCONFIG_SUB=<SOURCE_DIR>/config.sub"
            -P "${CMAKE_SOURCE_DIR}/cmake/patch-config-sub.cmake"
        CONFIGURE_COMMAND
            "${CMAKE_COMMAND}" -E env
            "CC=${CMAKE_C_COMPILER}"
            "CXX=${CMAKE_CXX_COMPILER}"
            "AR=${CMAKE_AR}"
            "RANLIB=${CMAKE_RANLIB}"
            "CFLAGS=${cflags}"
            "CPPFLAGS=${cflags}"
            "LDFLAGS=${ldflags}"
            "${SAGA_SH_EXECUTABLE}" <SOURCE_DIR>/configure
            --host=i686-linux-android
            "--prefix=${prefix}"
            --enable-shared=no
        BUILD_COMMAND "${SAGA_MAKE_EXECUTABLE}" -j
        INSTALL_COMMAND "${SAGA_MAKE_EXECUTABLE}" install
        BUILD_BYPRODUCTS "${prefix}/lib/libogg.a"
    )

    ExternalProject_Add(vorbis_external
        PREFIX "${CMAKE_BINARY_DIR}/external/vorbis"
        URL "https://ftp.osuosl.org/pub/xiph/releases/vorbis/libvorbis-1.3.2.tar.xz"
        URL_HASH "SHA256=43242e7c10253c0697da52dd0799bbad3ee1c392b99568f6c56a610712f1b916"
        DOWNLOAD_DIR "${CMAKE_SOURCE_DIR}/libs"
        BUILD_IN_SOURCE TRUE
        UPDATE_COMMAND ""
        PATCH_COMMAND
            "${CMAKE_COMMAND}"
            "-DCONFIG_SUB=<SOURCE_DIR>/config.sub"
            -P "${CMAKE_SOURCE_DIR}/cmake/patch-config-sub.cmake"
        CONFIGURE_COMMAND
            "${CMAKE_COMMAND}" -E env
            "CC=${CMAKE_C_COMPILER}"
            "CXX=${CMAKE_CXX_COMPILER}"
            "AR=${CMAKE_AR}"
            "RANLIB=${CMAKE_RANLIB}"
            "CFLAGS=${cflags}"
            "CPPFLAGS=${cflags}"
            "LDFLAGS=${ldflags}"
            "${SAGA_SH_EXECUTABLE}" <SOURCE_DIR>/configure
            --host=i686-linux-android
            "--prefix=${prefix}"
            --enable-shared=no
        BUILD_COMMAND "${SAGA_MAKE_EXECUTABLE}" -j
        INSTALL_COMMAND "${SAGA_MAKE_EXECUTABLE}" install
        BUILD_BYPRODUCTS
            "${prefix}/lib/libvorbis.a"
            "${prefix}/lib/libvorbisfile.a"
        DEPENDS ogg_external
    )

    add_custom_target(build_ogg_vorbis ALL DEPENDS vorbis_external)

    add_library(custom_ogg_vorbis INTERFACE)
    add_dependencies(custom_ogg_vorbis build_ogg_vorbis)
    target_include_directories(custom_ogg_vorbis SYSTEM INTERFACE "${prefix}/include")
    target_link_directories(custom_ogg_vorbis INTERFACE "${prefix}/lib")
    target_link_libraries(custom_ogg_vorbis INTERFACE "vorbisfile;vorbis;ogg")
endfunction()
