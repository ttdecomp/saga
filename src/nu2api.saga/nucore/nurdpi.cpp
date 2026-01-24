#include <stddef.h>

#include "nu2api.saga/nucore/nurdp.h"
#include "nu2api.saga/nucore/nustring.h"

typedef struct rdpictxt_s {
    int cur_tok;
    int cur_val;
    char *buf;
    int pos;
    nurdpgetvarfn *get_var_fn;
} RDPICTXT;

enum {
    TOK_ADD = 0,
    TOK_SUB = 1,
    TOK_MUL = 2,
    TOK_DIV = 3,
    TOK_MOD = 4,
    TOK_LPAREN = 5,
    TOK_RPAREN = 6,
    TOK_END = 7,
    TOK_NUM = 8,
    TOK_OR = 9,
    TOK_AND = 10,
    TOK_NOT = 11,
    TOK_XOR = 12,
    TOK_LOGICAL_OR = 13,
    TOK_LOGICAL_AND = 14,
    TOK_LOGICAL_NOT = 15,
    TOK_EQ = 16,
    TOK_NEQ = 17,
    TOK_LT = 18,
    TOK_GT = 19,
    TOK_LT_EQ = 20,
    TOK_GT_EQ = 21,
    TOK_CNT = 22,
};

static int ishexnum(char c) {
    switch (c) {
        case '0' ... '9':
        case 'A' ... 'F':
        case 'a' ... 'f':
            return 1;
        default:
            return 0;
    }
}

static int hexcodetoint(char c) {
    switch (c) {
        case '0' ... '9':
            return c - 0x30;
        case 'a' ... 'f':
            return c - 0x57;
        case 'A' ... 'F':
            return c - 0x37;
        default:
            return 0;
    }
}

static int isnum(char c) {
    if (((unsigned int)c - 0x30) < 10) {
        return 1;
    }

    return 0;
}

static void get_tok_rdpi(RDPICTXT *ctx) {
    char var_name_buf[256];
    int var_name_pos;
    int ret;

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
        case '%':
            ctx->cur_tok = TOK_MOD;
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
        case '|':
            ctx->cur_tok = TOK_OR;
            ctx->pos++;

            if (ctx->buf[ctx->pos] == '|') {
                ctx->cur_tok = TOK_LOGICAL_OR;
                ctx->pos++;
            }
            return;
        case '&':
            ctx->cur_tok = TOK_AND;
            ctx->pos++;

            if (ctx->buf[ctx->pos] == '&') {
                ctx->cur_tok = TOK_LOGICAL_AND;
                ctx->pos++;
            }
            return;
        case '=':
            ctx->cur_tok = TOK_EQ;
            ctx->pos++;

            if (ctx->buf[ctx->pos] == '=') {
                ctx->pos++;
            }
            return;
        case '>':
            ctx->cur_tok = TOK_GT;
            ctx->pos++;

            if (ctx->buf[ctx->pos] == '=') {
                ctx->pos++;
                ctx->cur_tok = TOK_GT_EQ;
            }
            return;
        case '<':
            ctx->cur_tok = TOK_LT;
            ctx->pos++;

            if (ctx->buf[ctx->pos] == '=') {
                ctx->pos++;
                ctx->cur_tok = TOK_LT_EQ;
            }
            return;
        case '^':
            ctx->cur_tok = TOK_XOR;
            ctx->pos++;
            break;
        case '!':
            ctx->cur_tok = TOK_LOGICAL_NOT;
            ctx->pos++;

            if (ctx->buf[ctx->pos] == '=') {
                ctx->cur_tok = TOK_NEQ;
                ctx->pos++;
            }
            return;
        case '~':
            ctx->cur_tok = TOK_NOT;
            ctx->pos++;
            break;
        case '$':
            ctx->pos++;
            ctx->cur_tok = TOK_NUM;
            ctx->cur_val = 0;

            while (ishexnum(ctx->buf[ctx->pos])) {
                ctx->cur_val <<= 4;
                ctx->cur_val += hexcodetoint(ctx->buf[ctx->pos]);
                ctx->pos++;
            }
            break;
        case '0' ... '9':
            ctx->cur_tok = TOK_NUM;
            ctx->cur_val = 0;

            while (isnum(ctx->buf[ctx->pos])) {
                ctx->cur_val *= 10;
                ctx->cur_val += ctx->buf[ctx->pos] - 0x30;
                ctx->pos++;
            }
            break;
        case 'A' ... 'Z':
        case 'a' ... 'z':
            var_name_pos = 0;

            while (NuIsAlNum(ctx->buf[ctx->pos])) {
                var_name_buf[var_name_pos] = ctx->buf[ctx->pos];
                ctx->pos++;
                var_name_pos++;
            }

            var_name_buf[var_name_pos] = '\0';

            ctx->cur_tok = TOK_NUM;
            ctx->cur_val = 0;

            ret = (*ctx->get_var_fn)(var_name_buf, NULL, &ctx->cur_val);
            break;
        default:
            break;
    }
}

