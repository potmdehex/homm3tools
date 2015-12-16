// Created by John Åkerblom 2014-11-21

#ifndef __PARSE_PLAYERS_H_DEF__
#define __PARSE_PLAYERS_H_DEF__

#include "../h3mlib.h"
#include "../internal/h3mlib_ctx.h"
#include "parse_player_ai.h"

int parse_players_roe(struct H3MLIB_CTX *ctx);
int parse_players_ab(struct H3MLIB_CTX *ctx);
int parse_players_sod(struct H3MLIB_CTX *ctx);

#endif
