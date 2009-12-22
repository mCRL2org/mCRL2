// Copyright (c) 2007, 2009 University of Twente
// Copyright (c) 2007, 2009 Michael Weber <michaelw@cs.utwente.nl>
// Copyright (c) 2009 Maks Verver <maksverver@geocities.com>
// Copyright (c) 2009 Eindhoven University of Technology
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef COMPATIBILITY_H_INCLUDED
#define COMPATIBILITY_H_INCLUDED

#include <boost/cstdint.hpp>

/*! \file Compatibility.h
    \brief Compatibility functions.

    This file declares functions that are used to achieve compatibility with
    platforms/compilers that do not support the POSIX and/or C99 standards.

    Contrary to common conventions, this file should be included last in source
    files that require it, so any platform-specific definitions can take
    precedence.
*/


#include <stdlib.h>

/* Exact-width integer types; normally defined in stdint.h (C99).

   NOTE: this assumes 16-bit shorts, 32-bit integers, 64-bit long longs,
         which is true on IA-32 and x84_64 architectures.
*/

#ifndef __int8_t_defined
#define __int8_t_defined
typedef signed char             int8_t;
typedef signed short int        int16_t;
typedef signed int              int32_t;
typedef signed long long int    int64_t;
#endif

#ifndef _STDINT_H
typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;
typedef unsigned long long int  uint64_t;
#endif


/* Case-insensitive string comparison functions.
   redefined here to support non-POSIX platforms. */
int compat_strcasecmp(const char *s1, const char *s2);
int compat_strncasecmp(const char *s1, const char *s2, size_t n);

#endif /* ndef COMPATIBILITY_H_INCLUDED */
