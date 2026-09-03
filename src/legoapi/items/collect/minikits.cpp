#include "decomp.h"
#include "globals.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

static GIZMOPICKUPSYS_s *GizmoPickupSys = &GizmoPickupSys_Game;

void GizmoPickups_InitSys(GIZMOPICKUPSYS_s *pickup_sys) {
    GizmoPickupSys = pickup_sys;
}

void MiniKits_Init(variptr_u *, variptr_u *) {
}

void CollectMinikit(nuvec_s *, char *, i32) {
}

void AllMiniKitsDone(AREASAVE_s *) {
}

void MiniKitDetector(nuvec_s *) {
}

void CharMiniKit_Draw(i32, numtx_s *, i32, float, float) {
}

void MiniKit_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void MiniKit_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void UpdateNewMiniKits(STATUSPACKET_s *, STATUS_STAGE_s *) {
}

void CollectAllMiniKits(AREASAVE_s *) {
}

void MiniKit_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void MiniKit_GameMsg_End(GAMEMESSAGE_s *) {
}

void ResetMinikitCounter() {
    minikitCounter_C = 0;
    minikitCounter_A = 0;
}

void AllMiniKits_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void AllMiniKits_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void SpecialMiniKits_Draw(WORLDINFO_s *) {
}

void AddStatusMiniKitParts() {
}

void SpecialMiniKits_Reset(WORLDINFO_s *world) {
    WORLDINFO_s *world_info = world;
    SPECIALMINIKITSYS_s *system = world_info->special_minikits;
    if (system == NULL || GizmoPickupSys->gizmo_type_id == -1) {
        return;
    }

    i32 count = system->count;
    SPECIALMINIKIT_s *item = system->items;
    if (count <= 0) {
        return;
    }

    i32 index = 0;
    for (;;) {
        item->pickup_gizmo = GizmoFindByName(world_info->gizmo_sys, gizmopickup_typeid, item->pickup_name);
        if ((item->flags & 0x20) != 0) {
            item->special_gizmo = GizmoFindByName(world_info->gizmo_sys, -1, item->special_name);
        }
        ++index;
        ++item;
        if (world_info->special_minikits->count <= index) {
            break;
        }
    }
}

void AllMiniKits_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void CharacterMiniKits_Dump(WORLDINFO_s *) {
}

void MiniKit_GameMsg_Update(GAMEMESSAGE_s *) {
}

void EffectOffProgress_Reset(LEVEL_PROGRESS_s *) {
}

void IncrementMinikitCounter(GameObject_s *) {
}

void EffectOffProgress_Update(LEVEL_PROGRESS_s *, char *, i32) {
}

void SpecialMiniKits_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
