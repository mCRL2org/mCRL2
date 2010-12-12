#ifndef ATYPES_H
#define ATYPES_H

#ifdef _MSC_VER
#include <stdint.h>
#else
#include <inttypes.h>
#endif
#include "stddef.h"
#include "abool.h"

#ifndef AT_64BIT
/* Covers gcc, icc, msvc and Solaris cc */
# if defined(__LP64__) || defined(_LP64) || defined(__lp64) || \
     defined(_ADDR64) || defined(__arch64__) || defined(_M_X64) || \
	 defined(_M_IA64) || defined(WIN64)
#  define AT_64BIT
# endif
#endif

#ifdef __cplusplus
extern "C"
{
#endif/* __cplusplus */

typedef size_t ShortHashNumber;
typedef size_t MachineWord;
typedef size_t HashNumber;

/* The largest size_t is used as an indicator that an element does not exist.
 *    This is used as a replacement of a negative number as an indicator of non
 *       existence */

/*  #define NON_EXISTING (size_t)(-1) */
static const size_t ATERM_NON_EXISTING_POSITION=(size_t)(-1);

/* Avoid warnings under windows, by renaming all strdup's into _strdup's, 
   and renaming the _strdup into strdup under other platforms than windows. 
   Furthermore, ssize_t is not defined on windows. */
#if defined(_MSC_VER) || defined(WIN32) || defined(WIN64)
#ifdef WIN64
typedef __int64 ssize_t;
#else
typedef int ssize_t;
#endif
#else
#define _strdup strdup
#endif

#ifdef AT_64BIT
#define ADDR_TO_SHORT_HNR(a) ((ShortHashNumber)(((((MachineWord)(a)) >> 2)&0xffffffff) ^ (((MachineWord)(a)) >> 34)))
#else
#define ADDR_TO_SHORT_HNR(a) (((ShortHashNumber)(a)) >> 2)
#endif /* AT_64BIT */

#define ADDR_TO_HNR(a) (((HashNumber)(a)) >> 2)

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif /* ATYPES_H */
