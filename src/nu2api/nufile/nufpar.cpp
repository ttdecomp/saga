#include "nu2api/nufile/nufpar.h"

#include <string.h>

#include "decomp.h"

#include "nu2api/nucore/nurdp.h"
#include "nu2api/nucore/nustring.h"
#include "nu2api/nufile/nufile.h"

struct SOCK;
struct SOCKROT;
struct SOCKPOSITION_s;
struct nugspline_s;
struct nuvec_s;

#define PARSER_COUNT 4
#define PARSER_BUF_SIZE 0x1000

static NUFPAR _fpars[PARSER_COUNT];
static char _fparbuffers[PARSER_COUNT * PARSER_BUF_SIZE];

static NUFPAR *fpars;
static char *fparbuffers;

static NUFPAR *NuAllocFPar() {
    if (fpars == NULL) {
        NuFParInit(PARSER_COUNT, PARSER_BUF_SIZE);
    }

    for (i32 i = 0; i < PARSER_COUNT; i++) {

        if (fpars[i].file_buf == NULL) {
            memset(&fpars[i], 0, sizeof(NUFPAR));

            fpars[i].file_buf = &fparbuffers[i * PARSER_BUF_SIZE];
            memset(fpars[i].file_buf, 0, PARSER_BUF_SIZE);

            return &fpars[i];
        }
    }

    return NULL;
}

static void NuFreeFPar(NUFPAR *parser) {
    parser->file_buf = NULL;
}

void NuFParInit(i32 max_count, i32 buf_size) {
    fpars = _fpars;
    memset(fpars, 0, PARSER_COUNT * sizeof(NUFPAR));

    fparbuffers = _fparbuffers;
}

NUFPAR *NuFParOpen(NUFILE file_handle) {
    NUFPAR *parser = NuAllocFPar();
    if (parser != NULL) {
        parser->file_handle = file_handle;
        parser->buf_end = -1;
        parser->line_num = -1;
        parser->command_pos = -1;
        parser->separator_list = NULL;
        parser->separator_tokens = NULL;
        parser->line_buf = parser->line_buf_store;
        parser->word_buf = parser->word_buf_store;
        parser->line_buf_size = 0x200;
        parser->word_buf_size = 0x200;

        NUWCHAR first = NuFileReadWChar(file_handle);
        parser->is_utf16 = 0;
        parser->is_utf8 = 0;

        unsigned char next;
        if (first == 0xfffe) {
            parser->is_utf16 = 2;
        } else if (first == 0xfeff) {
            parser->is_utf16 = 1;
        } else if (first == 0xbbef) {
            next = NuFileReadChar(file_handle);
            if (next == 0xbf) {
                parser->is_utf8 = 1;
            }
        } else if (first == 0xefbb) {
            next = NuFileReadChar(file_handle);
            if (next == 0xbf) {
                parser->is_utf8 = 2;
            }
        }

        if (parser->is_utf16 == 0 && parser->is_utf8 == 0) {
            NuFileSeek(file_handle, 0, NUFILE_SEEK_START);
        }

        parser->size = NuFileOpenSize(file_handle);
    }

    return parser;
}

void NuFParClose(NUFPAR *parser) {
    NuFreeFPar(parser);
}

NUFPAR *NuFParCreate(char *filename) {
    NUFILE file_handle;

    if (NuFileExists(filename)) {
        file_handle = NuFileOpen(filename, NUFILE_READ);
        if (file_handle != 0) {
            NUFPAR *parser = NuFParOpen(file_handle);
            if (parser != NULL) {
                NuStrCpy(parser->file_name, filename);
                return parser;
            }

            NuFileClose(file_handle);
        }
    }

    return NULL;
}

NUFPAR *NuFParCreateMem(char *name, char *buffer, i32 bufferSize) {
    // Stub
    return NULL;
}

void NuFParDestroy(NUFPAR *parser) {
    NUFILE file_handle = parser->file_handle;

    NuFParClose(parser);
    NuFileClose(file_handle);
}

