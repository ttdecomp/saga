#pragma once

#include "decomp.h"

struct GIZACTIONDEFN_s;

struct ACTIONINFO_s {
    const char *name;
    u32 flags;
};

struct EXTRAACTIONDATA_s {
    const char *name;
    i32 action;
};

void RegisterGizActions(GIZACTIONDEFN_s *definitions);
extern GIZACTIONDEFN_s game_gizactiondefs[];

extern "C" {
    i32 ActionFromName(const char *name);
    u32 ActionInfoFlags(i32 action);
    const char *ActionInfoName(i32 action);
    void SetActionInfo(ACTIONINFO_s *action_info, EXTRAACTIONDATA_s *extra_action_data);
}
