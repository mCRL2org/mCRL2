// Author(s): Sjoerd Cranen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file libraries/utilities/include/mcrl2/utilities/workarounds.h
/// \brief A file with workarounds for different compilers.


#ifndef WORKAROUNDS_H__
#define WORKAROUNDS_H__

#ifndef GL_BGRA
#  define GL_BGRA 0x80E1
#endif
#ifndef GL_UNSIGNED_INT_8_8_8_8_REV
#  define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#endif
#ifndef GL_FOG_COORD_SRC
#  define GL_FOG_COORD_SRC 0x8450
#endif
#ifndef GL_FRAGMENT_DEPTH
#  define GL_FRAGMENT_DEPTH 0x8452
#endif
#ifndef GL_CLAMP_TO_EDGE
#  define GL_CLAMP_TO_EDGE 0x812F
#endif

/*
Workaround for compilers that do not support C++11 nullptr. Especially,
GCC 4.4 and older do not yet have this keyword. File is included by passing
the -include flag on the command line.
*/
#ifndef MCRL2_HAVE_NULLPTR
#ifndef __cplusplus
#define nullptr ((void*)0)
#elif defined(__GNUC__)
#define nullptr __null
#elif defined(_WIN64)
#define nullptr 0LL
#else
#define nullptr 0L
#endif
#endif // defined(HAVE_NULLPTR)

/*
Workaround for compilers that do not support C++11 noexcept.
*/
#ifdef MCRL2_HAVE_NOEXCEPT
#define NOEXCEPT noexcept
#else
#define NOEXCEPT throw()
#endif // defined(HAVE_NOEXCEPT)

#endif // WORKAROUNDS_H__
