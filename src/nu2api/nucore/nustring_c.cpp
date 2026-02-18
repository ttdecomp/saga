#include "nu2api/nucore/nustring.h"

#include "decomp.h"
#include "nu2api/nufile/nufile.h"
#include "nu2api/nufile/nufpar.h"

void NuStrCat(char *str, const char *ext) {
    while (*str != '\0') {
        str++;
    }

    if (ext != NULL) {
        do {
            *str = *ext;
            str++;
        } while (*(ext++) != '\0');
    }
}

i32 NuStrCmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = *a;
        b_cursor = *b;

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

char *NuStrChr(char *src, char c) {
    while (*src != '\0') {
        if (*src == c) {
            return src;
        }

        src++;
    }

    return NULL;
}

i32 NuStrCpy(char *dst, const char *src) {
    char *dst_start = dst;

    if (src != NULL) {
        while (*src != '\0') {
            *dst = *src;
            dst++;
            src++;
        }
    }

    *dst = '\0';

    return dst - dst_start;
}

void NuStrFixExtPlatform(char *dst, char *src, char *ext, i32 dst_size, char *platform_string) {
    LOG_DEBUG("dst=%p, src=%s, ext=%s, dst_size=%d, platform_string=%s", dst, src, ext, dst_size,
              platform_string != NULL ? platform_string : "NULL");

    char *period;
    char *sep;
    char *win_sep;

    if (src == NULL || ext == NULL || dst == NULL) {
        return;
    }

    NuStrCpy(dst, src);

    period = NuStrRChr(dst, '.');

    sep = NuStrRChr(dst, '/');
    win_sep = NuStrRChr(dst, '\\');
    if (win_sep > sep) {
        sep = win_sep;
    }

    if (sep >= period) {
        NuStrCat(dst, "_");

        if (platform_string != NULL) {
            NuStrCat(dst, platform_string);
        }

        NuStrCat(dst, ".");
        NuStrCat(dst, ext);
    } else {
        *period = '\0';
        NuStrCat(period, "_");

        if (platform_string != NULL) {
            NuStrCat(period, platform_string);
        }

        NuStrCat(period, ".");
        NuStrCat(period, ext);
    }

    if (NuStrLen(dst) == dst_size - 1) {
        // Nothing here. Presumably some gated code in the original.
    }
}

i32 NuStrICmp(const char *a, const char *b) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = NuToUpper(*a);
        b_cursor = NuToUpper(*b);

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

char *NuStrIStr(char *str, char *sub) {
    char *str_cursor;
    char *sub_cursor;

    while (*str != '\0') {
        str_cursor = str;
        sub_cursor = sub;

        while (*sub_cursor != '\0') {
            if (*str_cursor == '\0') {
                break;
            }

            if (NuToUpper(*str_cursor) != NuToUpper(*sub_cursor)) {
                break;
            }

            str_cursor++;
            sub_cursor++;
        }

        if (*sub_cursor == '\0') {
            return str;
        }

        str++;
    }

    return NULL;
}

i32 NuStrLen(const char *str) {
    i32 count = 0;

    while (*str != '\0') {
        count++;
        str++;
    }

    return count;
}

i32 NuStrNCmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    if (n == 0) {
        return 0;
    }

    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_cursor = *a;
        b_cursor = *b;

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

i32 NuStrNICmp(const char *a, const char *b, i32 n) {
    char a_cursor;
    char b_cursor;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    if (n == 0) {
        return 0;
    }

    if (n == -1) {
        n = NuStrLen(a);
    } else if (n == -2) {
        n = NuStrLen(b);
    }

    do {
        a_cursor = NuToUpper(*a);
        b_cursor = NuToUpper(*b);

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
        n--;
    } while (a_cursor != '\0' && b_cursor != '\0' && n != 0);

    return 0;
}

i32 NuStrNCpy(char *dst, const char *src, i32 n) {
    i32 count = 0;

    if (src == NULL) {
        *dst = '\0';
        return 0;
    }

    do {
        n--;
        if (n < 1) {
            *dst = '\0';
            count++;

            break;
        }

        *dst = *src;
        dst++;

        count++;
    } while (*(src++) != '\0');

    return count - 1;
}

char *NuStrRChr(char *src, char c) {
    char *ptr = src;

    while (*ptr != '\0') {
        ptr++;
    }

    while (ptr >= src) {
        if (*ptr == c) {
            return ptr;
        }

        ptr--;
    }

    return NULL;
}

