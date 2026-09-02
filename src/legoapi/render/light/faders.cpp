#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nu3d/nuspecial.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Faders_Draw(WORLDINFO_s *) {
}

void Faders_Reset(WORLDINFO_s *world) {
    if (world->faders != NULL && world->fader_count > 0) {
        for (i32 i = 0; i < world->fader_count; ++i) {
            NuSpecialSetVisibility(&world->faders[i].special, 0);
        }
    }
}

void Faders_Configure(WORLDINFO_s *world, char *config) {
    (void)world;
    (void)config;
}
