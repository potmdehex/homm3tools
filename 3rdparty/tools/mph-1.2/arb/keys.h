#ifndef KEYS_H
#define KEYS_H

#define KEYMAXCHAR	255	/* 8-bit ASCII */

#ifndef KEYMAXLEN		/* allow override */
#define KEYMAXLEN	4096
#endif

typedef struct Keys	Keys;

struct Keys {
	char	**keys, **kmax, **kptr;
	long	*vals, *vptr;
	int	minlen, maxlen;
	int	minchar, maxchar;
	Arena	*ap;
};

extern Keys	*keynew(FILE *);
extern void	keyfree(Keys *);
extern int	keynum(Keys *);
extern long	keyvalue(Keys *, int);

#define keynum(kp)	(kp->kptr - kp->keys)
#define keyvalue(kp, i)	(kp->vals[i])		/* lvalue */

#endif
