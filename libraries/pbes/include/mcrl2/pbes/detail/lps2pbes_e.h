// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lps2pbes_e.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPS2PBES_E_H
#define MCRL2_PBES_DETAIL_LPS2PBES_E_H

#include "mcrl2/pbes/detail/lps2pbes_rhs.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename TermTraits, typename Parameters>
void E(const state_formulas::state_formula& x,
       Parameters& parameters,
       std::vector<pbes_equation>& result,
       TermTraits tr
      );

template <typename Derived, typename TermTraits, typename Parameters>
struct e_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  typedef std::vector<pbes_equation> result_type;

  Parameters& parameters;
  std::vector<result_type> result_stack;

  e_traverser(Parameters& parameters_, TermTraits)
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

  bool is_timed() const
  {
    return parameters.T != data::undefined_real_variable();
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

  void apply(const state_formulas::not_&)
  {
    throw mcrl2::runtime_error("e_traverser: negation is not supported!");
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

  void apply(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("e_traverser: implication is not supported!");
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
  void apply_mu_nu(const Expr& x, const fixpoint_symbol& sigma)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    data::variable_list xf = detail::mu_variables(x);
    data::variable_list xp = parameters.lps.process_parameters();
    const state_formulas::state_formula& phi = x.operand();
    data::variable_list e = xf + xp + Par(X, data::variable_list(), parameters.phi0);
    e = is_timed() ? parameters.T + e : e;
    propositional_variable v(X, e);
    pbes_expression expr = detail::RHS(phi, parameters, TermTraits());
    pbes_equation eqn(sigma, v, expr);
    std::vector<pbes_equation> result = { eqn };
    E(phi, parameters, result, TermTraits());
    push(result);
  }

  void apply(const state_formulas::nu& x)
  {
    apply_mu_nu(x, fixpoint_symbol::nu());
    mCRL2log(log::debug1, "lps2pbes") << "E(" << x << ") = " << core::detail::print_list(top()) << std::endl;
  }

  void apply(const state_formulas::mu& x)
  {
    apply_mu_nu(x, fixpoint_symbol::mu());
    mCRL2log(log::debug1, "lps2pbes") << "E(" << x << ") = " << core::detail::print_list(top()) << std::endl;
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_e_traverser: public Traverser<apply_e_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  typedef Traverser<apply_e_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_e_traverser(Parameters& parameters, TermTraits tr)
    : super(parameters, tr)
  {}
};

template <typename TermTraits, typename Parameters>
void E(const state_formulas::state_formula& x, Parameters& parameters, std::vector<pbes_equation>& result, TermTraits tr)
{
  apply_e_traverser<e_traverser, TermTraits, Parameters> f(parameters, tr);
  f.apply(x);
  assert(f.result_stack.size() == 1);
  result.insert(result.end(), f.top().begin(), f.top().end());
}

template <typename TermTraits, typename Parameters>
void E_structured(const state_formulas::state_formula& x,
                  Parameters& parameters,
                  std::vector<pbes_equation>& result,
                  TermTraits tr
                 );

template <typename Derived, typename TermTraits, typename Parameters>
struct e_structured_traverser: public e_traverser<Derived, TermTraits, Parameters>
{
  typedef e_traverser<Derived, TermTraits, Parameters> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::push;
  using super::top;
  using super::pop;
  using super::is_timed;
  using super::epsilon;
  using super::parameters;

  // typedef std::vector<pbes_equation> result_type;

  e_structured_traverser(Parameters& parameters,
                         TermTraits tr
                        )
    : super(parameters, tr)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <typename Expr>
  void apply_mu_nu(const Expr& x, const fixpoint_symbol& sigma)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    data::variable_list xf = detail::mu_variables(x);
    data::variable_list xp = parameters.lps.process_parameters();
    const state_formulas::state_formula& phi = x.operand();
    data::variable_list d = xf + xp + Par(X, data::variable_list(), parameters.phi0);
    d = is_timed() ? parameters.T + d : d;
    data::data_expression_list e = data::make_data_expression_list(d);
    propositional_variable v(X, d);
    std::vector<pbes_equation> equations;
    pbes_expression expr = detail::RHS_structured(phi, parameters, d, sigma, equations, TermTraits());
    pbes_equation eqn(sigma, v, expr);
    std::vector<pbes_equation> result = { eqn };
    result.insert(result.end(), equations.begin(), equations.end());
    E_structured(phi, parameters, result, TermTraits());
    push(result);
  }

  void apply(const state_formulas::nu& x)
  {
    apply_mu_nu(x, fixpoint_symbol::nu());
  }

  void apply(const state_formulas::mu& x)
  {
    apply_mu_nu(x, fixpoint_symbol::mu());
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_e_structured_traverser: public Traverser<apply_e_structured_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  typedef Traverser<apply_e_structured_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_e_structured_traverser(Parameters& parameters,
                               TermTraits tr
                              )
    : super(parameters, tr)
  {}
};

template <typename TermTraits, typename Parameters>
void E_structured(const state_formulas::state_formula& x,
                  Parameters& parameters,
                  std::vector<pbes_equation>& result,
                  TermTraits tr
                 )
{
  apply_e_structured_traverser<e_structured_traverser, TermTraits, Parameters> f(parameters, tr);
  f.apply(x);
  assert(f.result_stack.size() == 1);
  result.insert(result.end(), f.top().begin(), f.top().end());
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_E_H
