#pragma once

#include "decomp.h"

struct AIROW_s;
struct nuqthdr_s;
struct nunativegscene_s;
struct SHOPINPUT;

void Movie_Play(char *, VARIPTR *, VARIPTR *, float, i32 (*)(), float);

void Movies_ConfigureList(char *, VARIPTR *, VARIPTR *);
