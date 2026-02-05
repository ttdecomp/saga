#include "game/gizmo.h"
#include "decomp.h"
#include "game/gizmos/ai.h"
#include "game/gizmos/door.h"
#include "game/gizmos/edgizshadowmachine.h"
#include "game/gizmos/gizaimessage.h"
#include "game/gizmos/gizbombgen.h"
#include "game/gizmos/gizbuildits.h"
#include "game/gizmos/gizforce.h"
#include "game/gizmos/gizmopickups.h"
#include "game/gizmos/gizobstacles.h"
#include "game/gizmos/gizpanel.h"
#include "game/gizmos/gizrandom.h"
#include "game/gizmos/gizspecial.h"
#include "game/gizmos/giztimer.h"
#include "game/gizmos/giztorpmachine.h"
#include "game/gizmos/gizturrets.h"
#include "game/gizmos/grapples.h"
#include "game/gizmos/hatmachine.h"
#include "game/gizmos/lever.h"
#include "game/gizmos/minicut.h"
#include "game/gizmos/newblowup.h"
#include "game/gizmos/plugs.h"
#include "game/gizmos/portal.h"
#include "game/gizmos/push.h"
#include "game/gizmos/spinner.h"
#include "game/gizmos/technos.h"
#include "game/gizmos/teleport.h"
#include "game/gizmos/tubes.h"
#include "game/gizmos/zipups.h"

static int DefaultGizmo_GetOutput(GIZMO *, int, int) {
    return 0;
}

static char *DefaultGizmo_GetOutputName(GIZMO *, int) {
    return "default";
}

static int DefaultGizmo_GetNumOutputs(GIZMO *) {
    return 1;
}

ADDGIZMOTYPE Default_ADDGIZMOTYPE = {
    NULL,                       // name
    NULL,                       // prefix
    -1,                         // id
    0,                          // unknown1
    0,                          // unknown2
    NULL,                       // get_max_gizmos_fn
    NULL,                       // add_gizmos_fn
    NULL,                       // early_update_fn
    NULL,                       // late_update_fn
    NULL,                       // draw_fn
    NULL,                       // panel_draw_fn
    NULL,                       // unknown_fn
    NULL,                       // get_gizmo_name_fn
    DefaultGizmo_GetOutput,     // get_output_fn
    DefaultGizmo_GetOutputName, // get_output_name_fn
    DefaultGizmo_GetNumOutputs, // get_num_outputs_fn
    NULL,                       // activate_fn
    NULL,                       // activate_rev_fn
    NULL,                       // set_visibility_fn
    NULL,                       // get_visibility_fn
    NULL,                       // get_pos_fn
    NULL,                       // using_special_fn
    NULL,                       // bolt_hit_plat_fn
    NULL,                       // get_best_bolt_target_fn
    NULL,                       // bolt_hit_fn
    NULL,                       // allocate_progress_data_fn
    NULL,                       // clear_progress_fn
    NULL,                       // store_progress_fn
    NULL,                       // reset_fn
    NULL,                       // reserve_buffer_space_fn
    NULL,                       // load_fn
    NULL,                       // post_load_fn
    NULL,                       // add_level_sfx_fn
};

REGISTERGIZMOTYPEFN GizmoTypesLSW[] = {GizObstacles_RegisterGizmo,
                                       GizBuildIts_RegisterGizmo,
                                       GizForce_RegisterGizmo,
                                       NewBlowup_RegisterGizmo,
                                       GizmoPickups_RegisterGizmo,
                                       Levers_RegisterGizmo,
                                       Spinner_RegisterGizmo,
                                       MiniCut_RegisterGizmo,
                                       Tubes_RegisterGizmo,
                                       ZipUps_RegisterGizmo,
                                       GizTurrets_RegisterGizmo,
                                       GizBombGen_RegisterGizmo,
                                       AI_RegisterGizmo,
                                       GizSpecial_RegisterGizmo,
                                       GizAIMessage_RegisterGizmo,
                                       GizTimer_RegisterGizmo,
                                       GizRandom_RegisterGizmo,
                                       GizPanel_RegisterGizmo,
                                       HatMachine_RegisterGizmo,
                                       Push_RegisterGizmo,
                                       Door_RegisterGizmo,
                                       Teleport_RegisterGizmo,
                                       GizTorpMachine_RegisterGizmo,
                                       EdGizShadowMachine_RegisterGizmo,
                                       Portal_RegisterGizmo,
                                       Grapples_RegisterGizmo,
                                       Plugs_RegisterGizmo,
                                       Technos_RegisterGizmo,
                                       NULL};

#define GIZMO_TYPES_LSW_COUNT ((sizeof(GizmoTypesLSW) / sizeof(REGISTERGIZMOTYPEFN)) - 1)

VARIPTR *GizmoBufferAlloc(VARIPTR *buffer, VARIPTR *buffer_end, int size) {
    VARIPTR *ptr = NULL;

    if (buffer_end != NULL && buffer != NULL) {
        if (buffer_end->addr > buffer->addr + size) {
            ptr = (VARIPTR *)ALIGN(buffer->addr, 16);
            buffer->addr = (usize)ptr + size;
            memset(ptr, 0, size);
        }
    }

    return ptr;
}

void RegisterGizmoTypes(VARIPTR *buffer, VARIPTR *buffer_end, REGISTERGIZMOTYPEFN *register_gizmo_type_fns,
                        int unknown) {
    UNIMPLEMENTED();
}

void RegisterGizmoTypes_LSW(VARIPTR *buffer, VARIPTR *buffer_end) {
    REGISTERGIZMOTYPEFN gizmo_types[GIZMO_TYPES_LSW_COUNT + 1];
    memcpy(gizmo_types, GizmoTypesLSW, sizeof(GizmoTypesLSW));
    RegisterGizmoTypes(buffer, buffer_end, gizmo_types, 12);
}
