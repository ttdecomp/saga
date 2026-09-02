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

extern "C" i16 FindPlatInst(void *);

void SkinPlatform(terrsitu_s *, unsigned char *, PLATSKININFO *) {
}

void Platform_Init(WORLDINFO_s *) {
}

#ifdef __EMSCRIPTEN__
i32 PlatformChecks(i32, nuvec_s *) {
    return 0;
}
#else
void PlatformChecks(i32, nuvec_s *) {
}
#endif

void Platform_Reset(WORLDINFO_s *) {
    NuSpecialSetVisibility(&LevHSpecial[0], 0);
}

void PlatformConnect(char *, nuvec_s *, nuvec_s *, i32) {
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
