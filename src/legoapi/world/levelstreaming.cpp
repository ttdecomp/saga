#include "legoapi/world/level_shared.h"

#include <string.h>

void LevelStreaming_Update(WORLDINFO_s *world) {
    i32 i;

    if (LOADEROFF == 0 && BGLOAD != 0 && world->area != NULL && (*(u16 *)((char *)world->area + 0x7a) & 8) == 0 &&
        NewLData == 0) {
        if (Mission_Active(NULL) == 0) {
            no_more_loads = 0;
            other_level = -1;
            if (no_more_loads == 0 && other_level_override != -1) {
                other_level = other_level_override;
            }
            if (CUTSTOPGAME != 0 && CutStopInfo != NULL && *(i16 *)((char *)CutStopInfo + 0xec) != -1) {
                i32 cutlevel = *(i16 *)((char *)CutStopInfo + 0xec);
                i32 curlevel = world->level_idx;
                if (*(u8 *)((char *)LDataList + cutlevel * 0x144 + 0xaf) ==
                    *(u8 *)((char *)LDataList + curlevel * 0x144 + 0xaf)) {
                    other_level = cutlevel;
                }
            }
            if ((*(u8 *)((char *)world->current_level + 0x64) & 0xe0) == 0) {
                u8 *pos = (u8 *)world; // placeholder
                if (Players_AveragePos((struct nuvec_s *)(pos + 0x54), NULL) != 0) {
                    other_level = -1;
                    if (world->portal_count > 0) {
                        f32 best = 1000000.0f;
                        for (i = 0; i < world->portal_count; i++) {
                            u8 *entry = (u8 *)world->portal_list + i * 0x120;
                            i32 lev = *(i16 *)(entry + 0xf0);
                            if (lev != world->level_idx) {
                                f32 d = NuVecDistSqr((NUVEC *)(entry + 0xd4), (NUVEC *)(pos + 0x54), NULL);
                                if (d < best) {
                                    best = d;
                                    other_level = *(i16 *)(entry + 0xf0);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void LevelStreaming_DoorOverride(WORLDINFO_s *, LEVELDATA_s *, float, float *) {
}

void LevelProgress_ReserveBufferSpace(variptr_u *buf, variptr_u) {
    LevelProgressData = (void *)ALIGN(buf->addr, 0x10);
    buf->addr = (usize)LevelProgressData + 0x229b0;
}
