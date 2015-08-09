/* Created by John Åkerblom 2014-08-25 */

#ifndef __CROSSPLATFORM_H_DEF__
#define __CROSSPLATFORM_H_DEF__

/*
 * Taken from http://gcc.gnu.org/wiki/Visibility.
 * Why something from this site contains a comment about 
 * something gcc "seems" to do only Stallman knows.
 */
#if defined _WIN32 || defined __CYGWIN__
	#ifdef __GNUC__
		#define DLL_PUBLIC __attribute__ ((dllexport))
	#else
		#define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
	#endif
#else
	#if __GNUC__ >= 4
		#define DLL_PUBLIC __attribute__ ((visibility ("default")))
		#define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
	#else
		#define DLL_PUBLIC
		#define DLL_LOCAL
	#endif
#endif

#ifdef __GNUC__
        #define FORCEINLINE __extension__ __inline__
#elif _WIN32
        // Only export for DLL builds
        #ifndef _USRDLL
            #undef DLL_PUBLIC
            #define DLL_PUBLIC 
        #endif

        #include <windows.h>

        #ifndef memset
                #pragma optimize("", off)
                FORCEINLINE void *__memset(void *s, int c, unsigned int n)
                {
                        unsigned int i = 0;

                        for (i = 0; i < n; ++i) {
                                ((unsigned char *)s)[i] = c;
                        }

                        return s;
                }
                #define memset __memset
                #pragma optimize("", on)
        #endif
#endif

#endif

