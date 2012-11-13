#ifndef ATYPES_H
#define ATYPES_H

#include <cstddef>

// PRAGMAS BELOW ARE NOT USED IN THE TOOLSET ANYMORE.
// #if !defined(AT_64BIT) && !defined(AT_32BIT)
// #pragma message ( "Missing architecture: Guessing architecture..." )
// #pragma message ( "* Define AT_32BIT for i386 architecture." )
// #pragma message ( "* Define AT_64BIT for x86_x64 architecture." )
// 
// /* Covers gcc, icc, msvc and Solaris cc */  	 	 
// #if defined(__LP64__) || defined(_LP64) || defined(__lp64) ||
//      defined(_ADDR64) || defined(__arch64__) || defined(_M_X64) ||
//      defined(_M_IA64) || defined(WIN64)  	 	 
// #pragma message ( "Detected x86_x64 architecture." )
// #  define AT_64BIT
// #else
// #pragma message ( "Detected i386 architecture." )
// #  define AT_32BIT
// #endif    	 	 
// 
// #endif

namespace atermpp
{

typedef size_t HashNumber;
/* The largest size_t is used as an indicator that an element does not exist.
 *    This is used as a replacement of a negative number as an indicator of non
 *       existence */

static const size_t npos=(size_t)(-1);

/* Avoid warnings under windows, by renaming all strdup's into _strdup's,
   and renaming the _strdup into strdup under other platforms than windows.
   Furthermore, ssize_t is not defined on windows. */
#if defined(_MSC_VER) || defined(WIN32) || defined(WIN64)
#if defined(ssize_t)
#else
#ifndef HAVE_SSIZE_T
#if defined(WIN64) ||  defined(_WIN64) ||  defined(__WIN64__)
/* int64 is not supported by all GCC */
typedef __int64 ssize_t;
#else
#ifndef __MINGW32__
typedef int ssize_t;
#endif
#endif
/* prevent ssize_t redefinitions in other libraries */
#define HAVE_SSIZE_T
#endif

#endif
#else
#define _strdup strdup
#endif

/* Integers in BAF are always exactly 32 or 64 bits.  The size must be fixed so that
 *  * BAF terms can be exchanged between platforms. */
static const size_t INT_SIZE_IN_BAF = 64;

} // namespace atermpp

#endif /* ATYPES_H */
