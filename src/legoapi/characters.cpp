#include "decomp.h"
#include "legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void LSW_SetIndy(i32) {
}

void HairMovement(GameObject_s *) {
}

void HeadMovement(GameObject_s *) {
}

void TakeOverYoda(GameObject_s *, GameObject_s *, i32, i32) {
}

void fullcodename(i32) {
}

void CharScene_Draw(WORLDINFO_s *, i32, numtx_s *, numtx_s *) {
}

void CharScenes_Init(variptr_u *, variptr_u *) {
}

void FixUpCharacters(CHARFIXUP *) {
}

void PostAnimate_FETT(GameObject_s *) {
}

void ActivateCharacter(char *, nuvec_s *, i32) {
}

void FinishWeirdoNames(i32) {
}

void LoadAreaCharacters() {
}

void NewPlayerCharacter(GameObject_s *, i32, i32, i32) {
}

void ResetCharacterIdle(GameObject_s *, i32, i32) {
}

void UpdateCharacterIDs() {
}

void CharScenes_AreaDump() {
}

void CharScenes_AreaLoad(APICHARACTERMODELLIST_s *, variptr_u *, variptr_u) {
}

void DeactivateCharacter(char *) {
}

void LoadSingleCharacter(bgprocinfo_s *) {
}

void UpdateCharacterIdle(GameObject_s *) {
}

void UpdateCharacterLoad() {
}

void CharScenes_LevelDump(WORLDINFO_s *) {
}

void CollectAllCharacters(i32) {
}

void ResetCharacterBuffer(i32) {
}

void CollectCharcters_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void CollectCharcters_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void E1CharacterBonus_Init(WORLDINFO_s *) {
}

void LocalGetRandomLocator(AILOCATOR_s **, i32, float, nuvec_s *, float, i32, float, float) {
}

void PostAnimate_ASTROMECH(GameObject_s *) {
}

void CharScene_FindHSpecial(WORLDINFO_s *, i32) {
}

void LocalGetNearestLocator(AILOCATOR_s **, i32, float, nuvec_s *, float, i32, float, float) {
}

void newCharactersCollected(STATUSPACKET_s *) {
}

void CollectCharcters_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void RegisterGizmoTypes_Indy(variptr_u *, variptr_u *) {
}

void Area_CharIDInCurrentList(i32) {
}

void SetProtocolDroidFallAnim(GameObject_s *) {
}

void CollectCharactersOff_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void CollectCharactersOff_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void SetGameObjectCharacterData(GameObject_s *) {
}

void CollectCharactersOff_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void TakeOverYodaSeekDistanceHack(GameObject_s *, GameObject_s *, nuvec_s *) {
}

void SetProtocolDroidInterfaceAction(GameObject_s *) {
}

void SetProtocolDroidDeactivatedAction(GameObject_s *) {
}

void LoadPerm1() {
}

void LoadPerm2() {
}

void MapToGrid(nuvec_s *, nuvec_s *, i32 *, i32 *, nuvec_s *, nutexmanager_s *) {
}

static __used__ void SpecialRouteCharacterName(u8) {
}

static __used__ void SpecialRouteCharacterTypeID(char *) {
}

extern "C" {

        void InitFn_SpecialRouteCharacterName(void) {
        }

        void InitFn_SpecialRouteCharacterTypeID(void) {
        }

} // extern "C"

