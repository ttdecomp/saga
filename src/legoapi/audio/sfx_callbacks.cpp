#include "decomp.h"

struct nuvec_s;

extern "C" {

    void (*APIObjPlaySfxByIdFn)(i32, nuvec_s *);

    void SetAPIObjPlaySfxByIdFn(void (*play_sfx)(i32, nuvec_s *)) {
        APIObjPlaySfxByIdFn = play_sfx;
    }

} // extern "C"
