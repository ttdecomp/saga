#pragma once

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nucore/numemory.h"
#include "saveload/saveload.h"

struct bgprocinfo_s;
typedef struct bgprocinfo_s BGPROCINFO;

void InitOnce(i32 argc, char **param_2);

void StartPerm();
void LoadPerm();
void EndPerm();
