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
         /* Note: actually gcc seems to also supports this syntax. */
		#define DLL_PUBLIC __declspec(dllexport)
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

#endif
