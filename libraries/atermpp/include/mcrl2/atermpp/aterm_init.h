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

namespace atermpp
{
  /// \brief Initialize the ATerm++ Library. The specified argument t is used to mark the
  /// the bottom of the program stack.
  /// \param bottom_of_stack The bottom of stack (or a good approximation of it).
  /// All aterms in the range [bottom_of_stack,...[ will not be garbage collected.
  inline
  void aterm_init(const aterm& bottom_of_stack)
  {
    ATerm a = bottom_of_stack;
    ATinit(0, 0, &a);
  }

} // namespace atermpp

// \cond INTERNAL_DOCS
#if defined(_MSC_VER) || defined(__MINGW32__)
# define MCRL2_ATERMPP_INIT_(argc, argv, bottom) \
  ATinit(0, 0, reinterpret_cast< ATerm* >(&bottom));
#else
#define MCRL2_ATERMPP_INIT_(argc, argv, bottom) \
  ATinit(argc, argv, reinterpret_cast< ATerm* >(bottom));
#endif //defined(_MSC_VER) || defined(__MINGW32__)
// \endcond

/// \brief MCRL2_ATERMPP_INIT(argc, argv) initialises the ATerm library using
/// one of the parameters as the bottom of the stack. The parameter that is
/// actually depends on the platform:
/// - &argv on Windows platforms
/// - argv on non-Windows platforms
# define MCRL2_ATERMPP_INIT(argc, argv) \
  MCRL2_ATERMPP_INIT_(argc, argv, argv);

/// MCRL2_ATERMPP_INIT_DEBUG(argc, argv) initialises the ATerm library with
///  MCRL2_ATERMPP_INIT(argc,argv) and activates debugging checks
/// \see MCRL2_ATERMPP_INIT(argc, argv)
#if defined(NDEBUG)
# define MCRL2_ATERMPP_INIT_DEBUG(argc, argv)\
  MCRL2_ATERMPP_INIT_(argc, argv, argv)
#else
# define MCRL2_ATERMPP_INIT_DEBUG(argc, argv)\
  MCRL2_ATERMPP_INIT_(argc, argv, argv)\
  ATsetChecking(ATtrue);
#endif
/// MCRL2_ATERMPP_INIT_VERBOSE(argc, argv) initialises the ATerm library with
/// MCRL2_ATERMPP_INIT(argc,argv) and activates additional messages
/// \see MCRL2_ATERMPP_INIT(argc, argv)
#define MCRL2_ATERMPP_INIT_VERBOSE(argc, argv)\
  char* debug_args[3] = { "" , "-at-verbose" , "-at-print-gc-info" }; \
  MCRL2_ATERMPP_INIT_(3, debug_args, argv)\

#include "mcrl2/atermpp/aterm_make_match.h"

#endif // MCRL2_ATERMPP_ATERM_INIT_H
