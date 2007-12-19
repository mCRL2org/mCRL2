// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/atermpp/aterm_init.h
/// \brief Add your file description here.

#ifndef MCRL2_ATERMPP_ATERM_INIT_H
#define MCRL2_ATERMPP_ATERM_INIT_H

#include "mcrl2/atermpp/aterm.h"

namespace atermpp
{
  /// Initialize the ATerm++ Library. The specified argument t is used to mark the
  /// the bottom of the program stack. All aterms in the range [bottom_of_stack,...[
  /// will not be garbage collected.
  /// 
  inline
  void aterm_init(const aterm& bottom_of_stack)
  {
    ATerm a = bottom_of_stack;
    ATinit(0, 0, &a);
  }

/// Define a macro to hide the initialization statements of the ATerm Library.
#define ATERM_LIBRARY_INIT()   \
  aterm bottom_of_stack;       \
  aterm_init(bottom_of_stack);

} // namespace atermpp

#include "mcrl2/atermpp/aterm_make_match.h"

#endif // MCRL2_ATERMPP_ATERM_INIT_H
