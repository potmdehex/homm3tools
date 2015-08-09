/*
 * notation:
 *	W = set of keys (from stdin)
 *	m = |W|
 *	n = c*m for some constant c>1
 *	d = 2 or 3
 *	G = (V, E) where |V|=n, |E|=m and |e|=d for e in E
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>
#include <math.h>
#include "misc.h"
#include "arena.h"
#include "keys.h"
#include "graph.h"

#undef NDEBUG
#include <assert.h>

#define ALPHASZ	(KEYMAXCHAR+1)

static struct {
	int	d;
	double	c;
	int	n, m;
} gl = { 3, 1.23 };

int	maxlen=INT_MAX;
int	seed, sflag=0;
int	loop=0, numiter;
int	emitbinary=1;
int	*T[3], tsize;
Graph	*gp;
Keys	*kp;

void	(*ff)(const char *key, int []);

void
emitint(int i)
{
	if (emitbinary)
		fwrite(&i, sizeof i, 1, stdout);
	else
		printf("%d\n", i);
}

void
emitfloat(double f)
{
	if (emitbinary)
		fwrite(&f, sizeof f, 1, stdout);
	else
		printf("%f\n", f);
}

/*
 * Emit a language independent rep of the hash fn.
 */
void
emit(void)
{
	int i, j, t;

	printf("%d\n", emitbinary);

	emitint(gl.d);
	emitint(gl.n);
	emitint(gl.m);
	emitfloat(gl.c);
	emitint(maxlen);
	emitint(kp->minlen);
	emitint(kp->maxlen);
	emitint(kp->minchar);
	emitint(kp->maxchar);
	emitint(loop);
	emitint(numiter+1);
	emitint(sflag);
	emitint(seed);

	for (i=0; i<gl.n; i++)
		emitint(grnodevalue(gp, i));
	
	for (t=0; t<gl.d; t++)
		for (i=0; i<tsize; i += ALPHASZ)
			for (j=0; j<ALPHASZ; j++)
				emitint(T[t][i+j]);
}

void
ff2(const char *key, int f[])
{
	int i;
	unsigned v1, v2;

	for (i=v1=v2=0; *key; ++key) {
		v1 += T[0][i + *(unsigned char *)key];
		v2 += T[1][i + *(unsigned char *)key];
		i += ALPHASZ;
		if (i >= tsize)
			i = 0;
	}

	v1 %= gl.n;
	v2 %= gl.n;

	if (loop && (v2 == v1) && (++v2 >= gl.n))
		v2 = 0;

	f[0] = v1;
	f[1] = v2;
}

void
ff3(const char *key, int f[])
{
	int i;
	unsigned v1, v2, v3;

	for (i=v1=v2=v3=0; *key; ++key) {
		v1 += T[0][i + *(unsigned char *)key];
		v2 += T[1][i + *(unsigned char *)key];
		v3 += T[2][i + *(unsigned char *)key];
		i += ALPHASZ;
		if (i >= tsize)
			i = 0;
	}

	v1 %= gl.n;
	v2 %= gl.n;
	v3 %= gl.n;

	/*
	 * Reduce probability of loops - from Majewski's PERFES.
	 */
	if (loop) {
		if (v2 == v1 && ++v2 >= gl.n)
			v2 = 0;
		if (v3 == v1 && ++v3 >= gl.n)
			v3 = 0;
		if (v2 == v3) {
			if (++v3 >= gl.n)
				v3 = 0;
			if (v3 == v1 && ++v3 >= gl.n)
				v3 = 0;
		}
	}

	f[0] = v1;
	f[1] = v2;
	f[2] = v3;
}

void
verify(void)
{
	int i, j, h;
	int f[3];
	char **key;

	for (i=0, key=kp->keys; i<gl.m; ++i, ++key) {
		ff(*key, f);
		for (j=h=0; j<gl.d; j++)
			h += grnodevalue(gp, f[j]);
		assert(h % gl.m == i);
	}
}

