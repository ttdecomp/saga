# find dependencies
find_package(PkgConfig REQUIRED)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DEPENDENCIES vorbis vorbisfile sdl3 # libsquish
)

add_library(nuios SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/nuios/ios_graphics.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nuios/nutex_host.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nuios/nutex_ios_ex.cpp
)
target_include_directories(nuios PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/nuios")
target_link_libraries(nuios PRIVATE saga_headers)
target_compile_definitions(nuios PRIVATE HOST_BUILD)
target_compile_definitions(nuios PRIVATE NUIOS_BUILD)
if (CMAKE_HOST_WIN32)
    # allow undefined references to be resolved at runtime
    target_link_options(nuios PRIVATE "-Wl,--allow-shlib-undefined")
endif()

# use angle on windows
if(CMAKE_HOST_WIN32)
    list(APPEND DEPENDENCIES angleproject)
else()
    list(APPEND DEPENDENCIES glesv2 egl)
endif()

foreach(dep IN LISTS DEPENDENCIES)
    pkg_check_modules(${dep} REQUIRED ${dep})
endforeach()

set(TESTS load window)
foreach(test IN LISTS TESTS)
    set(NAME test_${test})

    add_executable(${NAME} ${CMAKE_CURRENT_SOURCE_DIR}/${test}.cpp)

    target_link_libraries(${NAME} PRIVATE saga)
    target_compile_features(${NAME} PRIVATE cxx_std_23)

    foreach(dep IN LISTS DEPENDENCIES)
        target_include_directories(${NAME} PRIVATE ${${dep}_INCLUDE_DIRS})
        target_link_libraries(${NAME} PRIVATE ${${dep}_LIBRARIES})
    endforeach()

    target_compile_definitions(${NAME} PRIVATE HOST_BUILD)
endforeach()
