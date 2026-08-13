#include "legoapi/world_shared.h"

#include "decomp.h"

void ClearGameObjects(APIOBJECTSYS_s *api_object_sys) {
    (void)api_object_sys;
}

GameObject_s *AddGameObject(i32 id) {
    (void)id;
    UNIMPLEMENTED();
    return NULL;
}
i32 InitCreature(GameObject_s *obj, i32 id, i32 param) {
    (void)obj;
    (void)id;
    (void)param;
    UNIMPLEMENTED();
    return 0;
}

void InitGameObjectLights(void) {
}
