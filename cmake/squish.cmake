function(add_squish)
    if(CMAKE_HOST_WIN32)
        add_custom_command(
            OUTPUT "${CMAKE_SOURCE_DIR}/libs/squishlib/build.txt"
            COMMAND echo "Building Squish for Android x86..."
            COMMAND env bash -c "${CMAKE_SOURCE_DIR}/libs/build-libsquish-windows.sh"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/libs"
            COMMENT "Building Squish library"
            VERBATIM
        )
    else()
        add_custom_command(
            OUTPUT "${CMAKE_SOURCE_DIR}/libs/squishlib/build.txt"
            COMMAND echo "Building Squish for Android x86..."
            COMMAND env bash -c "${CMAKE_SOURCE_DIR}/libs/build-libsquish.sh"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/libs"
            COMMENT "Building Squish library"
            VERBATIM
        )
    endif()

    add_custom_target(build_squish ALL DEPENDS "${CMAKE_SOURCE_DIR}/libs/squishlib/build.txt")

    add_library(custom_squish INTERFACE)
    add_dependencies(custom_squish build_squish)

    target_include_directories(custom_squish INTERFACE "${CMAKE_SOURCE_DIR}/libs/squishlib/include")
    target_link_directories(custom_squish INTERFACE "${CMAKE_SOURCE_DIR}/libs/squishlib/lib")
    target_link_libraries(custom_squish INTERFACE "squish")

endfunction()
