#include "decomp.h"
#include "globals.h"
#include "legoapi/gizmos/trigger/gizrandom.h"
#include "legoapi/legoapi_types.h"
#include "legoapi/world/world.h"
#include "nu2api/nu3d/nutex.h"
#include "nu2api/nucore/nustring.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void randyfloat() {
}

void createGizRandom(void *, i32 output_count, i32 *output_weights, char *name) {
    WORLDINFO *world = WorldInfo_CurrentlyLoading();
    if (world == NULL || world->giz_randoms == NULL || world->giz_randoms->count == world->giz_randoms->capacity) {
        return;
    }

    GIZRANDOM *random = &world->giz_randoms->randoms[world->giz_randoms->count];
    random->output_count = output_count;
    for (i32 index = 0; index < output_count; ++index) {
        random->output_weights[index] = output_weights[index];
    }
    NuStrNCpy(random->name, name, sizeof(random->name));
    ++world->giz_randoms->count;
    AddGizmo(world->gizmo_sys, gizrandom_gizmotype_id, NULL, random);
}

i32 RandomIDFromFlags(u32, u32, i32, APICHARACTERMODELLIST_s *, i32) {
    return -1;
}
