#pragma once

#include "game/gizmo.h"

extern int giztimer_gizmotype_id;

#ifdef __cplusplus

typedef struct GIZTIMER_s {
} GIZTIMER;

ADDGIZMOTYPE* GizTimer_RegisterGizmo(int type_id);
int GizTimer_GetMaxGizmos(void *timer);
void GizTimer_AddGizmos(GIZMOSYS *gizmo_sys, int, void *, void *);
void GizTimer_Update(void *, void *, float);
char *GizTimer_GetGizmoName(GIZMO *gizmo);
int GizTimer_GetOutput(GIZMO *gizmo, int, int);
char *GizTimer_GetOutputName(GIZMO *gizmo, int output_index);
int GizTimer_GetNumOutputs(GIZMO *gizmo);
void GizTimer_Activate(GIZMO *gizmo, int);
int GizTimer_ReserveBufferSpace(void *, int);
int GizTimer_Load(void *, void *);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
