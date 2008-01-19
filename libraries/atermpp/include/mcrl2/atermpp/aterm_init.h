// Author(s): Wieger Wesselink, Aad Mathijssen
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

} // namespace atermpp

/// MCRL2_ATERMPP_INIT(argc, argv) initialises the ATerm library using
/// one of the parameters as the bottom of the stack. The parameter that is
/// actually depends on the platform:
/// - &argv on Windows platforms
/// - argv on non-Windows platforms
#if defined(_MSC_VER) || defined(__MINGW32__)
#define MCRL2_ATERMPP_INIT(argc, argv)\
  ATinit(0, 0, reinterpret_cast<ATerm*>(&argv));
#else
#define MCRL2_ATERMPP_INIT(argc, argv)\
  ATinit(0, 0, reinterpret_cast<ATerm*>(argv));
#endif //defined(_MSC_VER) || defined(__MINGW32__)

/// MCRL2_ATERMPP_INIT_DEBUG(argc, argv) initialises the ATerm library with
/// debugging information enabled, using one of the parameters as the bottom
/// of the stack. The parameter that is actually depends on the platform:
/// - &argv on Windows platforms
/// - argv on non-Windows platforms
#if defined(_MSC_VER) || defined(__MINGW32__)
#define MCRL2_ATERMPP_INIT_DEBUG(argc, argv)\
  char* debug_args[3] = { "" , "-at-verbose" , "-at-print-gc-info" };\
  ATinit(3, debug_args, reinterpret_cast<ATerm*>(&argv));
#else
#define MCRL2_ATERMPP_INIT_DEBUG(argc, argv)\
  char* debug_args[3] = { "" , "-at-verbose" , "-at-print-gc-info" };\
  ATinit(3, debug_args, reinterpret_cast<ATerm*>(argv));
#endif //defined(_MSC_VER) || defined(__MINGW32__)

#include "mcrl2/atermpp/aterm_make_match.h"

#endif // MCRL2_ATERMPP_ATERM_INIT_H
