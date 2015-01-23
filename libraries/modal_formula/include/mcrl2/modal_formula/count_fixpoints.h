// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/modal_formula/count_fixpoints.h
/// \brief add your file description here.

#ifndef MCRL2_MODAL_FORMULA_COUNT_FIXPOINTS_H
#define MCRL2_MODAL_FORMULA_COUNT_FIXPOINTS_H

#include "mcrl2/modal_formula/state_formula.h"

namespace mcrl2 {

namespace state_formulas {

/// \cond INTERNAL_DOCS
//
/// \brief Function that counts the number of fixpoints in a state formula
namespace detail {

struct count_fixpoints_traverser: public state_formula_traverser<count_fixpoints_traverser>
{
  typedef state_formula_traverser<count_fixpoints_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  std::size_t result;

  count_fixpoints_traverser()
    : result(0)
  {}

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

} // namespace state_formulas

} // namespace mcrl2

#endif // MCRL2_MODAL_FORMULA_COUNT_FIXPOINTS_H
