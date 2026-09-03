#include "decomp.h"

struct AIPACKET_s;
struct APIOBJECT_s;
struct AISYS_s;
struct AIGROUP_s;
struct AIROW_s;
struct nufpar_s;

typedef void (*PREPARINGSPECIALMOVEFN)(AIPACKET_s *, APIOBJECT_s *, i32);
typedef i32 (*MIDSPECIALMOVEFN)(AISYS_s *, AIPACKET_s *, APIOBJECT_s *);

PREPARINGSPECIALMOVEFN PreparingForSpecialMoveFn = NULL;
MIDSPECIALMOVEFN MidSpecialMoveFn = NULL;

static __used__ void StarWars_PrepareHatch(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static __used__ void StarWars_PrepareBigJump(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static __used__ void StarWars_PrepareObstacle(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static __used__ void StarWars_PrepareHoverTube(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static __used__ void StarWars_PrepareTakeOverJump(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static __used__ void StarWars_PreparingForSpecialMove(AIPACKET_s *, APIOBJECT_s *, i32) {
}

static __used__ i32 StarWars_MidSpecialMove(AISYS_s *, AIPACKET_s *, APIOBJECT_s *) {
    return 0;
}

static __used__ i32 StarWars_MidSpecialMove_BigJump(AISYS_s *, AIPACKET_s *, APIOBJECT_s *) {
    return 0;
}

static __used__ i32 StarWars_MidSpecialMove_Default(AISYS_s *, AIPACKET_s *, APIOBJECT_s *) {
    return 0;
}

static __used__ i32 StarWars_MidSpecialMove_UseHatch(AISYS_s *, AIPACKET_s *, APIOBJECT_s *) {
    return 0;
}

static __used__ i32 StarWars_MidSpecialMove_StartJump(AISYS_s *, AIPACKET_s *, APIOBJECT_s *) {
    return 0;
}

static __used__ void SpecialMove_ConfigParticipant(nufpar_s *, i16 *, i8 *, i16 *) {
}

static __used__ void RowMoveWander(AIGROUP_s *, AIROW_s *, AIROW_s *, APIOBJECT_s *) {
}

static __used__ void RowMoveTowards(AIGROUP_s *, AIROW_s *, AIROW_s *, APIOBJECT_s *) {
}

extern "C" {

    void InitFn_MidSpecialMove(MIDSPECIALMOVEFN function) {
        MidSpecialMoveFn = function;
    }

    void InitFn_PreparingForSpecialMove(PREPARINGSPECIALMOVEFN function) {
        PreparingForSpecialMoveFn = function;
    }

} // extern "C"
