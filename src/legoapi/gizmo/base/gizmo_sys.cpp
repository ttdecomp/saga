#include "legoapi/world/world_shared.h"
#include "decomp.h"
struct FLOWBOX_s;

void *MiniKitCollection = NULL;

void *CreateGizmoSys(void *world, VARIPTR *buf, VARIPTR *buf_end) {
    (void)world;
    (void)buf;
    (void)buf_end;
    return NULL;
}
void LoadGizmoSys(GIZMOSYS_s *gizmo_sys, void *world, char *config_file) {
    (void)gizmo_sys;
    (void)world;
    (void)config_file;
}
void LoadEditorSplines(char *path, VARIPTR *buf, VARIPTR *buf_end) {
    (void)path;
    (void)buf;
    (void)buf_end;
}
void GizmoBlowupResetNameTable(void) {
}
void Hub_LoadAndFixUpMiniKits(WORLDINFO *world, VARIPTR *buf, VARIPTR *buf_end) {
    (void)world;
    (void)buf;
    (void)buf_end;
}
void MiniKit_Load(MINIKIT *minikit, i32 id, VARIPTR *buf, VARIPTR *buf_end, void *param) {
    (void)minikit;
    (void)id;
    (void)buf;
    (void)buf_end;
    (void)param;
}
void MiniKit_InitPieces(MINIKIT *minikit, i32 count, VARIPTR *buf, VARIPTR *buf_end) {
    (void)minikit;
    (void)count;
    (void)buf;
    (void)buf_end;
}
void CharacterMiniKits_Load(COLLECTION_s *collection, WORLDINFO *world, VARIPTR *buf, VARIPTR *buf_end) {
    (void)collection;
    (void)world;
    (void)buf;
    (void)buf_end;
}
void GizmoSysAddGizmos(GIZMOSYS_s *gizmo_sys, GIZFLOW_s *giz_flow, void *world) {
    (void)gizmo_sys;
    (void)giz_flow;
    (void)world;
}
void *LoadGizFlow(void *world, GIZMOSYS_s *gizmo_sys, char *path, VARIPTR *buf, VARIPTR *buf_end) {
    (void)world;
    (void)gizmo_sys;
    (void)path;
    (void)buf;
    (void)buf_end;
    return NULL;
}

static __used__ i32 Loop_CountLoopingInputsEx(FLOWBOX_s *, FLOWBOX_s *, i32, u8) {
    return 0;
}

static __used__ void CheckIfParentsFinished(GIZFLOW_s*, FLOWBOX_s*) {}

static __used__ void CheckOutputGizmoFlowBox(GIZFLOW_s*, FLOWBOX_s*, unsigned char) {}

static __used__ void CheckOutputActionFlowBox(GIZFLOW_s*, FLOWBOX_s*, unsigned char) {}

static __used__ void CheckOutputConditionFlowBox(GIZFLOW_s*, FLOWBOX_s*, unsigned char) {}
