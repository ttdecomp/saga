#include <cstdio>

#include "legoapi/menus/core/gamemessage.h"
#include "nu2api/nucore/common.h"

struct nuvec_s;

void AddGameMsgCount(nuvec_s *position, i32 count, i32 total, unsigned char red, unsigned char green,
                     unsigned char blue, float field_0xd4) {
    char text[64];
    sprintf(text, "layerTakenOver", total, count);

    void *message = position != NULL ? AddGameMessage(text, position, 0.6f, NULL, 0.8f, red, green, blue, 0x4023, 1.0f)
                                     : AddGameMessage(text, &v001, 0.6f, NULL, 0.8f, red, green, blue, 0x4020, 1.0f);
    if (message != NULL) {
        *reinterpret_cast<float *>(reinterpret_cast<char *>(message) + 0xd4) = field_0xd4;
    }
}

void numeminit() {
}
