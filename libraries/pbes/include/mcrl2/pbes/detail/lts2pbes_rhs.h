// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lts2pbes_rhs.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LTS2PBES_RHS_H
#define MCRL2_PBES_DETAIL_LTS2PBES_RHS_H

#include "mcrl2/atermpp/detail/aterm_list_utility.h"
#include "mcrl2/lts/lts_lts.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/detail/lps2pbes_par.h"
#include "mcrl2/pbes/detail/lps2pbes_sat.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"
#include "mcrl2/pbes/detail/lts2pbes_lts.h"
#include "mcrl2/utilities/progress_meter.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

typedef lts::lts_lts_t::states_size_type lts2pbes_state_type;

inline
core::identifier_string make_identifier(const core::identifier_string& name, lts2pbes_state_type s)
{
  return core::identifier_string(std::string(name) + "'" + std::to_string(s));
}

struct lts2pbes_parameters
{
  const state_formulas::state_formula& phi0; // the original formula
  const lts::lts_lts_t& lts0;
  const lts2pbes_lts& lts1;
  data::set_identifier_generator& id_generator;
  utilities::progress_meter& pm;

  lts2pbes_parameters(const state_formulas::state_formula& phi0_,
                      const lts::lts_lts_t& lts0_,
                      const lts2pbes_lts& lts1_,
                      data::set_identifier_generator& id_generator_,
                      utilities::progress_meter& pm_
                     )
    : phi0(phi0_), lts0(lts0_), lts1(lts1_), id_generator(id_generator_), pm(pm_)
  {}
};

template <typename TermTraits, typename Parameters>
pbes_expression RHS(const state_formulas::state_formula& x,
                    lts2pbes_state_type s,
                    Parameters& parameters,
                    TermTraits tr);

template <typename Derived, typename TermTraits, typename Parameters>
struct rhs_lts2pbes_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  typedef TermTraits tr;

  using super::enter;
  using super::leave;
  using super::apply;

  lts2pbes_state_type s;
  Parameters& parameters;
  std::vector<pbes_expression> result_stack;

  rhs_lts2pbes_traverser(lts2pbes_state_type s_,
                         Parameters& parameters_,
                         TermTraits
                        )
    : s(s_), parameters(parameters_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void push(const pbes_expression& x)
  {
    result_stack.push_back(x);
  }

  pbes_expression& top()
  {
    return result_stack.back();
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

  void leave(const state_formulas::true_&)
  {
    push(true_());
  }

  void leave(const state_formulas::false_&)
  {
    push(false_());
  }

  void apply(const state_formulas::not_&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: negation is not supported!");
  }

  void leave(const state_formulas::and_&)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(tr::and_(left, right));
  }

  void leave(const state_formulas::or_&)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(tr::or_(left, right));
  }

  void apply(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: implication is not supported!");
  }

  void apply(const state_formulas::forall& x)
  {
    derived().apply(x.body());
    top() = forall(x.variables(), top());
  }

  void apply(const state_formulas::exists& x)
  {
    derived().apply(x.body());
    top() = exists(x.variables(), top());
  }

  void apply(const state_formulas::must& x)
  {
    const auto& lts1 = parameters.lts1;
    std::vector<pbes_expression> v;
    assert(action_formulas::is_action_formula(x.formula()));
    const action_formulas::action_formula& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());
    const state_formulas::state_formula& phi = x.operand();

    // traverse all transitions s --a--> t
    const lts2pbes_lts::edge_list& m = lts1.edges(s);
    for (lts2pbes_lts::edge_list::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      lts2pbes_state_type t = i->second;
      const lps::multi_action& a = lts1.action_labels()[i->first];
      data::set_identifier_generator id_generator;
      v.push_back(imp(detail::Sat(a, alpha, id_generator, TermTraits()), RHS(phi, t, parameters, TermTraits())));
    }
    push(tr::join_and(v.begin(), v.end()));
  }

  void apply(const state_formulas::may& x)
  {
    const auto& lts1 = parameters.lts1;
    std::vector<pbes_expression> v;
    assert(action_formulas::is_action_formula(x.formula()));
    const action_formulas::action_formula& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());
    const state_formulas::state_formula& phi = x.operand();

    // traverse all transitions s --a--> t
    const lts2pbes_lts::edge_list& m = lts1.edges(s);
    for (lts2pbes_lts::edge_list::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      lts2pbes_state_type t = i->second;
      const lps::multi_action& a = lts1.action_labels()[i->first];
      data::set_identifier_generator id_generator;
      v.push_back(and_(detail::Sat(a, alpha, id_generator, TermTraits()), RHS(phi, t, parameters, TermTraits())));
    }
    push(tr::join_or(v.begin(), v.end()));
  }

  void leave(const state_formulas::yaled&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: yaled is not supported!");
  }

  void leave(const state_formulas::yaled_timed&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: yaled_timed is not supported!");
  }

  void leave(const state_formulas::delay&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: yaled is not supported!");
  }

  void leave(const state_formulas::delay_timed&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: delay_timed is not supported!");
  }

  void leave(const state_formulas::variable& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    const data::data_expression_list& e = x.arguments();
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }

  void apply(const state_formulas::nu& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = detail::mu_expressions(x);
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }

  void apply(const state_formulas::mu& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = detail::mu_expressions(x);
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_rhs_lts2pbes_traverser: public Traverser<apply_rhs_lts2pbes_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  typedef Traverser<apply_rhs_lts2pbes_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_lts2pbes_traverser(lts2pbes_state_type s, Parameters& parameters, TermTraits tr)
    : super(s, parameters, tr)
  {}
};

template <typename TermTraits, typename Parameters>
pbes_expression RHS(const state_formulas::state_formula& x,
                    lts2pbes_state_type s,
                    Parameters& parameters,
                    TermTraits tr)
{
  apply_rhs_lts2pbes_traverser<rhs_lts2pbes_traverser, TermTraits, Parameters> f(s, parameters, tr);
  f.apply(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LTS2PBES_RHS_H
