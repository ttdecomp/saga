#include <stdio.h>
#include <string.h>

#include "gameapi.saga/ai/aisys/aisys.h"

#include "nu2api.saga/nucore/common.h"
#include "nu2api.saga/nucore/nulist.h"
#include "nu2api.saga/nucore/nurdp.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"
#include "nu2api.saga/nufile/nufilepak.h"
#include "nu2api.saga/nufile/nufpar.h"

#include <stdio.h>
#include <string.h>

static void *load_pakfile;
static char *load_path;

static VARIPTR *load_buff;
static VARIPTR *load_endbuff;

static AISCRIPT *load_aiscript;
static AISTATE *load_aistate;

static NULISTHDR *load_actionshdr;
static NULISTHDR *load_conditionshdr;
static NULISTHDR *load_refscripthdr;

static AICONSTPARAMS aiscript_const[8];
static int aiscript_const_curr;

static int condition_has_no_goto;

AIACTIONDEF *game_aiactiondefs;
static AICONDITIONDEF *game_aiconditiondefs;

static f32 Condition_AlwaysTrue(AISYS *sys, AISCRIPTPROCESS *processor, AIPACKET *packet, char *arg, void *void_arg) {
    return *(f32 *)&void_arg;
}

static void *Condition_AlwaysTrueInit(AISYS *sys, char *arg, AISCRIPT *script) {
    f32 value;

    if (arg != NULL && NuStrLen(arg) != 0) {
        value = NuAToF(arg);

        return *(void **)&value;
    }

    value = 1.0f;
    return *(void **)&value;
}

AIACTIONDEF api_aiactiondefs[] = {{NULL, NULL, 0, 0, 0}};

AICONDITIONDEF api_aiconditiondefs[] = {{"PreviousResult", NULL, NULL},
                                        {"AlwaysTrue", &Condition_AlwaysTrue, &Condition_AlwaysTrueInit},
                                        {NULL, NULL, NULL}};

static int ExpressionRequiredNameLookup;
static int ExpressionNameLookupFailed;

int AiParseExpressionFailed;

GAMEPARAMTOf32 *GameParamTof32Fn;

static int AiParseExpressionNameLoopup(char *name, f32 *f32_out, int *int_out) {
    f32 value;

    ExpressionRequiredNameLookup = 1;
    value = 0.0f;
    ExpressionNameLookupFailed = 1;

    if (GameParamTof32Fn != NULL) {
        if ((*GameParamTof32Fn)(NULL, NULL, name, &value) != 0) {
            ExpressionNameLookupFailed = 0;
        }
    }

    if (f32_out != NULL) {
        *f32_out = value;
    }

    if (int_out != NULL) {
        *int_out = value;
    }

    return 0;
}

f32 AiParseExpression(char *expr) {
    f32 value;

    ExpressionRequiredNameLookup = 0;
    ExpressionNameLookupFailed = 0;
    AiParseExpressionFailed = 0;

    value = NuRDPFVar(expr, AiParseExpressionNameLoopup);

    if (ExpressionNameLookupFailed != 0) {
        AiParseExpressionFailed = 1;
    }

    return value;
}

static void *AIScriptBufferAlloc(VARIPTR *buf, VARIPTR *buf_end, int size) {
    void *ret;

    ret = NULL;
    if (buf == NULL || buf_end == NULL) {
        // Debug logging goes here.
    } else {
        if (buf_end->addr > buf->addr + size) {
            ret = (void *)ALIGN(buf->addr, 0x10);
            buf->void_ptr = (void *)((isize)ret + size);
            memset(ret, 0, size);
        } else {
            // Debug logging goes here.
        }
    }

    return ret;
}

static char *AIScriptCopyString(char *str, VARIPTR *buf, VARIPTR *buf_end) {
    char *dst;
    int len;

    dst = NULL;
    if (str != NULL) {
        len = NuStrLen(str);
        if (len != 0) {
            dst = (char *)AIScriptBufferAlloc(buf, buf_end, len + 1);
            NuStrCpy(dst, str);
        }
    }

    return dst;
}

static AIACTIONDEF *AIActionFind(char *name) {
    AIACTIONDEF *def;

    if (game_aiactiondefs != NULL) {
        for (def = game_aiactiondefs; def->name != NULL; def++) {
            if (NuStrICmp(name, def->name) == 0) {
                return def;
            }
        }
    }

    for (def = api_aiactiondefs; def->name != NULL; def++) {
        if (NuStrICmp(name, def->name) == 0) {
            return def;
        }
    }

    return NULL;
}

