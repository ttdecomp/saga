#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nuspecial.h"
#include "nu2api/numath/numtx.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

struct ShoveObject {
    nuhspecial_s *special;
    NUVEC position;
    f32 radius;
    i16 character_id;
    i16 padding;
};

struct ShoveObjectSystem {
    ShoveObject objects[16];
    i32 count;
};

static ShoveObjectSystem shovesys;

DECOMP_ASSERT(sizeof(ShoveObject) == 0x18, "ShoveObject size");
DECOMP_ASSERT(sizeof(ShoveObjectSystem) == 0x184, "ShoveObjectSystem size");

void AddShoveObject(nuhspecial_s *special, i16 character_id) {
    if (NuSpecialExistsFn(special) != 0) {
        if (shovesys.count < 16) {
            shovesys.objects[shovesys.count].special = special;
            shovesys.objects[shovesys.count].character_id = character_id;
            NuSpecialGetRadius(special, &shovesys.objects[shovesys.count].position,
                               &shovesys.objects[shovesys.count].radius);
            NuVecMtxTransform(&shovesys.objects[shovesys.count].position, &shovesys.objects[shovesys.count].position,
                              NuSpecialGetDrawMtx(special));
            shovesys.count++;
        }
    }
}

void ShoveObjectSysReset() {
    shovesys.count = 0;
}

void ShoveSystemCheckGameObject(GameObject_s *) {
}
