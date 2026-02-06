#pragma once

#include "game/gizmo.h"
#include "game/world.h"

extern int giztimer_gizmotype_id;

typedef struct GIZTIMER_s {
    f32 time_remaining;
    f32 start_time;
    char filler1[2];
    unsigned char flags; // unsure if this is actually what this is
    char filler2;
    char name[16];
} GIZTIMER;

#ifdef __cplusplus

ADDGIZMOTYPE *GizTimer_RegisterGizmo(int type_id);
int GizTimer_GetMaxGizmos(void *timer);
void GizTimer_AddGizmos(GIZMOSYS *gizmo_sys, int unknown1, void * world_info, void * unknown2);
void GizTimer_Update(void *world_info, void *, float delta_time);
char *GizTimer_GetGizmoName(GIZMO *gizmo);
int GizTimer_GetOutput(GIZMO *gizmo, int, int);
char *GizTimer_GetOutputName(GIZMO *gizmo, int output_index);
int GizTimer_GetNumOutputs(GIZMO *gizmo);
void GizTimer_Activate(GIZMO *gizmo, int unknown);
int GizTimer_ReserveBufferSpace(void *, int);
int GizTimer_Load(void *world_info, void *);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