__attribute__((noinline)) static void AIScriptCopyActions(NULISTHDR *src, NULISTHDR *dst, VARIPTR *buf,
                                                          VARIPTR *buf_end) {
    AIACTION *src_action;

    src_action = (AIACTION *)NuLinkedListGetHead(src);
    while (src_action != NULL) {
        AIACTION *dst_action;

        dst_action = (AIACTION *)AIScriptBufferAlloc(buf, buf_end, sizeof(AIACTION));
        if (dst_action != NULL) {
            dst_action->params = (char **)AIScriptBufferAlloc(buf, buf_end, src_action->param_count * sizeof(char *));
            dst_action->param_count = src_action->param_count;

            for (int i = 0; i < dst_action->param_count; i++) {
                dst_action->params[i] = AIScriptCopyString(src_action->params[i], buf, buf_end);
            }

            dst_action->def = src_action->def;

            NuLinkedListAppend(dst, &dst_action->list_node);
        }

        src_action = (AIACTION *)NuLinkedListGetNext(src, &src_action->list_node);
    }
}

static void xActions(NUFPAR *parser) {
    while (NuFParGetLine(parser) != 0) {
        AIACTION *action;

        NuFParGetWord(parser);

        if (NuStrICmp(parser->word_buf_ptr, "}") == 0) {
            return;
        }

        if (NuStrICmp(parser->word_buf_ptr, "DoMacro") == 0) {
            AIACTIONMACRO *macro;

            NuFParGetWord(parser);

            macro = (AIACTIONMACRO *)NuLinkedListGetHead(&load_aiscript->action_macros);
            while (macro != NULL) {
                if (NuStrICmp(macro->name, parser->word_buf_ptr) == 0) {
                    AIScriptCopyActions(&macro->actions, load_actionshdr, load_buff, load_endbuff);

                    break;
                }

                macro = (AIACTIONMACRO *)NuLinkedListGetNext(&load_aiscript->action_macros, &macro->list_node);
            }
        } else {
            AIACTIONDEF *def;
            char params[24][128];

            def = AIActionFind(parser->word_buf_ptr);
            if (def != NULL) {
                int param_count;

                action = (AIACTION *)AIScriptBufferAlloc(load_buff, load_endbuff, sizeof(AIACTION));
                if (action == NULL) {
                    continue;
                }

                action->list_node.next = NULL;

                NuLinkedListAppend(load_actionshdr, &action->list_node);

                action->def = def;

                param_count = 0;

                while (NuFParGetWord(parser) != 0) {
                    if (NuStrCmp(parser->word_buf_ptr, "\\") == 0) {
                        NuFParGetLine(parser);
                    } else {
                        NuStrCpy(params[param_count++], parser->word_buf_ptr);
                    }
                }

                if (param_count != 0) {
                    action->params =
                        (char **)AIScriptBufferAlloc(load_buff, load_endbuff, param_count * sizeof(char *));
                    if (action->params != NULL) {
                        action->param_count = param_count;

                        for (int i = 0; i < param_count; i++) {
                            action->params[i] = AIScriptCopyString(params[i], load_buff, load_endbuff);
                        }
                    }
                }
            } else {
                NuStrChr(parser->word_buf_ptr, '{');
            }
        }
    }
}

__attribute__((noinline)) static void ConditionsParseFinalise(AICONDITION *cond, NUFPAR *parser) {
    cond->keep_blocked = 0;

    NuFParGetWord(parser);

    cond->next_state_name = AIScriptCopyString(parser->word_buf_ptr, load_buff, load_endbuff);

    NuFParGetWord(parser);

    if (NuStrICmp(parser->word_buf_ptr, "KeepBlockedMessages") == 0) {
        cond->keep_blocked = 1;
    }
}

static AICONDITIONDEF *AIConditionFind(char *name) {
    AICONDITIONDEF *def;

    if (game_aiconditiondefs != NULL) {
        for (def = game_aiconditiondefs; def->name != NULL; def++) {
            if (NuStrICmp(name, def->name) == 0) {
                return def;
            }
        }
    }

    for (def = api_aiconditiondefs; def->name != NULL; def++) {
        if (NuStrICmp(name, def->name) == 0) {
            return def;
        }
    }

    return NULL;
}

