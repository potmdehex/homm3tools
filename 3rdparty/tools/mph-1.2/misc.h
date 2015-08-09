#ifndef MISC_H
#define MISC_H

#ifndef max
#define max(a, b) ((a) < (b) ? (b) : (a))
#endif

#ifndef min
#define min(a, b) ((a) < (b) ? (a) : (b))
#endif

extern void	*xmalloc(size_t);
extern void	*xrealloc(void *, size_t);
extern void	*xcalloc(size_t, size_t);
extern void	xfree(void *);

#endif
