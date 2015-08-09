// Created by John Åkerblom 2014-11-18

#ifndef __MSVC_COMP_STDINT_H_DEF__
#define __MSVC_COMP_STDINT_H_DEF__

#ifndef _MSC_VER
#include <stdint.h>
#elif _MSC_VER >= 1600
#include <stdint.h>
#else

// From http://msinttypes.googlecode.com/svn/trunk/stdint.h

// Visual Studio 6 and Embedded Visual C++ 4 doesn't
// realize that, e.g. char has the same size as __int8
// so we give up on __intX for them.
#if (_MSC_VER < 1300)
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#else
typedef signed __int8 int8_t;
typedef signed __int16 int16_t;
typedef signed __int32 int32_t;
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
#endif
typedef signed __int64 int64_t;
typedef unsigned __int64 uint64_t;

#endif

#endif