void NuFParSuspend(NUFPAR *parser) {
    if (parser->file_handle >= 0x400 && parser->file_handle < 0x800) {
        return;
    }

    if (parser->file_name[0] == '\0' || parser->file_handle == 0) {
        // Error logging goes here.
    }

    NuFileClose(parser->file_handle);

    parser->file_handle = 0;
}

void NuFParResume(NUFPAR *parser) {
    if (parser->file_handle >= 0x400 && parser->file_handle < 0x800) {
        return;
    }

    parser->file_handle = NuFileOpen(parser->file_name, NUFILE_READ);

    NuFileSeek(parser->file_handle, parser->char_pos, NUFILE_SEEK_START);

    parser->buf_end = parser->char_pos - 1;
}

static i32 old_line_pos;

#define CLAMP_LINE(pos) pos &(parser->line_buf_size - 1)
#define CLAMP_WORD(pos) pos &(parser->word_buf_size - 1)
#define CLAMP_WIDE_LINE(pos) pos &((parser->line_buf_size >> 1) - 1)
#define CLAMP_WIDE_WORD(pos) pos &((parser->word_buf_size >> 1) - 1)

i32 NuFParGetLine(NUFPAR *parser) {
    char c;
    i32 len;
    i32 at_end_of_line;
    i32 is_done;
    i32 in_quoted_text;

    in_quoted_text = 0;

    if (parser->is_utf16) {
        return NuFParGetLineW(parser);
    }

    len = 0;
    parser->line_pos = 0;
    parser->line_num++;
    is_done = false;

    do {
        c = NuGetChar(parser);
        switch (c) {
            case '\r':
                NuGetChar(parser);
            case '\n':
                if (len == 0 || !is_done) {
                    parser->line_num++;
                    is_done = false;

                    break;
                }
            case '\0':
                parser->line_buf[len] = '\0';
                return len;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = true;
                parser->line_buf[CLAMP_LINE(len)] = c;
                len++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (len == 0 || !is_done) {
                        at_end_of_line = false;

                        do {
                            c = NuGetChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetChar(parser);
                                case '\n':
                                case '\0':
                                    at_end_of_line = true;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        len = 0;
                        parser->line_pos = 0;
                        parser->line_num++;
                        is_done = false;
                    } else {
                        do {
                            c = NuGetChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetChar(parser);
                                case '\n':
                                case '\0':
                                    parser->line_buf[len] = '\0';
                                    return len;
                                default:
                                    break;
                            }
                        } while (true);

                        return len;
                    }

                    break;
                }
            default:
                is_done = true;
            case '\t':
            case ' ':
                parser->line_buf[CLAMP_LINE(len)] = c;
                len++;
                break;
        }
    } while (true);
}

i32 NuFParGetLineW(NUFPAR *parser) {
    NUWCHAR c;
    i32 len;
    NUWCHAR *line;
    i32 at_end_of_line;
    i32 is_done;
    i32 in_quoted_text;

    in_quoted_text = 0;
    line = (NUWCHAR *)parser->line_buf;
    len = 0;
    parser->line_pos = 0;
    parser->line_num++;
    is_done = false;

    do {
        c = NuGetWChar(parser);
        switch (c) {
            case '\r':
                NuGetWChar(parser);
            case '\n':
                if (len == 0 || !is_done) {
                    parser->line_num++;
                    is_done = false;

                    break;
                }
            case '\0':
                line[len] = '\0';
                return len;
            case '"':
                in_quoted_text = 1 - in_quoted_text;
                is_done = true;
                line[CLAMP_WIDE_WORD(len)] = c;
                len++;
                break;
            case ';':
                if (!in_quoted_text) {
                    if (len == 0 || !is_done) {
                        at_end_of_line = false;

                        do {
                            c = NuGetWChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetWChar(parser);
                                case '\n':
                                case '\0':
                                    at_end_of_line = true;
                                    break;
                                default:
                                    break;
                            }
                        } while (!at_end_of_line);

                        len = 0;
                        parser->line_pos = 0;
                        parser->line_num++;
                        is_done = false;
                    } else {
                        do {
                            c = NuGetWChar(parser);
                            switch (c) {
                                case '\r':
                                    NuGetWChar(parser);
                                case '\n':
                                case '\0':
                                    line[len] = '\0';
                                    return len;
                                default:
                                    break;
                            }
                        } while (true);

                        return len;
                    }

                    break;
                }
            default:
                is_done = true;
            case '\t':
            case ' ':
                line[CLAMP_WIDE_WORD(len)] = c;
                len++;
                break;
        }
    } while (true);
}

