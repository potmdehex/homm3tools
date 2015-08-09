/*
 * Read in a list of key/value pairs, one per line.
 * Each line can be at most KEYMAXLEN-1 characters long.
 *
 * Each line should be of the form "key=value", where 'value'
 * is assumed to be a long integer.
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

	kp->vals = xrealloc(kp->vals, newsz);
	kp->vptr = kp->vals + oldn;
}

static void
savekey(Keys *kp, char *key)
{
	char *cp, *ep;
	int len, maxchar, minchar;

	if (kp->kptr >= kp->kmax)
		grow(kp);

	maxchar = kp->maxchar;
	minchar = kp->minchar;

	for (ep=0, cp=key; *cp != '\n' && *cp; ++cp)	/* get rightmost '=' */
		if (*cp == '=')
			ep = cp;

	assert(*cp == '\n' && ep);	/* else line too big or malformed */
	*cp = *ep = 0;

	for (cp=key; *cp; ++cp) {
		maxchar = max(*cp, maxchar);
		minchar = min(*cp, minchar);
	}

	len = cp - key;

	kp->maxchar = maxchar;
	kp->minchar = minchar;

	kp->maxlen = max(len, kp->maxlen);
	kp->minlen = min(len, kp->minlen);

	memcpy(cp = aralloc(kp->ap, len+1), key, len+1);
	*kp->kptr++ = cp;
	*kp->vptr++ = atol(ep+1);
}

Keys *
keynew(FILE *fp)
{
	char key[KEYMAXLEN+1];
	Keys *kp;

	kp = xmalloc(sizeof *kp);
	kp->ap = arnew();
	kp->keys = kp->kmax = kp->kptr = kp->vals = 0;
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
	xfree(kp->vals);
	xfree(kp);
}
