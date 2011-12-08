#ifndef ATYPES_H
#define ATYPES_H

#include <cstddef>
#include "mcrl2/aterm/architecture.h"

namespace aterm
{

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

#ifdef AT_64BIT
inline
ShortHashNumber ADDR_TO_SHORT_HNR(const void* a)
{
  return (ShortHashNumber)(((((MachineWord)(a)) >> 2)&0xffffffff) ^(((MachineWord)(a)) >> 34));
}
#endif// AT_64BIT

#ifdef AT_32BIT
inline
ShortHashNumber ADDR_TO_SHORT_HNR(const void* a)
{
  return ((ShortHashNumber)(a)) >> 2;
}
#endif // AT_32BIT

inline
HashNumber ADDR_TO_HNR(const void* a)
{
  return ((HashNumber)(a)) >> 2;
}


} // namespace aterm

#endif /* ATYPES_H */
