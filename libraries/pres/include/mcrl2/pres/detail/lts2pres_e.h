// Author(s): Jan Friso Groote. Based on lts2pbes_e.h by Wieger Wesselink.
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/lts2pres_e.h
/// \brief add your file description here.

#ifndef MCRL2_PRES_DETAIL_LTS2PRES_E_H
#define MCRL2_PRES_DETAIL_LTS2PRES_E_H

#include "mcrl2/pres/detail/lts2pres_rhs.h"

namespace mcrl2::pres_system::detail
{

template <typename TermTraits, typename Parameters>
void E_lts2pres(const state_formulas::state_formula& x,
       Parameters& parameters,
       std::vector<pres_equation>& result,
       TermTraits tr
      );

template <typename Derived, typename TermTraits, typename Parameters>
struct e_lts2pres_traverser: public state_formulas::state_formula_traverser<Derived>
{
  using super = state_formulas::state_formula_traverser<Derived>;
  using super::enter;
  using super::leave;
  using super::apply;

  using result_type = std::vector<pres_equation>;

  Parameters& parameters;
  std::vector<result_type> result_stack;

  e_lts2pres_traverser(Parameters& parameters_, TermTraits)
    : parameters(parameters_)
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

  void apply(const state_formulas::minus&)
  {
    throw mcrl2::runtime_error("e_lts2pres_traverser: 'minus' is not supported!");
  }

  void leave(const state_formulas::and_&)
  {
    std::vector<pres_equation> right = pop();
    std::vector<pres_equation> left = pop();
    push(left + right);
  }

  void leave(const state_formulas::or_&)
  {
    std::vector<pres_equation> right = pop();
    std::vector<pres_equation> left = pop();
    push(left + right);
  }

  void apply(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("e_lts2pres_traverser: implication is not supported!");
  }

  void leave(const state_formulas::infimum&)
  {
    // skip
  }

  void leave(const state_formulas::supremum&)
  {
    // skip
  }

  void leave(const state_formulas::sum&)
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
    const core::identifier_string& X = x.name();
    data::variable_list d = detail::mu_variables(x);

    std::vector<pres_equation> v;

    // traverse all states of the LTS
    for (lts2pres_state_type s = 0; s < parameters.lts1.state_count(); s++)
    {
      core::identifier_string X_s = make_identifier(X, s);
      propositional_variable Xs(X_s, d + Par(X, data::variable_list(), parameters.phi0));
      v.push_back(pres_equation(sigma, Xs, RHS(x.operand(), s, parameters, TermTraits())));
      parameters.pm.step();
    }

    E_lts2pres(x.operand(), parameters, v, TermTraits());
    push(v);
  }

  void apply(const state_formulas::nu& x)
  {
    handle_mu_nu(x, fixpoint_symbol::nu());
  }

  void apply(const state_formulas::mu& x)
  {
    handle_mu_nu(x, fixpoint_symbol::mu());
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_e_lts2pres_traverser: public Traverser<apply_e_lts2pres_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  using super = Traverser<apply_e_lts2pres_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_e_lts2pres_traverser(Parameters& parameters, TermTraits tr)
    : super(parameters, tr)
  {}
};

template <typename TermTraits, typename Parameters>
void E_lts2pres(const state_formulas::state_formula& x,
       Parameters& parameters,
       std::vector<pres_equation>& result,
       TermTraits tr
      )
{
  apply_e_lts2pres_traverser<e_lts2pres_traverser, TermTraits, Parameters> f(parameters, tr);
  f.apply(x);
  assert(f.result_stack.size() == 1);
  result.insert(result.end(), f.top().begin(), f.top().end());
}

} // namespace mcrl2::pres_system::detail

#endif // MCRL2_PRES_DETAIL_LTS2PRES_E_H
