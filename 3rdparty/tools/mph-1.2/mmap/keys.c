/*
 * Read in a list of keys, one per line.
 * Requires mmap(2) and getpagesize(2).
 */

#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include "misc.h"
#include "keys.h"

#undef NDEBUG
#include <assert.h>

static void
readkeys(Keys *kp)
{
	char *map, *start, c;
	int minchar, maxchar;
	int minlen, maxlen;
	int numkeys;

	start = map = kp->keys;
	minchar = KEYMAXCHAR;
	maxchar = 0;
	minlen = INT_MAX;
	maxlen = 0;
	numkeys = 0;

	while (*map)
		if ((c = *map++) == '\n') {
			maxlen = max(map-start-1, maxlen);
			minlen = min(map-start-1, minlen);
			++numkeys;
			start = map;
		}
		else {
			maxchar = max(c, maxchar);
			minchar = min(c, minchar);
		}

	kp->minchar = minchar;
	kp->maxchar = maxchar;
	kp->minlen = minlen;
	kp->maxlen = maxlen;
	kp->numkeys = numkeys;
}

Keys *
keynew(int fd)
{
	Keys *kp;
	char *map;
	struct stat st;
	size_t mapsz, pagesz;

	if (fstat(fd, &st) < 0) {
		perror("keynew:fstat");
		return 0;
	}

	pagesz = getpagesize();
	mapsz = st.st_size + 2 * sizeof(unsigned long);
	mapsz = (mapsz + pagesz-1) & ~(pagesz-1);

	map = mmap(NULL, mapsz, PROT_READ, MAP_PRIVATE, fd, 0);
	if ((long)map == -1) {
		perror("keynew:mmap");
		return 0;
	}

	kp = xmalloc(sizeof *kp);
	kp->minchar = KEYMAXCHAR;
	kp->maxchar = 0;
	kp->minlen = INT_MAX;
	kp->maxlen = 0;
	kp->numkeys = 0;
	kp->keys = map;
	kp->mapsz = mapsz;

	if (st.st_size > 0)
		readkeys(kp);

	return kp;
}

void
keyfree(Keys *kp)
{
	if (!kp)
		return;
	(void)munmap(kp->keys, kp->mapsz);
	xfree(kp);
}
