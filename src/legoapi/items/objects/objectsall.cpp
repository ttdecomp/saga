#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Buck_Start(GameObject_s *, GameObject_s *) {
}

void DoBuckStart(GameObject_s *) {
}

void BlockInBlock(WORLDINFO_s *, pushblock_s *, i32, pushblock_s **) {
}

void Boulder_Kill(PART_s *, i32) {
}

void Boulder_Move(PART_s *, float) {
}

void Buck_MoveCode(GameObject_s *, i32) {
}

void FindNextBreak(unsigned char *, i32) {
}

void FindNearestBreak(unsigned char *, i32) {
}

void BuckStartExtra_LSW(GameObject_s *) {
}

void Buck_StartRiderJump(GameObject_s *, GameObject_s *) {
}

void SetEffectVisibility(char *, i32) {
}

void Conveyor_AdjustSpeed(nuvec_s *) {
}

void AddDevice(nufile_device_s *) {
}

extern NUGSCN *IconScene_FindById(i32 character_id);

// LevelObjects_InitForLevel @0x475630. Creates the runtime model table and
// resolves each registered model from the scene selected by its table kind.
// Found models are hidden in their source scene: their runtime handle is the
// template used by gizmos and other placed objects to draw them elsewhere.
void LevelObjects_InitForLevel(WORLDINFO_s *world) {
    usize aligned = ALIGN(world->giz_buffer.addr, 4);
    world->lev_objs = reinterpret_cast<LEVEL_OBJECT_RUNTIME *>(aligned);
    world->giz_buffer.addr = aligned + static_cast<usize>(LEVELOBJECTCOUNT) * sizeof(LEVEL_OBJECT_RUNTIME);
    memset(world->lev_objs, 0, static_cast<usize>(LEVELOBJECTCOUNT) * sizeof(LEVEL_OBJECT_RUNTIME));

    if (ObjTabList == NULL || LEVELOBJECTCOUNT <= 0) {
        return;
    }

    for (i32 object_index = 0; object_index < LEVELOBJECTCOUNT; ++object_index) {
        LEVELOBJECT &object_type = ObjTabList[object_index];
        LEVEL_OBJECT_RUNTIME &object = world->lev_objs[object_index];

        if (NuStrICmp(object_type.name, "power_up") == 0) {
            KNOBS = object_index;
        }

        NUGSCN *scene = NULL;
        switch (object_type.kind) {
            case LEVEL_OBJECT_SCENE_LEVEL:
                scene = world->current_gscn;
                break;

            case LEVEL_OBJECT_SCENE_AREA:
                scene = area_scene != NULL ? area_scene : world->current_gscn;
                break;

            case LEVEL_OBJECT_SCENE_CHARACTER_ICON:
                for (i32 character_id = 0; character_id < CHARCOUNT; ++character_id) {
                    scene = IconScene_FindById(character_id);
                    if (scene != NULL && NuSpecialFind(scene, &object.special, object_type.name, 1) != 0) {
                        goto object_resolved;
                    }
                }
                scene = big_icon_scene != NULL ? big_icon_scene : world->icons_gscn;
                break;

            case LEVEL_OBJECT_SCENE_SAVE_ICON:
                scene = saveicon_scene;
                break;

            case LEVEL_OBJECT_SCENE_VEHICLE:
                scene = vehicle_scene;
                break;

            case LEVEL_OBJECT_SCENE_BUTTON:
                scene = button_scene;
                break;

            case LEVEL_OBJECT_SCENE_THINGS:
            default:
                if (area_scene != NULL && world->area != NULL &&
                    (world->area->flags & AREAFLAG_OVERRIDE_THINGS_SCENE) != 0) {
                    if (NuSpecialFind(area_scene, &object.special, object_type.name, 1) != 0) {
                        goto object_resolved;
                    }
                }
                scene = things_scene;
                break;
        }

        if (scene != NULL) {
            NuSpecialFind(scene, &object.special, object_type.name, 1);
        }

    object_resolved:
        object.active = static_cast<u8>(NuSpecialExistsFn(&object.special));
        if (object.active != 0) {
            NuSpecialSetVisibility(&object.special, 0);
        }
    }
}

void EquivalentObjects_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
