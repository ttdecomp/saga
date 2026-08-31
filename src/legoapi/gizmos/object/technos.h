#pragma once

#include "decomp_assert.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "nu2api/numath/nuvec.h"

struct TECHNO_CONFIG {
    f32 interaction_time;
    i16 floor_target_object_id;
    i16 active_effect_id;
    i16 success_effect_id;
    i16 failure_effect_id;
    NUVEC idle_offset;
    NUVEC active_offset;
    NUVEC complete_offset;
    i16 activation_effect_id;
    i16 completion_effect_id;
};

DECOMP_ASSERT(sizeof(TECHNO_CONFIG) == 0x34, "TECHNO_CONFIG size");

extern TECHNO_CONFIG TechnoSys;

extern i32 techno_gizmotype_id;

#ifdef __cplusplus

typedef struct TECHNO_s TECHNO;

ADDGIZMOTYPE *Technos_RegisterGizmo(i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
