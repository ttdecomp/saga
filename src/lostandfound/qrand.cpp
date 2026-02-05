#include "qrand.h"

int qseed = 0x3039;

u16 qrand(void) {
    qseed = qseed * 0x24cd + 1 & 0xffff;

    return qseed;
}
