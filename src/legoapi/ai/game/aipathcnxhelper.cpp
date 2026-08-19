#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void AIPathCalcExtents(AIPATH_s *) {
}

void AIPathCheckExtents(AIPATH_s *, nuvec_s *) {
}

void pathEditorDrawNode(nuvec_s *, float, float, float, u32, numtl_s *, i32, i32) {
}

void AIPathCnxHelperSysReset(WORLDINFO_s *, AIPATHCNXHELPERSYS_s *) {
}

void AIPathCnxHelperSys_Find(AIPATHCNXHELPERSYS_s *, GameObject_s *, AIPATHCNX_s *, unsigned char, unsigned char,
                             i32 (*)(AIPATHCNXHELPER_s *, GameObject_s *, AIPATHCNX_s *, unsigned char,
                                     unsigned char)) {
}

void AIPathCnxControlSysReset(AIPATHCNXCONTROLSYS_s *) {
}

void AIPathCnxControlSysUpdate(AIPATHCNXCONTROLSYS_s *) {
}

void AIPathCnxControllerCreate(AIPATHCNXCONTROLSYS_s *, AISYS_s *, AIPATH_s *, char *, char *, i32, char *, i32, i32) {
}

void AIPathCnxControllerDestroy(AIPATHCNXCONTROLSYS_s *, AIPATHCNXCONTROLLER_s *) {
}

void AIPathCnxSetTemporaryBlock(AIPATH_s *, char *, char *, i32) {
}

void AIPathCnxHelperSys_AddHelper(AIPATHCNXHELPERSYS_s *, AIPATHCNX_s *, unsigned char, void *, unsigned char) {
}

void pathEditorDrawConnectionInfo(nuvec_s *, float, nuvec_s *, u32, i32) {
}

void AIPathCnxControllerSetOnRange(AIPATHCNXCONTROLLER_s *, i32, i32) {
}
