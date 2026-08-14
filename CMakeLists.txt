cmake_minimum_required(VERSION 3.10)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

option(BUILD_FOR_HOST "Build for host system" OFF)

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

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

include("${CMAKE_SOURCE_DIR}/cmake/ogg-vorbis.cmake")
include("${CMAKE_SOURCE_DIR}/cmake/squish.cmake")

if (BUILD_FOR_HOST)  
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-stack-protector -msse2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-stack-protector -msse2")

    if (NOT CMAKE_HOST_WIN32)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -march=i686 -fsanitize=address")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -march=i686 -fsanitize=address")
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
set(SAGA_SOURCE_FILES

    ${CMAKE_CURRENT_SOURCE_DIR}/src/batman.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/globals.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/legoapi_helpers.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/legoapi_misc_c.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/aiscript.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/aisys.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/aistate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/gameai_actions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/aisys/cc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/gameai_bt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/gameai_cs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/gameai_d.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/gameai_grab.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/gameai_sockpar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/ai/gameai_tel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edanimall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edbriall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edui_fnt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edui.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edgraall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edlevelall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edmenucallbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edpartall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edptlall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edrtlall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edsplines.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edstubs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edtoolsall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edtoolsall_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edanimcallbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edbricallbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edgracallbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edpartcallbacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/edtools/edrtlcallbacks.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameapi/gui/apimenu.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gameframework/saveload.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/NewTerrain.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/crc/crc.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/nuwind/nuwind.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/java/jni_stub.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai_sys.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/area.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/batarang.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/bolts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/camera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/carrying.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/character.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cheat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cheats.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/climbing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/collection.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/credits.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/customise.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cutscene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cutscenes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/door.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/edlevelall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/edpath.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/fade.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/game_object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gameanim.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gameantinode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gamehint.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gamelib_ogg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gamelib_utilities.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gameobjects.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gamepads.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gcutscn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo_sys.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_gizactions.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_gizbuildits.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_gizforce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_gizmopickups.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_gizobstacles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_gizpanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_gizturrets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_grabber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_grapples.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_hatmachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_lever.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_newblowup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_spinner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_teleport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos_tubes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/hub.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/hud.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/input.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/legoapi_misc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/area_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/aieditor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/creature_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc_a_game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/legoapi_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/levelconfig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/locator_editor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/level.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/memory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/minicamcut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/mission.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/move.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/network.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/occlusion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/parts.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/placeable.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/players.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/qrand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/rtl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/scene.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/sceneobject.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/sfx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/shader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/socksysall.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/specialmoves.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/starwars.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/store.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/suit.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/tagging.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/terrain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/text.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/timer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/tm.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/virtual.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/ai.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/attractos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/edgizshadowmachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizaimessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizbombgen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizbuildits.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizforce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizmopickups.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizobstacles.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizpanel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizportal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizrandom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizspecial.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/giztimer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/giztorpmachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/gizturrets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/grapples.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/guidelines.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/hatmachine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/ledges.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/lever.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/minicut.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/newblowup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/plugs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/push.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/securitydoors.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/shards.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/signals.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/spinner.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/technos.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/teleport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/tightropes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/tubes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/zipups.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/game.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/startup.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legogame/target_android.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/NuRenderDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/NuScreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nucamera.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nucamvu0.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nudlist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nugscn.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/numtl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuocclusion.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuportal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuprim.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuqfnt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nurndr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nurndr_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuscreen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nushader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuspline.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nutex.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nutexanm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuvport.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/nuwater.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nugscn_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/numtl_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nuqfnt_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nurenderthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nurndr_android.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nuvertexformat_android.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/generic/nucamera_gen.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuandroid/nuphoneos.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuInputDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuInputManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuMemoryManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuMemoryPool.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuThreadManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/NuVirtualTouchDevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/deflate.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/implode.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nu2api_nucore_misc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuapi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nucore.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nucore_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nukeyboard.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nulist.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nulst.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/numem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/numemory.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/numouse.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nunew.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nupad.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nupad_interface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuptrblock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nurdpf.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nurdpi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nustring.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nustring_c.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuthread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nutime.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/nuvideo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/NuInputDevice_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/NuThread_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/bgproc_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nuapi_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/numemory_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nupad_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nutime_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nucore/android/nuvideo_android.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/NuFileDeviceAndroidAPK.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nudatfile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufile_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufile_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufilebase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufiledevice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufilepak.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/nufpar.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/numc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/tmclient.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/android/NuFileAndroidDeviceAPK.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nufile/android/nufile_android.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nufloat.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/numaths.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/numaths_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/numtx.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuplane.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuquat.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nurand.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nutrig.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nutrig.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nutrig_gen.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuvec.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/nuvec4.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numath/vumath.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numusic/numusic.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/numusic/sfx.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuplatform/nudevicespecs.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuplatform/nuplatform.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nu2api_nusound_misc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nu2api_nusound_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nu2api_nusound_types.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound3_include.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_buffer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_bus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_clock.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_decoder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_effect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_effect_doppler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_effect_fader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_effect_pitchramp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_listener.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_loader.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_loader_ogg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_loader_wav.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_memorymanager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_mixer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_routing.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_sample.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_source.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_streamer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_streamer_thread.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_system.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_voice.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_voice_android.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_weakptr.cpp
    #${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nusound/nusound_decoder_ogg.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechAutoJumpManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechAutofireAddon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechEdgeStopAddon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouch.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchBonusCavalryController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchButton.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchContextTasks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchDeathStarTurretController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchGestureBasedController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchGestureTracker.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchMainController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchMenuController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchPodraceController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchSpeederChaseBikeController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechInputTouchVirtualConsoleController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechJumpAutopilotAddon.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechObjectInterface.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechSystems.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/MechInputTouch/MechTouchUIElements.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/AndroidOBBUtils.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/CRC16.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/CRC16_plain.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/Ftp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/Network.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/TouchHacks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/Transporter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/V2SessionManager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/VirtualStackAllocator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gamelib/util/gamelib_util_misc.cpp

)

# add platform-specific source files and build rules
if (BUILD_FOR_HOST)
    list(APPEND SAGA_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host-tests/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host-tests/nuios/ios_graphics.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host-tests/nuios/nutex_host.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/host-tests/nuios/nutex_ios_ex.cpp
    )

    add_executable(saga ${SAGA_SOURCE_FILES})
    
    include(src/host.cmake)
else()
    list(APPEND SAGA_SOURCE_FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nutex_android.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nu3d/android/nutex_ios_ex.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/nu2api/nuandroid/ios_graphics.cpp
    )

    add_executable(saga ${SAGA_SOURCE_FILES})

    include(src/target.cmake)
endif()

# set common compile options and definitions
target_include_directories(saga PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_compile_options(saga PRIVATE -fno-exceptions -fno-rtti -Wno-write-strings -DANDROID)
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
    set_source_files_properties(${f} PROPERTIES COMPILE_DEFINITIONS "__FILENAME__=\"src/${FILENAME_REL}\"")
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
        COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/scripts/check_duplicate_definitions.py ${CMAKE_CURRENT_SOURCE_DIR}/src
        COMMAND ${CHECK_SYMBOLS_CMD}
        COMMENT "Running clang-tidy, duplicate-definition check, and symbol baseline check"
    )
else()
    message(WARNING "clang-tidy not found")
endif()
