// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/find_equalities.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_FIND_EQUALITIES_H
#define MCRL2_PBES_FIND_EQUALITIES_H

#include "mcrl2/data/find_equalities.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <template <class> class Traverser, class Derived>
struct find_equalities_traverser: public data::detail::find_equalities_traverser<Traverser, Derived>
{
  typedef data::detail::find_equalities_traverser<Traverser, Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::push;
  using super::pop;
  using super::top;
  using super::below_top;

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void leave(const and_& x)
  {
    auto& left = below_top();
    auto const& right = top();
    left.join_and(right);
    pop();
  }

  void leave(const or_& x)
  {
    auto& left = below_top();
    auto const& right = top();
    left.join_or(right);
    pop();
  }

  void leave(const imp& x)
  {
    auto& left = below_top();
    auto const& right = top();
    left.swap();
    left.join_or(right);
    pop();
  }

  void leave(const not_& x)
  {
    top().swap();
  }

  void leave(const forall& x)
  {
    top().delete_(x.variables());
  }

  void leave(const exists& x)
  {
    top().delete_(x.variables());
  }

  // N.B. Use apply here, to avoid going into the recursion
  void apply(const propositional_variable_instantiation& x)
  {
    push(data::detail::find_equalities_expression());
  }

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

struct find_equalities_traverser_inst: public pbes_system::detail::find_equalities_traverser<pbes_system::data_expression_traverser, find_equalities_traverser_inst>
{
  typedef pbes_system::detail::find_equalities_traverser<pbes_system::data_expression_traverser, find_equalities_traverser_inst> super;

  using super::enter;
  using super::leave;
  using super::apply;
};

} // namespace detail

inline
std::map<data::variable, std::set<data::data_expression> > find_equalities(const pbes_expression& x)
{
  detail::find_equalities_traverser_inst f;
  f.apply(x);
  assert(f.expression_stack.size() == 1);
  f.top().close();
  return f.top().equalities;
}

inline
std::map<data::variable, std::set<data::data_expression> > find_inequalities(const pbes_expression& x)
{
  detail::find_equalities_traverser_inst f;
  f.apply(x);
//////////////////////////////////////
  if (f.expression_stack.size() != 1)
  {
    throw std::runtime_error("expression_stack error");
  }
//////////////////////////////////////
  assert(f.expression_stack.size() == 1);
  f.top().close();
  return f.top().inequalities;
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_FIND_EQUALITIES_H