u8 NuToLower(u8 c) {
    if (c >= 'A' && c <= 'Z') {
        c += 0x20;
    } else if (c > 0xbf && c < 0xe0) {
        c += 0x20;
    }

    return c;
}

u8 NuToUpper(u8 c) {
    if (c >= 'a' && c <= 'z') {
        c -= 0x20;
    } else if (c > 0xdf) {
        c -= 0x20;
    }

    return c;
}

NUWCHAR NuToLowerW(NUWCHAR c) {
    if (c >= 0x41 && c <= 0x5a) {
        c += 0x20;
    } else if (c > 0xbf && c < 0xe0) {
        c += 0x20;
    }

    return c;
}

NUWCHAR NuToUpperW(NUWCHAR c) {
    if (c >= 0x61 && c <= 0x7a) {
        c -= 0x20;
    } else if (c > 0xdf && c < 0x100) {
        c -= 0x20;
    }

    return c;
}

f32 NuAToF(char *string) {
    f32 dividend = 0.0f;
    f32 divisor = 1.0f;

    char c = *string;
    string++;

    if (c == '-') {
        divisor = -1.0f;

        c = *string;
        string++;
    }

    while (c >= '0' && c <= '9') {
        dividend *= 10.0f;
        dividend += (f32)(c - 0x30);

        c = *string;
        string++;
    }

    if (c == '.') {
        c = *string;
        string++;

        while (c >= '0' && c <= '9') {
            divisor *= 10.0f;
            dividend *= 10.0f;
            dividend += (f32)(c - 0x30);

            c = *string;
            string++;
        }
    }

    return dividend / divisor;
}

i32 NuAToI(char *string) {
    i32 value = 0;
    i32 sign = 0;

    char c = *string;
    string++;

    if (c == '-') {
        sign = -1;

        c = *string;
        string++;
    }

    while (c >= '0' && c <= '9') {
        value = (value << 3) + 2 * value;
        value = value + (i32)c - 0x30;

        c = *string;
        string++;
    }

    if (sign != 0) {
        return value * sign;
    }

    return value;
}

i32 NuHexStringToI(char *string) {
    i32 value = 0;

    for (; *string != '\0'; string++) {
        value <<= 4;
        i32 upper = (i32)NuToUpper(*string);

        if (upper <= '9' && upper >= '0') {
            value |= upper - 0x30;
        } else if (upper <= 'F' && upper >= 'A') {
            value |= upper - 0x37;
        } else {
            return 0;
        }
    }

    return value;
}

void NuAsciiToUnicode(NUWCHAR16 *dst, char *src) {
    i32 pos;

    pos = 0;
    *dst = 0;

    if (src == NULL) {
        return;
    }

    if (dst == NULL) {
        return;
    }

    for (; src[pos] != '\0'; pos++) {
        dst[pos] = (u8)src[pos];
    }

    dst[pos] = '\0';
}

void NuUnicodeToAscii(char *dst, NUWCHAR16 *src) {
    i32 pos;

    pos = 0;

    if (src == NULL) {
        return;
    }

    if (dst == NULL) {
        return;
    }

    *dst = '\0';

    for (; src[pos] != '\0'; pos++) {
        if ((src[pos] & 0xff00) != 0) {
            switch (src[pos]) {
                // U+2018 LEFT SINGLE QUOTATION MARK
                case 0x2018:
                    dst[pos] = 0x91;
                    break;
                // U+2019 RIGHT SINGLE QUOTATION MARK
                case 0x2019:
                    dst[pos] = 0x92;
                    break;
                // U+2013 EN DASH
                case 0x2013:
                    dst[pos] = 0x96;
                    break;
                // U+2026 HORIZONTAL ELLIPSIS
                case 0x2026:
                    dst[pos] = 0x85;
                    break;
                // U+2122 TRADE MARK SIGN
                case 0x2122:
                    dst[pos] = 0x99;
                    break;
                default:
                    dst[pos] = '?';
            }
        } else {
            dst[pos] = src[pos];
        }
    }

    dst[pos] = '\0';
}

