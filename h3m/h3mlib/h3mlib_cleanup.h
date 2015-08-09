// Created by John Åkerblom 2014-11-18

#ifndef __H3MLIB_CLEANUP_H_DEF__
#define __H3MLIB_CLEANUP_H_DEF__

#include "h3mlib_ctx.h"

#include <utarray.h>

int h3mlib_cleanup(struct H3MLIB_CTX **ctx);

void free_players(union H3M_PLAYER **players);
void free_dyn_pointers(UT_array * dyn_pointers, int free_members);

#endif