i32 NuFParGetWord(NUFPAR *parser) {
    i32 len;
    i32 in_quoted_text;
    i32 found_quotes = 0;

    if (parser->is_utf16) {
        return NuFParGetWordW(parser);
    }

    len = 0;
    in_quoted_text = 0;

    old_line_pos = parser->line_pos;

    while (parser->line_buf[CLAMP_LINE(parser->line_pos)] != 0) {
        char c = parser->line_buf[parser->line_pos];

        if (parser->separator_tokens != NULL && !in_quoted_text) {
            if (NuStrChr(parser->separator_tokens, c) != NULL) {
                if (len == 0) {
                    parser->word_buf[len] = c;
                    len++;
                    parser->line_pos++;
                }

                parser->word_buf[CLAMP_WORD(len)] = '\0';

                return len;
            }
        }

        if (parser->separator_list != NULL && !in_quoted_text) {
            if (NuStrChr(parser->separator_list, c) != NULL) {
                c = ' ';
            }
        }

        switch (c) {
            case ' ':
            case ',':
            case '\t':
                if (!in_quoted_text) {
                    if (len != 0) {
                        parser->word_buf[CLAMP_WORD(len)] = '\0';
                        return len;
                    }

                    break;
                }
            default:
                if (c == '"') {
                    in_quoted_text = 1 - in_quoted_text;
                    found_quotes = 1;
                } else {
                    parser->word_buf[CLAMP_WORD(len)] = c;
                    len++;
                }

                break;
        }

        parser->line_pos++;

        if (found_quotes && in_quoted_text == 0 && len == 0) {
            break;
        }
    }

    parser->word_buf[CLAMP_WORD(len)] = '\0';
    return len;
}

i32 NuFParGetWordW(NUFPAR *parser) {
    NUWCHAR *line = (NUWCHAR *)parser->line_buf;
    NUWCHAR *word = (NUWCHAR *)parser->word_buf;
    i32 len = 0;
    i32 in_quoted_text = 0;

    old_line_pos = parser->line_pos;

    NUWCHAR c;

    while (line[CLAMP_WIDE_LINE(parser->line_pos)] != 0) {
        c = line[parser->line_pos];

        if (parser->separator_tokens != NULL && !in_quoted_text) {
            if (NuStrChr(parser->separator_tokens, c) != NULL) {
                if (len == 0) {
                    word[len] = c;
                    len++;
                    parser->line_pos++;
                }

                word[CLAMP_WIDE_WORD(len)] = '\0';
                return len;
            }
        }

        if (parser->separator_list != NULL && !in_quoted_text) {
            if (NuStrChr(parser->separator_list, c) != NULL) {
                c = ' ';
            }
        }

        switch (c & 0xff) {
            case ' ':
            case ',':
            case '\t':
                if (!in_quoted_text) {
                    if (len != 0) {
                        word[CLAMP_WIDE_WORD(len)] = '\0';
                        return len;
                    }

                    break;
                }
            default:
                if (c == '"') {
                    if (in_quoted_text && line[parser->line_pos + 1] == '"') {
                        word[CLAMP_WIDE_WORD(len)] = c;
                        len++;
                        parser->line_pos++;
                    } else {
                        in_quoted_text = 1 - in_quoted_text;
                    }
                } else {
                    word[CLAMP_WIDE_WORD(len)] = c;
                    len++;
                }

                break;
        }

        parser->line_pos++;
    }

    word[CLAMP_WIDE_WORD(len)] = '\0';
    return len;
}

void NuFParUnGetWord(NUFPAR *parser) {
    parser->line_pos = old_line_pos;
}

