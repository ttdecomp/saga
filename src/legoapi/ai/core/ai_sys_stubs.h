#ifndef LEGOAPI_AI_CORE_AI_SYS_STUBS_H
#define LEGOAPI_AI_CORE_AI_SYS_STUBS_H

#include "decomp.h"

// AI world-query helpers (module legoapi/ai/core, ai_sys_stubs.cpp). These are
// C-linkage symbols (unmangled). AIPAthFindPathCnx is intentionally not here:
// episodeI and episodeII call it with different arities (each byte-matched), so
// it is declared locally in those files.

#ifdef __cplusplus
extern "C" {
#endif

    struct AISYS_s;

    void *AIPathFindLocator(AISYS_s *aisys, char *name);
    void *AISysFindArea(void *ai_sys, char *name);
    void *AIPathFindNode(AISYS_s *aisys, char *name, i32 unknown);

#ifdef __cplusplus
}
#endif

#endif
