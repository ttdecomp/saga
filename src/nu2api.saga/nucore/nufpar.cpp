#include "nu2api.saga/nucore/nufpar.h"
#include "nu2api.saga/nucore/nustring.h"
#include "nu2api.saga/nufile/nufile.h"

#include <string.h>

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

    for (int i = 0; i < PARSER_COUNT; i++) {

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

void NuFParInit(int max_count, int buf_size) {
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
        parser->line_buf = parser->_line_buf;
        parser->word_buf = parser->_word_buf;
        parser->line_buf_size = 0x200;
        parser->word_buf_size = 0x200;

        NUWCHAR first = NuFileReadWChar(file_handle);
        parser->unicode = 0;
        parser->utf8 = 0;

        unsigned char next;
        if (first == 0xfffe) {
            parser->unicode = 2;
        } else if (first == 0xfeff) {
            parser->unicode = 1;
        } else if (first == 0xbbef) {
            next = NuFileReadChar(file_handle);
            if (next == 0xbf) {
                parser->utf8 = 1;
            }
        } else if (first == 0xefbb) {
            next = NuFileReadChar(file_handle);
            if (next == 0xbf) {
                parser->utf8 = 2;
            }
        }

        if (parser->unicode == 0 && parser->utf8 == 0) {
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
        file_handle = NuFileOpen(filename, NUFILE_MODE_READ);
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

void NuFParDestroy(NUFPAR *parser) {
    NUFILE file_handle = parser->file_handle;

    NuFParClose(parser);
    NuFileClose(file_handle);
}

static int old_line_pos;

#define CLAMP_LINE(pos) pos &(parser->line_buf_size - 1)
#define CLAMP_WORD(pos) pos &(parser->word_buf_size - 1)

int NuFParGetWord(NUFPAR *parser) {
    int len;
    int in_quoted_text;
    int found_quotes = 0;

    if (parser->unicode) {
        return NuFParGetWordW(parser);
    }

    len = 0;
    in_quoted_text = 0;

    old_line_pos = parser->line_pos;

    while (parser->line_buf[CLAMP_LINE(parser->line_pos)] != 0) {
        char c = parser->line_buf[parser->line_pos];

        if (parser->separator_tokens != NULL && !in_quoted_text && NuStrChr(parser->separator_tokens, c) != NULL) {
            if (len == 0) {
                parser->word_buf[len] = c;
                len++;
                parser->line_pos++;
            }

            parser->word_buf[CLAMP_WORD(len)] = '\0';

            return len;
        }

        if (parser->separator_list != NULL && !in_quoted_text && NuStrChr(parser->separator_list, c) != NULL) {
            c = ' ';
        }

        if ((c == ' ' || c == ',' || c == '\t') && !in_quoted_text) {
            if (len != 0) {
                parser->word_buf[CLAMP_WORD(len)] = '\0';
                return len;
            }
        } else if (c == '"') {
            in_quoted_text = 1 - in_quoted_text;
            found_quotes = 1;
        } else {
            parser->word_buf[CLAMP_WORD(len)] = c;
            len++;
        }

        parser->line_pos++;

        if (found_quotes && in_quoted_text == 0 && len == 0) {
            break;
        }
    }

    parser->word_buf[CLAMP_WORD(len)] = '\0';
    return len;
}

#define CLAMP_WIDE_LINE(pos) pos &((parser->line_buf_size >> 1) - 1)
#define CLAMP_WIDE_WORD(pos) pos &((parser->word_buf_size >> 1) - 1)

int NuFParGetWordW(NUFPAR *parser) {
    NUWCHAR *line = (NUWCHAR *)parser->line_buf;
    NUWCHAR *word = (NUWCHAR *)parser->word_buf;
    int len = 0;
    int in_quoted_text = 0;

    old_line_pos = parser->line_pos;

    NUWCHAR c;

    while (line[CLAMP_WIDE_LINE(parser->line_pos)] != 0) {
        c = line[parser->line_pos];

        if (parser->separator_tokens != NULL && !in_quoted_text && NuStrChr(parser->separator_tokens, c) != NULL) {
            if (len == 0) {
                word[len] = c;
                len++;
                parser->line_pos++;
            }

            word[CLAMP_WIDE_WORD(len)] = '\0';
            return len;
        }

        if (parser->separator_list != NULL && !in_quoted_text && NuStrChr(parser->separator_list, c) != NULL) {
            c = ' ';
        }

        if ((((c & 0xff) == ' ') || (c & 0xff) == ',' || (c & 0xff) == '\t') && !in_quoted_text) {
            if (len != 0) {
                word[CLAMP_WIDE_WORD(len)] = '\0';
                return len;
            }
        } else if (c == '"') {
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

        parser->line_pos++;
    }

    word[CLAMP_WIDE_WORD(len)] = '\0';
    return len;
}

void NuFParUnGetWord(NUFPAR *parser) {
    parser->line_pos = old_line_pos;
}

float NuFParGetFloat(NUFPAR *parser) {
    char buf[64];

    NuFParGetWord(parser);
    if (parser->unicode) {
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

int NuFParGetInt(NUFPAR *parser) {
    char buf[64];

    NuFParGetWord(parser);
    if (parser->unicode) {
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

int NuFParPushCom(NUFPAR *parser, NUFPCOMJMP *commands) {
    if (parser->command_pos >= 7) {
        return -1;
    }

    parser->command_pos++;

    parser->command_stack.jump[parser->command_pos] = commands;
    parser->command_stack2.jump[parser->command_pos] = NULL;

    return parser->command_pos;
}

int NuFParPushCom2(NUFPAR *parser, NUFPCOMJMP *commands, NUFPCOMJMP *commands2) {
    if (parser->command_pos >= 7) {
        return -1;
    }

    parser->command_pos++;

    parser->command_stack.jump[parser->command_pos] = commands;
    parser->command_stack2.jump[parser->command_pos] = commands2;

    return parser->command_pos;
}

int NuFParPushComCTX(NUFPAR *parser, NUFPCOMJMPCTX *commands) {
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

int NuFParInterpretWord(NUFPAR *parser) {
    char buf[64];
    int i;

    if (parser->unicode) {
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
            if (!NuStrICmp(parser->command_stack.jump[parser->command_pos][i].fn_name, buf)) {
                parser->command_stack.jump[parser->command_pos][i].fn(parser);

                return 1;
            }
        }

        if (parser->command_stack2.jump[parser->command_pos] != NULL) {
            for (i = 0; parser->command_stack2.jump[parser->command_pos][i].fn_name != NULL; i++) {
                if (!NuStrICmp(parser->command_stack2.jump[parser->command_pos][i].fn_name, buf)) {
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

int NuFParInterpretWordCTX(NUFPAR *parser, void *ctx) {
    char buf[64];
    int i;

    if (parser->unicode) {
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
            if (!NuStrICmp(parser->command_stack.jump_ctx[parser->command_pos][i].fn_name, buf)) {
                parser->command_stack.jump_ctx[parser->command_pos][i].fn(parser, ctx);

                return 1;
            }
        }

        if (parser->command_stack2.jump_ctx[parser->command_pos] != NULL) {
            for (i = 0; parser->command_stack2.jump_ctx[parser->command_pos][i].fn_name != NULL; i++) {
                if (!NuStrICmp(parser->command_stack2.jump_ctx[parser->command_pos][i].fn_name, buf)) {
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

uint NuFParGetLine(nufpar_s *parser) {
    bool bVar1;
    char cVar2;
    uint uVar3;
    int local_1c;
    uint local_10;

    local_1c = 0;
    if (parser->unicode != '\0') {
        // uVar3 = NuFParGetLineW(parser);
        // return uVar3;
        UNIMPLEMENTED();
    }
    local_10 = 0;
    parser->line_pos = 0;
    parser->line_num = parser->line_num + 1;
    bVar1 = false;
LAB_0026f78b:
    do {
        cVar2 = NuGetChar(parser);
        switch (cVar2) {
            case '\0':
            L859:
                parser->line_buf[local_10] = '\0';
                return local_10;
            default:
            L858:
                bVar1 = true;
            case '\t':
            case ' ':
                parser->line_buf[parser->line_buf_size - 1U & local_10] = cVar2;
                local_10 = local_10 + 1;
                break;
            case '\n':
                goto L861;
            case '\r':
                NuGetChar(parser);
            L861:
                if ((local_10 != 0) && (bVar1))
                    goto L859;
                parser->line_num = parser->line_num + 1;
                bVar1 = false;
                break;
            case '\"':
                local_1c = 1 - local_1c;
                bVar1 = true;
                parser->line_buf[parser->line_buf_size - 1U & local_10] = cVar2;
                local_10 = local_10 + 1;
                break;
            case ';':
                if (local_1c != 0)
                    goto L858;
                if ((local_10 == 0) || (!bVar1))
                    goto LAB_0026f859;
                goto LAB_0026f8d6;
        }
    } while (true);
LAB_0026f859:
    bVar1 = false;
    do {
        cVar2 = NuGetChar(parser);
        if (cVar2 == '\n') {
        LAB_0026f88d:
            bVar1 = true;
        } else {
            if (cVar2 == '\r') {
                NuGetChar(parser);
                goto LAB_0026f88d;
            }
            if (cVar2 == '\0')
                goto LAB_0026f88d;
        }
    } while (!bVar1);
    local_10 = 0;
    parser->line_pos = 0;
    parser->line_num = parser->line_num + 1;
    bVar1 = false;
    goto LAB_0026f78b;
    while (true) {
        if (cVar2 == '\r') {
            NuGetChar(parser);
            break;
        }
        if (cVar2 == '\0')
            break;
    LAB_0026f8d6:
        cVar2 = NuGetChar(parser);
        if (cVar2 == '\n')
            break;
    }
    parser->line_buf[local_10] = '\0';
    return local_10;
}

char NuGetChar(nufpar_s *parser) {
    char cVar1;
    int iVar2;
    int length;

    iVar2 = parser->buf_end;
    if (iVar2 < 0) {
        iVar2 = 0;
    }
    if (parser->buf_end < parser->char_pos) {
        if (parser->size < parser->buf_end + 1) {
            return '\0';
        }
        iVar2 = parser->size - iVar2;
        length = 0x1000;
        if (iVar2 < 0x1001) {
            length = iVar2;
        }
        iVar2 = NuFileRead(parser->file_handle, parser->file_buf, length);
        parser->buf_start = parser->buf_end + 1;
        parser->buf_end = parser->buf_end + iVar2;
        if (iVar2 == 0) {
            return '\0';
        }
    }
    cVar1 = parser->file_buf[parser->char_pos - parser->buf_start];
    parser->char_pos = parser->char_pos + 1;
    return cVar1;
}