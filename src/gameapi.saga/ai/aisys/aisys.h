#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nulist.h"

typedef struct AISYS_s AISYS;

typedef struct AISCRIPTPROCESS_s AISCRIPTPROCESS;

typedef struct AIPACKET_s AIPACKET;

typedef int GAMEPARAMTOFLOAT(AIPACKET *, AISCRIPTPROCESS *, char *, f32 *);

typedef struct AISTATE_s {
    NULISTLNK list_node;
    NULISTHDR conditions;
    NULISTHDR actions;
    char *name;
    NULISTHDR ref_scripts;
} AISTATE;

typedef struct AICONSTPARAMS_s {
    char name[32];
    f32 default_val;
} AICONSTPARAMS;

typedef struct AISCRIPTPARAMS_s {
    char *name;
    f32 default_val;
} AISCRIPTPARAMS;

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

typedef struct AIACTION_s {
    NULISTLNK list_node;
    char **params;
    int param_count;
    AIACTIONDEF *def;
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
    AICONDITIONDEF *def;
    char *next_state_name;
    AISTATE *next_state;
    struct AICONDITION_s *param_cond;
} AICONDITION;

typedef struct AIREFSCRIPT_s {
    NULISTLNK list_node;
    char *name;
    AISCRIPT *script;
    char *return_state_name;
    AISTATE *return_state;
    u32 check_global_scripts : 1;
    u32 check_local_scripts : 1;
    NULISTHDR conditions;
} AIREFSCRIPT;

#ifdef __cplusplus
extern "C" {
#endif

    extern int AiParseExpressionFailed;
    extern AICONDITIONDEF api_aiconditiondefs[];
    extern AIACTIONDEF api_aiactiondefs[];
    extern AIACTIONDEF *game_aiactiondefs;

#ifdef __cplusplus
}
#endif

f32 AiParseExpression(char *expr);

void AIScriptOpenPakFileParse(AISCRIPT_s **script_ref, void *pak, char *filename, char *path, VARIPTR *buf,
                              VARIPTR *buf_end);
