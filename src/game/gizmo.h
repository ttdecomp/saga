#pragma once

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/numath/nuvec.h"

typedef struct GIZMO_s {

} GIZMO;

typedef struct GIZMOSYS_s {
    void *object;
} GIZMOSYS;

typedef struct GIZMOSET_s {

} GIZMOSET;

typedef struct BOLT_s {

} BOLT; // defined here for now, TODO: move to its own file

typedef int (*GIZMOGETMAXGIZMOSFN)(void *);
typedef void (*GIZMOADDGIZMOSFN)(GIZMOSYS *gizmo_sys, int, void *, void *);
typedef void (*GIZMOLATEUPDATEFN)(void *, void *, float);
typedef int (*GIZMOGETOUTPUTFN)(GIZMO *gizmo, int, int);
typedef char *(*GIZMOGETOUTPUTNAMEFN)(GIZMO *gizmo, int output_index);
typedef int (*GIZMOGETNUMOUTPUTSFN)(GIZMO *gizmo);
typedef void (*GIZMODRAWFN)(void *, void *, float);
typedef char *(*GIZMOGETGIZMONAMEFN)(GIZMO *);
typedef void (*GIZMOACTIVATEFN)(GIZMO *gizmo, int);
typedef int (*GIZMOACTIVATEREVFN)(GIZMO *gizmo, int, int);
typedef void (*GIZMOSETVISIBILITYFN)(GIZMO *gizmo, int);
typedef void (*GIZMOALLOCATEPROGRESSDATAFN)(VARIPTR *, VARIPTR *);
typedef void (*GIZMOCLEARPROGRESSFN)(void *, void *);
typedef void (*GIZMOSTOREPROGRESSFN)(void *, void *, void *);
typedef void (*GIZMORESETFN)(void *, void *, void *);
typedef int (*GIZMORESERVEBUFFERSPACEFN)(void *, int);
typedef int (*GIZMOLOADFN)(void *, void *);
typedef int (*GIZMOGETPOSFN)(GIZMO *gizmo);
typedef int (*GIZMOBOLTHITPLATFN)(void *, void *, BOLT *, unsigned char *);
typedef int *(*GIZMOGETBESTBOLTTARGETFN)(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         int, int, int);
typedef int (*GIZMOBOLTHITFN)(void *, void *, void *, NUVEC *, int, float, NUVEC *, NUVEC *, BOLT *, unsigned int,
                              unsigned char *);
typedef void (*GIZMOPOSTLOADFN)(void *, void *);
typedef void (*GIZMOADDLEVELSFXFN)(void *, void *, int *, int *, int);
typedef int (*GIZMOGETVISIBILITYFN)(GIZMO*);
typedef void (*GIZMOUSINGSPECIALFN)(GIZMO**, void*, int, char*);
typedef void (*GIZMOPANELDRAWFN)(void*, void*, float);
typedef void (*GIZMOEARLYUPDATEFN)(void*, void*, float);

typedef struct ADDGIZMOTYPE_s {
    char *name;
    char *prefix;
    i16 unknown1;
    u8 unknown2;
    u8 unknown3;
    GIZMOGETMAXGIZMOSFN get_max_gizmos_fn;
    GIZMOADDGIZMOSFN add_gizmos_fn;
    GIZMOEARLYUPDATEFN early_update_fn;
    GIZMOLATEUPDATEFN late_update_fn;
    GIZMODRAWFN draw_fn;
    GIZMOPANELDRAWFN panel_draw_fn;
    void *unknown_fn;
    GIZMOGETGIZMONAMEFN get_gizmo_name_fn;
    GIZMOGETOUTPUTFN get_output_fn;
    GIZMOGETOUTPUTNAMEFN get_output_name_fn;
    GIZMOGETNUMOUTPUTSFN get_num_outputs_fn;
    GIZMOACTIVATEFN activate_fn;
    GIZMOACTIVATEREVFN activate_rev_fn;
    GIZMOSETVISIBILITYFN set_visibility_fn;
    GIZMOGETVISIBILITYFN get_visibility_fn;
    GIZMOGETPOSFN get_pos_fn;
    GIZMOUSINGSPECIALFN using_special_fn;
    GIZMOBOLTHITPLATFN bolt_hit_plat_fn;
    GIZMOGETBESTBOLTTARGETFN get_best_bolt_target_fn;
    GIZMOBOLTHITFN bolt_hit_fn;
    GIZMOALLOCATEPROGRESSDATAFN allocate_progress_data_fn;
    GIZMOCLEARPROGRESSFN clear_progress_fn;
    GIZMOSTOREPROGRESSFN store_progress_fn;
    GIZMORESETFN reset_fn;
    GIZMORESERVEBUFFERSPACEFN reserve_buffer_space_fn;
    GIZMOLOADFN load_fn;
    GIZMOPOSTLOADFN post_load_fn;
    GIZMOADDLEVELSFXFN add_level_sfx_fn;
} ADDGIZMOTYPE;

extern ADDGIZMOTYPE Default_ADDGIZMOTYPE;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
