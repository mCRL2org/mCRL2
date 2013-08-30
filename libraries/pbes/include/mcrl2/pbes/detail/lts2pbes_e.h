// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lts2pbes_e.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LTS2PBES_E_H
#define MCRL2_PBES_DETAIL_LTS2PBES_E_H

#include "mcrl2/pbes/detail/lts2pbes_rhs.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename TermTraits>
std::vector<pbes_equation> E(const state_formulas::state_formula& x0,
                             const state_formulas::state_formula& x,
                             const lts::lts_lts_t& lts0,
                             const lts2pbes_lts& lts1,
                             utilities::progress_meter& pm,
                             TermTraits tr
                            );

template <typename Derived, typename TermTraits>
struct e_lts2pbes_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  typedef std::vector<pbes_equation> result_type;

  const state_formulas::state_formula& phi0; // the original formula
  const lts::lts_lts_t& lts0;
  const lts2pbes_lts& lts1;
  utilities::progress_meter& m_progress_meter;
  std::vector<result_type> result_stack;

  e_lts2pbes_traverser(const state_formulas::state_formula& phi0_,
                       const lts::lts_lts_t& lts0_,
                       const lts2pbes_lts& lts1_,
                       utilities::progress_meter& pm,
                       TermTraits
                      )
    : phi0(phi0_), lts0(lts0_), lts1(lts1_), m_progress_meter(pm)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void push(const result_type& x)
  {
    result_stack.push_back(x);
  }

  result_type& top()
  {
    return result_stack.back();
  }

  const result_type& top() const
  {
    return result_stack.back();
  }

  result_type pop()
  {
    result_type result = top();
    result_stack.pop_back();
    return result;
  }

  // the empty equation list
  result_type epsilon() const
  {
    return result_type();
  }

  void leave(const data::data_expression&)
  {
    push(epsilon());
  }

  void leave(const state_formulas::true_&)
  {
    push(epsilon());
  }

  void leave(const state_formulas::false_&)
  {
    push(epsilon());
  }

  void operator()(const state_formulas::not_&)
  {
    throw mcrl2::runtime_error("e_lts2pbes_traverser: negation is not supported!");
  }

  void leave(const state_formulas::and_&)
  {
    std::vector<pbes_equation> right = pop();
    std::vector<pbes_equation> left = pop();
    push(left + right);
  }

  void leave(const state_formulas::or_&)
  {
    std::vector<pbes_equation> right = pop();
    std::vector<pbes_equation> left = pop();
    push(left + right);
  }

  void operator()(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("e_lts2pbes_traverser: implication is not supported!");
  }

  void leave(const state_formulas::forall&)
  {
    // skip
  }

  void leave(const state_formulas::exists&)
  {
    // skip
  }

  void leave(const state_formulas::must&)
  {
    // skip
  }

  void leave(const state_formulas::may&)
  {
    // skip
  }

  void leave(const state_formulas::yaled&)
  {
    push(epsilon());
  }

  void leave(const state_formulas::yaled_timed&)
  {
    push(epsilon());
  }

  void leave(const state_formulas::delay&)
  {
    push(epsilon());
  }

  void leave(const state_formulas::delay_timed&)
  {
    push(epsilon());
  }

  void leave(const state_formulas::variable&)
  {
    push(epsilon());
  }

  template <typename Expr>
  void handle_mu_nu(const Expr& x, const fixpoint_symbol& sigma)
  {
    using atermpp::detail::operator+;
    core::identifier_string X = x.name();
    data::variable_list d = detail::mu_variables(x);

    std::vector<pbes_equation> v;

    // traverse all states of the LTS
    for (lts2pbes_state_type s = 0; s < lts1.state_count(); s++)
    {
      core::identifier_string X_s = make_identifier(X, s);
      propositional_variable Xs(X_s, d + Par(X, data::variable_list(), phi0));
      v.push_back(pbes_equation(sigma, Xs, RHS(phi0, x.operand(), lts0, lts1, s, m_progress_meter, TermTraits())));
      m_progress_meter.step();
    }

    push(v + E(phi0, x.operand(), lts0, lts1, m_progress_meter, TermTraits()));
  }

  void operator()(const state_formulas::nu& x)
  {
    handle_mu_nu(x, fixpoint_symbol::nu());
  }

  void operator()(const state_formulas::mu& x)
  {
    handle_mu_nu(x, fixpoint_symbol::mu());
  }
};

template <template <class, class> class Traverser, typename TermTraits>
struct apply_e_lts2pbes_traverser: public Traverser<apply_e_lts2pbes_traverser<Traverser, TermTraits>, TermTraits>
{
  typedef Traverser<apply_e_lts2pbes_traverser<Traverser, TermTraits>, TermTraits> super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_e_lts2pbes_traverser(const state_formulas::state_formula& phi0,
                             const lts::lts_lts_t& lts0,
                             const lts2pbes_lts& lts1,
                             utilities::progress_meter& pm,
                             TermTraits tr
                            )
    : super(phi0, lts0, lts1, pm, tr)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

template <typename TermTraits>
std::vector<pbes_equation> E(const state_formulas::state_formula& x0,
                             const state_formulas::state_formula& x,
                             const lts::lts_lts_t& lts0,
                             const lts2pbes_lts& lts1,
                             utilities::progress_meter& pm,
                             TermTraits tr
                            )
{
  apply_e_lts2pbes_traverser<e_lts2pbes_traverser, TermTraits> f(x0, lts0, lts1, pm, tr);
  f(x);
  assert(f.result_stack.size() == 1);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LTS2PBES_E_H
