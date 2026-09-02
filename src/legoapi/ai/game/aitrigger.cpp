#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void AITriggerSetCreate(AITRIGGERSETSYS_s *, FLOWBOX_s *) {
}

void AITriggerSetSysReset(AITRIGGERSETSYS_s *system) {
    if (system == NULL) {
        return;
    }

    memset(system, 0, sizeof(*system));
    for (i32 i = 0; i < 64; ++i) {
        system->field_0x4280[i] = -1;
        system->field_0x42c0[i] = -1;
    }
    for (i32 i = 0; i < 32; ++i) {
        for (i32 j = 0; j < 8; ++j) {
            system->sets[i].trigger_indices[j] = -1;
        }
    }
}

void AITriggerSysAutoSetUp(WORLDINFO_s *, AITRIGGERSETSYS_s *) {
}

void AITriggerSetAddTrigger(AISYS_s *, AITRIGGERSET_s *, GIZMO_s *) {
}

void AITriggerSetSysProcess(AITRIGGERSETSYS_s *) {
}
