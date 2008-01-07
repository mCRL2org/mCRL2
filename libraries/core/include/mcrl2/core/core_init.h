// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/core_init.h
/// \brief A macro that hides the initialization statements for ATerm and Core Library.
///
/// This macro should be used as the first statement in the main body.

#ifndef MCRL2_CORE_CORE_INIT_H
#define MCRL2_CORE_CORE_INIT_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/utilities/aterm_ext.h"

namespace lps
{
/// MCRL2_CORE_LIBRARY_INIT(p) initialises the following:
/// - the ATerm library using p as the bottom of the stack
/// - the constructor functions used by the Core library
#define MCRL2_CORE_LIBRARY_INIT(p)       \
  MCRL2_ATERM_INIT(p)                    \
  gsEnableConstructorFunctions();

} // namespace lps

#endif // MCRL2_CORE_CORE_INIT_H
