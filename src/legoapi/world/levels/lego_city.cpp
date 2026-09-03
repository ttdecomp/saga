#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"

static u8 prevOnTaunTaun;
static u8 prevOnTractor;
static u8 prevOnMoonCar;
static u8 prevOnTownCar;

void LegoCity_Init(WORLDINFO_s *) {
}

void LegoCity_Reset(WORLDINFO_s *world) {
    prevOnTaunTaun = 0;
    prevOnTractor = 0;
    prevOnMoonCar = 0;
    prevOnTownCar = 0;

    GIZMOPICKUP_s *pickup = world->pickup_sys->pickups;
    if (pickup == NULL) {
        return;
    }
    if (world->pickup_sys->pickup_count <= 0) {
        return;
    }

    for (i32 pickup_index = 0; pickup != NULL && pickup_index < world->pickup_sys->pickup_count;
         ++pickup_index, ++pickup) {
        if ((pickup->runtime_flags & 8) == 0) {
            switch (pickup->type_id) {
                case 3:
                case 4:
                case 5:
                case 6:
                    pickup->collected = 0;
                    break;
                default:
                    break;
            }
        }
    }
}

void LegoCity_Update(WORLDINFO_s *) {
}
