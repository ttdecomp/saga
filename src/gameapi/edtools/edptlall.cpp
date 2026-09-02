#include "gameapi_edtools_types.h"
#include "legoapi/legoapi_types.h"

extern "C" {
    extern edpp_particle_s edpp_ptls[512];
    void DebFreeInstantly(i32 *handle);
    void edppRestartAllEffectsInLevel(void);
}

// Particle list editor subsystem stubs (static, internal linkage).

static __used__ void edptlcbPageMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetGroup(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbStarMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbStopPage(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbClearPage(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbGhostMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbGroupMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetDetail(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbStartPage(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbBounceMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbDetailMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetMaxThin(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetSoundID(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSoundXMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSoundsMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSwitchMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetDpadMode(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetDrawflag(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetSwitchId(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSoundIDMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbCutClipboard(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbDpadModeMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbDrawflagMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetSwitchVar(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlChangeRepeatBox(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbClipboardMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbDeleteOrphans(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetSwitchType(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbApplyGhostTime(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbApplyNumGhosts(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbApplyStarRatio(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbCancelPageMenu(eduimenu_s *, eduimenu_s *) {
}
static __used__ void edptlcbCancelStarMenu(eduimenu_s *, eduimenu_s *) {
}
static __used__ void edptlcbChangeDistortX(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbChangeDistortY(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbChangeRampTime(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbEmptyClipboard(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbOrphanListMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbPasteClipboard(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbResetParticles(eduimenu_s *, eduiitem_s *, u32) {
    for (i32 i = 0; i < 512; ++i) {
        i32 *instance_id = &edpp_ptls[i].instance_id;
        if (*instance_id != 99999 && *instance_id != -1) {
            DebFreeInstantly(instance_id);
            *instance_id = 99999;
        }
    }
    edppRestartAllEffectsInLevel();
}
static __used__ void edptlcbSetMasterGroup(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSetScaleFactor(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbSwitchTypeMenu(eduimenu_s *, eduiitem_s *, u32) {
}
static __used__ void edptlcbTestDetailMenu(eduimenu_s *, eduiitem_s *, u32) {
}
