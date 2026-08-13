#include "legoapi/world_shared.h"

i32 ActionFromQuiet(i32 idx) {
    static i16 ActionPairTab[14] = {-1};
    if (idx != -1) {
        i16 *pair = ActionPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 14;
        }
    }
    return -1;
}
i32 AmbientFromQuiet(i32 idx) {
    static i16 AmbientPairTab[2] = {-1};
    if (idx != -1) {
        i16 *pair = AmbientPairTab;
        while (*pair != -1) {
            if (*pair == idx) {
                return pair[1];
            }
            pair += 2;
        }
    }
    return -1;
}

extern "C" void ResetSounds(void) {
}

void SetLevelSfxBits(WORLDINFO *world) {
    (void)world;
}
void ResetLevSfx(WORLDINFO *world) {
    // SFX bit array and counter in the filler8 region.
    // TODO: these offsets must be replaced with typed struct fields.
    //   0x4720 → filler8[0x2c]: SFX bit array (0x400 bytes, stride 0x10)
    //   0x4b14 → filler8[0x420]: SFX counter
    i16 *sfx = (i16 *)&world->filler8[0x2c];
    for (i32 i = 0; i < 0x40; i++) {
        sfx[i] = -1;
    }
    *(i32 *)&world->filler8[0x420] = 0;
}

void InitSpecialSfx(WORLDINFO *world) {
    (void)world;
}
void LoadSpecialSfxFile(WORLDINFO *world) {
    (void)world;
}
