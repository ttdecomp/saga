#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/items/objects/gameobjects.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" i16 FindPlatInst(i32);
extern TERRSET *CurTerr;
extern TERRAIN_TRACK_SLOT *CurTrackInfo;
extern i16 castnum;

TERRAIN_TRACK_SLOT *AllocTerrId();

void SkinPlatform(terrsitu_s *, unsigned char *, PLATSKININFO *) {
}

void Platform_Init(WORLDINFO_s *world) {
    NuSpecialFind(world->current_gscn, &LevHSpecial[0], const_cast<char *>("slave1_level"), 0);
}

i32 PlatformChecks(i32 count, nuvec_s *) {
    return count;
}

void Platform_Reset(WORLDINFO_s *) {
    NuSpecialSetVisibility(&LevHSpecial[0], 0);
}

void PlatformConnect(char *track_id, nuvec_s *position_delta, nuvec_s *movement_delta, i32 platform_index) {
    if (CurTrackInfo == NULL) {
        CurTrackInfo = AllocTerrId();
        if (CurTrackInfo != NULL) {
            CurTrackInfo->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
            CurTrackInfo->platform_index = static_cast<i16>(platform_index);
            CurTrackInfo->id = track_id;
            CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
            position_delta->y = 0.0f;
            movement_delta->y = 0.0f;
        }
    } else {
        CurTrackInfo->flags |= TERRAIN_TRACK_FLAG_CONNECTED;
        CurTrackInfo->platform_index = static_cast<i16>(platform_index);
        CurTrackInfo->platform_contact_state = TERRAIN_TRACK_CONTACT_ACTIVE;
    }

    castnum = CurTerr->platforms[platform_index].terrain_group_index;
}

void SkinPlatformSize(i32, unsigned char *, PLATSKININFO *) {
}

void CharPlatforms_Reset(CHARPLATFORMSYS_s *system) {
    if (system == NULL) {
        return;
    }

    for (i32 i = 0; i < HIGHGAMEOBJECT; ++i) {
        if ((Obj[i].apiobj.field_0x1f8 & 1) != 0) {
            Obj[i].field_0x107c = -1;
        }
    }

    for (i32 i = 0; i < system->platform_count; ++i) {
        CHARPLATFORM_s *platform = &system->platforms[i];
        NuSpecialSetVisibility(&platform->special, 0);
        platform->platform_id = FindPlatInst(NuSpecialGetInstanceix(&platform->special));
        platform->object = NULL;
        if (platform->platform_id != -1) {
            GameObject_s *object = FindGameObject(platform->object_id, 0, 1, 0, 1);
            if (object != NULL) {
                object->field_0x107c = platform->platform_id;
                platform->object = object;
            }
        }
    }
}

void CharPlatforms_Update(CHARPLATFORMSYS_s *) {
}

void FindReflectionNoPlatforms(nuvec_s *) {
}

void CharPlatform_FindObjFromPlatID(CHARPLATFORMSYS_s *, i32) {
}

void CharPlatforms_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
