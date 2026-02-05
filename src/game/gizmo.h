#pragma once

#include "nu2api.saga/nucore/common.h"

typedef struct GIZMO_s {

} GIZMO;

typedef struct GIZMOSYS_s {
    void* object;
} GIZMOSYS;

typedef int (*GIZMOGETMAXGIZMOSFN)(void*);
typedef void (*GIZMOADDGIZMOSFN)(GIZMOSYS*, int, void*, void*);
typedef void (*GIZMOLATEUPDATEFN)(void*, void*, float);
typedef int (*GIZMOGETOUTPUTFN)(GIZMO*, int, int);
typedef char* (*GIZMOGETOUTPUTNAMEFN)(GIZMO* gizmo, int output_index);
typedef int (*GIZMOGETNUMOUTPUTSFN)(GIZMO*);
typedef void (*GIZMODRAWFN)(void*, void*, float);
typedef char* (*GIZMOGETGIZMONAMEFN)(GIZMO*);
typedef void (*GIZMOACTIVATEFN)(GIZMO*, int);
typedef int (*GIZMOACTIVATEREVFN)(GIZMO*, int, int);
typedef void (*GIZMOSETVISIBILITYFN)(GIZMO*, int);
typedef void (*GIZMOALLOCATEPROGRESSDATAFN)(VARIPTR*, VARIPTR*);
typedef void (*GIZMOCLEARPROGRESSFN)(void*, void*);
typedef void (*GIZMOSTOREPROGRESSFN)(void*, void*, void*);
typedef void (*GIZMORESETFN)(void*, void*, void*);
typedef int (*GIZMORESERVEBUFFERSPACEFN)(void*, int);
typedef int (*GIZMOLOADFN)(void*, void*);

typedef struct ADDGIZMOTYPE_s {
    char *name;
    char *prefix;
    i16 id;
    u8 unknown1;
    u8 unknown2;
    GIZMOGETMAXGIZMOSFN get_max_gizmos_fn;
    GIZMOADDGIZMOSFN add_gizmos_fn;
    void *early_update_fn;
    GIZMOLATEUPDATEFN late_update_fn;
    GIZMODRAWFN draw_fn;
    void *panel_draw_fn;
    void *unknown_fn;
    GIZMOGETGIZMONAMEFN get_gizmo_name_fn;
    GIZMOGETOUTPUTFN get_output_fn;
    GIZMOGETOUTPUTNAMEFN get_output_name_fn;
    GIZMOGETNUMOUTPUTSFN get_num_outputs_fn;
    GIZMOACTIVATEFN activate_fn;
    GIZMOACTIVATEREVFN activate_rev_fn;
    GIZMOSETVISIBILITYFN set_visibility_fn;
    void *get_visibility_fn;
    void *get_pos_fn;
    void *using_special_fn;
    void *bolt_hit_plat_fn;
    void *get_best_bolt_target_fn;
    void *bolt_hit_fn;
    GIZMOALLOCATEPROGRESSDATAFN allocate_progress_data_fn;
    GIZMOCLEARPROGRESSFN clear_progress_fn;
    GIZMOSTOREPROGRESSFN store_progress_fn;
    GIZMORESETFN reset_fn;
    GIZMORESERVEBUFFERSPACEFN reserve_buffer_space_fn;
    GIZMOLOADFN load_fn;
    void *post_load_fn;
    void *add_level_sfx_fn;
} ADDGIZMOTYPE;

extern ADDGIZMOTYPE Default_ADDGIZMOTYPE;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
