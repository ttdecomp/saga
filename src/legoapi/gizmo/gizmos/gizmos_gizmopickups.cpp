#include "decomp.h"
#include "legoapi/gizmos/fx/gizmopickups.h"
#include "legoapi/legoapi_types.h"

void GizmoPickup_InBox(WORLDINFO_s *, i32, nuvec_s *, nuvec_s *) {
}

void GizmoPickups_Collide(WORLDINFO_s *, GameObject_s *, i32) {
}

void GizmoPickups_SetOnOff() {
}

void GizmoPickup_FindByName(WORLDINFO_s *, char *) {
}

void GizmoPickup_CollectCoin(WORLDINFO_s *, nuvec_s *, i32, i32, GameObject_s *, i32) {
}

void GizmoPickup_FindNearest(WORLDINFO_s *, nuvec_s *, float *) {
}

void GizmoPickups_TotalScore(void *) {
}

void GizmoPickup_BeenTurnedOn(GIZMOPICKUP_s *) {
}

void GizmoPickup_NumberOfType(WORLDINFO_s *, i32, char) {
}

void GizmoPickup_TurnOnPickup(GIZMOPICKUP_s *) {
}

// Static pickup collision helpers. Moved from gizmisc_stubs.cpp.

static __used__ float GizmoPickups_Collide2D(GameObject_s *) {
    return 0;
}

static __used__ GIZMOPICKUP_s *GizmoPickups_CollideList(GameObject_s *, GIZMOPICKUP_s *, int) {
    return nullptr;
}

static __used__ void Pup_CollectHeart(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectCharKit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectMinikit(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectPowerUp(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_CollectRedBrick(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32) {
}

static __used__ void Pup_UpdatePowerUp(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdateBlueCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

static __used__ void Pup_UpdatePurpleCoin(WORLDINFO_s *, GIZMOPICKUP_s *) {
}

extern void Pup_CollectCoin(WORLDINFO_s *, GIZMOPICKUP_s *, i32, GameObject_s *, i32);

u8 CoinTab[4] = {0, 1, 2, 3};

extern GIZMOPICKUPTYPE_s GizmoPickupType[10];

GIZMOPICKUPSYS_s GizmoPickupSys_Game = {
    {GizmoPickupType, CoinTab, {10, 0, 4}, 4, 2},
};

GIZMOPICKUPTYPE_s GizmoPickupType[10] = {
    {"Silver Coin", NULL, NULL, 0x00400473, 0x000a00b7, 0.045f, 0.045f, 10.0f, 0.0f, NULL,
     (GIZMOPICKUPCALLBACK)Pup_CollectCoin, -1, -1, NULL, NULL},
    {"Gold Coin", NULL, NULL, 0x00400467, 0x006400bf, 0.045f, 0.045f, 10.0f, 0.0f, NULL,
     (GIZMOPICKUPCALLBACK)Pup_CollectCoin, -1, -1, NULL, NULL},
    {"Blue Coin", "PickupCoinB", NULL, 0x00400462, 0x03e800c7, 0.045f, 0.045f, 6.0f, 0.0f,
     (GIZMOPICKUPCALLBACK)Pup_UpdateBlueCoin, (GIZMOPICKUPCALLBACK)Pup_CollectCoin, 8, -1, NULL, NULL},
    {"Purple Coin", "PickupCoinB", NULL, 0x00400470, 0x271000d5, 0.045f, 0.045f, 6.0f, 0.0f,
     (GIZMOPICKUPCALLBACK)Pup_UpdatePurpleCoin, (GIZMOPICKUPCALLBACK)Pup_CollectCoin, 8, -1, NULL, NULL},
    {"Minikit", "MK-Appear", "MK-Pickup", 0x0045006d, 0x000000ce, 0.125f, 0.15f, 2.0f, 0.05f, NULL,
     (GIZMOPICKUPCALLBACK)Pup_CollectMinikit, 20, -1, NULL, NULL},
    {"Heart", NULL, NULL, 0x00020068, 0x000000cb, 0.1f, 0.08f, 4.0f, 0.0f, NULL, (GIZMOPICKUPCALLBACK)Pup_CollectHeart,
     -1, -1, NULL, NULL},
    {"Red Brick", "MK-Appear", NULL, 0x00490072, 0x000000d2, 0.12f, 0.06f, 2.0f, 0.1f, NULL,
     (GIZMOPICKUPCALLBACK)Pup_CollectRedBrick, 99, -1, NULL, NULL},
    {"Charkit", "MK-Appear", "MK-Pickup", 0x00210063, 0x000000cf, 0.125f, 0.15f, 2.0f, 0.05f,
     (GIZMOPICKUPCALLBACK)Pup_CollectCharKit, (GIZMOPICKUPCALLBACK)Pup_CollectCoin, 20, -1, NULL, NULL},
    {"Torpedo", NULL, NULL, 0x00020074, 0x00000079, 0.1f, 0.1f, 5.0f, 0.0f, NULL, NULL, -1, -1, NULL, NULL},
    {"Power Up", "MK-Appear", NULL, 0x00010075, 0x000000d0, 0.125f, 0.125f, 2.0f, 0.05f,
     (GIZMOPICKUPCALLBACK)Pup_UpdatePowerUp, (GIZMOPICKUPCALLBACK)Pup_CollectPowerUp, -1, 209, NULL, NULL},
};