void NuUnicodeToUTF8(NUWCHAR8 *dst, NUWCHAR16 *src) {
    if (src == NULL) {
        return;
    }

    if (dst == NULL) {
        return;
    }

    *dst = '\0';

    for (; *src != 0; src++) {
        if (*src < 0x80) {
            *dst++ = *src;
        } else if (*src < 0x800) {
            *dst++ = ((*src >> 0x6) & 0x1f) | 0xc0;
            *dst++ = (*src & 0x3f) | 0x80;
        } else {
            *dst++ = ((*src >> 0xc) & 0x1f) | 0xe0;
            *dst++ = ((*src >> 0x6) & 0x3f) | 0x80;
            *dst++ = (*src & 0x3f) | 0x80;
        }
    }

    *dst = '\0';
}

void NuStrUpr(char *dst, const char *src) {
    for (; *src != '\0'; src++) {
        *dst = NuToUpper(*src);
        dst++;
    }

    *dst = *src;
}

i32 NuStrCpyW(NUWCHAR *dst, NUWCHAR *src) {
    i32 len;

    len = 0;

    if (src == NULL) {
        *dst = 0;
    } else {
        do {
            *dst = *src;
            dst++;
            len++;
        } while (*src++ != 0);
    }

    return len;
}

i32 NuStrICmpW(NUWCHAR *a, NUWCHAR *b) {
    NUWCHAR a_cursor;
    NUWCHAR b_cursor;

    if (a == NULL) {
        return -1;
    }

    if (b == NULL) {
        return 1;
    }

    do {
        a_cursor = NuToUpperW(*a);
        b_cursor = NuToUpperW(*b);

        if (a_cursor > b_cursor) {
            return 1;
        }

        if (a_cursor < b_cursor) {
            return -1;
        }

        a++;
        b++;
    } while (a_cursor != '\0' && b_cursor != '\0');

    return 0;
}

i32 NuStrLenW(const NUWCHAR *str) {
    i32 count = 0;

    while (*str != 0) {
        count++;
        str++;
    }

    return count;
}

i32 NuIsAlNum(char c) {
    switch (c) {
        case 'A' ... 'Z':
        case 'a' ... 'z':
        case '0' ... '9':
        case '_':
            return 1;
            break;
        default:
            return 0;
    }
}

static NUSTRINGBANK StringBank[3];
static i16 CurrentStringBank;
static i32 nustring_format = 1;
char *NuBlankString = " ";

