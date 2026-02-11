#include "legoapi/apiobject.h"
#include "nu2api.saga/nu3d/nugscn.h"
#include "nu2api.saga/nu3d/nuspline.h"
#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nulist.h"
#include "nu2api.saga/numath/nuang.h"
#include "nu2api.saga/numath/nuvec.h"

struct AISCRIPTACTIONDEF_s;
struct AISCRIPTCONDITIONDEF_s;

typedef struct AIREFSCRIPT_s {
    NULISTLNK list_node;
    char *name;
    struct AISCRIPT_s *script;
    char *return_state_name;
    struct AISTATE_s *return_state;
    u32 check_global_scripts : 1;
    u32 check_level_scripts : 1;
    NULISTHDR conditions;
} AIREFSCRIPT;

typedef struct AISTATE_s {
    NULISTLNK list_node;
    NULISTHDR conditions;
    NULISTHDR actions;
    char *name;
    NULISTHDR ref_scripts;
} AISTATE;

typedef struct AIACTION_s {
    NULISTLNK list_node;
    char **params;
    int param_count;
    struct AISCRIPTACTIONDEF_s *def;
} AIACTION;

typedef struct AICONDITION_s {
    NULISTLNK list_node;
    f32 param_val;
    char type;
    unsigned char param_idx;
    u16 bool_and : 1;
    u16 keep_blocked : 1;
    u16 is_param_idx_valid : 1;
    u16 is_complex : 1;
    char *complex_arg;
    char *arg;
    void *void_arg;
    struct AISCRIPTCONDITIONDEF_s *def;
    char *next_state_name;
    AISTATE *next_state;
    struct AICONDITION_s *param_cond;
} AICONDITION;

typedef struct AIACTIONMACRO_s {
    NULISTLNK list_node;
    char *name;
    NULISTHDR actions;
} AIACTIONMACRO;

typedef struct AICONDITIONMACRO_s {
    NULISTLNK list_node;
    char *name;
    NULISTHDR conditions;
} AICONDITIONMACRO;

typedef struct AISCRIPTPARAMS_s {
    char *name;
    f32 default_val;
} AISCRIPTPARAMS;

typedef struct AICONSTPARAMS_s {
    char name[32];
    f32 default_val;
} AICONSTPARAMS;

typedef struct AISCRIPT_s {
    NULISTLNK list_node;
    char *name;
    char *derived_from;
    NULISTHDR states;
    AISCRIPTPARAMS params[4];
    AISTATE *base_state;
    u32 is_level_script : 1;
    u32 is_derived : 1;
    u32 is_derived_from_level_script : 1;
    NULISTHDR ref_scripts;
    NULISTHDR condition_macros;
    NULISTHDR action_macros;
} AISCRIPT;

typedef struct AIPATHCNX_s {
} AIPATHCNX;

typedef struct AIROUTE_s {
} AIROUTE;

typedef struct AIANTINODE_s {
} AIANTINODE;

typedef struct AIPATH_s {
} AIPATH;

typedef struct AIPATHSYS_s {
} AIPATHSYS;

typedef struct AIPATHINFO_s {
    AIPATH *path;
    AIPATHCNX *connection;
    u8 direction;

    u16 game_flags;
    u16 next_check;

    u16 unknown_flag_1;
    u16 unknown_flag_2;
    u16 unknown_flag_4;
    u16 unknown_flag_8;

    f32 dist;
    f32 width;
} AIPATHINFO;

typedef struct AILOCATOR_s {
} AILOCATOR;

typedef struct AILOCATORSET_s {
} AILOCATORSET;

typedef struct AIAREA_s {
} AIAREA;

typedef struct AISCRIPTPROCESSSTACK_s {
    f32 complex_params[4];
    u8 is_first_time_state;
    u8 force_complex_eval;
} AISCRIPTPROCESSSTACK;

