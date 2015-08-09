/*
 * d=2
 * n=38
 * m=36
 * c=1.05
 * maxlen=1
 * minklen=2
 * maxklen=9
 * minchar=97
 * maxchar=121
 * loop=0
 * numiter=34603
 * seed=
 */

static int g[] = {
	9, 16, 30, 30, 26, 34, 33, 23, 18, 19,
	9, 11, 23, 33, 31, 29, 20, 6, 24, 32,
	21, 21, 29, 11, 18, -1, 32, 34, 16, 17,
	25, 27, 8, 29, 11, 15, 11, 0, 
};

static int T0[] = {
	31, 11, 4, 16, 13, 29, 26, 13, 36, 4,
	2, 27, 14, 7, 26, 3, 6, 18, 30, 1,
	16, 26, 34, 14, 37, 
};

static int T1[] = {
	37, 23, 5, 0, 14, 34, 34, 1, 33, 24,
	31, 10, 2, 29, 1, 8, 21, 29, 20, 18,
	7, 16, 15, 17, 21, 
};

#define uchar unsigned char

int
hash(const uchar *key)
{
	unsigned f0, f1;
	const uchar *kp = key;

	for (f0=f1=0; *kp; ++kp) {
		if (*kp < 97 || *kp > 121)
			return -1;
		if (kp-key > 8)
			return -1;
		f0 += T0[-97 + *kp];
		f1 += T1[-97 + *kp];
	}

	if (kp-key < 2)
		return -1;

	f0 %= 38;
	f1 %= 38;

	return (g[f0] + g[f1]) % 36;
}
