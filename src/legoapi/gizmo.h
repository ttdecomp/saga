#pragma once

#include "nu2api/nucore/common.h"
#include "nu2api/numath/nuvec.h"

typedef struct GIZMO_s {
    void *object;
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

typedef i32 (*GIZMOGETMAXGIZMOSFN)(void *);
typedef void (*GIZMOADDGIZMOSFN)(GIZMOSYS *gizmo_sys, i32, void *, void *);
typedef void (*GIZMOLATEUPDATEFN)(void *, void *, float delta_time);
typedef i32 (*GIZMOGETOUTPUTFN)(GIZMO *gizmo, i32, i32);
typedef char *(*GIZMOGETOUTPUTNAMEFN)(GIZMO *gizmo, i32 output_index);
typedef i32 (*GIZMOGETNUMOUTPUTSFN)(GIZMO *gizmo);
typedef void (*GIZMODRAWFN)(void *, void *, float);
typedef char *(*GIZMOGETGIZMONAMEFN)(GIZMO *);
typedef void (*GIZMOACTIVATEFN)(GIZMO *gizmo, i32);
typedef i32 (*GIZMOACTIVATEREVFN)(GIZMO *gizmo, i32, i32);
typedef void (*GIZMOSETVISIBILITYFN)(GIZMO *gizmo, i32);
typedef void *(*GIZMOALLOCATEPROGRESSDATAFN)(VARIPTR *, VARIPTR *);
typedef void (*GIZMOCLEARPROGRESSFN)(void *, void *);
typedef void (*GIZMOSTOREPROGRESSFN)(void *, void *, void *);
typedef void (*GIZMORESETFN)(void *, void *, void *);
typedef void *(*GIZMORESERVEBUFFERSPACEFN)(void *);
typedef i32 (*GIZMOLOADFN)(void *, void *);
typedef i32 (*GIZMOGETPOSFN)(GIZMO *gizmo);
typedef i32 (*GIZMOBOLTHITPLATFN)(void *, void *, BOLT *, unsigned char *);
typedef i32 *(*GIZMOGETBESTBOLTTARGETFN)(GIZMOSET *, float *, NUVEC *, NUVEC *, void *, NUVEC *, NUVEC *, float, float,
                                         i32, i32, i32);
typedef i32 (*GIZMOBOLTHITFN)(void *, void *, void *, NUVEC *, i32, float, NUVEC *, NUVEC *, BOLT *, u32,
                              unsigned char *);
typedef void (*GIZMOPOSTLOADFN)(void *, void *);
typedef void (*GIZMOADDLEVELSFXFN)(void *, void *, i32 *, i32 *, i32);
typedef i32 (*GIZMOGETVISIBILITYFN)(GIZMO *);
typedef void (*GIZMOUSINGSPECIALFN)(GIZMO **, void *, i32, char *);
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

typedef ADDGIZMOTYPE *(*REGISTERGIZMOTYPEFN)(i32);

typedef struct gizmotype_s {
    char name[32];
    char prefix[8];
    GIZMOFNS fns;
    VARIPTR *buffer;
} GIZMOTYPE;

typedef struct GIZMOTYPES_s {
    i32 count;
    i32 unknown;
    GIZMOTYPE *types;
} GIZMOTYPES;

extern ADDGIZMOTYPE Default_ADDGIZMOTYPE;

#ifdef __cplusplus

VARIPTR *GizmoBufferAlloc(VARIPTR *buffer, VARIPTR *buffer_end, i32 size);
void RegisterGizmoTypes(VARIPTR *buffer, VARIPTR *buffer_end, REGISTERGIZMOTYPEFN *register_gizmo_type_fns,
                        i32 unknown);
void RegisterGizmoTypes_LSW(VARIPTR *buffer, VARIPTR *buffer_end);
GIZMO *AddGizmo(GIZMOSYS *gizmo_sys, i32 type_id, char *name, void *object);
i32 GizmoGetTypeIDByName(GIZMOSYS *gizmo_sys, char *name);
void GizmoSetVisibility(GIZMOSYS *gizmo_sys, GIZMO *gizmo, i32 visibility, i32 unknown);
i32 GizmoGetVisibility(GIZMOSYS *gizmo_sys, GIZMO *gizmo);
void GizmoActivate(GIZMOSYS *gizmo_sys, GIZMO *gizmo, i32 unknown1, i32 unknown2);
char *GizmoGetOutputName(GIZMOSYS *gizmo_sys, GIZMO *gizmo, i32 output_index);
i32 GizmoGetOutput(GIZMOSYS *gizmo_sys, GIZMO *gizmo, i32 unknown1, i32 unknown2);
void GizmoSysEarlyUpdate(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
void GizmoSysLateUpdate(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
void GizmoSysDraw(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
void GizmoSysPanelDraw(GIZMOSYS *gizmo_sys, void *world_info, float delta_time);
i32 GizmoSys_BoltHitPlat(GIZMOSYS *gizmo_sys, void *world_info, BOLT *bolt, unsigned char *unknown);
i32 ResetGizmoType(GIZMOSYS *gizmo_sys, i32 type_id, char *name);
void GizmoSysClearLevelProgress(void *unknown, i32 type_id);

extern "C" {
#endif

#ifdef __cplusplus
}
#endif
