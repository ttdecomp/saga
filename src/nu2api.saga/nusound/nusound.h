#pragma once

#include "decomp.h"
#include "globals.h"

struct nusound_filename_info_s {
    char *name;
};

#ifdef __cplusplus

nusound_filename_info_s *ConfigureMusic(char *file, VARIPTR *bufferStart, VARIPTR *bufferEnd);

#endif