void
assign(void)
{
	int v, e, unsetv;
	int i, j, sum;

	for (v=0; v<gl.n; v++)
		grnodevalue(gp, v) = -1;

	for (i=0; i<gl.m; i++) {
		for (j=sum=0, e = grstpop(gp); j<gl.d; j++) {
			if (grnodevalue(gp, v = gredgenode(gp, e, j)) < 0)
				grnodevalue(gp, unsetv = v) = 0;
			else
				sum += grnodevalue(gp, v);
		}
		grnodevalue(gp, unsetv) = (gredgevalue(gp, e) - sum) % gl.m;
		if (grnodevalue(gp, unsetv) < 0)
			grnodevalue(gp, unsetv) += gl.m;
	}
}

void
gentables(void)
{
	int i, j, t;

	for (t=0; t<gl.d; t++)
		for (i=0; i<tsize; i += ALPHASZ)
			for (j=0; j<ALPHASZ; j++)
				T[t][i+j] = random() % gl.n;
}

int
hasloop(int f[])
{
	return f[0] == f[1] || (gl.d == 3 && (f[0] == f[2] || f[1] == f[2]));
}

int
genedges(void)
{
	int e, j, f[3];
	char **key;

	grdeledges(gp);

	for (e=0, key=kp->keys; e<gl.m; ++e, ++key) {
		ff(*key, f);
		if (hasloop(f)) {
			fprintf(stderr, "mph: loop\n");
			return -1;
		}
		for (j=0; j<gl.d; j++)
			gredgenode(gp, e, j) = f[j];
		graddedge(gp, e);
	}

	return 0;
}

void
map(void)
{
	for (numiter=0; ; ++numiter) {
		gentables();
		if (genedges() < 0)
			continue;
		if (!grcyclic(gp))
			break;
		fprintf(stderr, "mph: cycle\n");
	}
}

void
memalloc(void)
{
	int t;

	for (t=0; t<gl.d; t++)
		T[t] = xmalloc(tsize * sizeof(int));
	gp = grnew(gl.n, gl.m, gl.d);
}

void
memfree(void)
{
	int t;

	grfree(gp);
	keyfree(kp);

	for (t=0; t<gl.d; t++)
		xfree(T[t]);
}

void
usage(void)
{
	fprintf(stderr, "usage: mph [-a] [-m maxlen] [-l] [-d (2|3)] "
			 "[-c x.y] [-s|-S seed]\n");
	exit(1);
}

void
readkeys(void)
{
	kp = keynew(stdin);
	gl.m = keynum(kp);
	gl.n = ceil(gl.c * gl.m);
	maxlen = min(kp->maxlen, maxlen);
	tsize = maxlen * ALPHASZ;
}

void
getargs(int argc, char **argv)
{
	int opt, cflag=0;
	extern char *optarg;

	while ((opt = getopt(argc, argv, "alsm:d:c:S:")) != -1)
		switch (opt) {
		case 'a':
			emitbinary = 0;
			break;
		case 'l':
			loop = 1;
			break;
		case 'S':
			if (sflag++)
				usage();
			srandom(seed = atoi(optarg));
			break;
		case 's':
			if (sflag++)
				usage();
			srandom(seed = time(0));
			break;
		case 'd':
			gl.d = atoi(optarg);
			if (cflag || (gl.d != 2 && gl.d != 3))
				usage();
			if (gl.d == 2)
				gl.c = 2.09;
			break;
		case 'c':
			gl.c = atof(optarg);
			cflag++;
			break;
		case 'm':
			maxlen = atoi(optarg);
			break;
		case '?':
			usage();
			break;
		}

	if (gl.d == 2)
		ff = ff2;
	else
		ff = ff3;
}

int
main(int argc, char **argv)
{
	getargs(argc, argv);
	readkeys();
	memalloc();
	map();
	assign();
	verify();
	emit();

	return 0;
}
