// Created by John Åkerblom 2014-11-16
#pragma once

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
