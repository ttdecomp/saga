#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

struct CUSTOMISER;

void (*DisguiseAdjustFn)(i32 character_id, i32 disguise_part, NUVEC *scale, NUVEC *offset);

void Customiser_Set100PercentPieces(CUSTOMISER *) {
}

i32 Customiser_FindPieceByName(CUSTOMISER *, char *, i32 *category, i32 *piece) {
    if (category != NULL) {
        category[0] = -1;
    }
    if (piece != NULL) {
        piece[0] = -1;
    }
    return 0;
}
