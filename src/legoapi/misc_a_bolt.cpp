#include "decomp.h"
#include "legoapi_types.h"

struct spacelevel_s;

static __used__ void DrawWeapons(GameObject_s*, int, float) {}

static __used__ bool Bolt_RayCast(BOLT_s*, nuvec_s*, nuvec_s*, float) { return {}; }

static __used__ void DrawCoinTotal(int, int) {}

static __used__ void DrawPackButton(GAMEMESSAGE_s*, nuvec_s*, float) {}

static __used__ void DrawSpaceLevel(spacelevel_s*) {}

static __used__ unsigned int BoltInitSfx_LSW(GameObject_s*) { return {}; }

static __used__ void DrawStarFighter(starfighter_s*) {}

static __used__ void DrawEpisodesMenu(int, float) {}

static __used__ void DrawParaphernalia(GameObject_s*) {}

static __used__ void DrawLightningBolts(GameObject_s*, GameObject_s*, int) {}

static __used__ void Bolt_Debris_Default(BOLT_s*, nuvec_s*, int, nuvec_s*, int) {}

static __used__ void DrawFalconSpotLights(GameObject_s*) {}

static __used__ unsigned int Batarang_GetTargetPos(BATARANG_s*, int, nuvec_s*) { return {}; }

static __used__ void CollideBoltStarFighter(BOLT_s*, starfighter_s*, _vuv_s*, _vuv_s*) {}

static __used__ void DrawCharacterAttachments(GameObject_s*, numtx_s*) {}

static __used__ void DrawWeapon_SetSabreObjects(GameObject_s*, int, int, int, int, int*, int*) {}

static __used__ void Bolt_GetShootOrigin_Default(GameObject_s*, nuvec_s*) {}

static __used__ void Bolt_GetShootDirection_Default(GameObject_s*, nuvec_s*) {}
