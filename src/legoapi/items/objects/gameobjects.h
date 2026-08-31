#ifndef LEGOAPI_ITEMS_OBJECTS_GAMEOBJECTS_H
#define LEGOAPI_ITEMS_OBJECTS_GAMEOBJECTS_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

// Game-object lookup API (module legoapi/items/objects, gameobjects.cpp).
// GameObject_s is normally a complete type via apiobject.h; the returns here
// only need a declaration.

struct GameObject_s;
struct AISYS_s;
struct WORLDINFO_s;

struct GAMEFOG_STATE {
    f32 high_quality_colour_channels[4];
    f32 high_quality_start;
    f32 high_quality_end;
    f32 low_quality_start;
    f32 low_quality_end;
    f32 high_quality_density;
    u32 colour;
    f32 low_quality_density;
    f32 low_quality_colour_channels[4];
    u32 low_quality_colour;
};

DECOMP_ASSERT(sizeof(GAMEFOG_STATE) == 0x40, "GAMEFOG_STATE size");

extern GAMEFOG_STATE GameFog;

void *GameBufferAlloc(VARIPTR *buf, VARIPTR *buf_end, i32 size);

GameObject_s *FindGameObject(i32 id, u32 type, i32 a3, i32 a4, i32 a5);
i32 Game_IgnoreInput();
GameObject_s *GetNamedGameObject(AISYS_s *aisys, char *name);
APIOBJECT_s *GameAPIOBJECTFromObjID(u8 object_id);
i32 GameDrawCharacterModel(CHARACTERMODEL_s *model, ANIMPACKET_s *animation, NUMTX *matrix, NUMTX *secondary_matrix,
                           NUMTX *tertiary_matrix, NUMTX *auxiliary_matrix, GameObject_s *object, u32 flags);
extern "C" i32 InModelList(APICHARACTERMODELLIST_s *list, i32 id, i32 *out_index);

void GameAISysInit();
void GameAISysReset(struct AISYS_s *aisys);
void GameAnimSys_Update(GAMEANIMSYS_s *system);
void GameAudio_SetActionMusicTimes(f32 fade_in_time, f32 fade_out_time);

// Original returns exit status; code in NuMain checks it.
i32 Game_Exit(i32 last_area);

#endif
