// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/print.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_PRINT_H
#define MCRL2_MODAL_FORMULA_PRINT_H

#include "mcrl2/core/print.h"

namespace mcrl2 {

namespace state_formulas {

/// \brief Pretty prints a term.
/// \param[in] t A term
template <typename T>
std::string pp(const T& t)
{
  return core::pp(t);
}

} // namespace state_formulas

namespace action_formulas {

/// \brief Pretty prints a term.
/// \param[in] t A term
template <typename T>
std::string pp(const T& t)
{
  return core::pp(t);
}

} // namespace action_formulas

namespace regular_formulas {

/// \brief Pretty prints a term.
/// \param[in] t A term
template <typename T>
std::string pp(const T& t)
{
  return core::pp(t);
}

} // namespace regular_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_PRINT_H