typedef struct AISCRIPTPROCESS_s {
    AISCRIPT *base_script;
    AISCRIPT *script;

    AISTATE *state;
    NULISTLNK *action_node;
    AISTATE *next_state;
    f32 params[4];
    f32 script_timer;

    AISCRIPTPROCESSSTACK param_stack[2];

    u32 is_first_time_action : 1;

    AIREFSCRIPT *active_refs[4];
    i32 active_ref_count;

    u8 action_data_1;
    u8 action_data_2;
    u16 action_data_6;
    void *action_data_3;
    f32 action_data_4;
    f32 action_data_5;

    NUVEC *action_pos;

    AIPATHINFO path_info;

    f32 action_timer;

    AIAREA *unknown_a0;
    AILOCATOR *unknown_a4;
    AILOCATORSET *unknown_a8;
    NUGSPLINE *unknown_ac;

    // Types uncertain.
    u8 unknown_b0;
    u16 unknown_b2;

    u8 interrupt_priority;
    u8 interrupt_id;

    u16 action_data_7;

    f32 interrupt_timer;
    AISTATE *interrupt_state;
    AISTATE *return_to_state;

    // Type uncertain.
    u32 unknown_c4;
} AISCRIPTPROCESS;

typedef struct AICREATURE_s {
} AICREATURE;

typedef struct AIROW_s {
    AIPATHINFO path_info;

    NUVEC pos;
    NUANG y_rot;

    AIPATHCNX *next_connection;
    u8 next_direction;

    u8 is_alive;

    u32 is_clockwise : 1;
    u32 is_turning : 1;
} AIROW;

typedef struct AIGROUP_s {
    APIOBJECT *leader;

    i16 rotation_speed;

    u8 row_count;
    u8 member_count;
    u8 count_across;

    APIOBJECT *members[16];

    i32 member_is_alive;

    u32 is_used : 1;
    u32 can_respawn : 1;
    u32 is_reversed : 1;
    u32 is_in_formation : 1;
    u32 is_row_turning : 1;

    AIROW rows[4];

    f32 radius;
    f32 x_spacing;
    f32 z_spacing;
    f32 max_speed;
} AIGROUP;

typedef struct AISYS_s {
    // Type uncertain.
    i32 unknown;

    VARIPTR buf;
    VARIPTR buf_end;
    i32 buf_size;

    AIPATHSYS *path_sys;

    i32 creature_count;
    AICREATURE *creatures;

    NULISTHDR scripts;

    i32 locator_count;
    AILOCATOR *locators;

    i32 locator_set_count;
    AILOCATORSET *locator_sets;

    i32 area_count;
    AIAREA *areas;

    AIGROUP groups[16];

    i32 antinode_count;
    AIANTINODE *antinodes;

    u8 next_area_check;
    u8 goody_idx;

    i16 has_done_reset : 1;

    // The AI system for game-specific logic.
    void *game_sys;

    APIOBJECT *player_1;
    APIOBJECT *player_2;

    NUGSCN *scene;
} AISYS;

typedef struct AIPACKET_s {
} AIPACKET;

typedef int AIACTIONFN(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char **, int, int, f32);
typedef f32 AICONDITIONFN(AISYS *, AISCRIPTPROCESS *, AIPACKET *, char *, void *);
typedef void *AICONDITIONINITFN(AISYS *, char *, AISCRIPT *);

typedef struct AISCRIPTACTIONDEF_s {
    char *name;
    AIACTIONFN *eval_fn;
    char unknown;
    char unknown2;
    i16 unknown3;
} AIACTIONDEF;

typedef struct AISCRIPTCONDITIONDEF_s {
    char *name;
    AICONDITIONFN *eval_fn;
    AICONDITIONINITFN *init_fn;
} AICONDITIONDEF;

typedef int GAMEPARAMTOFLOAT(AIPACKET *, AISCRIPTPROCESS *, char *, f32 *);

#ifdef __cplusplus
extern "C" {
#endif
    extern i32 AiParseExpressionFailed;

    extern AICONDITIONDEF api_aiconditiondefs[];

    extern AIACTIONDEF api_aiactiondefs[];
    extern AIACTIONDEF *game_aiactiondefs;

    extern NULISTHDR global_aiscripts;

    extern i32 ai_usepackfile;

    extern GAMEPARAMTOFLOAT *GameParamToFloatFn;

    void AIScriptLoadAll(char *path, VARIPTR *buf, VARIPTR *buf_end, AISYS *sys);
    void AIScriptLoadAllPakFile(void *pak, char *path, VARIPTR *buf, VARIPTR *buf_end, AISYS *sys);

    void AIScriptClearInterrupt(AISCRIPTPROCESS *processor, char *state_name);

    AISTATE *AIStateFind(char *name, AISCRIPT *script);
#ifdef __cplusplus
}
#endif

f32 AiParseExpression(char *expr);

void AIScriptOpenPakFileParse(AISCRIPT **script_ref, void *pak, char *filename, char *path, VARIPTR *buf,
                              VARIPTR *buf_end);