__attribute__((noinline)) static void AIScriptCopyConditions(NULISTHDR *src, NULISTHDR *dst, VARIPTR *buf,
                                                             VARIPTR *buf_end) {
    AICONDITION *src_cond;

    src_cond = (AICONDITION *)NuLinkedListGetHead(src);
    while (src_cond != NULL) {
        AICONDITION *dst_cond;

        dst_cond = (AICONDITION *)AIScriptBufferAlloc(buf, buf_end, sizeof(AICONDITION));
        if (dst_cond != NULL) {
            memset(dst_cond, 0, sizeof(AICONDITION));
            memmove(dst_cond, src_cond, sizeof(AICONDITION));

            dst_cond->list_node.next = NULL;
            dst_cond->list_node.prev = NULL;

            dst_cond->arg = AIScriptCopyString(src_cond->arg, buf, buf_end);
            dst_cond->next_state_name = AIScriptCopyString(src_cond->next_state_name, buf, buf_end);

            NuLinkedListAppend(dst, &dst_cond->list_node);
        }

        src_cond = (AICONDITION *)NuLinkedListGetNext(src, &src_cond->list_node);
    }
}

enum { EQ = 0, LT = 1, GT = 2, LT_EQ = 3, GT_EQ = 4, NEQ = 5 };

static void xConditions(NUFPAR *parser) {
    int complex_idx = 0;

    while (NuFParGetLine(parser) != 0) {
        AICONDITION *cond;
        AICONDITIONDEF *def;

        NuFParGetWord(parser);

        if (NuStrICmp(parser->word_buf_ptr, "}") == 0) {
            return;
        }

        if (NuStrICmp(parser->word_buf_ptr, "IF") == 0) {
            NuFParGetWord(parser);

            def = AIConditionFind(parser->word_buf_ptr);
            if (def == NULL) {
                continue;
            }

            cond = (AICONDITION *)AIScriptBufferAlloc(load_buff, load_endbuff, sizeof(AICONDITION));
            if (cond == NULL) {
                continue;
            }

            memset(cond, 0, sizeof(AICONDITION));

            NuLinkedListAppend(load_conditionshdr, &cond->list_node);

            cond->def = def;

            NuFParGetWord(parser);

            if (NuStrICmp(parser->word_buf_ptr, "=") == 0 || NuStrICmp(parser->word_buf_ptr, "==") == 0) {
                cond->type = EQ;
            } else if (NuStrICmp(parser->word_buf_ptr, "<") == 0) {
                cond->type = LT;
            } else if (NuStrICmp(parser->word_buf_ptr, ">") == 0) {
                cond->type = GT;
            } else if (NuStrICmp(parser->word_buf_ptr, "<=") == 0) {
                cond->type = LT_EQ;
            } else if (NuStrICmp(parser->word_buf_ptr, ">=") == 0) {
                cond->type = GT_EQ;
            } else if (NuStrICmp(parser->word_buf_ptr, "!=") == 0) {
                cond->type = NEQ;
            } else {
                cond->arg = AIScriptCopyString(parser->word_buf_ptr, load_buff, load_endbuff);

                NuFParGetWord(parser);

                if (NuStrICmp(parser->word_buf_ptr, "=") == 0 || NuStrICmp(parser->word_buf_ptr, "==") == 0) {
                    cond->type = EQ;
                } else if (NuStrICmp(parser->word_buf_ptr, "<") == 0) {
                    cond->type = LT;
                } else if (NuStrICmp(parser->word_buf_ptr, ">") == 0) {
                    cond->type = GT;
                } else if (NuStrICmp(parser->word_buf_ptr, "<=") == 0) {
                    cond->type = LT_EQ;
                } else if (NuStrICmp(parser->word_buf_ptr, ">=") == 0) {
                    cond->type = GT_EQ;
                } else if (NuStrICmp(parser->word_buf_ptr, "!=") == 0) {
                    cond->type = NEQ;
                }
            }

            NuFParGetWord(parser);

            if (cond->param_cond == NULL) {
                char param_idx;
                char *param;

                param = NuStrIStr(parser->word_buf_ptr, "param");
                if (param == NULL) {
                    for (int i = 0; i < 4; i++) {
                        char *param_name = load_aiscript->params[i].name;

                        if (param_name != NULL && NuStrICmp(param_name, parser->word_buf_ptr) == 0) {
                            cond->param_idx = i;
                            cond->is_param_idx_valid = 1;

                            break;
                        }
                    }
                } else {
                    cond->param_idx = NuAToI(param);

                    if (cond->param_idx < 4) {
                        cond->is_param_idx_valid = 1;
                    }
                }

                if (!cond->is_param_idx_valid) {
                    int i;

                    for (i = 0; i < aiscript_const_curr; i++) {
                        if (NuStrICmp(aiscript_const[i].name, parser->word_buf_ptr) == 0) {
                            cond->param_val = aiscript_const[i].default_val;

                            break;
                        }
                    }

                    if (i == aiscript_const_curr) {
                        cond->param_val = AiParseExpression(parser->word_buf_ptr);

                        if (AiParseExpressionFailed) {
                            cond->is_complex = 1;
                            cond->param_idx = complex_idx++;
                            cond->complex_arg = AIScriptCopyString(parser->word_buf_ptr, load_buff, load_endbuff);
                        }
                    }
                }
            }

            NuFParGetWord(parser);

            if (NuStrICmp(parser->word_buf_ptr, "goto") == 0) {
                ConditionsParseFinalise(cond, parser);
            } else if (NuStrICmp(parser->word_buf_ptr, "and") == 0) {
                cond->bool_and = 1;
            }
        } else if (NuStrICmp(parser->word_buf_ptr, "OnMacro") == 0) {
            AICONDITIONMACRO *macro;

            NuFParGetWord(parser);

            macro = (AICONDITIONMACRO *)NuLinkedListGetHead(&load_aiscript->condition_macros);
            while (macro != NULL) {
                if (NuStrICmp(macro->name, parser->word_buf_ptr) == 0) {
                    AICONDITION *macro_last;

                    AIScriptCopyConditions(&macro->conditions, load_conditionshdr, load_buff, load_endbuff);
                    macro_last = (AICONDITION *)NuLinkedListGetTail(load_conditionshdr);

                    NuFParGetWord(parser);

                    if (NuStrICmp(parser->word_buf_ptr, "goto") == 0) {
                        ConditionsParseFinalise(macro_last, parser);
                    } else if (NuStrICmp(parser->word_buf_ptr, "and") == 0) {
                        macro_last->bool_and = 1;
                    }

                    break;
                }

                macro = (AICONDITIONMACRO *)NuLinkedListGetNext(&load_aiscript->condition_macros, &macro->list_node);
            }
        } else {
            NuStrChr(parser->word_buf_ptr, '{');
        }
    }
}

