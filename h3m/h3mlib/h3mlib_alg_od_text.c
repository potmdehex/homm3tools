// Created by John Åkerblom 2014-12-18

#include "h3mlib.h"
#include "h3mlib_ctx.h"

#include <math.h>               // abs()

struct DOT_CTX {
    struct H3MLIB_CTX *h3mlib_ctx;
    const char *def;
    int oa_index;
};

static void _init_dot_ctx(struct H3MLIB_CTX *h3mlib_ctx, const char *def,
    int oa_index, struct DOT_CTX *dot_ctx)
{
    dot_ctx->h3mlib_ctx = h3mlib_ctx;
    dot_ctx->def = def;
    dot_ctx->oa_index = oa_index;
}

static void _dot(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    h3m_add_od(dot_ctx->h3mlib_ctx, dot_ctx->oa_index, x, y, z, NULL);
}

// Everything below except the parameters passed to _init_dot_ctx is generic

static void _line(int x1, int y1, int x2, int y2, int z,
    struct DOT_CTX *dot_ctx)
{
    int x_diff = (x1 - x2);
    int y_diff = (y1 - y2);
    int diff = (0 != x_diff) ? abs(x_diff) : abs(y_diff);
    int x_inc = (0 == x_diff) ? 0 : ((0 > x_diff) ? 1 : -1);
    int y_inc = (0 == y_diff) ? 0 : ((0 > y_diff) ? 1 : -1);
    int cur_x = x1;
    int cur_y = y1;
    int i;

    for (i = 0; i < diff + 1; ++i) {
        _dot(cur_x, cur_y, z, dot_ctx);
        cur_x += x_inc;
        cur_y += y_inc;
    }
}

static int _a(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y + 1, x, y + 4, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);

    _line(x + 3, y + 1, x + 3, y + 4, z, dot_ctx);
    return 4;
}

static int _b(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x + 3, y + 1, z, dot_ctx);
    _dot(x + 3, y + 3, z, dot_ctx);
    return 4;
}

static int _c(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y + 1, x, y + 3, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x + 3, y + 1, z, dot_ctx);
    _dot(x + 3, y + 3, z, dot_ctx);
    return 4;
}

static int _d(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);
    _line(x + 3, y + 1, x + 3, y + 3, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);
    return 4;
}

static int _e(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);
    _line(x, y, x + 2, y, z, dot_ctx);
    _line(x, y + 2, x + 2, y + 2, z, dot_ctx);
    _line(x, y + 4, x + 2, y + 4, z, dot_ctx);
    return 3;
}

static int _f(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);
    _line(x, y, x + 2, y, z, dot_ctx);
    _line(x, y + 2, x + 2, y + 2, z, dot_ctx);
    return 3;
}

static int _g(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y + 1, x, y + 3, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x + 3, y + 3, z, dot_ctx);

    _line(x + 2, y + 2, x + 3, y + 2, z, dot_ctx);

    return 4;
}

static int _h(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);
    _line(x + 3, y, x + 3, y + 4, z, dot_ctx);

    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    return 4;
}

static int _i(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x + 2, y, z, dot_ctx);
    _line(x, y + 4, x + 2, y + 4, z, dot_ctx);
    _line(x + 1, y + 1, x + 1, y + 4, z, dot_ctx);
    return 3;
}

static int _j(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x + 3, y, z, dot_ctx);
    _line(x + 3, y, x + 3, y + 4, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x, y + 3, z, dot_ctx);
    return 4;
}

static int _k(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);

    _line(x + 1, y + 2, x + 3, y, z, dot_ctx);
    _line(x + 2, y + 3, x + 3, y + 4, z, dot_ctx);
    return 4;
}

static int _l(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);

    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);
    return 3;
}

static int _m(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);
    _line(x + 4, y, x + 4, y + 4, z, dot_ctx);

    _line(x + 1, y + 1, x + 2, y + 2, z, dot_ctx);
    _dot(x + 3, y + 1, z, dot_ctx);

    return 5;
}

static int _n(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);
    _line(x + 3, y, x + 3, y + 4, z, dot_ctx);

    _line(x + 1, y + 1, x + 2, y + 2, z, dot_ctx);

    return 4;
}

static int _o(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y + 1, x, y + 3, z, dot_ctx);
    _line(x + 3, y + 1, x + 3, y + 3, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    return 4;
}

