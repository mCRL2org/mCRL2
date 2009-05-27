// Author(s): Jeroen van der Wulp
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/deprecation.h
/// \brief Function to force garbage collection of the ATerm Library.

#ifndef MCRL2_CORE_DEPRECATION_H__
#define MCRL2_CORE_DEPRECATION_H__

#if (3 < __GNUC__)
# define DEPRECATED(x) x __attribute__ ((deprecated))
#elif defined(__VISUALC__) && (__VISUALC__ >= 1300)
# define DEPRECATED(x) __declspec(deprecated) x
#else
# define DEPRECATED(x) x
#endif

#endif // MCRL2_CORE_DEPRECATION_H__
