#include "decomp.h"
#include "batman.h"
#include "globals.h"
#include "legoapi/props/doors/door.h"
#include "legoapi/world/area.h"
#include "legoapi/world/level.h"
#include "legoapi/world/mission.h"
#include "legoapi/world/world.h"

// Level-system stubs that have not yet been split into a subsystem file.

i32 GetTableLocator(void) {
    return 0;
}

void getSpawnLocator(float, char *) {
}

void NewLevelFromMenu(LEVELDATA_s *level, i32 menu_id, i32 menu_y, i32) {
    if (no_more_loads == 0) {
        i32 *abort = &abort_load;
        no_more_loads = 1;
        *abort = 1;
    }

    Door_Reset();
    NewLData = level;
    new_level_from_menu = 1;
    newlevelfrommenu_newmenuid = menu_id;
    newlevelfrommenu_newmenuy = menu_y;

    if (HUB_ADATA != NULL && WORLD->area == HUB_ADATA) {
        return;
    }

    if (SuperStory != 0) {
        hub_from_superstory = SuperStoryEpisode;
        return;
    }

    MISSIONDATA *mission = Mission_Active(NULL);
    if (mission != NULL && NewLData != NULL && NewLData == HUB_LDATA) {
        hub_from_mission = static_cast<i8>(mission->count);
    }
}

i32 GetCounterLocator(i32) {
    return 0;
}

void OffPlat(i32) {
}
