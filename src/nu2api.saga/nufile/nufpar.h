#ifndef NU2API_SAGA_NUCORE_NUFPAR
#define NU2API_SAGA_NUCORE_NUFPAR

#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"

#include "decomp.h"

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

    char _line_buf[514];
    char _word_buf[514];
    char *line_buf;
    char *word_buf;
    int line_buf_size;
    int word_buf_size;

    int line_num;
    int line_pos;
    int char_pos;

    int buf_start;
    int buf_end;

    union {
        NUFPCOMJMP *jump[8];
        NUFPCOMJMPCTX *jump_ctx[8];
    } command_stack;
    union {
        NUFPCOMJMP *jump[8];
        NUFPCOMJMPCTX *jump_ctx[8];
    } command_stack2;
    int command_pos;

    int size;
    char is_utf16;
    char is_utf8;
    char *separator_list;
    char *separator_tokens;
} NUFPAR;

C_API_START

void NuFParInit(int max_count, int buf_size);

NUFPAR *NuFParOpen(NUFILE file_handle);
void NuFParClose(NUFPAR *parser);

NUFPAR *NuFParCreate(char *filename);
void NuFParDestroy(NUFPAR *parser);

int NuFParGetLine(NUFPAR *parser);
int NuFParGetLineW(NUFPAR *parser);
int NuFParGetWord(NUFPAR *parser);
int NuFParGetWordW(NUFPAR *parser);
void NuFParUnGetWord(NUFPAR *parser);

float NuFParGetFloat(NUFPAR *parser);
float NuFParGetFloatRDP(NUFPAR *parser);
int NuFParGetInt(NUFPAR *parser);
int NuFParGetIntRDP(NUFPAR *parser);

int NuFParPushCom(NUFPAR *parser, NUFPCOMJMP *commands);
int NuFParPushCom2(NUFPAR *parser, NUFPCOMJMP *commands, NUFPCOMJMP *commands2);
int NuFParPushComCTX(NUFPAR *parser, NUFPCOMJMPCTX *commands);
void NuFParPopCom(NUFPAR *parser);

nufpcomfn *NuFParSetInterpreterErrorHandler(nufpcomfn *fn);

int NuFParInterpretWord(NUFPAR *parser);
int NuFParInterpretWordCTX(NUFPAR *parser, void *ctx);

NUWCHAR NuGetWChar(NUFPAR *parser);

C_API_END

char NuGetChar(NUFPAR *parser);

#endif
