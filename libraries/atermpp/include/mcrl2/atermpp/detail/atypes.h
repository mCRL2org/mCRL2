// Author(s): Wieger Wesselink, Jan Friso Groote. Based on the aterm library by Paul Klint and others.
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/detail/atypes.h
/// \brief Some elementary constants and workaround. 

#ifndef ATYPES_H
#define ATYPES_H

#include <cstddef>

/* Avoid warnings under windows, by renaming all strdup's into _strdup's,
 * and renaming the _strdup into strdup under other platforms than windows. */
#ifndef _MSC_VER
#define _strdup strdup
#endif

/* ssize_t is not defined on windows, define it here. */
#ifdef _MSC_VER
#if !defined(ssize_t) && !defined(HAVE_SSIZE_T)
#ifdef _WIN64
typedef __int64 ssize_t;
#else
typedef _W64 int ssize_t;
#endif // _WIN64
#define HAVE_SSIZE_T
#endif // !defined(ssize_t) && !defined(HAVE_SSIZE_T)
#endif // _MSC_VER

namespace atermpp
{

/* The largest std::size_t is used as an indicator that an element does not exist.
 *    This is used as a replacement of a negative number as an indicator of non
 *       existence */

static const std::size_t npos=(std::size_t)(-1);

} // namespace atermpp

#endif /* ATYPES_H */
