// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lps2pbes_sat.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPS2PBES_SAT_H
#define MCRL2_PBES_DETAIL_LPS2PBES_SAT_H

#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/data/detail/find.h"
#include "mcrl2/modal_formula/replace.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename TermTraits>
pbes_expression Sat(const lps::multi_action& a,
                    const action_formulas::action_formula& x,
                    data::set_identifier_generator& id_generator,
                    TermTraits tr
                   );

template <typename Derived, typename TermTraits>
struct sat_traverser: public action_formulas::action_formula_traverser<Derived>
{
  typedef action_formulas::action_formula_traverser<Derived> super;
  typedef TermTraits tr;

  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  const lps::multi_action& a;
  data::set_identifier_generator& id_generator;
  std::vector<pbes_expression> result_stack;

  sat_traverser(const lps::multi_action& a_, data::set_identifier_generator& id_generator_, TermTraits)
    : a(a_), id_generator(id_generator_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void push(const pbes_expression& x)
  {
    result_stack.push_back(x);
  }

  const pbes_expression& top() const
  {
    return result_stack.back();
  }

  pbes_expression pop()
  {
    pbes_expression result = top();
    result_stack.pop_back();
    return result;
  }

  void leave(const data::data_expression& x)
  {
    push(x);
  }

  void leave(const action_formulas::multi_action& x)
  {
    push(lps::equal_multi_actions(a, lps::multi_action(x.actions())));
  }

  void leave(const action_formulas::true_&)
  {
    push(true_());
  }

  void leave(const action_formulas::false_&)
  {
    push(false_());
  }

  void operator()(const action_formulas::not_& x)
  {
    push(tr::not_(Sat(a, x.operand(), id_generator, TermTraits())));
  }

  void leave(const action_formulas::and_&)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(tr::and_(left, right));
  }

  void leave(const action_formulas::or_&)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(tr::or_(left, right));
  }

  void leave(const action_formulas::imp&)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(tr::imp(left, right));
  }

  void operator()(const action_formulas::forall& x)
  {
    data::mutable_map_substitution<> sigma_x = pbes_system::detail::make_fresh_variables(x.variables(), id_generator, false);
    std::set<data::variable> sigma_x_variables = data::substitution_variables(sigma_x);
    action_formulas::action_formula alpha = x.body();
    data::variable_list y = data::replace_variables(x.variables(), sigma_x);
    push(tr::forall(y, Sat(a, action_formulas::replace_variables_capture_avoiding(alpha, sigma_x, sigma_x_variables), id_generator, TermTraits())));
  }

  void operator()(const action_formulas::exists& x)
  {
    data::mutable_map_substitution<> sigma_x = pbes_system::detail::make_fresh_variables(x.variables(), id_generator, false);
    std::set<data::variable> sigma_x_variables = data::substitution_variables(sigma_x);
    action_formulas::action_formula alpha = x.body();
    data::variable_list y = data::replace_variables(x.variables(), sigma_x);
    push(tr::exists(y, Sat(a, action_formulas::replace_variables_capture_avoiding(alpha, sigma_x, sigma_x_variables), id_generator, TermTraits())));
  }

  void operator()(const action_formulas::at& x)
  {
    data::data_expression t = a.time();
    action_formulas::action_formula alpha = x.operand();
    data::data_expression u = x.time_stamp();
    push(tr::and_(Sat(a, alpha, id_generator, TermTraits()), data::equal_to(t, u)));
  }
};

template <template <class, class> class Traverser, typename TermTraits>
struct apply_sat_traverser: public Traverser<apply_sat_traverser<Traverser, TermTraits>, TermTraits>
{
  typedef Traverser<apply_sat_traverser<Traverser, TermTraits>, TermTraits> super;
  using super::enter;
  using super::leave;
  using super::operator();
  using super::top;

  apply_sat_traverser(const lps::multi_action& a, data::set_identifier_generator& id_generator, TermTraits tr)
    : super(a, id_generator, tr)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <typename TermTraits>
pbes_expression Sat(const lps::multi_action& a,
                    const action_formulas::action_formula& x,
                    data::set_identifier_generator& id_generator,
                    TermTraits tr
                   )
{
  apply_sat_traverser<sat_traverser, TermTraits> f(a, id_generator, tr);
  f(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_SAT_H
