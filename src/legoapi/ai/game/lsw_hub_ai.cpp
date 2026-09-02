#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/gizmo/base/GizObstacleObjectInterface.h"
#include "legoapi/gizmo/base/gizmo.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/ai/core/ai_sys_stubs.h"
#include "legoapi/world/world.h"
#include "legoapi/world/world_shared.h"
#include "nu2api/nu3d/nutex.h"

#include <stdio.h>
#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

struct HUBAI_s {
    AIAREA *areas[10];
    AIPATH *paths[10];
    AILOCATOR *area_locators[10][4];
    i16 area;
    i16 unknown_0x0f2;
    i16 active_counter;
    i16 counter_count;
    u8 unknown_0x0f8[0x104 - 0xf8];
    GIZAIMESSAGE_s *serving_customer_message;
    GIZAIMESSAGE_s *area_message;
    AILOCATOR *counter_a[4];
    AILOCATOR *counter_b[4];
    void *customers[8];
    AILOCATOR *serve_player;
};

DECOMP_ASSERT(sizeof(HUBAI_s) == 0x150, "HUBAI_s size");

HUBAI_s hub_ai = {};

char *hub_areas[10] = {"MAINROOM", "EPISODE1", "EPISODE2", "EPISODE3", "EPISODE4",
                       "EPISODE5", "EPISODE6", "JUNKYARD", "BONUS",    "BOUNTY"};

extern GIZOBSTACLE_s *LevGizObst[8];

void LSW_Hub_InitAI(WORLDINFO_s *world) {
    memset(&hub_ai, 0, sizeof(hub_ai));

    char name[32];
    for (i32 area = 0; area < 10; ++area) {
        hub_ai.areas[area] = AISysFindArea(world->ai_sys, hub_areas[area]);
        hub_ai.paths[area] = AISysFindPath(world->ai_sys, hub_areas[area]);

        for (i32 locator = 0; locator < 4; ++locator) {
            sprintf(name, "%s_%d", hub_areas[area], locator + 1);
            hub_ai.area_locators[area][locator] = AIPathFindLocator(world->ai_sys, name);
        }
    }

    hub_ai.area = -1;
    hub_ai.serving_customer_message = SetGizAIMessage(gizaimessagesys, "ServingCustomer", 0.0f, NULL);
    hub_ai.area_message = SetGizAIMessage(gizaimessagesys, "Area", 0.0f, NULL);
    hub_ai.counter_count = 0;

    for (i32 counter = 0; counter < 4; ++counter) {
        sprintf(name, "CounterA_%d", counter);
        hub_ai.counter_a[counter] = AIPathFindLocator(world->ai_sys, name);
        sprintf(name, "CounterB_%d", counter);
        hub_ai.counter_b[counter] = AIPathFindLocator(world->ai_sys, name);
        if (hub_ai.counter_a[counter] != NULL && hub_ai.counter_b[counter] != NULL) {
            hub_ai.counter_count++;
        }
    }

    hub_ai.serve_player = AIPathFindLocator(world->ai_sys, "ServePlayer");
    hub_ai.active_counter = 0;
    for (i32 i = 0; i < 8; ++i) {
        hub_ai.customers[i] = NULL;
    }
}

void LSW_Hub_ResetAI(WORLDINFO_s *world) {
    hub_ai.area = -1;
    LevGizObst[0] = GizObstacle_FindByName(world->giz_obstacle_sys, "BandDoor_Open");
}

void LSW_Hub_UpdateAI(WORLDINFO_s *world) {
    if (hub_character_ready != -1) {
        SetLevelSfxBits(world);
        hub_character_ready = -1;
    }
}
