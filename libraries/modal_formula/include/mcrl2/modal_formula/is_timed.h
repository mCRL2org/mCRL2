// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
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

namespace mcrl2::state_formulas
{

namespace detail {

/// \brief Function that determines if a state formula is time dependent
// \brief Visitor for checking if a state formula is timed.
struct is_timed_traverser: public action_label_traverser<is_timed_traverser>
{
  using super = action_label_traverser<is_timed_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  bool result = false;

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

} // namespace mcrl2::state_formulas

#endif // MCRL2_MODAL_FORMULA_IS_TIMED_H
