/* Created by John Åkerblom 2014-08-25 */

#ifndef __ADVAPI_RAND_H_DEF__
#define __ADVAPI_RAND_H_DEF__

#if defined _WIN32 && NO_CRT

#include <windows.h>

#define CRYPTACQUIRECONTEXTA CryptAcquireContextA
#define CRYPTGENRANDOM CryptGenRandom
#define CRYPTRELEASECONTEXT CryptReleaseContext

extern unsigned char *rand_table;
extern unsigned char *rand_table_pos;

FORCEINLINE void advapi_rand_init(unsigned int size)
{
        HCRYPTPROV hProvider = 0;

        rand_table = HeapAlloc(GetProcessHeap(), 0, size);
        rand_table_pos = rand_table;

        CRYPTACQUIRECONTEXTA(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT);
        CRYPTGENRANDOM(hProvider, size, rand_table);
        CRYPTRELEASECONTEXT(hProvider, 0);
}

FORCEINLINE void advapi_rand_exit(void)
{
        HeapFree(GetProcessHeap(), 0, rand_table);
        rand_table = NULL;
        rand_table_pos = NULL;
}

FORCEINLINE unsigned char advapi_rand_byte(void)
{
        return *rand_table_pos++;
}

/* Add functions for other random data sizes here as needed */

#endif

#endif

