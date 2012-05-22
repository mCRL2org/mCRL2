#ifndef ATYPES_H
#define ATYPES_H

#include <cstddef>
#include "mcrl2/atermpp/detail/architecture.h"

namespace atermpp
{

// typedef size_t ShortHashNumber;
// typedef size_t MachineWord;
typedef size_t HashNumber;
/* Although atypes.h defines MachineWord, it wasn't used here:
 *    typedef unsigned long header_type; */
// typedef MachineWord header_type;


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

/* #ifdef AT_64BIT
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
*/

static const size_t MAX_HEADER_BITS = 64;

inline
size_t AT_TABLE_SIZE(const size_t table_class)
{
  return (size_t)1<<(table_class);
}

inline
size_t AT_TABLE_MASK(const size_t table_class)
{
  return AT_TABLE_SIZE(table_class)-1;
}

/* Integers in BAF are always exactly 32 bits.  The size must be fixed so that
 *  * BAF terms can be exchanged between platforms. */
static const size_t INT_SIZE_IN_BAF = 32;

} // namespace atermpp

#endif /* ATYPES_H */
