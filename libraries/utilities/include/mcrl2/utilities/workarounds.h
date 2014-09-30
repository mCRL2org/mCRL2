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
