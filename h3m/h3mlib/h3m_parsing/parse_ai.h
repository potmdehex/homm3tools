// Created by John Åkerblom 2014-11-21

#ifndef __PARSE_AI_H_DEF__
#define __PARSE_AI_H_DEF__

#include "../h3mlib.h"
#include "../h3mlib_ctx.h"

int parse_ai(struct H3MLIB_CTX *ctx);

int parse_ai_custom_heroes(struct H3MLIB_CTX *ctx);
int parse_ai_hero_settings(struct H3MLIB_CTX *ctx);
int parse_ai_lose_cond(struct H3MLIB_CTX *ctx);
int parse_ai_rumor(struct H3MLIB_CTX *ctx);
int parse_ai_teams(struct H3MLIB_CTX *ctx);
int parse_ai_win_cond(struct H3MLIB_CTX *ctx);

#endif
