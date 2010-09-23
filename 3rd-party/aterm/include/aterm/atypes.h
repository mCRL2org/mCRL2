#ifndef ATYPES_H
#define ATYPES_H

#ifdef _MSC_VER
#include <stdint.h>
#else
#include <inttypes.h>
#endif
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

typedef unsigned int ShortHashNumber;

#ifdef AT_64BIT
typedef int64_t MachineWord;
typedef uint64_t HashNumber;
#else
typedef long MachineWord;
typedef unsigned long HashNumber;
#endif

#ifdef AT_64BIT
#define ADDR_TO_SHORT_HNR(a) ((ShortHashNumber)(((((uintptr_t)(char*)(a)) >> 2)&0xffffffff) ^ (((uintptr_t)(char*)(a)) >> 34)))
#else
#define ADDR_TO_SHORT_HNR(a) (((ShortHashNumber)(char*)(a)) >> 2)
#endif /* AT_64BIT */

#define ADDR_TO_HNR(a) (((HashNumber)(a)) >> 2)

#ifdef __cplusplus
}
#endif/* __cplusplus */ 

#endif /* ATYPES_H */
