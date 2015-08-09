#if defined _WIN32 && NO_CRT
unsigned char *rand_table;
unsigned char *rand_table_pos;
#else 
	/* warning: ISO C forbids an empty translation unit [-Wpedantic] */
	typedef void empty; 
#endif

