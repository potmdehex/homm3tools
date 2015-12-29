// Created by John Åkerblom 2014-11-18

#ifndef __H3MLIB_CTX_H_DEF__
#define __H3MLIB_CTX_H_DEF__

#include "../h3m_structures/h3m.h"
#include "../h3m_modembed/h3m_modembed.h"
#include "../meta/h3mlib_meta.h"
#include "../h3m_parsing/h3mlib_parsing.h"

struct H3MLIB_CALLBACKS {
    h3m_parse_cb_t cb_parse;
    h3m_error_cb_t cb_error;
    h3m_custom_def_cb_t cb_def;
    void *cb_data;
};

struct H3MLIB_CTX {
    struct H3M h3m;
    struct H3M_MODEMBED h3m_code;
    struct H3MLIB_META meta;
    struct H3MLIB_PARSING parsing;
    struct H3MLIB_CALLBACKS callbacks;
};

#endif
