#include "legoapi_types.h"
#include "decomp.h"
struct EDANTINODE_s;

void GameAntinode_Update(GAMEANTINODESYS_s *) {
}

void GameAntinode_Debug_DrawGrid(WORLDINFO_s *) {
}

void GameAntinode_FindGridPosition(WORLDINFO_s *, nuvec_s *, float, float, unsigned char *, unsigned char *,
                                   unsigned char *, unsigned char *) {
}

void GameAntinode_RegisterAntiNode(GAMEANTINODESYS_s *, nuvec_s *, float, float, float, u16, i32, float) {
}

void GameAntinode_UnregisterAntiNode(GAMEANTINODESYS_s *, GAMEANTINODE_s *) {
}

void GameAntinode_UpdateAntiNodeUsingData(GAMEANTINODESYS_s *, GAMEANTINODE_s *, nuvec_s *, u16, GAMEANTINODEDATA_s *,
                                          float, i32) {
}

void GameAntinode_RegisterAntiNodeUsingData(GAMEANTINODESYS_s *, nuvec_s *, u16, GAMEANTINODEDATA_s *, float, i32) {
}

static __used__ void * CreateAntinode(nuvec_s*) { return {}; }

static __used__ void antinodeEditor_AntinodeMoved(EDANTINODE_s*) {}

static __used__ void * antinodeEditor_GetNearestAntinode(int) { return {}; }

static __used__ void antinodeEditor_cbSetType(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbSelectType(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbAntiNodeFlagsToggle(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbDeleteAntinode(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbCancelDeleteAntinodeMenu(eduimenu_s*, eduimenu_s*) {}

static __used__ void antinodeEditor_cbCancelMenu(eduimenu_s*, eduimenu_s*) {}
