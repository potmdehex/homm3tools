#include <stdio.h>

extern int hash(const char *);

int
main()
{
	char word[4096+1];

	while (gets(word))	/* XXX gets() */
		printf("%s=%d\n", word, hash(word));

	return 0;
}