static void xRefScript(NUFPAR *parser) {
    AIREFSCRIPT *ref_script;
    VARIPTR orig_buf;

    if (load_refscripthdr == NULL) {
        return;
    }

    orig_buf = *load_buff;

    ref_script = (AIREFSCRIPT *)AIScriptBufferAlloc(load_buff, load_endbuff, sizeof(AIREFSCRIPT));
    if (ref_script == NULL) {
        return;
    }

    memset(ref_script, 0, sizeof(AIREFSCRIPT));

    while (NuFParGetLine(parser) != 0) {
        while (NuFParGetWord(parser) != 0) {
            char *cursor;

            if (NuStrICmp(parser->word_buf_ptr, "}") == 0) {
                goto done;
            }

            if ((cursor = NuStrIStr(parser->word_buf_ptr, "Script")) != NULL) {
                ref_script->name = AIScriptCopyString(cursor + strlen("Script") + 1, load_buff, load_endbuff);
            } else if ((cursor = NuStrIStr(parser->word_buf_ptr, "ReturnState")) != NULL) {
                ref_script->return_state_name =
                    AIScriptCopyString(cursor + strlen("ReturnState") + 1, load_buff, load_endbuff);
            } else if (NuStrICmp(parser->word_buf_ptr, "CONDITIONS") == 0) {
                load_conditionshdr = &ref_script->conditions;
                condition_has_no_goto = 1;

                xConditions(parser);

                load_conditionshdr = NULL;
                condition_has_no_goto = 0;
            } else if (NuStrIStr(parser->word_buf_ptr, "Source") != NULL) {
                if (NuStrIStr(parser->word_buf_ptr, "Global") != NULL) {
                    ref_script->check_global_scripts = 1;
                } else if (NuStrIStr(parser->word_buf_ptr, "Local") != NULL) {
                    ref_script->check_local_scripts = 1;
                }
            }
        }
    }

done:
    if (ref_script->name != NULL) {
        if (!ref_script->check_global_scripts && !ref_script->check_local_scripts) {
            ref_script->check_global_scripts = 1;
            ref_script->check_local_scripts = 1;
        }

        NuLinkedListAppend(load_refscripthdr, &ref_script->list_node);
    } else {
        *load_buff = orig_buf;
    }
}

