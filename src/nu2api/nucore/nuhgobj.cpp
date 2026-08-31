#include "nu2api/nucore/nuhgobj.h"

#include "nu2api/nucore/nustring.h"

extern "C" i32 NuHGobjGetLayerIndex(char *name, nuhgobj_s *object) {
    for (i32 i = 0; i < object->render_count; ++i) {
        if (NuStrICmp(name, object->render_parts[i].name) == 0) {
            return i;
        }
    }
    return -1;
}