static int i_expr(RDPICTXT *ctx);

static int prim(RDPICTXT *ctx) {
    int expr;

    switch (ctx->cur_tok) {
        case TOK_NUM:
            get_tok_rdpi(ctx);

            return ctx->cur_val;
        case TOK_SUB:
            get_tok_rdpi(ctx);

            return -prim(ctx);
        case TOK_NOT:
            get_tok_rdpi(ctx);

            return ~prim(ctx);
        case TOK_LOGICAL_NOT:
            get_tok_rdpi(ctx);

            return !prim(ctx);
        case TOK_LPAREN:
            get_tok_rdpi(ctx);
            expr = i_expr(ctx);
            get_tok_rdpi(ctx);

            return expr;
        case TOK_END:
            return 1;
        default:
            return 0;
    }
}

static int term(RDPICTXT *ctx) {
    int term = prim(ctx);

    while (true) {
        switch (ctx->cur_tok) {
            case TOK_MUL:
                get_tok_rdpi(ctx);

                term *= prim(ctx);
                break;
            case TOK_DIV:
                get_tok_rdpi(ctx);

                term /= prim(ctx);
                break;
            case TOK_MOD:
                get_tok_rdpi(ctx);

                term %= prim(ctx);
                break;
            case TOK_EQ:
                get_tok_rdpi(ctx);

                term = term == prim(ctx);
                break;
            case TOK_NEQ:
                get_tok_rdpi(ctx);

                term = term != prim(ctx);
                break;
            case TOK_LT:
                get_tok_rdpi(ctx);

                term = term < prim(ctx);
                break;
            case TOK_GT:
                get_tok_rdpi(ctx);

                term = term > prim(ctx);
                break;
            case TOK_LT_EQ:
                get_tok_rdpi(ctx);

                term = term <= prim(ctx);
                break;
            case TOK_GT_EQ:
                get_tok_rdpi(ctx);

                term = term >= prim(ctx);
                break;
            default:
                return term;
        }
    }
}

static int i_expr(RDPICTXT *ctx) {
    int rhs;
    int expr = term(ctx);

    while (true) {
        switch (ctx->cur_tok) {
            case TOK_ADD:
                get_tok_rdpi(ctx);

                expr += term(ctx);
                break;
            case TOK_SUB:
                get_tok_rdpi(ctx);

                expr -= term(ctx);
                break;
            case TOK_AND:
                get_tok_rdpi(ctx);

                expr &= term(ctx);
                break;
            case TOK_OR:
                get_tok_rdpi(ctx);

                expr |= term(ctx);
                break;
            case TOK_XOR:
                get_tok_rdpi(ctx);

                expr ^= term(ctx);
                break;
            case TOK_LOGICAL_AND:
                get_tok_rdpi(ctx);

                rhs = term(ctx);
                expr = expr != 0 && rhs != 0;
                break;
            case TOK_LOGICAL_OR:
                get_tok_rdpi(ctx);

                rhs = term(ctx);
                expr = expr != 0 || rhs != 0;
                break;
            default:
                return expr;
        }
    }
}

int NuRDPI(char *input) {
    return NuRDPIVar(input, NULL);
}

int NuRDPIVar(char *input, nurdpgetvarfn *get_var_fn) {
    RDPICTXT ctx;
    int ret;

    ctx.buf = input;
    ctx.pos = 0;
    ctx.get_var_fn = get_var_fn;

    get_tok_rdpi(&ctx);

    ret = i_expr(&ctx);

    return ret;
}
