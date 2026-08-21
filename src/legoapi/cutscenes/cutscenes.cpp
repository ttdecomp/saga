#include "legoapi/legoapi_types.h"

CUTINFO *CutScene_Find(CUTSYS *, char *) {
    return NULL;
}

void CutScenes_End() {
}

void CutScenes_Draw(WORLDINFO_s *) {
}

void CutScenes_Stop(CUTSYS *) {
}

void CutScenes_Reset(WORLDINFO_s *) {
}

void CutScenes_Start(WORLDINFO_s *) {
}

void CutScenes_Update(WORLDINFO_s *, i32) {
}

void CutScene_FindInst(CUTSYS *, char *) {
}

void CutScenes_Destroy(CUTSYS *) {
}

void CutScene_HasPlayed(CUTINFO *) {
}

void CutScene_SnapToEnd(CUTINFO *) {
}

void CutScene_StartAudio() {
}

void CutScene_IsSkippable(CUTINFO *) {
}

void CutScene_StartFn_LSW(CUTINFO *) {
}

void CutScenes_InitSystem(CUTSCENESYS *) {
}

void CutScene_DrawSubtitles() {
}

void CutScene_StoppedFn_LSW(CUTINFO *) {
}

void CutScenes_BGLoadManager() {
}

void CutScenes_ConfigureList(char *, variptr_u *, variptr_u) {
}

void CutScene_PreUpdateFn_LSW(CUTINFO *) {
}

void CutScene_PostUpdateFn_LSW() {
}

void CutScene_PlayingOrRequested(CUTINFO *) {
}

void CutScene_ReplaceCharacterModelFn_LSW(CUTINFO *, NUGCUTCHAR_s *) {
}

void ResetScene(nugscn_s *, SCENEPROGRESS_s *) {
}

void NewCutScene(CUTINFO *, CUTSYS *, char *, i32) {
}

void Exit_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void Fade_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void RelocateCutScene(NUGCUTSCENE_s *, variptr_u *) {
}

void GoldBrick_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void GoldBrick_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void GoldBrick_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

void LevelComplete_LSW_Draw(STATUS_STAGE_s *, STATUSPACKET_s *, i32) {
}

void LevelComplete_LSW_Skip(STATUS_STAGE_s *, STATUSPACKET_s *) {
}

void LevelComplete_LSW_Update(STATUS_STAGE_s *, STATUSPACKET_s *, float) {
}

static __used__ void Titles_Draw(WORLDINFO_s *) {
}

static __used__ void Titles_Init(WORLDINFO_s *) {
}

static __used__ void Titles_Update(WORLDINFO_s *) {
}
