// Author(s): Wieger Wesselink, Aad Mathijssen
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

#define MCRL2_ATERMPP_INIT(argc, argv) atermpp::aterm_init();

namespace atermpp
{
/// \brief Initialize the ATerm++ Library. The specified argument t is used to mark the
/// the bottom of the program stack.

inline
void aterm_init();

} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_INIT_H