f32 NuFParGetFloat(NUFPAR *parser) {
    char buf[64];

    NuFParGetWord(parser);
    if (parser->is_utf16) {
        NuUnicodeToAscii(buf, (NUWCHAR16 *)parser->word_buf);
    } else {
        NuStrCpy(buf, parser->word_buf);
    }

    if (buf[0] != '\0') {
        return NuAToF(buf);
    } else {
        return 0.0f;
    }
}

f32 NuFParGetFloatRDP(NUFPAR *parser) {
    char buf[64];

    NuFParGetWord(parser);
    if (parser->is_utf16) {
        NuUnicodeToAscii(buf, (NUWCHAR16 *)parser->word_buf);
    } else {
        NuStrCpy(buf, parser->word_buf);
    }

    if (buf[0] != '\0') {
        return NuRDPF(buf);
    } else {
        return 0.0f;
    }
}

i32 NuFParGetInt(NUFPAR *parser) {
    char buf[64];

    NuFParGetWord(parser);
    if (parser->is_utf16) {
        NuUnicodeToAscii(buf, (NUWCHAR16 *)parser->word_buf);
    } else {
        NuStrCpy(buf, parser->word_buf);
    }

    if (buf[0] != '\0') {
        if (buf[0] == '$') {
            return NuHexStringToI(buf + 1);
        } else if (buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) {
            return NuHexStringToI(buf + 2);
        } else {
            return NuAToI(buf);
        }
    } else {
        return 0;
    }
}

i32 NuFParGetIntRDP(NUFPAR *parser) {
    char buf[64];

    NuFParGetWord(parser);
    if (parser->is_utf16) {
        NuUnicodeToAscii(buf, (NUWCHAR16 *)parser->word_buf);
    } else {
        NuStrCpy(buf, parser->word_buf);
    }

    if (buf[0] != '\0') {
        return NuRDPI(buf);
    } else {
        return 0;
    }
}

i32 NuFParPushCom(NUFPAR *parser, NUFPCOMJMP *commands) {
    if (parser->command_pos >= 7) {
        return -1;
    }

    parser->command_pos++;

    parser->command_stack.jump[parser->command_pos] = commands;
    parser->command_stack2.jump[parser->command_pos] = NULL;

    return parser->command_pos;
}

i32 NuFParPushCom2(NUFPAR *parser, NUFPCOMJMP *commands, NUFPCOMJMP *commands2) {
    if (parser->command_pos >= 7) {
        return -1;
    }

    parser->command_pos++;

    parser->command_stack.jump[parser->command_pos] = commands;
    parser->command_stack2.jump[parser->command_pos] = commands2;

    return parser->command_pos;
}

i32 NuFParPushComCTX(NUFPAR *parser, NUFPCOMJMPCTX *commands) {
    if (parser->command_pos >= 7) {
        return -1;
    }

    parser->command_pos++;

    parser->command_stack.jump_ctx[parser->command_pos] = commands;
    parser->command_stack2.jump_ctx[parser->command_pos] = NULL;

    return parser->command_pos;
}

void NuFParPopCom(NUFPAR *parser) {
    if (parser->command_pos > -1) {
        parser->command_pos--;
    }
}

nufpcomfn *fnInterpreterError;

nufpcomfn *NuFParSetInterpreterErrorHandler(nufpcomfn *fn) {
    nufpcomfn *old = fnInterpreterError;

    fnInterpreterError = fn;

    return old;
}

