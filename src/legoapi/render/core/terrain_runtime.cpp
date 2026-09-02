#include "legoapi/legoapi_types.h"

extern TERRSET *CurTerr;

extern "C" {
    extern f32 timeincrement;

    void DebrisSetTimeIncrement(f32 increment) {
        timeincrement = increment;
    }

    void TerrainTrackFlush(void) {
        if (CurTerr == NULL) {
            return;
        }

        for (i32 slot_index = 0; slot_index < TERRAIN_TRACK_SLOT_COUNT; ++slot_index) {
            CurTerr->track_slots[slot_index].id = NULL;
        }
    }
}
