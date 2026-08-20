#include "legoapi/world/level_shared.h"

#include <string.h>

void LevelStreaming_Update(WORLDINFO_s *world) {
    i32 i;

    if (LOADEROFF == 0 && BGLOAD != 0 && *(void **)((char *)world + 0x12c) != NULL &&
        (*(u16 *)(*(char **)((char *)world + 0x12c) + 0x7a) & 8) == 0 && NewLData == 0) {
        if (Mission_Active(NULL) == 0) {
            no_more_loads = 0;
            other_level = -1;
            if (no_more_loads == 0 && other_level_override != -1) {
                other_level = other_level_override;
            }
            if (CUTSTOPGAME != 0 && CutStopInfo != NULL && *(i16 *)((char *)CutStopInfo + 0xec) != -1) {
                i32 cutlevel = *(i16 *)((char *)CutStopInfo + 0xec);
                i32 curlevel = *(i32 *)((char *)world + 0x11c);
                if (*(u8 *)((char *)LDataList + cutlevel * 0x144 + 0xaf) ==
                    *(u8 *)((char *)LDataList + curlevel * 0x144 + 0xaf)) {
                    other_level = cutlevel;
                }
            }
            if ((*(u8 *)(*(char **)((char *)world + 0x128) + 0x64) & 0xe0) == 0) {
                u8 *pos = (u8 *)world; // placeholder
                if (Players_AveragePos((struct nuvec_s *)(pos + 0x54), NULL) != 0) {
                    other_level = -1;
                    if (*(i32 *)((char *)world + 0x46a0) > 0) {
                        f32 best = 1000000.0f;
                        for (i = 0; i < *(i32 *)((char *)world + 0x46a0); i++) {
                            u8 *entry = *(u8 **)((char *)world + 0x469c) + i * 0x120;
                            i32 lev = *(i16 *)(entry + 0xf0);
                            if (lev != *(i32 *)((char *)world + 0x11c)) {
                                f32 d = NuVecDistSqr(entry + 0xd4, pos + 0x54, 0);
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
    LevelProgressData = (void *)((buf->addr + 0xf) & ~0xf);
    buf->addr = (usize)LevelProgressData + 0x229b0;
}
