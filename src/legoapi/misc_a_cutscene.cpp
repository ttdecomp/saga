#include "decomp.h"
#include "legoapi_types.h"

struct CUTSCENEPLAYERCLIP;
struct NUGCUTRIGID_s;
struct instNUGCUTCHAR_s;
struct instNUGCUTRIGID_s;

static __used__ void CutScene_Configure(CUTINFO*, char*, variptr_u*, variptr_u*) {}

static __used__ void bgAckStreamCutScene(bgprocinfo_s*) {}

static __used__ void bgLoadStreamCutScene(bgprocinfo_s*) {}

static __used__ unsigned int CutScenePlayer_Accept(CUTSCENEPLAYERCLIP*) { return {}; }

static __used__ void CutScene_DrawCharacter(instNUGCUTSCENE_s*, NUGCUTSCENE_s*, instNUGCUTCHAR_s*, NUGCUTCHAR_s*, float, int) {}

static __used__ void CutScene_EvalCharacter(instNUGCUTSCENE_s*, NUGCUTSCENE_s*, instNUGCUTCHAR_s*, NUGCUTCHAR_s*, float) {}

static __used__ unsigned int CutScene_FindCharacters(NUGCUTSCENE_s*) { return {}; }

static __used__ void CutScene_ResetCharacters(instNUGCUTSCENE_s*) {}

static __used__ void CutScene_RigidPostRender(NUGCUTRIGID_s*, instNUGCUTRIGID_s*, numtx_s*) {}

static __used__ void * CutScene_CreateCharacterInstance(NUGCUTCHAR_s*, instNUGCUTCHAR_s*, variptr_u*) { return {}; }

static __used__ void CutScene_OverrideConfigFileName_LSW(char*, int, int) {}

static __used__ void copyAnims(NUGCUTSCENE_s*, NUGCUTSCENE_s*) {}
