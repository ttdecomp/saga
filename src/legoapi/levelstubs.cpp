#include "legoapi/world_shared.h"

extern "C" void NuRndrInitWorld(void); // Defined in nurndr_android.c

void LevObj_FixUpPlatIDs(WORLDINFO *world) {
    (void)world;
}
void Doors_Init(WORLDINFO *world) {
    (void)world;
}
void LevelSplines_InitForLevel(WORLDINFO *world) {
    (void)world;
}
void LevelObjects_InitForLevel(WORLDINFO *world) {
    (void)world;
}
void BoltTypes_Init(WORLDINFO *world) {
    (void)world;
}
void BoltTypes_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void EquivalentObjects_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Teleports_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Doors_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Faders_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void CharPlatforms_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Grabber_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void Pulses_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void TrafficAnimSys_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void SpecialMiniKits_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void GizForceSFX_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void RippleEffects_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void PortalDoors_Configure(WORLDINFO *world, char *config) {
    (void)world;
    (void)config;
}
void LoadLights(WORLDINFO *world, char *path) {
    (void)world;
    (void)path;
}
void *GameAnimSys_Create(VARIPTR *buf, VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}
void *GameAntnode_CreateSys(WORLDINFO *world, VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)world;
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}

// --- Extern "C": functions with C linkage in original ---
extern "C" {
    void SockSys_Configure(void *sock_sys, char *config, i32 param, void *buf, void *buf_end, void *gscn) {
        (void)sock_sys;
        (void)config;
        (void)param;
        (void)buf;
        (void)buf_end;
        (void)gscn;
    }
    void SockSys_GenerateData(void *sock_sys, void *buf, void *buf_end) {
        (void)sock_sys;
        (void)buf;
        (void)buf_end;
    }
    void rtlResetDynamic(void) {
    }
    void SetPartRTLSet(i32 rtl_set) {
        (void)rtl_set;
    }
    i32 rtlFindByUserId(i32 rtl_set, i32 user_id) {
        (void)rtl_set;
        (void)user_id;
        return -1;
    }
    void rtlGetDirection(i32 rtl_set, i32 id, void **out) {
        (void)rtl_set;
        (void)id;
        (void)out;
    }
    void NewMenu(i32 menu_id, i32 menu_y, i32 param3) {
        (void)menu_id;
        (void)menu_y;
        (void)param3;
    }
} // extern "C"
