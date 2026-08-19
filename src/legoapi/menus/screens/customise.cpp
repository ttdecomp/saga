#include "legoapi/legoapi_types.h"

void Customiser_Init(CUSTOMISER *) {
}

void Customiser_Reset(CUSTOMISER *) {
}

void Customiser_Draw3D(CUSTOMISER *) {
}

void Customiser_Update(CUSTOMISER *, WORLDINFO_s *) {
}

void CustomiserMenu_End() {
}

void Customiser_DumpAll(CUSTOMISER *, WORLDINFO_s *) {
}

void Customiser_GetIcon(CUSTOMISER *, CUSTOMISESAVE_s *, i32) {
}

void Customiser_LoadAll(CUSTOMISER *, WORLDINFO_s *) {
}

void CustomiserMenu_Draw(MENU_s *) {
}

void Customiser_Configure(char *, variptr_u *, variptr_u *, i32, i32, i32 (*)(CUSTOMPIECE *),
                          void (*)(CUSTOMPIECE *, nufpar_s *), i32 (*)(char *), CUSTOMISESAVE_s *, i16 *) {
}

void Customiser_InitNames(CUSTOMISER *) {
}

void CustomiserMenu_Update(MENU_s *) {
}

void Customiser_PieceConfig(CUSTOMPIECE *, nufpar_s *) {
}

void Customiser_MenuAvailable(CUSTOMISER *) {
}

void Customiser_NextPieceLeft(CUSTOMISER *, i32, i32, i32, i32) {
}

void Customiser_NextPieceRight(CUSTOMISER *, i32, i32, i32, i32) {
}

void Customiser_PieceAvailable(CUSTOMPIECE *) {
}

void Customiser_SetAnimsToLoad(CUSTOMISER *, i32) {
}

void Customiser_SetNameAndIcon(CUSTOMISER *, i32) {
}

void Customiser_DrawAccessories(CUSTOMISER *, GameObject_s *, numtx_s *) {
}

void Customiser_DumpAccessories(CUSTOMISER *) {
}

void Customiser_FindPieceByName(CUSTOMISER *, char *, i32 *, i32 *) {
}

void Customiser_LoadAccessories(CUSTOMISER *, APICHARACTERMODELLIST_s *) {
}

void Customiser_TransformToPanel(CUSTOMISER *) {
}

void Customiser_AddPartAccessories(CUSTOMISER *, GameObject_s *, i32, i32, float) {
}

void Customiser_SetUpCharacterData(CUSTOMISER *) {
}

void Customiser_SaveModelTextureIDs(CUSTOMISER *, CHARACTERMODEL_s *) {
}

void Customiser_Set100PercentPieces(CUSTOMISER *) {
}

void Customiser_GetActiveWeirdoIndex(i32 *, i32 *) {
}

void Customiser_ResetModelTextureIDs(CUSTOMISER *) {
}

void Customiser_RestoreModelTextureIDs(CUSTOMISER *) {
}

void Customiser_CopyDefaultPiecesToSave(CUSTOMISER *, CUSTOMISESAVE_s *) {
}

static __used__ bool Customiser_PieceAvailable_Default(CUSTOMPIECE *) {
    return {};
}

void Customise_GetToggleString(i32) {
}