static void xEndState(NUFPAR *parser) {
    NuFParPopCom(parser);
}

static NUFPCOMJMP cfgtab_State[] = {
    {"CONDITIONS", &xConditions},
    {"ACTIONS", &xActions},
    {"REFERENCESCRIPT", &xRefScript},
    {"}", &xEndState},
    {NULL, NULL},
};

static void xState(NUFPAR *parser) {
    if (load_aiscript != NULL && NuFParGetWord(parser) != 0) {
        load_aistate = (AISTATE *)AIScriptBufferAlloc(load_buff, load_endbuff, sizeof(AISTATE));

        // This can create an invalid pointer when `load_aistate` is NULL, but
        // fortunately doesn't ever seem to be accessed.
        load_refscripthdr = &load_aistate->ref_scripts;

        if (load_aistate == NULL) {
            return;
        }

        memset(load_aistate, 0, sizeof(AISTATE));

        load_aistate->name = AIScriptCopyString(parser->word_buf_ptr, load_buff, load_endbuff);

        if (NuStrICmp(load_aistate->name, "Base") == 0) {
            NuLinkedListInsert(&load_aiscript->states, &load_aistate->list_node);
        } else {
            NuLinkedListAppend(&load_aiscript->states, &load_aistate->list_node);
        }

        NuFParPushCom(parser, cfgtab_State);

        while (NuFParGetLine(parser) != 0) {
            NuFParGetWord(parser);

            load_conditionshdr = &load_aistate->conditions;
            load_actionshdr = &load_aistate->actions;

            NuFParInterpretWord(parser);
        }

        NuFParPopCom(parser);

        load_aistate = NULL;
        load_actionshdr = NULL;
        load_conditionshdr = NULL;
        load_refscripthdr = NULL;
    }
}

static void xParam(NUFPAR *parser) {
    u32 param_idx;

    if (load_aiscript == NULL) {
        return;
    }

    param_idx = NuFParGetInt(parser);
    if (param_idx >= 4) {
        return;
    }

    if (NuFParGetWord(parser) == 0) {
        return;
    }

    load_aiscript->params[param_idx].name = AIScriptCopyString(parser->word_buf_ptr, load_buff, load_endbuff);
    load_aiscript->params[param_idx].default_val = NuFParGetf32(parser);
}

static void xConst(NUFPAR *parser) {
    int cur;
    f32 default_val;

    NuStrLen(parser->word_buf_ptr);
    NuFParGetWord(parser);
    NuStrNCpy(aiscript_const[aiscript_const_curr].name, parser->word_buf_ptr, 0x20);

    cur = aiscript_const_curr;
    default_val = NuFParGetf32(parser);
    aiscript_const_curr++;

    aiscript_const[cur].default_val = default_val;
}

static void xConditionMacro(NUFPAR *parser) {
    AICONDITIONMACRO *macro;

    if (NuFParGetWord(parser) == 0) {
        return;
    }

    macro = (AICONDITIONMACRO *)AIScriptBufferAlloc(load_buff, load_endbuff, sizeof(AICONDITIONMACRO));
    if (macro == NULL) {
        return;
    }

    macro->list_node.next = NULL;
    macro->name = AIScriptCopyString(parser->word_buf_ptr, load_buff, load_endbuff);

    NuLinkedListAppend(&load_aiscript->condition_macros, &macro->list_node);

    load_conditionshdr = &macro->conditions;
    condition_has_no_goto = 1;

    xConditions(parser);

    load_conditionshdr = NULL;
    condition_has_no_goto = 0;
}

static void xActionMacro(NUFPAR *parser) {
    AIACTIONMACRO *macro;

    if (NuFParGetWord(parser) == 0) {
        return;
    }

    macro = (AIACTIONMACRO *)AIScriptBufferAlloc(load_buff, load_endbuff, sizeof(AIACTIONMACRO));
    if (macro == NULL) {
        return;
    }

    macro->list_node.next = NULL;
    macro->name = AIScriptCopyString(parser->word_buf_ptr, load_buff, load_endbuff);

    NuLinkedListAppend(&load_aiscript->action_macros, &macro->list_node);

    load_actionshdr = &macro->actions;

    xActions(parser);

    load_actionshdr = NULL;
}