i32 NuFParInterpretWord(NUFPAR *parser) {
    char buf[64];
    i32 i;

    if (parser->is_utf16) {
        NuUnicodeToAscii(buf, (NUWCHAR16 *)parser->word_buf);
    } else {
        NuStrCpy(buf, parser->word_buf);
    }

    if (buf[0] == '\0') {
        return 0;
    }

    if (buf[0] == ';') {
        return 0;
    }

    if (parser->command_pos > -1) {
        for (i = 0; parser->command_stack.jump[parser->command_pos][i].fn_name != NULL; i++) {
            if (NuStrICmp(parser->command_stack.jump[parser->command_pos][i].fn_name, buf) == 0) {
                parser->command_stack.jump[parser->command_pos][i].fn(parser);

                return 1;
            }
        }

        if (parser->command_stack2.jump[parser->command_pos] != NULL) {
            for (i = 0; parser->command_stack2.jump[parser->command_pos][i].fn_name != NULL; i++) {
                if (NuStrICmp(parser->command_stack2.jump[parser->command_pos][i].fn_name, buf) == 0) {
                    parser->command_stack2.jump[parser->command_pos][i].fn(parser);

                    return 1;
                }
            }
        }
    }

    if (fnInterpreterError != NULL) {
        (*fnInterpreterError)(parser);
    }

    return 0;
}

i32 NuFParInterpretWordCTX(NUFPAR *parser, void *ctx) {
    char buf[64];
    i32 i;

    if (parser->is_utf16) {
        NuUnicodeToAscii(buf, (NUWCHAR16 *)parser->word_buf);
    } else {
        NuStrCpy(buf, parser->word_buf);
    }

    if (buf[0] == '\0') {
        return 0;
    }

    if (buf[0] == ';') {
        return 0;
    }

    if (parser->command_pos > -1) {
        for (i = 0; parser->command_stack.jump_ctx[parser->command_pos][i].fn_name != NULL; i++) {
            if (NuStrICmp(parser->command_stack.jump_ctx[parser->command_pos][i].fn_name, buf) == 0) {
                parser->command_stack.jump_ctx[parser->command_pos][i].fn(parser, ctx);

                return 1;
            }
        }

        if (parser->command_stack2.jump_ctx[parser->command_pos] != NULL) {
            for (i = 0; parser->command_stack2.jump_ctx[parser->command_pos][i].fn_name != NULL; i++) {
                if (NuStrICmp(parser->command_stack2.jump_ctx[parser->command_pos][i].fn_name, buf) == 0) {
                    parser->command_stack2.jump_ctx[parser->command_pos][i].fn(parser, ctx);

                    return 1;
                }
            }
        }
    }

    if (fnInterpreterError != NULL) {
        (*fnInterpreterError)(parser);
    }

    return 0;
}

char NuGetChar(NUFPAR *parser) {
    char c;
    i32 len;
    i32 length;

    i32 read_to = parser->buf_end >= 0 ? parser->buf_end : 0;

    if (parser->char_pos > parser->buf_end) {
        if (parser->buf_end + 1 <= parser->size) {
            len = NuFileRead(parser->file_handle, parser->file_buf,
                             parser->size - read_to <= 0x1000 ? parser->size - read_to : 0x1000);

            parser->buf_start = parser->buf_end + 1;
            parser->buf_end += len;
        } else {
            return '\0';
        }

        if (len == 0) {
            return '\0';
        }
    }

    c = parser->file_buf[parser->char_pos - parser->buf_start];
    parser->char_pos++;

    return c;
}

NUWCHAR NuGetWChar(NUFPAR *parser) {
    unsigned char lo;
    unsigned char hi;
    NUWCHAR c;

    lo = NuGetChar(parser);
    hi = NuGetChar(parser);

    c = lo + (hi << 8);

    return c;
}
static __used__ void pftaRepeat(nufpar_s *) {
}

static __used__ void pftaRepend(nufpar_s *) {
}

static __used__ void pftaTexAdj(nufpar_s *) {
}

static __used__ void pftaTexAdjR(nufpar_s *) {
}

static __used__ void pftaUntiltex(nufpar_s *) {
}

static __used__ void pftaScriptMask(nufpar_s *) {
}

static __used__ void pftaScriptname(nufpar_s *) {
}

static __used__ void pftaOn(nufpar_s *) {
}

static __used__ void pftaEnd(nufpar_s *) {
}

static __used__ void pftaOff(nufpar_s *) {
}

static __used__ void pftaRet(nufpar_s *) {
}

static __used__ void pftaTex(nufpar_s *) {
}

static __used__ void pftaBtex(nufpar_s *) {
}

