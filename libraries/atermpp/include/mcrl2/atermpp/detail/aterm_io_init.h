// Author(s): Wieger Wesselink, Aad Mathijssen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_init.h
/// \brief Initialization of the aterm Library.

#ifndef MCRL2_ATERMPP_ATERM_IO_INIT_H
#define MCRL2_ATERMPP_ATERM_IO_INIT_H

namespace atermpp
{
namespace detail
{
/// \brief Initialize binary i/o on windows machines. 

void aterm_io_init();

} // namespace detail
} // namespace atermpp

#endif // MCRL2_ATERMPP_ATERM_IO_INIT_H
