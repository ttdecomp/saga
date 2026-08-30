#include "decomp.h"
#include "globals.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <new>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void CollectPowerUp(GameObject_s *, nuvec_s *, u16, i32) {
}

void FindMtlInHGObj(nugscn_s *, i32) {
}

// CreateThingManager @0x4e8b50: allocate a 0x24-byte GameThingManager from the
// MemoryManager pool (zeroed) and construct it with room for 4 things. The
// ctor stores the object in theGameThings.
void CreateThingManager() {
    void *obj = reinterpret_cast<MemoryManager *>(theMemoryManager)->AllocPool(0x24, 1);
    new (obj) GameThingManager(4);
}
