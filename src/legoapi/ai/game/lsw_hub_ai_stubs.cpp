#include "decomp.h"
#include "legoapi/ai/game/lsw_hub_ai.h"
#include "nu2api/nucore/nustring.h"

struct AISYS_s;
struct AISCRIPT_s;

enum {
    HUB_AREA_COUNT = 10,
};

extern char *hub_areas[HUB_AREA_COUNT];

void *Condition_InHubAreaInit(AISYS_s *, char *argument, AISCRIPT_s *) {
    AI_HUB_AREA_ARGUMENT result = {};
    result.value = -1;
    if (argument != NULL) {
        for (isize area = 0; area < HUB_AREA_COUNT; ++area) {
            if (NuStrICmp(argument, hub_areas[area]) == 0) {
                result.value = area;
                break;
            }
        }
    }
    return result.pointer;
}
