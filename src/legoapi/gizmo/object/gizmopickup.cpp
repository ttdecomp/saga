#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/area.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void ReleaseTakeOver(GameObject_s *, i32) {
}

void SuperCounters_Reset(i32 area_index) {
    if (area_index != -1) {
        AREADATA *area = &ADataList[area_index];
        SUPERCOUNTER *super_counters = area->super_counters;
        if (super_counters != NULL && area->super_counter_count != 0) {
            for (i32 i = 0; i < area->super_counter_count; ++i) {
                super_counters[i].reset_value = 0;
            }
        }
    }
}

void UpdatePickupFlicker() {
}

void RegisterTakeOverObject(GameObject_s *) {
}

void SuperCounters_FindPickup(WORLDINFO_s *, GIZMO_s *, nuvec_s *, SUPERCOUNTERPICKUP **) {
}

void SuperCounter_AnyCollected(SUPERCOUNTER *, WORLDINFO_s *) {
}

void SuperCounters_FixUpGizmos(WORLDINFO_s *) {
}

void SuperCounters_ResetProcessed(WORLDINFO_s *world) {
    if (world->area != NULL && world->area->super_counters != NULL && world->area->super_counter_count != 0) {
        for (i32 i = 0; i < world->area->super_counter_count; ++i) {
            world->area->super_counters[i].processed_flags &= ~2;
        }
    }
}

void SuperCounter_ActivateGizmoPickup(GIZMO_s *, GIZMOPICKUP_s *) {
}

void SuperCounter_FindFromNameAndLevel(char *, WORLDINFO_s *, SUPERCOUNTERPICKUP **) {
}
