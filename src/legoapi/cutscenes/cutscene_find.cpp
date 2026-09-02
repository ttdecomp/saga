#include "legoapi/cutscenes/cutscenes.h"

#include "nu2api/nucore/nustring.h"

CUTINFO *CutScene_Find(CUTSYS *cutscene_system, char *name) {
    if (name == NULL || cutscene_system == NULL) {
        return NULL;
    }

    for (i32 i = 0; i < cutscene_system->count; ++i) {
        if (NuStrICmp(cutscene_system->cuts[i]->name, name) == 0) {
            return cutscene_system->cuts[i];
        }
    }
    return NULL;
}
