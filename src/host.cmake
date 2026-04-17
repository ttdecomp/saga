find_package(PkgConfig REQUIRED)

# find and link dependencies
set(DEPENDENCIES vorbis vorbisfile sdl3 # libsquish
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

target_compile_definitions(saga PRIVATE HOST_BUILD)