static int _p(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);

    _dot(x + 3, y + 1, z, dot_ctx);
    return 4;
}

static int _q(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y + 1, x, y + 3, z, dot_ctx);
    _line(x + 3, y + 1, x + 3, y + 3, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x + 2, y + 3, z, dot_ctx);
    _line(x + 3, y + 4, x + 4, y + 4, z, dot_ctx);

    return 4;
}

static int _r(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 4, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);

    _dot(x + 3, y + 1, z, dot_ctx);

    _line(x + 3, y + 3, x + 3, y + 4, z, dot_ctx);

    return 4;
}

static int _s(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x + 1, y, x + 3, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    _line(x, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x, y + 1, z, dot_ctx);
    _dot(x + 3, y + 3, z, dot_ctx);
    return 4;
}

static int _t(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x + 4, y, z, dot_ctx);
    _line(x + 2, y + 1, x + 2, y + 4, z, dot_ctx);
    return 5;
}

static int _u(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 3, z, dot_ctx);
    _line(x + 3, y, x + 3, y + 3, z, dot_ctx);

    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);
    return 4;
}

static int _v(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 1, z, dot_ctx);
    _line(x + 4, y, x + 4, y + 1, z, dot_ctx);

    _line(x + 1, y + 2, x + 1, y + 3, z, dot_ctx);
    _line(x + 3, y + 2, x + 3, y + 3, z, dot_ctx);

    _dot(x + 2, y + 4, z, dot_ctx);

    return 5;
}

static int _w(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 3, z, dot_ctx);
    _line(x + 4, y, x + 4, y + 3, z, dot_ctx);

    _line(x + 2, y + 2, x + 2, y + 3, z, dot_ctx);

    _dot(x + 1, y + 4, z, dot_ctx);
    _dot(x + 3, y + 4, z, dot_ctx);

    return 5;
}

static int _x(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x + 4, y + 4, z, dot_ctx);

    _line(x, y + 4, x + 1, y + 3, z, dot_ctx);
    _line(x + 3, y + 1, x + 4, y, z, dot_ctx);

    return 5;
}

static int _y(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 1, z, dot_ctx);
    _line(x + 4, y, x + 4, y + 1, z, dot_ctx);

    _dot(x + 1, y + 2, z, dot_ctx);
    _dot(x + 3, y + 2, z, dot_ctx);

    _line(x + 2, y + 3, x + 2, y + 4, z, dot_ctx);
    return 5;
}

static int _z(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x + 4, y, z, dot_ctx);
    _line(x, y + 4, x + 4, y + 4, z, dot_ctx);

    _line(x + 1, y + 3, x + 3, y + 1, z, dot_ctx);
    return 5;
}

static int _0(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    return _o(x, y, z, dot_ctx);
}

static int _1(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x + 1, y, x + 1, y + 4, z, dot_ctx);
    _dot(x, y, z, dot_ctx);
    return 2;
}

static int _2(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x, y + 4, x + 3, y + 4, z, dot_ctx);

    _line(x + 1, y + 3, x + 3, y + 1, z, dot_ctx);
    _dot(x, y + 1, z, dot_ctx);

    return 4;
}

static int _3(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    _line(x, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x + 3, y + 1, z, dot_ctx);
    _dot(x + 3, y + 3, z, dot_ctx);
    return 4;
}

static int _4(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 2, z, dot_ctx);
    _line(x + 3, y, x + 3, y + 4, z, dot_ctx);

    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    return 4;
}

static int _5(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x, y + 2, z, dot_ctx);

    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x, y + 4, x + 1, y + 4, z, dot_ctx);
    _dot(x + 1, y + 2, z, dot_ctx);
    _dot(x + 2, y + 3, z, dot_ctx);

    return 3;
}

static int _6(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _dot(x + 3, y + 3, z, dot_ctx);

    _line(x, y + 1, x, y + 3, z, dot_ctx);
    return 4;
}

static int _7(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x, y, x + 3, y, z, dot_ctx);

    _line(x + 1, y + 3, x + 3, y + 1, z, dot_ctx);
    _dot(x + 1, y + 4, z, dot_ctx);
    return 4;
}

