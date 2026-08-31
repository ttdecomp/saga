if(EMSCRIPTEN)
    include(src/host-wasm.cmake)
    return()
endif()

find_package(PkgConfig REQUIRED)

# find and link dependencies
set(DEPENDENCIES vorbis vorbisfile sdl3
)

# use angle on windows
if(CMAKE_HOST_WIN32)
    list(APPEND DEPENDENCIES angleproject)
else()
    list(APPEND DEPENDENCIES glesv2 egl)
endif()

foreach(dep IN LISTS DEPENDENCIES)
    pkg_check_modules(${dep} REQUIRED ${dep})
endforeach()

foreach(dep IN LISTS DEPENDENCIES)
    target_include_directories(saga PRIVATE ${${dep}_INCLUDE_DIRS})
    target_link_libraries(saga PRIVATE ${${dep}_LIBRARIES})
endforeach()

# Some distributions ship a broken libsquish.pc that advertises
# `-llibsquish` even though the library is named `libsquish`. Use pkg-config
# for its include path and let CMake resolve the actual library filename.
pkg_check_modules(libsquish REQUIRED libsquish)
set(HOST_BUNDLED_SQUISH_LIBRARY "${CMAKE_SOURCE_DIR}/libs/squishlib/lib/libsquish.a")
if(EXISTS "${HOST_BUNDLED_SQUISH_LIBRARY}")
    set(HOST_SQUISH_LIBRARY "${HOST_BUNDLED_SQUISH_LIBRARY}")
    target_include_directories(saga PRIVATE "${CMAKE_SOURCE_DIR}/libs/squishlib/include")
else()
    find_library(HOST_SQUISH_LIBRARY NAMES squish libsquish HINTS ${libsquish_LIBRARY_DIRS} REQUIRED)
    target_include_directories(saga PRIVATE ${libsquish_INCLUDE_DIRS})
endif()
target_link_libraries(saga PRIVATE ${HOST_SQUISH_LIBRARY})

target_compile_definitions(saga PRIVATE HOST_BUILD)
