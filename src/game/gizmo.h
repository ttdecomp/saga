#pragma once

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/numath/nuvec.h"

typedef struct GIZMO_s {
    union {
        void *void_ptr;
        struct GIZTIMER_s *timer;
        struct nuportal_s *portal;
    } object;
    i16 unknown;
    u8 type_id;
} GIZMO;

typedef struct GIZMOSET_s {
    struct gizmotype_s *type;
    i32 count;
    i32 max_count;
    GIZMO *gizmos;
    void *unknown;
} GIZMOSET;

typedef struct GIZMOSYS_s {
    GIZMOSET *sets;
    char *error_log;
    u8 flags;
} GIZMOSYS;

typedef struct BOLT_s {

} BOLT; // defined here for now, TODO: move to its own file

typedef int (*GIZMOGETMAXGIZMOSFN)(void *);
typedef void (*GIZMOADDGIZMOSFN)(GIZMOSYS *gizmo_sys, int, void *, void *);
typedef void (*GIZMOLATEUPDATEFN)(void *, void *, float delta_time);
typedef int (*GIZMOGETOUTPUTFN)(GIZMO *gizmo, int, int);
typedef char *(*GIZMOGETOUTPUTNAMEFN)(GIZMO *gizmo, int output_index);
typedef int (*GIZMOGETNUMOUTPUTSFN)(GIZMO *gizmo);
typedef void (*GIZMODRAWFN)(void *, void *, float);
typedef char *(*GIZMOGETGIZMONAMEFN)(GIZMO *);
typedef void (*GIZMOACTIVATEFN)(GIZMO *gizmo, int);
typedef int (*GIZMOACTIVATEREVFN)(GIZMO *gizmo, int, int);
typedef void (*GIZMOSETVISIBILITYFN)(GIZMO *gizmo, int);
typedef void *(*GIZMOALLOCATEPROGRESSDATAFN)(VARIPTR *, VARIPTR *);
typedef void (*GIZMOCLEARPROGRESSFN)(void *, void *);
typedef void (*GIZMOSTOREPROGRESSFN)(void *, void *, void *);
typedef void (*GIZMORESETFN)(void *, void *, void *);
typedef void *(*GIZMORESERVEBUFFERSPACEFN)(void *);
typedef int (*GIZMOLOADFN)(void *, void *);
typedef int (*GIZMOGETPOSFN)(GIZMO *gizmo);
typedef int (*GIZMOBOLTHITPLATFN)(void *, void *, BOLT *, unsigned char *);
typedef int *(*GIZMOGETBESTBOLTTARGETFN)(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         int, int, int);
typedef int (*GIZMOBOLTHITFN)(void *, void *, void *, NUVEC *, int, float, NUVEC *, NUVEC *, BOLT *, unsigned int,
                              unsigned char *);
typedef void (*GIZMOPOSTLOADFN)(void *, void *);
typedef void (*GIZMOADDLEVELSFXFN)(void *, void *, int *, int *, int);
typedef int (*GIZMOGETVISIBILITYFN)(GIZMO *);
typedef void (*GIZMOUSINGSPECIALFN)(GIZMO **, void *, int, char *);
typedef void (*GIZMOPANELDRAWFN)(void *, void *, float);
typedef void (*GIZMOEARLYUPDATEFN)(void *, void *, float delta_time);

typedef struct GIZMOFNS_s {
    i16 unknown1;
    // pretty sure these two are just padding and can be removed, but keeping them for now to be safe
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
} GIZMOFNS;

typedef struct ADDGIZMOTYPE_s {
    char *name;
    char *prefix;
    GIZMOFNS fns;
} ADDGIZMOTYPE;

typedef ADDGIZMOTYPE *(*REGISTERGIZMOTYPEFN)(int);

typedef struct gizmotype_s {
    char name[32];
    char prefix[8];
    GIZMOFNS fns;
    VARIPTR *buffer;
} GIZMOTYPE;

typedef struct GIZMOTYPES_s {
    int count;
    int unknown;
    GIZMOTYPE *types;
} GIZMOTYPES;

extern ADDGIZMOTYPE Default_ADDGIZMOTYPE;

#ifdef __cplusplus

VARIPTR *GizmoBufferAlloc(VARIPTR *buffer, VARIPTR *buffer_end, int size);
void RegisterGizmoTypes(VARIPTR *buffer, VARIPTR *buffer_end, REGISTERGIZMOTYPEFN *register_gizmo_type_fns,
                        int unknown);
void RegisterGizmoTypes_LSW(VARIPTR *buffer, VARIPTR *buffer_end);
GIZMO *AddGizmo(GIZMOSYS *gizmo_sys, int type_id, char *name, void *object);
int GizmoGetTypeIDByName(GIZMOSYS *gizmo_sys, char *name);
void GizmoSetVisibility(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int visibility, int unknown);
int GizmoGetVisibility(GIZMOSYS *gizmo_sys, GIZMO *gizmo);
void GizmoActivate(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int unknown1, int unknown2);
char *GizmoGetOutputName(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int output_index);
int GizmoGetOutput(GIZMOSYS *gizmo_sys, GIZMO *gizmo, int unknown1, int unknown2);
void GizmoSysEarlyUpdate(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
void GizmoSysLateUpdate(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
void GizmoSysDraw(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
void GizmoSysPanelDraw(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
int GizmoSys_BoltHitPlat(GIZMOSYS *gizmo_sys, void *world_info, BOLT *bolt, unsigned char *unknown);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
