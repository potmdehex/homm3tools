#ifndef ARENA_H
#define ARENA_H

typedef struct Arena	Arena;

struct Arena {
	Arena	*prev;
	char	*avail;
	char	*limit;
};

extern Arena	*arnew(void);
extern void	arfree(Arena *);
extern void	*aralloc(Arena *, size_t);	/* not aligned */

#endif