static __used__ void pftaGoto(nufpar_s *) {
}

static __used__ void pftaGosub(nufpar_s *) {
}

static __used__ void pftaLabel(nufpar_s *) {
}

static __used__ void pftaRate(nufpar_s *) {
}

static __used__ void pftaTexR(nufpar_s *) {
}

static __used__ void pftaWait(nufpar_s *) {
}

static __used__ void pftaXDef(nufpar_s *) {
}

static __used__ void pftaXRef(nufpar_s *) {
}

static __used__ void xChild_Col(nufpar_s *) {
}

static __used__ void xCondition(nufpar_s *) {
}

static __used__ void xGizmoName(nufpar_s *) {
}

static __used__ void xGizmoType(nufpar_s *) {
}

static __used__ void xNumGizmos(nufpar_s *) {
}

static __used__ void xAIAssistID(nufpar_s *) {
}

static __used__ void xOutputOnly(nufpar_s *) {
}

static __used__ void xParent_Col(nufpar_s *) {
}

static __used__ void xStartInvis(nufpar_s *) {
}

static __used__ void xNotFreeplay(nufpar_s *) {
}

static __used__ void xFlowBoxCount(nufpar_s *) {
}

static __used__ void xNotStoryMode(nufpar_s *) {
}

static __used__ void xReverseInvis(nufpar_s *) {
}

static __used__ void xConditionType(nufpar_s *) {
}

static __used__ void xGizRandomTime(nufpar_s *) {
}

static __used__ void xMonitorInputs(nufpar_s *) {
}

static __used__ void xRand_NumOutputs(nufpar_s *) {
}

static __used__ void xRand_OutputChance(nufpar_s *) {
}

static __used__ void xName(nufpar_s *) {
}

static __used__ void xChild(nufpar_s *) {
}

static __used__ void xGizmo(nufpar_s *) {
}

static __used__ void xAction(nufpar_s *) {
}

static __used__ void xParent(nufpar_s *) {
}

static __used__ void xFlowBox(nufpar_s *) {
}

static __used__ void xReverse(nufpar_s *) {
}

static __used__ void xCollapse(nufpar_s *) {
}

static __used__ void xEndDeact(nufpar_s *) {
}

static __used__ void xEndInvis(nufpar_s *) {
}

static __used__ void xGizTimer(nufpar_s *) {
}

static __used__ void RE_end_colour(nufpar_s *) {
}

static __used__ void RE_end_radius(nufpar_s *) {
}

static __used__ void RE_effect_type(nufpar_s *) {
}

static __used__ void RE_start_colour(nufpar_s *) {
}

static __used__ void RE_start_radius(nufpar_s *) {
}

static __used__ void RE_texture_name(nufpar_s *) {
}

static __used__ void RE_life(nufpar_s *) {
}

static __used__ void SockCamATSTDIST(nufpar_s *, void *) {
}

static __used__ void SockCamATSTLIFT(nufpar_s *, void *) {
}

static __used__ void SockCamATSTTILT(nufpar_s *, void *) {
}

static __used__ void SockManCam_MAX_X(nufpar_s *, void *) {
}

static __used__ void SockManCam_MAX_Y(nufpar_s *, void *) {
}

static __used__ void SockCamATSTCAMRANGE(nufpar_s *, void *) {
}

static __used__ void SockCamATSTTILTRATE(nufpar_s *, void *) {
}

static __used__ void SockTerrainCamInActive(nufpar_s *, void *) {
}

static __used__ void SockCamCAMERARAYTILTDIST(nufpar_s *, void *) {
}

static __used__ void SockCamCAMERARAYTILTHEIGHT(nufpar_s *, void *) {
}




static __used__ void Traffic_animobj(nufpar_s *) {
}

static __used__ void Traffic_tfactor(nufpar_s *) {
}

static __used__ void Traffic_vehicle(nufpar_s *) {
}

static __used__ void TrafficAnim_yoffset(nufpar_s *) {
}

static __used__ void Traffic_rand_interval(nufpar_s *) {
}

static __used__ void Traffic_frame_interval(nufpar_s *) {
}
