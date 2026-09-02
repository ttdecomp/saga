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

GameObject_s *FindGameObject(i32 id, u32 type, i32 a3, i32 a4, i32 a5);
GameObject_s *GetNamedGameObject(AISYS_s *aisys, char *name);
f32 GameShadow(GameObject_s *object, NUVEC *position, f32 scale, i32 flags);
i32 GameDrawCharacterModel(CHARACTERMODEL_s *model, ANIMPACKET_s *animation, NUMTX *matrix, NUMTX *secondary_matrix,
                           NUMTX *tertiary_matrix, NUMTX *auxiliary_matrix, GameObject_s *object, u32 flags);
extern "C" i32 InModelList(APICHARACTERMODELLIST_s *list, i32 id, i32 *out_index);

void GameAISysInit();
void GameAISysReset(struct AISYS_s *aisys);

// Original returns exit status; code in NuMain checks it.
i32 Game_Exit(i32 last_area);

#endif