static int _8(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _line(x + 3, y + 1, x + 3, y + 3, z, dot_ctx);
    _line(x, y + 1, x, y + 3, z, dot_ctx);
    return 4;
}

static int _9(int x, int y, int z, struct DOT_CTX *dot_ctx)
{
    _line(x + 1, y, x + 2, y, z, dot_ctx);
    _line(x + 1, y + 2, x + 2, y + 2, z, dot_ctx);
    _line(x + 1, y + 4, x + 2, y + 4, z, dot_ctx);

    _line(x + 3, y + 1, x + 3, y + 3, z, dot_ctx);
    _dot(x, y + 1, z, dot_ctx);
    return 3;
}

int h3mlib_alg_od_text(struct H3MLIB_CTX *ctx, const char *def, int oa_index,
    int x, int y, int z, const char *text)
{
    const char *p = text;
    int x_width = 0;
    struct DOT_CTX dot_ctx;

    _init_dot_ctx(ctx, def, oa_index, &dot_ctx);

    while (*p != 0) {
        switch (*p++) {
        case ' ':
            x_width = 2;
            break;
        case 'A':
            x_width = _a(x, y, z, &dot_ctx);
            break;
        case 'B':
            x_width = _b(x, y, z, &dot_ctx);
            break;
        case 'C':
            x_width = _c(x, y, z, &dot_ctx);
            break;
        case 'D':
            x_width = _d(x, y, z, &dot_ctx);
            break;
        case 'E':
            x_width = _e(x, y, z, &dot_ctx);
            break;
        case 'F':
            x_width = _f(x, y, z, &dot_ctx);
            break;
        case 'G':
            x_width = _g(x, y, z, &dot_ctx);
            break;
        case 'H':
            x_width = _h(x, y, z, &dot_ctx);
            break;
        case 'I':
            x_width = _i(x, y, z, &dot_ctx);
            break;
        case 'J':
            x_width = _j(x, y, z, &dot_ctx);
            break;
        case 'K':
            x_width = _k(x, y, z, &dot_ctx);
            break;
        case 'L':
            x_width = _l(x, y, z, &dot_ctx);
            break;
        case 'M':
            x_width = _m(x, y, z, &dot_ctx);
            break;
        case 'N':
            x_width = _n(x, y, z, &dot_ctx);
            break;
        case 'O':
            x_width = _o(x, y, z, &dot_ctx);
            break;
        case 'P':
            x_width = _p(x, y, z, &dot_ctx);
            break;
        case 'Q':
            x_width = _q(x, y, z, &dot_ctx);
            break;
        case 'R':
            x_width = _r(x, y, z, &dot_ctx);
            break;
        case 'S':
            x_width = _s(x, y, z, &dot_ctx);
            break;
        case 'T':
            x_width = _t(x, y, z, &dot_ctx);
            break;
        case 'U':
            x_width = _u(x, y, z, &dot_ctx);
            break;
        case 'V':
            x_width = _v(x, y, z, &dot_ctx);
            break;
        case 'W':
            x_width = _w(x, y, z, &dot_ctx);
            break;
        case 'X':
            x_width = _x(x, y, z, &dot_ctx);
            break;
        case 'Y':
            x_width = _y(x, y, z, &dot_ctx);
            break;
        case 'Z':
            x_width = _z(x, y, z, &dot_ctx);
            break;
        case '0':
            x_width = _0(x, y, z, &dot_ctx);
            break;
        case '1':
            x_width = _1(x, y, z, &dot_ctx);
            break;
        case '2':
            x_width = _2(x, y, z, &dot_ctx);
            break;
        case '3':
            x_width = _3(x, y, z, &dot_ctx);
            break;
        case '4':
            x_width = _4(x, y, z, &dot_ctx);
            break;
        case '5':
            x_width = _5(x, y, z, &dot_ctx);
            break;
        case '6':
            x_width = _6(x, y, z, &dot_ctx);
            break;
        case '7':
            x_width = _7(x, y, z, &dot_ctx);
            break;
        case '8':
            x_width = _8(x, y, z, &dot_ctx);
            break;
        case '9':
            x_width = _9(x, y, z, &dot_ctx);
            break;
        default:
            // return -1;
            x_width = _a(x, y, z, &dot_ctx);
            break;
        }

        x += x_width + 2;
    }

    return 0;
}
