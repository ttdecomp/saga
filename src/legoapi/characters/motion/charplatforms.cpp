#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern TERRSET *CurTerr;
extern TERRAIN_TRACK_SLOT *CurTrackInfo;
extern i16 castnum;

TERRAIN_TRACK_SLOT *AllocTerrId();

void SkinPlatform(terrsitu_s *, unsigned char *, PLATSKININFO *) {
}

void Platform_Init(WORLDINFO_s *world) {
    NuSpecialFind(world->current_gscn, reinterpret_cast<void **>(&LevHSpecial[0]), const_cast<char *>("slave1_level"),
                  0);
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

void CharPlatforms_Reset(CHARPLATFORMSYS_s *) {
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
