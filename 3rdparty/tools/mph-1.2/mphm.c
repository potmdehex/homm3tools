/*
 * Useful to find the minimum 'maxlen' we can pass to mph's -m option.
 * Used by mphminm (which see).
 *
 * This program is only meant for small keysets - it is quite
 * time and space inefficient. Besides, minimizing 'maxlen' is
 * only useful for small keysets anyway (see mph.doc).
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "misc.h"
#include "arena.h"
#include "keys.h"

int m=0;

struct keymax {
	int	index;		/* input word index */
	char	**eclass;	/* equivalence classes */
	int	*slot;		/* next empty slot in equivalence class */
};

#define uchar unsigned char

int
charcomp(const void *a, const void *b)
{
	uchar aa = *(uchar *)a, bb = *(uchar *)b;

	/*
 	 * NULLs compare larger than every other character.
 	 */
	if (aa == bb)
		return 0;
	else if (!aa)
		return 1;
	else if (!bb)
		return -1;
	else
		return aa-bb;
}

/*
 * Compare first equivalence class of each word.
 */
int
keycomp(const void *a, const void *b)
{
	struct keymax *aa = (struct keymax *)a, *bb = (struct keymax *)b;
	return strcmp(aa->eclass[0], bb->eclass[0]);
}

void
usage(void)
{
	fprintf(stderr, "usage: mphm -m maxlen\n");
	exit(2);
}

void
getargs(int argc, char **argv)
{
	int opt;
	extern char *optarg;
	extern int optind;

	while ((opt = getopt(argc, argv, "m:")) != -1)
		switch (opt) {
		case 'm':
			m = atoi(optarg);
			break;
		case '?':
			usage();
			break;
		}

	if (m <= 0 || optind != argc)
		usage();
}

int
main(int argc, char **argv)
{
	Keys *kp;
	struct keymax *kmp;
	char **key, *cp;
	int i, j, k, n, mm;

	getargs(argc, argv);
	kp = keynew(stdin);
	n = keynum(kp);
	mm = 1 + (kp->maxlen + kp->maxlen - 1)/m;
	kmp = xmalloc(sizeof *kmp * n);

	for (i=0, key=kp->keys; i<n; i++, key++) {

		/*
		 * the callocs() are necessary - leave them alone
		 */
		kmp[i].index = i;
		kmp[i].slot = xcalloc(1, sizeof *kmp[i].slot * m);
		kmp[i].eclass = xmalloc(sizeof *kmp[i].eclass * m);
		for (j=0; j<m; j++)
			kmp[i].eclass[j] =
				xcalloc(1, sizeof *kmp[i].eclass[j] * mm);

		/*
		 * create character position equivalence classes
		 */
		for (j=0, cp=*key; *cp; cp++) {
			kmp[i].eclass[j][kmp[i].slot[j]++] = *cp;
			if (++j == m)
				j = 0;
		}

		/*
		 * sort characters in each equivalence class - this
		 * allows us to use strcmp() to test for equivalence
		 * class equality
		 */
		for (j=0; j<m; j++)
			qsort(kmp[i].eclass[j], kmp[i].slot[j],
				sizeof *kmp[i].eclass[j], charcomp);
	}

	/*
	 * optimization for comparison loop below
	 */
	qsort(kmp, n, sizeof *kmp, keycomp);

	/* 
	 * potentially O(n^2), but above qsort(kmp, ..) helps reduce this
	 */
	for (i=0; i<n; i++)

		for (j=i+1; j<n; j++) {

			/*
			 * compare corresponding equivalence classes - break
			 * on first non-match
			 */
			for (k=0; k<m; k++)
				if (strcmp(kmp[i].eclass[k], kmp[j].eclass[k]))
					break;

			/*
			 * optimization courtesy of qsort(kmp, ..)
			 */
			if (k == 0)
				break;

			/*
			 * all equivalence classes match -> conflict
			 */
			if (k == m) {
				fprintf(stderr, "mphm: %d %d conflict\n",
					kmp[i].index+1, kmp[j].index+1);
				return 1;
			}
		}

	return 0;
}
