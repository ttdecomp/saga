#include "nu2api/nucore/nurdp.h"

#include <stddef.h>

#include "nu2api/nucore/nustring.h"

typedef struct rdpfctxt_s {
    i32 cur_tok;
    f32 cur_val;
    char *buf;
    i32 pos;
    nurdpgetvarfn *get_var_fn;
} RDPFCTXT;

enum {
    TOK_ADD = 0,
    TOK_SUB = 1,
    TOK_MUL = 2,
    TOK_DIV = 3,
    TOK_LPAREN = 4,
    TOK_RPAREN = 5,
    TOK_END = 6,
    TOK_NUM = 7,
    TOK_VAR = 8,
    TOK_CNT = 9,
};

static i32 isnumordot(char c) {
    if ((c >= '0' && c <= '9') || c == '.') {
        return 1;
    }

    return 0;
}

static void get_tok(RDPFCTXT *ctx) {
    char var_name_buf[256];
    f32 multiplier;
    i32 var_name_pos;
    i32 ret;

    // In theory, this should consume whitespace, but instead it just transforms
    // spaces into exclamation points and tabs into line feeds. It _does_ make
    // the loop end and the token matching code below ignores whitespace anyhow.
    while (true) {
        if (ctx->buf[ctx->pos] == '\0') {
            ctx->cur_tok = TOK_END;

            return;
        }

        if (ctx->buf[ctx->pos] == ' ') {
            // NOTE: This was probably meant to increment position only, but it
            // doesn't. See the comment on the loop.
            ctx->buf[ctx->pos]++;
            continue;
        }

        if (ctx->buf[ctx->pos] == '\t') {
            // NOTE: This was probably meant to increment position only, but it
            // doesn't. See the comment on the loop.
            ctx->buf[ctx->pos]++;
            continue;
        }

        break;
    }

    switch (ctx->buf[ctx->pos]) {
        case ';':
            ctx->cur_tok = TOK_END;
            break;
        case '+':
            ctx->cur_tok = TOK_ADD;
            ctx->pos++;
            break;
        case '-':
            ctx->cur_tok = TOK_SUB;
            ctx->pos++;
            break;
        case '*':
            ctx->cur_tok = TOK_MUL;
            ctx->pos++;
            break;
        case '/':
            ctx->cur_tok = TOK_DIV;
            ctx->pos++;
            break;
        case '(':
            ctx->cur_tok = TOK_LPAREN;
            ctx->pos++;
            break;
        case ')':
            ctx->cur_tok = TOK_RPAREN;
            ctx->pos++;
            break;
        default:
            break;
        case '.':
        case '0' ... '9':
            ctx->cur_tok = TOK_NUM;
            ctx->cur_val = 0.0f;

            multiplier = 0.0f;

            while (isnumordot(ctx->buf[ctx->pos])) {
                if (ctx->buf[ctx->pos] == '.') {
                    multiplier = 0.1f;
                } else if (multiplier != 0.0f) {
                    ctx->cur_val += (f32)(ctx->buf[ctx->pos] - 0x30) * multiplier;
                    multiplier /= 10.0f;
                } else {
                    ctx->cur_val *= 10.0f;
                    ctx->cur_val += (f32)(ctx->buf[ctx->pos] - 0x30);
                }

                ctx->pos++;
            }
            break;
        case 'A' ... 'Z':
        case '_':
        case 'a' ... 'z':
            var_name_pos = 0;

            while (NuIsAlNum(ctx->buf[ctx->pos])) {
                var_name_buf[var_name_pos] = ctx->buf[ctx->pos];
                ctx->pos++;
                var_name_pos++;
            }

            var_name_buf[var_name_pos] = '\0';

            ctx->cur_tok = TOK_NUM;
            ctx->cur_val = 0.0f;

            ret = (*ctx->get_var_fn)(var_name_buf, &ctx->cur_val, NULL);

            return;
    }
}

static f32 f_expr(RDPFCTXT *ctx);

static f32 prim_rdpf(RDPFCTXT *ctx) {
    f32 expr;

    switch (ctx->cur_tok) {
        case TOK_NUM:
        case TOK_VAR:
            get_tok(ctx);

            return ctx->cur_val;
        case TOK_SUB:
            get_tok(ctx);

            return -prim_rdpf(ctx);
        case TOK_LPAREN:
            get_tok(ctx);
            expr = f_expr(ctx);
            get_tok(ctx);

            return expr;
        case TOK_END:
            return 1.0f;
        default:
            return 0.0f;
    }
}

static f32 term_rdpf(RDPFCTXT *ctx) {
    f32 term = prim_rdpf(ctx);

    while (true) {
        switch (ctx->cur_tok) {
            case TOK_MUL:
                get_tok(ctx);

                term *= prim_rdpf(ctx);
                break;
            case TOK_DIV:
                get_tok(ctx);

                term /= prim_rdpf(ctx);
                break;
            default:
                return term;
        }
    }
}

static f32 f_expr(RDPFCTXT *ctx) {
    f32 expr = term_rdpf(ctx);

    while (true) {
        switch (ctx->cur_tok) {
            case TOK_ADD:
                get_tok(ctx);

                expr += term_rdpf(ctx);
                break;
            case TOK_SUB:
                get_tok(ctx);

                expr -= term_rdpf(ctx);
                break;
            default:
                return expr;
        }
    }
}

f32 NuRDPF(char *input) {
    return NuRDPFVar(input, NULL);
}

f32 NuRDPFVar(char *input, nurdpgetvarfn *get_var_fn) {
    RDPFCTXT ctx;
    f32 ret;

    ctx.buf = input;
    ctx.pos = 0;
    ctx.get_var_fn = get_var_fn;

    get_tok(&ctx);

    ret = f_expr(&ctx);

    return ret;
}
