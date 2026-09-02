#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

struct SHOVESYS_s {
    u8 data[0x180];
    i32 active;
};

static SHOVESYS_s shovesys;

void AddShoveObject(nuhspecial_s *, i16) {
}

void ShoveObjectSysReset() {
    shovesys.active = 0;
}

void ShoveSystemCheckGameObject(GameObject_s *) {
}
