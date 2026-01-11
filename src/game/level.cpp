#include "game/level.h"

void Level_SetDefaults(LEVELDATA *level) {
    level->field170_0xe4[0] = 5.87783e-39f;
    level->field170_0xe4[1] = 2.3510066e-38f;
    level->field170_0xe4[2] = 1.1755123e-38f;
    level->field170_0xe4[3] = 1.469547e-39f;
    level->field174_0xf4 = 10;
    level->field175_0xf5 = 8;
    level->field176_0xf6 = 10;
    level->field177_0xf7 = 10;
    level->field178_0xf8 = 10;
    level->field179_0xf9 = 10;
    level->field180_0xfa = 10;
    level->max_obstacles = 32;
    level->field182_0xfc = 0x10;
    level->field183_0xfd = 0x40;
    level->field184_0xfe = 4;
    level->field185_0xff = 8;
    level->field186_0x100 = 2;
    level->field187_0x101 = 0x20;
    level->field188_0x102 = 5;
    level->field189_0x103 = 10;
    level->field190_0x104 = 0x20;
    level->field191_0x105 = 0x60;
    level->field192_0x106 = 5;
    level->field193_0x107 = 5;
    level->field194_0x108 = 8;
    level->field195_0x109 = 2;
    level->field196_0x10a = 0x20;
    level->field197_0x10b = 0x10;
    level->field198_0x10c = 8;
    level->field199_0x10d = 0x10;
    level->field200_0x10e = 4;
    level->field201_0x10f = 8;
    level->field202_0x110 = 2;
    level->field203_0x111 = 0x28;
    level->field216_0x128 = 512.0f;
    level->field215_0x124 = 3.0f;
    level->field204_0x112 = 0x20;
    level->max_bombgens = 8;
    level->field206_0x114 = 5;
    level->field207_0x115 = 10;
}