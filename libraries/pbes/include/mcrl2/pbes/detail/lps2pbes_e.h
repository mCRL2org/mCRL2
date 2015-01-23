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

template <typename TermTraits>
std::vector<pbes_equation> E(const state_formulas::state_formula& x0,
                                 const state_formulas::state_formula& x,
                                 const lps::linear_process& lps,
                                 data::set_identifier_generator& id_generator,
                                 const data::variable& T,
                                 TermTraits tr
                                );

template <typename Derived, typename TermTraits>
struct e_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::apply;

  typedef std::vector<pbes_equation> result_type;

  const state_formulas::state_formula& phi0; // the original formula
  const lps::linear_process& lps;
  data::set_identifier_generator& id_generator;
  const data::variable& T;
  std::vector<result_type> result_stack;

  e_traverser(const state_formulas::state_formula& phi0_,
              const lps::linear_process& lps_,
              data::set_identifier_generator& id_generator_,
              const data::variable& T_,
              TermTraits
             )
    : phi0(phi0_), lps(lps_), id_generator(id_generator_), T(T_)
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
    return T != data::undefined_real_variable();
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
  void handle_mu_nu(const Expr& x, const fixpoint_symbol& sigma)
  {
    using atermpp::detail::operator+;
    core::identifier_string X = x.name();
    data::variable_list xf = detail::mu_variables(x);
    data::variable_list xp = lps.process_parameters();
    state_formulas::state_formula phi = x.operand();
    data::variable_list e = xf + xp + Par(X, data::variable_list(), phi0);
    e = is_timed() ? T + e : e;
    propositional_variable v(X, e);
    pbes_expression expr = detail::RHS(phi0, phi, lps, id_generator, T, TermTraits());
    pbes_equation eqn(sigma, v, expr);
    push(std::vector<pbes_equation>() + eqn + E(phi0, phi, lps, id_generator, T, TermTraits()));
  }

  void apply(const state_formulas::nu& x)
  {
    handle_mu_nu(x, fixpoint_symbol::nu());
    mCRL2log(log::debug1, "lps2pbes") << "E(" << x << ") = " << core::detail::print_list(top()) << std::endl;
  }

  void apply(const state_formulas::mu& x)
  {
    handle_mu_nu(x, fixpoint_symbol::mu());
    mCRL2log(log::debug1, "lps2pbes") << "E(" << x << ") = " << core::detail::print_list(top()) << std::endl;
  }
};

template <template <class, class> class Traverser, typename TermTraits>
struct apply_e_traverser: public Traverser<apply_e_traverser<Traverser, TermTraits>, TermTraits>
{
  typedef Traverser<apply_e_traverser<Traverser, TermTraits>, TermTraits> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_e_traverser(const state_formulas::state_formula& phi0,
                    const lps::linear_process& lps,
                    data::set_identifier_generator& id_generator,
                    const data::variable& T,
                    TermTraits tr
                   )
    : super(phi0, lps, id_generator, T, tr)
  {}
};

template <typename TermTraits>
std::vector<pbes_equation> E(const state_formulas::state_formula& x0,
                                 const state_formulas::state_formula& x,
                                 const lps::linear_process& lps,
                                 data::set_identifier_generator& id_generator,
                                 const data::variable& T,
                                 TermTraits tr
                                )
{
  apply_e_traverser<e_traverser, TermTraits> f(x0, lps, id_generator, T, tr);
  f.apply(x);
  assert(f.result_stack.size() == 1);
  return f.top();
}

template <typename TermTraits>
std::vector<pbes_equation> E_structured(const state_formulas::state_formula& x0,
                                            const state_formulas::state_formula& x,
                                            const lps::linear_process& lps,
                                            data::set_identifier_generator& id_generator,
                                            data::set_identifier_generator& propvar_generator,
                                            const data::variable& T,
                                            TermTraits tr
                                           );

template <typename Derived, typename TermTraits>
struct e_structured_traverser: public e_traverser<Derived, TermTraits>
{
  typedef e_traverser<Derived, TermTraits> super;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::push;
  using super::top;
  using super::pop;
  using super::is_timed;
  using super::epsilon;
  using super::phi0;
  using super::lps;
  using super::id_generator;
  using super::T;

  // typedef std::vector<pbes_equation> result_type;

  data::set_identifier_generator& propvar_generator;

  e_structured_traverser(const state_formulas::state_formula& phi0,
                         const lps::linear_process& lps,
                         data::set_identifier_generator& id_generator,
                         data::set_identifier_generator& propvar_generator_,
                         const data::variable& T,
                         TermTraits tr
                        )
    : super(phi0, lps, id_generator, T, tr),
    	propvar_generator(propvar_generator_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  template <typename Expr>
  void handle_mu_nu(const Expr& x, const fixpoint_symbol& sigma)
  {
    using atermpp::detail::operator+;
    core::identifier_string X = x.name();
    data::variable_list xf = detail::mu_variables(x);
    data::variable_list xp = lps.process_parameters();
    state_formulas::state_formula phi = x.operand();
    data::variable_list d = xf + xp + Par(X, data::variable_list(), phi0);
    d = is_timed() ? T + d : d;
    data::data_expression_list e = data::make_data_expression_list(d);
    propositional_variable v(X, d);
    std::vector<pbes_equation> Z;
    pbes_expression expr = detail::RHS_structured(phi0, phi, lps, id_generator, propvar_generator, d, sigma, Z, T, TermTraits());
    pbes_equation eqn(sigma, v, expr);
    push(std::vector<pbes_equation>() + eqn + Z + E_structured(phi0, phi, lps, id_generator, propvar_generator, T, TermTraits()));
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

template <template <class, class> class Traverser, typename TermTraits>
struct apply_e_structured_traverser: public Traverser<apply_e_structured_traverser<Traverser, TermTraits>, TermTraits>
{
  typedef Traverser<apply_e_structured_traverser<Traverser, TermTraits>, TermTraits> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_e_structured_traverser(const state_formulas::state_formula& phi0,
                               const lps::linear_process& lps,
                               data::set_identifier_generator& id_generator,
                               data::set_identifier_generator& propvar_generator,
                               const data::variable& T,
                               TermTraits tr
                              )
    : super(phi0, lps, id_generator, propvar_generator, T, tr)
  {}
};

template <typename TermTraits>
std::vector<pbes_equation> E_structured(const state_formulas::state_formula& x0,
                                            const state_formulas::state_formula& x,
                                            const lps::linear_process& lps,
                                            data::set_identifier_generator& id_generator,
                                            data::set_identifier_generator& propvar_generator,
                                            const data::variable& T,
                                            TermTraits tr
                                           )
{
  apply_e_structured_traverser<e_structured_traverser, TermTraits> f(x0, lps, id_generator, propvar_generator, T, tr);
  f.apply(x);
  assert(f.result_stack.size() == 1);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_E_H
