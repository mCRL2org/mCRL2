// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_init.h
/// \brief Initialization of the ATerm Library.

#ifndef MCRL2_ATERMPP_ATERM_INIT_H
#define MCRL2_ATERMPP_ATERM_INIT_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{
/// \deprecated
/// \brief This function is no longer needed. It will be removed after the next release.
inline
void aterm_init(const aterm&)
{}

} // namespace atermpp

/// \deprecated
/// \brief These macros are no longer needed. They will be removed after the next release.
#define MCRL2_ATERMPP_INIT_(argc, argv, bottom)
#define MCRL2_ATERMPP_INIT(argc, argv)
#define MCRL2_ATERMPP_INIT_DEBUG(argc, argv)

#endif // MCRL2_ATERMPP_ATERM_INIT_H
