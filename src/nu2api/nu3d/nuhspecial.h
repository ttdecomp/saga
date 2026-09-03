#pragma once

#include "decomp_assert.h"

struct nugscn_s;
struct NUDISPLAYSPECIAL;

struct nuhspecial_s {
    nugscn_s *scene;
    void *special;
    struct NUDISPLAYSPECIAL *display_special;
};

DECOMP_ASSERT(sizeof(nuhspecial_s) == 0xc, "nuhspecial_s size");
