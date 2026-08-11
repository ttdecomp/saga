#include "legoapi/gameobject.h"

struct TerrainEntry {
    u32 unknown;
    u32 flags;
    u32 value;
};

TerrainEntry TerLayer[32];
TerrainEntry TerSurface[32];
i32 LevSafePlatID[2];
i32 IDLESPEEDINNARROWSOCKSONLY;
i32 disable_narrow_socks;

void InitSurfaceInfo(GameObject_s *) {}

i32 SetObjOnSurface(GameObject_s *, i32) {
    return 0;
}

bool NoLayerKill(GameObject_s *) {
    return false;
}

i32 ObjInNarrowSock(GameObject_s *, void *, i32) {
    return 0;
}

bool MosEisleyC_PastBarrier(GameObject_s *) {
    return false;
}
