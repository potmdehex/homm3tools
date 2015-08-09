/*
 * A (non-aligning) arena allocator.
 * Derived from CII's Arena interface, whose copyright stmt appears below.
 * See http://www.cs.princeton.edu/software/cii/.
 */

/*
 * The author of this software is David R. Hanson.
 *
 * Copyright (c) 1994,1995,1996 by David R. Hanson. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose, subject to the provisions described below, without fee is
 * hereby granted, provided that this entire notice is included in all
 * copies of any software that is or includes a copy or modification of
 * this software and in all copies of the supporting documentation for
 * such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY. IN PARTICULAR, THE AUTHOR DOES MAKE ANY REPRESENTATION OR
 * WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY OF THIS SOFTWARE OR
 * ITS FITNESS FOR ANY PARTICULAR PURPOSE.
 */

#include <stdlib.h>
#include <string.h>
#include "misc.h"
#include "arena.h"

#define ARENAMIN 8192

Arena *
arnew(void)
{
	return xcalloc(1, sizeof(Arena));
}

void
arfree(Arena *ap)
{
	Arena *prev, *pprev;

	for (prev = ap->prev; prev; prev = pprev) {
		pprev = prev->prev;
		xfree(prev);
	}

	xfree(ap);
}

void *
aralloc(Arena *ap, size_t n)
{
	if (ap->avail + n > ap->limit) {
		size_t m = max(n, ARENAMIN);
		Arena *newap = xmalloc(m);
		*newap = *ap;
		ap->avail = (char *)(newap+1);
		ap->limit = (char *)newap + m;
		ap->prev = newap;
	}

	ap->avail += n;
	return ap->avail - n;
}
