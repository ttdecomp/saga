#ifndef LEGOAPI_ITEMS_OBJECTS_GAMEOBJECTS_H
#define LEGOAPI_ITEMS_OBJECTS_GAMEOBJECTS_H

#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/numath/nuvec.h"

// Game-object lookup API (module legoapi/items/objects, gameobjects.cpp).
// GameObject_s is normally a complete type via apiobject.h; the returns here
// only need a declaration.

struct GameObject_s;

GameObject_s *FindGameObject(i32 id, u32 type, i32 a3, i32 a4, i32 a5);
GameObject_s *GetNamedGameObject(AISYS_s *aisys, char *name);

#endif
