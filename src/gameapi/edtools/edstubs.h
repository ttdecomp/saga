#pragma once

#include "nu2api/nu3d/nugscn.h"
#include "nu2api/nucore/common.h"

// Editor subsystem stubs — all symbols with the "ed" prefix.
// Definitions are in edstubs.cpp.

#ifdef __cplusplus
extern "C" {
#endif

    void edbitsRegisterBaseScene(NUGSCN *scene);
    void edanimRegisterBaseScene(NUGSCN *scene);
    i32 edanimLoadPage(char *path, NUGSCN *scene);
    void edbitsRegisterBaseTerrain(void *terrain);
    void edppStartPage(i32 page);
    void edpartStartPage(i32 page);
    void edbriStartPage(i32 page);
    void edgraStartPage(i32 page);
    i32 edgraLoadPage(char *path, void *gscn, i32 terrain, void *buf, void *buf_end);
    i32 edbriLoadPage(char *path, void *gscn);
    void edpartSetParticlePage(i32 page);
    i32 edpartLoadPage(char *path, i32 param, void *gscn);
    void edbitsRegisterThingsScene(NUGSCN *scene);
    void edgraClumpsReset(void);
    void edanimParamReset(void);

#ifdef __cplusplus
}
#endif
