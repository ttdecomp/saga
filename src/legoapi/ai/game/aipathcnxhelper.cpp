#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>
#include "nu2api/nucore/nulist.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void AIPathCalcExtents(AIPATH_s *) {
}

void AIPathCheckExtents(AIPATH_s *, nuvec_s *) {
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
