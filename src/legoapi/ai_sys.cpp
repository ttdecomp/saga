#include "legoapi/world_shared.h"
#include "decomp.h"
struct nuhspecial_s;
struct minitrooperteam_s;
struct nuvec_s;

extern "C" void *AISysLoad(void *buf, void *buf_end, i32 size, void *gscn, char *dir, char *name, char *param) {
    (void)buf;
    (void)buf_end;
    (void)size;
    (void)gscn;
    (void)dir;
    (void)name;
    (void)param;
    return NULL;
}
void *AIPathCnxControlSysCreate(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AIPathCnxHelperSysCreate(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
void *AITriggerSetSysCreate(VARIPTR *buf, VARIPTR *buf_end) {
    (void)buf;
    (void)buf_end;
    return NULL;
}
void GameAIScriptAddLevelSfx(WORLDINFO *world, NULISTHDR *scripts) {
    (void)world;
    (void)scripts;
}
void *CreateClimbObjectSys(VARIPTR *buf, VARIPTR *buf_end, i32 count) {
    (void)buf;
    (void)buf_end;
    (void)count;
    return NULL;
}
extern "C" void *APIObjectSysInit(i32 size, void *buf, void *buf_end) {
    (void)size;
    (void)buf;
    (void)buf_end;
    return NULL;
}

static __used__ unsigned int AIRespawnOnPath(APIOBJECT_s*) { return {}; }

static __used__ int Collide2Objects(APIOBJECT_s*, APIOBJECT_s*) { return {}; }

static __used__ unsigned int CalculateIntersection(AISYS_s*, AIPACKET_s*, APIOBJECT_s*, AIPATHCNX_s*, AIPATHCNX_s*) { return {}; }

static __used__ unsigned int AIBigJumpToDestination(APIOBJECT_s*, nuvec_s*) { return {}; }

static __used__ unsigned int AISysCharacterTestPathCnx(AISYS_s*, APIOBJECT_s*, AIPACKET_s*, AIPATHCNX_s*, int, float*) { return {}; }

static __used__ unsigned int AISysCheckAntinode_Circle(APIOBJECT_s*, AIANTINODE_s*, nuvec_s*, float) { return {}; }

static __used__ unsigned int AISysCheckAntinode_Ellipse(APIOBJECT_s*, AIANTINODE_s*, nuvec_s*, float) { return {}; }

static __used__ unsigned int AISysCheckAntinode_Rectangle(APIOBJECT_s*, AIANTINODE_s*, nuvec_s*, float) { return {}; }

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

