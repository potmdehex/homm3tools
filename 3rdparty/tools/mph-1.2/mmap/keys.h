#ifndef KEYS_H
#define KEYS_H

#define KEYMAXCHAR	255	/* 8-bit ASCII */

typedef struct Keys	Keys;

struct Keys {
	char	*keys;
	int	minlen, maxlen;
	int	minchar, maxchar;
	int	numkeys;
	size_t	mapsz;
};

extern Keys	*keynew(int);
extern void	keyfree(Keys *);
extern int	keynum(Keys *);

#define keynum(kp)	(kp->numkeys)

#endif
