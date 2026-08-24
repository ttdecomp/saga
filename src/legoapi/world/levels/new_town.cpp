#include "legoapi/legoapi_types.h"

#include "globals.h"
#include "legoapi/gizmos/object/newblowup.h"

#include <stdio.h>

// ===========================================================================
// New Town (bonus; original episodeII.cpp TU member)
// ===========================================================================

void NewTown_Init(WORLDINFO_s *world) {
    LevGizmo[0] = GizmoFindByName(world->gizmo_sys, blowup_gizmotype_id, "newtown");
    char buf[0x18];
    i32 i = 1;
    for (;;) {
        sprintf(buf, "%i", i);
        GIZMOBLOWUP_s *g = GizmoBlowUp_FindByName(world, buf);
        if (g == NULL)
            break;
        g->field_0xa0 |= 2;
        i++;
    }
}

void NewTown_Reset(WORLDINFO_s *) {
}

void NewTown_Update(WORLDINFO_s *) {
}
