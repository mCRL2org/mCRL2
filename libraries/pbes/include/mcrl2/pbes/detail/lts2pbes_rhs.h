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
  return core::identifier_string(std::string(name) + "@" + boost::lexical_cast<std::string>(s));
}

inline
pbes_expression RHS(const state_formulas::state_formula& x0,
                    const state_formulas::state_formula& x,
                    const lts::lts_lts_t& lts0,
                    const lts2pbes_lts& lts1,
                    lts2pbes_state_type s,
                    const utilities::progress_meter& pm
                   );

template <typename Derived>
struct rhs_lts2pbes_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  const state_formulas::state_formula& phi0; // the original formula
  const lts::lts_lts_t& lts0;
  const lts2pbes_lts& lts1;
  lts2pbes_state_type s;
  const utilities::progress_meter& m_progress_meter;
  atermpp::vector<pbes_expression> result_stack;

  rhs_lts2pbes_traverser(const state_formulas::state_formula& phi0_,
                         const lts::lts_lts_t& lts0_,
                         const lts2pbes_lts& lts1_,
                         lts2pbes_state_type s_,
                         const utilities::progress_meter& pm
                        )
    : phi0(phi0_), lts0(lts0_), lts1(lts1_), s(s_), m_progress_meter(pm)
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

  void operator()(const state_formulas::not_&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: negation is not supported!");
  }

  void leave(const state_formulas::and_&)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(pbes_expr_optimized::and_(left, right));
  }

  void leave(const state_formulas::or_&)
  {
    pbes_expression right = pop();
    pbes_expression left = pop();
    push(pbes_expr_optimized::or_(left, right));
  }

  void operator()(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("rhs_lts2pbes_traverser: implication is not supported!");
  }

  void operator()(const state_formulas::forall& x)
  {
    derived()(x.body());
    top() = forall(x.variables(), top());
  }

  void operator()(const state_formulas::exists& x)
  {
    derived()(x.body());
    top() = exists(x.variables(), top());
  }

  void operator()(const state_formulas::must& x)
  {
    atermpp::vector<pbes_expression> v;
    action_formulas::action_formula alpha = x.formula();
    state_formulas::state_formula phi = x.operand();

    // traverse all transitions s --a--> t
    const lts2pbes_lts::edge_list& m = lts1.edges(s);
    for (lts2pbes_lts::edge_list::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      lts2pbes_state_type t = i->second;
      const lps::multi_action& a = lts1.action_labels()[i->first];
      data::set_identifier_generator id_generator;
      v.push_back(imp(detail::Sat(a, alpha, id_generator), RHS(phi0, phi, lts0, lts1, t, m_progress_meter)));
    }
    push(pbes_expr_optimized::join_and(v.begin(), v.end()));
  }

  void operator()(const state_formulas::may& x)
  {
    atermpp::vector<pbes_expression> v;
    action_formulas::action_formula alpha = x.formula();
    state_formulas::state_formula phi = x.operand();

    // traverse all transitions s --a--> t
    const lts2pbes_lts::edge_list& m = lts1.edges(s);
    for (lts2pbes_lts::edge_list::const_iterator i = m.begin(); i != m.end(); ++i)
    {
      lts2pbes_state_type t = i->second;
      const lps::multi_action& a = lts1.action_labels()[i->first];
      data::set_identifier_generator id_generator;
      v.push_back(and_(detail::Sat(a, alpha, id_generator), RHS(phi0, phi, lts0, lts1, t, m_progress_meter)));
    }
    push(pbes_expr_optimized::join_or(v.begin(), v.end()));
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
    core::identifier_string X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = x.arguments();
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), phi0)));
  }

  void operator()(const state_formulas::nu& x)
  {
    using atermpp::detail::operator+;
    core::identifier_string X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = detail::mu_expressions(x);
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), phi0)));
  }

  void operator()(const state_formulas::mu& x)
  {
    using atermpp::detail::operator+;
    core::identifier_string X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = detail::mu_expressions(x);
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), phi0)));
  }
};

template <template <class> class Traverser>
struct apply_rhs_lts2pbes_traverser: public Traverser<apply_rhs_lts2pbes_traverser<Traverser> >
{
  typedef Traverser<apply_rhs_lts2pbes_traverser<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_rhs_lts2pbes_traverser(const state_formulas::state_formula& x0,
                               const lts::lts_lts_t& lts0,
                               const lts2pbes_lts& lts1,
                               lts2pbes_state_type s,
                               const utilities::progress_meter& pm
                              )
    : super(x0, lts0, lts1, s, pm)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
pbes_expression RHS(const state_formulas::state_formula& x0,
                    const state_formulas::state_formula& x,
                    const lts::lts_lts_t& lts0,
                    const lts2pbes_lts& lts1,
                    lts2pbes_state_type s,
                    const utilities::progress_meter& pm
                   )
{
  apply_rhs_lts2pbes_traverser<rhs_lts2pbes_traverser> f(x0, lts0, lts1, s, pm);
  f(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LTS2PBES_RHS_H
