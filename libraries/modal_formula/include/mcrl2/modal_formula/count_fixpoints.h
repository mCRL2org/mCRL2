// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/count_fixpoints.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_COUNT_FIXPOINTS_H
#define MCRL2_MODAL_FORMULA_COUNT_FIXPOINTS_H

#include "mcrl2/modal_formula/traverser.h"

namespace mcrl2::state_formulas
{

/// \cond INTERNAL_DOCS
//
/// \brief Function that counts the number of fixpoints in a state formula
namespace detail {

struct count_fixpoints_traverser: public state_formula_traverser<count_fixpoints_traverser>
{
  using super = state_formula_traverser<count_fixpoints_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  std::size_t result = 0;

  void enter(const mu& /* x */)
  {
    result++;
  }

  void enter(const nu& /* x */)
  {
    result++;
  }
};

} // namespace detail
/// \endcond

/// \brief Counts the number of fixpoints in a state formula
/// \param x A state formula
/// \return The number of fixpoints in a state formula
inline
std::size_t count_fixpoints(const state_formula& x)
{
  detail::count_fixpoints_traverser f;
  f.apply(x);
  return f.result;
}

} // namespace mcrl2::state_formulas

#endif // MCRL2_MODAL_FORMULA_COUNT_FIXPOINTS_H
