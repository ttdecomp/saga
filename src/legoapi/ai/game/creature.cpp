#include "decomp.h"
#include "gameapi/ai/aisys/aisys.h"
#include "globals.h"
#include "legoapi/characters/core/character.h"
#include "legoapi/characters/core/players.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/mission.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

extern "C" {
    AIGROUP *CreateAIGroup(AISYS *system, u8 count_across, f32 x_spacing, f32 z_spacing, f32 max_speed);
    void AddToAIGroup(AIGROUP *group, GameObject_s *object);
}

void InitAICreatures(AISYS_s *system) {
    if (Mission_Active(NULL) != NULL || netclient != 0 || system == NULL) {
        return;
    }

    for (i32 creature_index = 0; creature_index < system->creature_count; ++creature_index) {
        AICREATURE &creature = system->creatures[creature_index];
        if ((creature.flags & 0x20) != 0 && g_lowEndLevelBehaviour != 0) {
            continue;
        }

        const i32 count = creature.count;
        if (count == 0) {
            continue;
        }

        AIGROUP *group = NULL;
        for (i32 member = 0; member < count; ++member) {
            if (NOAICREATURES != 0 && (GCDataList[creature.type].flags_090 & 0x40) == 0) {
                continue;
            }
            if (((static_cast<u64>(creature.active_mask) >> member) & 1) == 0) {
                continue;
            }

            GameObject_s *object = AddCreature(creature.type, 1);
            if (object == NULL) {
                continue;
            }

            object->apiobj.field_0x1f8 &= ~0x1000U;
            object->apiobj.field_0x1f4 |= 0x400;

            const u32 model_flags = apicharsys->char_data[creature.type].model_flags;
            if ((model_flags & 0x200) != 0) {
                object->apiobj.field_0x1f4 |= 0x404;
            } else if ((model_flags & 0x400) != 0) {
                object->apiobj.field_0x1f4 |= 0x401;
            }
            object->field_0x1050 |= (model_flags & 0x1000) != 0 ? 5 : 1;
            object->ai.field_0x134 = static_cast<u8>(creature_index);

            if (member == 0 && count > 1 && creature.start_stagger == 0.0f) {
                GAMECHARACTERDATA *character =
                    static_cast<GAMECHARACTERDATA *>(object->apiobj.character_data->field11_0x24);
                group = CreateAIGroup(system, creature.count_across, creature.x_spacing, creature.z_spacing,
                                      character->movement_speed);
            }
            if (group != NULL) {
                AddToAIGroup(group, object);
            }

            object->ai.area = creature.area;
            object->ai.locator = creature.locator;
            object->ai.respawn_locator = creature.respawn_locator;
            object->ai.creature_set = creature.set;
        }
    }

    system->has_done_reset = 0;
}

void ResetAICreature(GameObject_s *, AISYS_s *) {
}

void SnapCreaturePos(GameObject_s *, nuvec_s *, i32, AIPATHINFO_s *, i32) {
}

void ResetAICreatures(AISYS_s *) {
}

void CreatureCrate_Stop(PART_s *) {
}

void CreatureCrate_DrawFn(PART_s *) {
}

void CreatureCrate_MoveFn(PART_s *, float) {
}

void SpawnCreatureFromCrate(GameObject_s *, float, float) {
}

void SpawnMeleeCreatureType(i32) {
}

void AlertSurroundingCreatures(GameObject_s *, nuvec_s *) {
}