static void xDeriveFromScript(NUFPAR *parser) {
    if (load_aiscript == NULL || load_aiscript->derived_from != NULL) {
        return;
    }

    load_aiscript->is_derived_from_level_script = 0;

    while (NuFParGetLine(parser) != 0) {
        while (NuFParGetWord(parser) != 0) {
            char *cursor;

            if (NuStrICmp(parser->word_buf_ptr, "}") == 0) {
                goto done;
            }

            if ((cursor = NuStrIStr(parser->word_buf_ptr, "Script")) != NULL) {
                load_aiscript->derived_from =
                    AIScriptCopyString(cursor + strlen("Script") + 1, load_buff, load_endbuff);
            } else if (NuStrIStr(parser->word_buf_ptr, "Source") != NULL) {
                if (NuStrIStr(parser->word_buf_ptr, "Global") != NULL) {
                    load_aiscript->is_derived_from_level_script = 0;
                } else if (NuStrIStr(parser->word_buf_ptr, "Level") != NULL) {
                    load_aiscript->is_derived_from_level_script = 1;
                }
            }
        }
    }

done:
    if (load_aiscript->derived_from != NULL) {
        load_aiscript->is_derived = 1;
    }
}

static void xInclude(NUFPAR *parser) {
    AISCRIPT *tmp_script;
    VARIPTR *tmp_buf;
    VARIPTR *tmp_buf_end;

    tmp_script = load_aiscript;
    tmp_buf_end = load_endbuff;
    tmp_buf = load_buff;

    NuFParGetWord(parser);
    NuFParSuspend(parser);

    AIScriptOpenPakFileParse(&load_aiscript, load_pakfile, parser->word_buf_ptr, load_path, load_buff, load_endbuff);

    NuFParResume(parser);

    load_endbuff = tmp_buf_end;
    load_aiscript = tmp_script;
    load_buff = tmp_buf;
}

static NUFPCOMJMP cfgtab_Script[] = {
    {"STATE", &xState},
    {"PARAM", &xParam},
    {"CONST", &xConst},
    {"CONDITIONMACRO", &xConditionMacro},
    {"ACTIONMACRO", &xActionMacro},
    {"REFERENCESCRIPT", &xRefScript},
    {"DERIVEFROMSCRIPT", &xDeriveFromScript},
    {"INCLUDE", &xInclude},
    {NULL, NULL},
};

void AIScriptOpenPakFileParse(AISCRIPT_s **script_ref, void *pak, char *filename, char *path, VARIPTR *buf,
                              VARIPTR *buf_end) {
    char script_name[64];
    char *ext;
    char str_buf[128];
    NUFPAR *parser;
    AISCRIPT *script;

    NuStrCpy(script_name, filename);

    ext = NuStrIStr(script_name, ".scp");
    if (ext == NULL) {
        return;
    }

    *ext = '\0';

    sprintf(str_buf, "%s\\%s", path, filename);

    if (pak != NULL) {
        int item_handle;

        item_handle = NuFilePakGetItem(pak, filename);
        if (item_handle == 0) {
            goto no_pak;
        } else {
            void *item_buf;
            int size;
            NUFILE file;

            NuFilePakGetItemInfo(pak, item_handle, &item_buf, &size);
            file = NuMemFileOpen(item_buf, size, NUFILE_READ);

            if (file != 0) {
                parser = NuFParOpen(file);

            } else {
                goto no_pak;
            }
        }
    } else {
    no_pak:
        parser = NuFParCreate(str_buf);
    }

    if (parser == NULL) {
        return;
    }

    if (*script_ref == NULL) {
        *script_ref = (AISCRIPT *)AIScriptBufferAlloc(buf, buf_end, sizeof(AISCRIPT));
        ;
        if (*script_ref == NULL) {
            goto done;
        }

        memset(*script_ref, 0, sizeof(AISCRIPT));

        NuStrCpy(str_buf, filename);

        ext = NuStrIStr(str_buf, ".scp");
        if (ext != NULL) {
            *ext = '\0';
        }

        (*script_ref)->name = AIScriptCopyString(str_buf, buf, buf_end);

        if (*script_ref == NULL) {
            goto done;
        }
    }

    script = *script_ref;

    load_endbuff = buf_end;
    load_buff = buf;
    load_aiscript = script;
    aiscript_const_curr = 0;

    NuFParPushCom(parser, cfgtab_Script);

    while (NuFParGetLine(parser) != 0) {
        NuFParGetWord(parser);

        load_refscripthdr = &script->ref_scripts;

        NuFParInterpretWord(parser);
    }

    load_buff = NULL;
    load_endbuff = NULL;
    load_aiscript = NULL;
    load_refscripthdr = NULL;

done:
    NuFParDestroy(parser);
}
