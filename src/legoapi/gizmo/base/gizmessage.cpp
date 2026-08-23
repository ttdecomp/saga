#include "decomp.h"
#include "legoapi/legoapi_types.h"
#include "nu2api/nu3d/nutex.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void GetGizAIMessage(GIZAIMESSAGESYS_s *, char const *, GIZAIMESSAGE_s *) {
}

GIZAIMESSAGE_s *SetGizAIMessage(GIZAIMESSAGESYS_s *, char const *, float, GIZAIMESSAGE_s *) {
    return NULL;
}

GIZAIMESSAGE_s *CheckGizAIMessage(GIZAIMESSAGESYS_s *, char const *, GIZAIMESSAGE_s *) {
}

void QueryGizAIMessage(GIZAIMESSAGESYS_s *, GIZAIMESSAGE_s *) {
}

void ClearGizAIMessageSys(GIZAIMESSAGESYS_s *) {
}

void GizAIMessage_GetName(GIZAIMESSAGE_s *) {
}

void ResetGizAIMessageSys(GIZAIMESSAGESYS_s *) {
}

void CreateGizAIMessageSys(variptr_u *, variptr_u *, i32) {
}
