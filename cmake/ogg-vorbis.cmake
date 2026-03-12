function(add_ogg_vorbis)
    if(CMAKE_HOST_WIN32)
        add_custom_command(
            OUTPUT "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/build.txt"
            COMMAND echo "Building Ogg Vorbis for Android x86..."
            COMMAND env bash -c "${CMAKE_SOURCE_DIR}/libs/build-ogg-vorbis-windows.sh"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/libs"
            COMMENT "Building Ogg Vorbis library"
            VERBATIM
        )
    else()
        add_custom_command(
            OUTPUT "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/build.txt"
            COMMAND echo "Building Ogg Vorbis for Android x86..."
            COMMAND env bash -c "${CMAKE_SOURCE_DIR}/libs/build-ogg-vorbis.sh"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/libs"
            COMMENT "Building Ogg Vorbis library"
            VERBATIM
        )
    endif()

    add_custom_target(build_ogg_vorbis ALL DEPENDS "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/build.txt")

    add_library(custom_ogg_vorbis INTERFACE)
    add_dependencies(custom_ogg_vorbis build_ogg_vorbis)

    target_include_directories(custom_ogg_vorbis SYSTEM INTERFACE  "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/include")
    target_link_directories(custom_ogg_vorbis INTERFACE "${CMAKE_SOURCE_DIR}/libs/ogg-vorbis/lib")
    target_link_libraries(custom_ogg_vorbis INTERFACE "vorbisfile;vorbis;ogg")

endfunction()
