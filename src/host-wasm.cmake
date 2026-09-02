include(FetchContent)

if(CMAKE_VERSION VERSION_LESS 3.16)
    message(FATAL_ERROR "The WASM host build requires CMake 3.16 or newer")
endif()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(INSTALL_CMAKE_PACKAGE_MODULE OFF CACHE BOOL "" FORCE)
set(INSTALL_DOCS OFF CACHE BOOL "" FORCE)
set(INSTALL_PKG_CONFIG_MODULE OFF CACHE BOOL "" FORCE)

set(SDL_SHARED OFF CACHE BOOL "" FORCE)
set(SDL_STATIC ON CACHE BOOL "" FORCE)
set(SDL_INSTALL OFF CACHE BOOL "" FORCE)
set(SDL_TESTS OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)
set(SDL_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SDL_PTHREADS ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL3
    URL https://github.com/libsdl-org/SDL/releases/download/release-3.4.10/SDL3-3.4.10.tar.gz
    URL_HASH SHA256=12b34280415ec8418c864408b93d008a20a6530687ee613d60bfbd20411f2785
)
FetchContent_MakeAvailable(SDL3)

FetchContent_Declare(
    ogg
    URL https://github.com/xiph/ogg/releases/download/v1.3.6/libogg-1.3.6.tar.gz
    URL_HASH SHA256=83e6704730683d004d20e21b8f7f55dcb3383cdf84c0daedf30bde175f774638
)
FetchContent_MakeAvailable(ogg)

set(OGG_INCLUDE_DIR "${ogg_SOURCE_DIR}/include" CACHE PATH "" FORCE)
set(OGG_LIBRARY ogg CACHE STRING "" FORCE)
FetchContent_Declare(
    vorbis
    URL https://ftp.osuosl.org/pub/xiph/releases/vorbis/libvorbis-1.3.7.tar.xz
    URL_HASH SHA256=b33cc4934322bcbf6efcbacf49e3ca01aadbea4114ec9589d1b1e9d20f72954b
)
FetchContent_MakeAvailable(vorbis)

add_squish()
target_link_libraries(saga PRIVATE SDL3::SDL3 vorbisfile vorbis Ogg::ogg custom_squish)
target_compile_definitions(saga PRIVATE HOST_BUILD)
target_compile_options(saga PRIVATE -pthread)
set(SAGA_WASM_SHELL "${CMAKE_CURRENT_SOURCE_DIR}/src/host-wasm-shell.html")
target_link_options(saga PRIVATE
    -pthread
    -sPROXY_TO_PTHREAD=1
    -sOFFSCREEN_FRAMEBUFFER=1
    -sFULL_ES2=1
    -sMAX_WEBGL_VERSION=2
    -sALLOW_MEMORY_GROWTH=1
    -sINITIAL_MEMORY=268435456
    -sSTACK_SIZE=5242880
    -sASSERTIONS=1
    -sEXPORTED_RUNTIME_METHODS=FS,callMain
    --shell-file
    "${SAGA_WASM_SHELL}"
    --emrun
)
set_property(TARGET saga APPEND PROPERTY LINK_DEPENDS "${SAGA_WASM_SHELL}")
set_target_properties(saga PROPERTIES SUFFIX ".html")
