set(SAGA_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/batman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/nu2api.saga/numath/nuvec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nu2api.saga/numath/nufloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nu2api.saga/nufile/nufile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nu2api.saga/nustr/nustr.c
    ${CMAKE_CURRENT_SOURCE_DIR}/nu2api.saga/nuthread/nuthread.c
)

set(SAGA_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(saga ${SAGA_SOURCE_FILES})

set_target_properties(saga PROPERTIES ENABLE_EXPORTS TRUE)

target_include_directories(saga PUBLIC ${SAGA_INCLUDE_DIRS})