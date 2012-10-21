#ifndef ATYPES_H
#define ATYPES_H

#include <cstddef>
#include "mcrl2/atermpp/detail/architecture.h"

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

static const size_t MAX_HEADER_BITS = 64;

/* Integers in BAF are always exactly 32 bits.  The size must be fixed so that
 *  * BAF terms can be exchanged between platforms. */
static const size_t INT_SIZE_IN_BAF = 32;

} // namespace atermpp

#endif /* ATYPES_H */