i32 NuStringTableLoadCSV(char *filepath, VARIPTR *buf, VARIPTR buf_end, char *label, char *language) {
    i32 parse_buf_size;
    NUSTRINGBANK *bank;
    NUWCHAR label_utf16[0x40];
    NUWCHAR lang_utf16[0x40];
    void *mem_buf;
    i32 bytes_read;
    char *word_buf;
    char *line_buf;
    char *tmp_buf;
    NUFILE mem_file;
    NUFPAR *parser;
    NUSTRING *strings;
    i32 column;
    i32 label_column;
    i32 lang_column;
    i32 len;
    i32 i;
    i32 max_strings;

    label_column = -1;
    lang_column = -1;
    i = 0;
    max_strings = 0;
    word_buf = NULL;
    line_buf = NULL;
    tmp_buf = NULL;
    parse_buf_size = 0x8000;
    strings = NULL;
    bank = &StringBank[CurrentStringBank];

    NuAsciiToUnicode(label_utf16, label);
    NuAsciiToUnicode(lang_utf16, language);

    bank->strings = NULL;
    bank->max_strings = 0;
    bank->string_count = 0;

    mem_buf = (void *)(buf_end.addr - 0x100000);

    bytes_read = NuFileLoadBuffer(filepath, mem_buf, 0x100000);

    word_buf = (char *)((usize)mem_buf + bytes_read);
    line_buf = (char *)((usize)mem_buf + bytes_read + parse_buf_size);
    tmp_buf = (char *)((usize)mem_buf + bytes_read + parse_buf_size + parse_buf_size);

    mem_file = NuMemFileOpen(mem_buf, bytes_read, NUFILE_READ);

    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);

        if (parser != NULL) {
            parser->word_buf = word_buf;
            parser->line_buf = line_buf;

            parser->line_buf_size = parse_buf_size;
            parser->word_buf_size = parse_buf_size;

            while (NuFParGetLine(parser) != 0) {
                max_strings++;
            }

            NuFParClose(parser);
        }

        NuFileClose(mem_file);
    }

    strings = (NUSTRING *)ALIGN(buf->addr, 0x4);
    buf->addr = ALIGN(buf->addr, 0x4);
    buf->addr = buf->addr + max_strings * sizeof(NUSTRING);

    mem_file = NuMemFileOpen(mem_buf, bytes_read, NUFILE_READ);

    if (mem_file != 0) {
        parser = NuFParOpen(mem_file);

        if (parser != NULL) {
            parser->word_buf = word_buf;
            parser->line_buf = line_buf;

            parser->line_buf_size = parse_buf_size;
            parser->word_buf_size = parse_buf_size;

            while (NuFParGetLine(parser) != 0) {
                column = 0;

                while (NuFParGetWord(parser) != 0) {
                    if (NuStrICmpW((NUWCHAR *)parser->word_buf, label_utf16) == 0) {
                        label_column = column;
                    }

                    if (NuStrICmpW((NUWCHAR *)parser->word_buf, lang_utf16) == 0) {
                        lang_column = column;
                    }

                    column++;
                }

                if (label_column >= 0 && lang_column >= 0) {
                    break;
                }
            }

            if (label_column >= 0 && lang_column >= 0) {
                while (NuFParGetLine(parser) != 0) {
                    column = 0;

                    while (NuFParGetWord(parser) != 0) {
                        if (column == label_column) {
                            len = NuStrLenW((NUWCHAR *)parser->word_buf) + 1;

                            strings[i].id = buf->char_ptr;
                            buf->addr += len * sizeof(char);

                            NuUnicodeToAscii(strings[i].id, (NUWCHAR16 *)parser->word_buf);
                        }

                        if (column == lang_column) {
                            if (nustring_format == 1) {
                                len = NuStrLenW((NUWCHAR *)parser->word_buf) + 1;

                                strings[i].str_utf16 = (NUWCHAR *)ALIGN(buf->addr, 0x2);
                                buf->addr = ALIGN(buf->addr, 0x2);

                                // Using `sizeof(NUWCHAR)` here causees this not
                                // to match.
                                buf->addr += len * 2;

                                NuStrCpyW(strings[i].str_utf16, (NUWCHAR *)parser->word_buf);
                            } else {
                                NuUnicodeToUTF8((NUWCHAR8 *)tmp_buf, (NUWCHAR16 *)parser->word_buf);

                                // We can only do this after conversion, as the
                                // size in bytes of a UTF-8 string can't be
                                // easily predicted from its UTF-16 equivalent.
                                len = NuStrLen(tmp_buf) + 1;

                                // There's no need here to align to 2 bytes, but
                                // it's done anyhow.
                                strings[i].str_utf8 = (NUWCHAR8 *)ALIGN(buf->addr, 0x2);
                                buf->addr = ALIGN(buf->addr, 0x2);
                                buf->addr += len * sizeof(NUWCHAR8);

                                NuStrCpy((char *)strings[i].str_utf8, tmp_buf);
                            }
                        }

                        column++;

                        if (column > label_column && column > lang_column) {
                            if (strings[i].id != NULL && strings[i].str_utf16 != NULL) {
                                i++;
                            } else {
                                strings[i].id = NULL;
                                strings[i].str_utf16 = NULL;
                            }

                            break;
                        }
                    }
                }
            }

            NuFParClose(parser);

            bank->strings = strings;
            bank->string_count = i;
            bank->max_strings = max_strings;
        }

        NuFileClose(mem_file);
    }

    return bank->string_count;
}

void NuStringTableSetBank(i32 bank) {
    if (bank >= 0 && bank < 3) {
        CurrentStringBank = bank;
    }
}

void NuStringTableSetFormat(i32 format) {
    nustring_format = format;
}

NUWCHAR *NuStringTableGetByName(char *name) {
    i32 j;
    i32 i;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < StringBank[i].string_count; j++) {
            if (StringBank[i].strings[j].id != NULL && StringBank[i].strings[j].str_utf16 != NULL) {
                if (NuStrICmp(StringBank[i].strings[j].id, name) == 0) {
                    return StringBank[i].strings[j].str_utf16;
                }
            }
        }
    }

    // This is a little horrifying, as `NuBlankString` appears to be declared as
    // a string literal. This works out largely because of alignment
    // requirements.
    return (NUWCHAR *)NuBlankString;
}
