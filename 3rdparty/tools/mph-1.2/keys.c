/*
 * Read in a list of keys, one per line.
 * Each line can be at most KEYMAXLEN-1 characters long.
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "misc.h"
#include "arena.h"
#include "keys.h"

#undef NDEBUG
#include <assert.h>

static void
grow(Keys *kp)
{
	size_t oldn, newn, newsz;

	oldn = kp->kmax - kp->keys;
	assert(oldn < ULONG_MAX/(2 * sizeof(char *)));
	newn = max(32, oldn*2);
	newsz = sizeof(char *) * newn;
	kp->keys = xrealloc(kp->keys, newsz);
	kp->kmax = kp->keys + newn;
	kp->kptr = kp->keys + oldn;
}

static void
savekey(Keys *kp, char *key)
{
	char *cp;
	int len, maxchar, minchar;

	if (kp->kptr >= kp->kmax)
		grow(kp);

	maxchar = kp->maxchar;
	minchar = kp->minchar;

	for (cp=key; *cp != '\n' && *cp; ++cp) {
		maxchar = max(*cp, maxchar);
		minchar = min(*cp, minchar);
	}

	assert(*cp == '\n');	/* else key too big */
	*cp = 0;
	len = cp - key;

	kp->maxchar = maxchar;
	kp->minchar = minchar;

	kp->maxlen = max(len, kp->maxlen);
	kp->minlen = min(len, kp->minlen);

	memcpy(cp = aralloc(kp->ap, len+1), key, len+1);
	*kp->kptr++ = cp;
}

Keys *
keynew(FILE *fp)
{
	char key[KEYMAXLEN+1];
	Keys *kp;

	kp = xmalloc(sizeof *kp);
	kp->ap = arnew();
	kp->keys = kp->kmax = kp->kptr = 0;
	kp->maxchar = 0;
	kp->minchar = KEYMAXCHAR;
	kp->minlen = KEYMAXLEN;
	kp->maxlen = 0;

	while (fgets(key, sizeof key, fp))
		savekey(kp, key);

	return kp;
}

void
keyfree(Keys *kp)
{
	if (!kp)
		return;
	arfree(kp->ap);
	xfree(kp->keys);
	xfree(kp);
}
