#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>
#include "nu2api/nucore/nulist.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void AIPathCalcExtents(AIPATH *path) {
    const f32 max_float = 3.402823466e+38f;
    path->bounds_min.x = max_float;
    path->bounds_min.y = max_float;
    path->bounds_min.z = max_float;
    path->bounds_max.x = -max_float;
    path->bounds_max.y = -max_float;
    path->bounds_max.z = -max_float;

    for (i32 node_index = 0; node_index < path->node_count; ++node_index) {
        AIPATHNODE &node = path->nodes[node_index];
        const f32 min_x = node.position.x - node.radius;
        const f32 min_z = node.position.z - node.radius;
        const f32 max_x = node.position.x + node.radius;
        const f32 max_z = node.position.z + node.radius;

        if (min_x < path->bounds_min.x) {
            path->bounds_min.x = min_x;
        }
        if (node.min_height < path->bounds_min.y) {
            path->bounds_min.y = node.min_height;
        }
        if (min_z < path->bounds_min.z) {
            path->bounds_min.z = min_z;
        }
        if (max_x > path->bounds_max.x) {
            path->bounds_max.x = max_x;
        }
        if (node.max_height > path->bounds_max.y) {
            path->bounds_max.y = node.max_height;
        }
        if (max_z > path->bounds_max.z) {
            path->bounds_max.z = max_z;
        }
    }

    NUVEC margin;
    NuVecSub(&margin, &path->bounds_max, &path->bounds_min);
    NuVecScale(&margin, &margin, 0.1f);
    NuVecSub(&path->bounds_min, &path->bounds_min, &margin);
    NuVecAdd(&path->bounds_max, &path->bounds_max, &margin);
}

i32 AIPathCheckExtents(AIPATH *path, NUVEC *position) {
    if (path->bounds_min.x > position->x || path->bounds_min.y > position->y || path->bounds_min.z > position->z ||
        position->x > path->bounds_max.x || position->y > path->bounds_max.y) {
        return 0;
    }
    return position->z <= path->bounds_max.z;
}

void pathEditorDrawNode(nuvec_s *, float, float, float, u32, numtl_s *, i32, i32) {
}

extern "C" {
    void (*AIPathCnxHelperSysInitFn)(WORLDINFO_s *) = NULL;
}

void AIPathCnxHelperSysReset(WORLDINFO_s *world, AIPATHCNXHELPERSYS_s *system) {
    if (system != NULL) {
        if (system->helper_count != 0) {
            memset(system->helpers, 0, system->helper_count * sizeof(*system->helpers));
            system->helper_count = 0;
        }
        if (AIPathCnxHelperSysInitFn != NULL) {
            AIPathCnxHelperSysInitFn(world);
        }
    }
}

void AIPathCnxHelperSys_Find(AIPATHCNXHELPERSYS_s *, GameObject_s *, AIPATHCNX_s *, unsigned char, unsigned char,
                             i32 (*)(AIPATHCNXHELPER_s *, GameObject_s *, AIPATHCNX_s *, unsigned char,
                                     unsigned char)) {
}

void AIPathCnxControlSysReset(AIPATHCNXCONTROLSYS_s *system) {
    if (system == NULL) {
        return;
    }

    system->available_controllers.head = NULL;
    system->available_controllers.tail = NULL;
    system->field_0x10 = 0;
    system->field_0x14 = 0;
    memset(system->controllers, 0, system->controller_count * sizeof(*system->controllers));
    for (i32 i = 0; i < system->controller_count; ++i) {
        NuLinkedListAppend(&system->available_controllers, &system->controllers[i].links);
    }
}

void AIPathCnxControlSysUpdate(AIPATHCNXCONTROLSYS_s *) {
}

void AIPathCnxControllerCreate(AIPATHCNXCONTROLSYS_s *, AISYS_s *, AIPATH_s *, char *, char *, i32, char *, i32, i32) {
}

void AIPathCnxControllerDestroy(AIPATHCNXCONTROLSYS_s *, AIPATHCNXCONTROLLER_s *) {
}

void AIPathCnxSetTemporaryBlock(AIPATH_s *, char *, char *, i32) {
}

void AIPathCnxHelperSys_AddHelper(AIPATHCNXHELPERSYS_s *, AIPATHCNX_s *, unsigned char, void *, unsigned char) {
}

void pathEditorDrawConnectionInfo(nuvec_s *, float, nuvec_s *, u32, i32) {
}

void AIPathCnxControllerSetOnRange(AIPATHCNXCONTROLLER_s *, i32, i32) {
}
