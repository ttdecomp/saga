#include "decomp.h"

struct EDAIPATH_s;
struct EDAIPATHNODE_s;
struct EDAISHAREDPATHNODE_s;
struct AIPATH_s;
struct eduimenu_s;
struct eduiitem_s;
struct nuvec_s;
struct nupad_s;

static __used__ void ParseAIPathCnxFlag(char *) {
}

static __used__ void pathEditorDrawPath(EDAIPATH_s *, i32) {
}

static __used__ void TestPointPathCheck(nuvec_s *, EDAIPATHNODE_s *, EDAIPATHNODE_s *, f32 *, f32 *, i32 *, f32) {
}

static __used__ void pathEditor_cbCreatePath(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbDeletePath(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbRenameNode(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbRenamePath(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_PathNodeMoved(EDAIPATHNODE_s *) {
}

static __used__ i32 pathEditor_GetNearestNode(EDAIPATH_s *, i32) {
    return 0;
}

static __used__ void pathEditor_cbSetShareNode(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbShareNodeMenu(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditorCalcRouteIterator(AIPATH_s *, f32 *, u8 *, i32, i32, f32, i32) {
}

static __used__ void pathEditor_cbCnxFlagsToggle(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbDeletePathNode(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbRenameNodeMenu(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbRenamePathMenu(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbSelectPathMenu(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbSetCurrentPath(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_DestroySharedNode(EDAISHAREDPATHNODE_s *) {
}

static __used__ void pathEditor_cbNodeFlagsToggle(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbCancelSelectMenu(eduimenu_s *, eduimenu_s *) {
}

static __used__ void pathEditor_cbDisconnectPathNode(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditorCalculateDistanceTable(AIPATH_s *, i32, variptr_u *, variptr_u *) {
}

static __used__ void pathEditor_cbCancelDeleteAreaMenu(eduimenu_s *, eduimenu_s *) {
}

static __used__ void pathEditor_cbCancelDeleteNodeMenu(eduimenu_s *, eduimenu_s *) {
}

static __used__ void pathEditor_cbCancelRenameNodeMenu(eduimenu_s *, eduimenu_s *) {
}

static __used__ void pathEditor_cbCancelRenamePathMenu(eduimenu_s *, eduimenu_s *) {
}

static __used__ void pathEditor_cbDrawWallsplinesToggle(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void pathEditor_cbCancelDeleteCreatureMenu(eduimenu_s *, eduimenu_s *) {
}

static __used__ void pathEditor_cbCancelDisconnectNodeMenu(eduimenu_s *, eduimenu_s *) {
}

static __used__ void routeEditor_cbRouteUsers(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void routeEditor_cbCreateRoute(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void routeEditor_cbDeleteRoute(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void routeEditor_cbRenameRoute(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void routeEditor_cbSetRouteUsers(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void routeEditor_cbRenameRouteMenu(eduimenu_s *, eduiitem_s *, u32) {
}

static __used__ void routeEditor_cbCancelRouteUsers(eduimenu_s *, eduimenu_s *) {
}

static __used__ void routeEditor_cbCancelRenameRouteMenu(eduimenu_s *, eduimenu_s *) {
}

extern "C" {

    void pathEditorCreateData(void) {
    }

    void pathEditorDrawPaths(void) {
    }

    void pathEditorSaveData(void) {
    }

    void pathEditor_CalcNodeIXs(void) {
    }

    void pathEditor_GetPath(void) {
    }

    void pathEditor_OnPathCheck(void) {
    }

    void pathEditor_QuickOnPathCheck(void) {
    }

    void pathEditor_UpdateNodesOnPlatforms(void) {
    }

} // extern "C"

void pathEditor_Enter(void) {
}

void pathEditor_Render(i32, i32, float, float) {
}

void pathEditor_Process(nupad_s *) {
}
