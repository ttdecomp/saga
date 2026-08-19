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

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/legoapi_helpers.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/legoapi_misc_c.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/legoapi_misc.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/core/ai_sys.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/core/ai_sys_stubs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/editor/aieditor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/game/aipathcnxhelper.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/core/aisysall.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/game/aitrigger.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/androidbatman.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/animation.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/area.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/area_editor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/arcade.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/attracto.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/audio/audio.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/batarang.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/bolt.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/bolts.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/buffers.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/cable.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/camera.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/movement/carrying.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/core/character.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/core/characters.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/core/charconfig.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/charpivot.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/charplatforms.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/charshadows.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/config/cheat.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/config/cheats.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/chris.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/movement/climbing.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/base/collection.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/config/config.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/game/creature.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/editor/creature_editor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/credits.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/customise.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cutscenes/cutscene.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cutscenes/cutscene_stubs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cutscenes/cutscenes.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/detonator.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/doors/door.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/doors/doors.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/movement/dropinout.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/edlevelall.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/editor/edpath.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/fx/edsplines.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/editor/edtoolsall.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episode.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episodeI.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episodeII.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episodeIII.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episodeIV.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episodeV.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episodeVI.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/episodes.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/fx/explosions.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/light/fade.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/light/faders.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/combat/fighting.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/startup/game.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/fx/game_deb.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/base/game_object.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/core/gameai.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/gameanim.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/game/gameantinode.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/core/gamehint.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/audio/gamelib_ogg.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/gamelib_utilities.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/gameliball.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/gamemenuall.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/nugraph.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/base/apiobject.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/stream.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/core/gamemessages.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/gameobjects.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/input/gamepads.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/gamestatus_lsw.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cutscenes/gcutscn.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/base/gizactions.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizbombgen.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizbuildit.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/base/gizflow.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizforce.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/base/gizmessage.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizminicut.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/base/gizmo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/base/gizmo_sys.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizmoblowups.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizmopickup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizmopickups.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_gizactions.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_gizbuildits.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_gizforce.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_gizmopickups.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_gizobstacles.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_gizpanel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_gizturrets.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_grabber.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_grapples.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_hatmachine.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_lever.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_newblowup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_spinner.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_teleport.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/gizmos/gizmos_tubes.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizobstacle.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizpanel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizportal.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizrandom.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/gizspecial.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/giztimers.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmo/object/giztorpedo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/grabber.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/guidelines.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/hatmachine.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/core/hint.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/combat/hits.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/hub.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/core/hud.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/fx/inflate.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/input/input.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/movement/jumping.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/ledge.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/core/legoai.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/legoapi_status.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/level.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/levelconfig.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/levels.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/lever.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/light/lighting.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/editor/locator_editor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/lsw_hub_ai.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/startup/main.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/memory.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/menus.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/cutscenes/minicamcut.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/minikits.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/game/misc_a_game.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/mission.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/episodes/missions.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/motion/move.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/movies.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/net/netplay.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/net/network.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/objectsall.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/light/occlusion.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/core/panel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/fx/particles.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/fx/parts.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/placeable.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/core/playeritems.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/characters/core/players.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/plugs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/fx/pulses.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/movement/pushblocks.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/movement/pushing.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/input/qrand.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/audio/radio.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/render.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/render_stubs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/fx/ripples.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/combat/rope.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/rtl.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/config/saveload.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/base/scene.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/base/sceneobject.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/screen.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/doors/securitydoor.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/audio/sfx.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/shader.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/light/shadow.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/shard.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/shop.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/combat/shovesys.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/signal.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/simpletons.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/snake.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/system/socksysall.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/system/socksysall_stubs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/specialmoves.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/audio/specialsfx.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/speederchase.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/startup/startup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/starwars.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/ai/game/starwars_gameai.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/screens/store.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/streaks.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/suit.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/supportall.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/light/surfaces.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/tagging.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/techno.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/teleport.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/terrain.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/render/core/terrain_stubs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/menus/core/text.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/thermaldetonators.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/things.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/tightrope.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/input/timer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/input/timing.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/tm.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/collect/torpedo.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/items/objects/traffic.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/character/transform.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/tubes.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/misc/utilities.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/core/startup/virtual.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/actions/combat/whip.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/world/world.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/props/objects/zipup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/trigger/ai.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/traps/attractos.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door/door.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/fx/edgizshadowmachine.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/trigger/gizaimessage.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/traps/gizbombgen.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/object/gizbuildits.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/traps/gizforce.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/fx/gizmopickups.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/object/gizobstacles.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/object/gizpanel.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/transport/gizportal.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/trigger/gizrandom.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/trigger/gizspecial.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/trigger/giztimer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/traps/giztorpmachine.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/traps/gizturrets.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/transport/grapples.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/fx/guidelines.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/object/hatmachine.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/transport/ledges.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/object/lever.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/trigger/minicut.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/object/newblowup.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door/plugs.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door/push.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door/securitydoors.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/traps/shards.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/trigger/signals.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door/spinner.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/object/technos.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/transport/teleport.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/transport/tightropes.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/transport/tubes.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/src/legoapi/gizmos/door/zipups.cpp


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
