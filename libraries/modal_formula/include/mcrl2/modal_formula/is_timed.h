// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/is_timed.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_IS_TIMED_H
#define MCRL2_MODAL_FORMULA_IS_TIMED_H

#include "mcrl2/modal_formula/traverser.h"

namespace mcrl2 {

namespace state_formulas {

namespace detail {

/// \brief Function that determines if a state formula is time dependent
// \brief Visitor for checking if a state formula is timed.
struct is_timed_traverser: public state_formula_traverser<is_timed_traverser>
{
  typedef state_formula_traverser<is_timed_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result;

  is_timed_traverser()
    : result(false)
  {}

  void enter(const delay_timed& /* x */)
  {
    result = true;
  }

  void enter(const yaled_timed& /* x */)
  {
    result = true;
  }

  void enter(const action_formulas::at& /* x */)
  {
    result = true;
  }
};

} // namespace detail

/// \brief Checks if a state formula is timed
/// \param x A state formula
/// \return True if a state formula is timed
inline
bool is_timed(const state_formula& x)
{
  detail::is_timed_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_IS_TIMED_H
