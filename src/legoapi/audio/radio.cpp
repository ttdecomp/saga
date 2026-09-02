#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

#include <string.h>

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

static u32 radios[40];
extern i32 *radios_playing;

void ResetRadios() {
    memset(radios, 0, sizeof(radios));
    radios_playing = 0;
}

void UpdateRadios() {
}

void PlayRadio(char *, char *, i32) {
}
