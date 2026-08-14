#include "decomp.h"
#include "legoapi_types.h"

// Static misc gameplay/API helpers. Stubbed to satisfy the symbol baseline.

struct AIROW_s;

static __used__ void GameAILoad(AISYS_s *, int, nugscn_s *, variptr_u *, variptr_u *) {
}

static __used__ void FormationMove(AIGROUP_s *, int (*)(AIGROUP_s *, AIROW_s *, AIROW_s *, APIOBJECT_s *)) {
}

static __used__ APIOBJECT_s *GetNamedAPIObject(AISYS_s *, char *) {
    return nullptr;
}

static __used__ void *GetNextConnection(AIPACKET_s const *, int *) {
    return nullptr;
}

static __used__ nuvec_s *GetAICreatureOrigin(AISYS_s *, AIPACKET_s *) {
    return nullptr;
}

static __used__ int GameAIActionParseSpeed(char *, unsigned char *) {
    return 0;
}

static __used__ void GameFindAlternativeSpecialObject(AISYS_s *, nuhspecial_s *) {
}

static __used__ void GenerateTrooperTeamShape(minitrooperteam_s *, int) {
}

static __used__ void FillSockPosition(SOCKSYS *, SOCKPOSITION_s *) {
}

static __used__ void EndBolt_EwokTorpedo(BOLT_s *) {
}

static __used__ void GoThroughDoor_ExtraCode(WORLDINFO_s *, DOOR_s *) {
}

static __used__ float fxy(unsigned int, unsigned int) {
    return 0;
}

static __used__ unsigned int fxyda(float, float) {
    return 0;
}

static __used__ float fxydaf(float, float) {
    return 0;
}

static __used__ int icomp(const void *, const void *) {
    return 0;
}

static __used__ void InitUI() {
}

static __used__ void FireCode(GameObject_s *, int, int, float, int) {
}
