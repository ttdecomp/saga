#pragma once

#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"

struct nufpar_s;

typedef void nufpcomfn(struct nufpar_s *);
typedef void nufpcomctxfn(struct nufpar_s *, void *);

typedef struct nufpcomjmp_s {
    char *fn_name;
    nufpcomfn *fn;
} NUFPCOMJMP;

typedef struct nufpcomjmpctx_s {
    char *fn_name;
    nufpcomctxfn *fn;
} NUFPCOMJMPCTX;

typedef struct nufpar_s {
    char *file_buf;
    NUFILE file_handle;
    char file_name[256];

    char line_buf_store[514];
    char word_buf_store[514];
    char *line_buf;
    char *word_buf;
    i32 line_buf_size;
    i32 word_buf_size;

    i32 line_num;
    i32 line_pos;
    i32 char_pos;

    i32 buf_start;
    i32 buf_end;

    union {
        NUFPCOMJMP *jump[8];
        NUFPCOMJMPCTX *jump_ctx[8];
    } command_stack;
    union {
        NUFPCOMJMP *jump[8];
        NUFPCOMJMPCTX *jump_ctx[8];
    } command_stack2;
    i32 command_pos;

    i32 size;
    char is_utf16;
    char is_utf8;
    char *separator_list;
    char *separator_tokens;
} NUFPAR;

#ifdef __cplusplus
extern "C" {
#endif
    void NuFParInit(i32 max_count, i32 buf_size);

    NUFPAR *NuFParOpen(NUFILE file_handle);
    void NuFParClose(NUFPAR *parser);

    NUFPAR *NuFParCreate(char *filename);
    NUFPAR *NuFParCreateMem(char *name, char *buffer, i32 bufferSize);
    void NuFParDestroy(NUFPAR *parser);

    void NuFParSuspend(NUFPAR *parser);
    void NuFParResume(NUFPAR *parser);

    i32 NuFParGetLine(NUFPAR *parser);
    i32 NuFParGetLineW(NUFPAR *parser);
    i32 NuFParGetWord(NUFPAR *parser);
    i32 NuFParGetWordW(NUFPAR *parser);
    void NuFParUnGetWord(NUFPAR *parser);

    f32 NuFParGetFloat(NUFPAR *parser);
    f32 NuFParGetFloatRDP(NUFPAR *parser);
    i32 NuFParGetInt(NUFPAR *parser);
    i32 NuFParGetIntRDP(NUFPAR *parser);

    i32 NuFParPushCom(NUFPAR *parser, NUFPCOMJMP *commands);
    i32 NuFParPushCom2(NUFPAR *parser, NUFPCOMJMP *commands, NUFPCOMJMP *commands2);
    i32 NuFParPushComCTX(NUFPAR *parser, NUFPCOMJMPCTX *commands);
    void NuFParPopCom(NUFPAR *parser);

    nufpcomfn *NuFParSetInterpreterErrorHandler(nufpcomfn *fn);

    i32 NuFParInterpretWord(NUFPAR *parser);
    i32 NuFParInterpretWordCTX(NUFPAR *parser, void *ctx);

    NUWCHAR NuGetWChar(NUFPAR *parser);
#ifdef __cplusplus
}
#endif

char NuGetChar(NUFPAR *parser);
