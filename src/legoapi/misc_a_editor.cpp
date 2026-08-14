#include "decomp.h"
#include "legoapi_types.h"

struct EDLOCATOR_s;
struct EDLOCATORSET_s;
struct EDANTINODE_s;

static __used__ void * CreateAntinode(nuvec_s*) { return {}; }

static __used__ void * CreateCreature(int, nuvec_s*, int) { return {}; }

static __used__ void DestroyLocator(EDLOCATOR_s*) {}

static __used__ unsigned int AddLocatorToSet(EDLOCATORSET_s*, EDLOCATOR_s*, EDLOCATOR_s*) { return {}; }

static __used__ void creatureEditor_Updated(EDCREATURE_s*) {}

static __used__ void areaEditor_cbDeleteArea(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void areaEditor_cbRenameArea(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void aieditor_cbSetEditorMode(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbSetType(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetType(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_ngroup(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void aieditor_cbCancelSaveMenu(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_nacross(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbGroupMenu(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetScript(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_xspacing(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_zspacing(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbCancelMenu(eduimenu_s*, eduimenu_s*) {}

static __used__ void antinodeEditor_cbSelectType(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void areaEditor_cbRenameAreaMenu(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbCancelMenu(eduimenu_s*, eduimenu_s*) {}

static __used__ void creatureEditor_cbSelectType(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetLocator(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbVisionMenu(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_AntinodeMoved(EDANTINODE_s*) {}

static __used__ void creatureEditor_cbFlagsToggle(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbResetParams(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbRespawnMenu(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_difficulty(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void areaEditor_cbCancelRenameMenu(eduimenu_s*, eduimenu_s*) {}

static __used__ void creatureEditor_cbScriptParams(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSelectScript(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_assigntoset(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSelectLocator(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetActivation(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_heardistance(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_viewdistance(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbDeleteAntinode(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void areaEditor_cbAreaCylinderToggle(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbActivationMenu(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbDeleteCreature(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbRenameCreature(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetScriptParam(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetTriggerArea(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_max_t_respawn(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_maxviewheight(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_min_t_respawn(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_minviewheight(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_stagger_start(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_max_n_respawns(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cb_min_n_respawns(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void aieditor_cbCancelSelectEditorMode(eduimenu_s*, eduimenu_s*) {}

static __used__ void * antinodeEditor_GetNearestAntinode(int) { return {}; }

static __used__ void creatureEditor_cbSelectTriggerArea(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetAreaActivation(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSetRespawnLocator(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbRenameCreatureMenu(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbAntiNodeFlagsToggle(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void creatureEditor_cbSelectRespawnLocator(eduimenu_s*, eduiitem_s*, unsigned int) {}

static __used__ void antinodeEditor_cbCancelDeleteAntinodeMenu(eduimenu_s*, eduimenu_s*) {}
