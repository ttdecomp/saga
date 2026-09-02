#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void CharShadows_Draw() {
}

void CharShadows_Reset(PLAYERPACKET_s *packet) {
    for (i32 i = 0; i < 5; ++i) {
        packet->char_shadows[i].position.y = 2000000.0f;
    }
}

void CharShadows_Update() {
}
