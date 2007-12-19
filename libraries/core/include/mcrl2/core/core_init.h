// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/core/core_init.h
/// \brief Add your file description here.

#ifndef MCRL2_CORE_CORE_INIT_H
#define MCRL2_CORE_CORE_INIT_H

#include "mcrl2/atermpp/aterm.h"

namespace lps
{
/// Define a macro to hide the initialization statements of the Core Library.
#define MCRL2_CORE_LIBRARY_INIT()       \
  atermpp::aterm bottom_of_stack;       \
  atermpp::aterm_init(bottom_of_stack); \
  gsEnableConstructorFunctions();

} // namespace lps

#endif // MCRL2_CORE_CORE_INIT_H
