#pragma once

#include "nu2api/nucore/common.h"

// World areas open state (module legoapi/world, areas.cpp).

void Areas_OpenAll(i32 mode);
void Areas_ConfigureResidents(VARIPTR *buffer, VARIPTR *buffer_end);

extern i32 openlevels;
