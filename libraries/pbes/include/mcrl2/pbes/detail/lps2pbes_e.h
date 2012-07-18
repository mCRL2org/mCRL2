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

atermpp::vector<pbes_equation> E(const state_formulas::state_formula& x0,
                                 const state_formulas::state_formula& x,
                                 const lps::linear_process& lps,
                                 const data::variable& T = data::variable()
                                );

template <typename Derived>
struct e_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

#if BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif

  typedef atermpp::vector<pbes_equation> result_type;

  const state_formulas::state_formula& phi0; // the original formula
  const lps::linear_process& lps;
  const data::variable& T;
  std::vector<result_type> result_stack;

  e_traverser(const state_formulas::state_formula& phi0_,
                const lps::linear_process& lps_,
                const data::variable& T_ = data::variable()
               )
    : phi0(phi0_), lps(lps_), T(T_)
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
    return T != data::variable();
  }

  // the empty equation list
  result_type epsilon() const
  {
    return result_type();
  }

  void leave(const data::data_expression& x)
  {
    push(epsilon());
  }

  void leave(const state_formulas::true_& x)
  {
    push(epsilon());
  }

  void leave(const state_formulas::false_& x)
  {
    push(epsilon());
  }

  void operator()(const state_formulas::not_& x)
  {
    throw mcrl2::runtime_error("e_traverser: negation is not supported!");
  }

  void leave(const state_formulas::and_& x)
  {
    atermpp::vector<pbes_equation> right = pop();
    atermpp::vector<pbes_equation> left = pop();
    push(left + right);
  }

  void leave(const state_formulas::or_& x)
  {
    atermpp::vector<pbes_equation> right = pop();
    atermpp::vector<pbes_equation> left = pop();
    push(left + right);
  }

  void operator()(const state_formulas::imp& x)
  {
    throw mcrl2::runtime_error("e_traverser: implication is not supported!");
  }

  void leave(const state_formulas::forall& x)
  {
    // skip
  }

  void leave(const state_formulas::exists& x)
  {
    // skip
  }

  void leave(const state_formulas::must& x)
  {
    // skip
  }

  void leave(const state_formulas::may& x)
  {
    // skip
  }

  void leave(const state_formulas::yaled& x)
  {
    push(epsilon());
  }

  void leave(const state_formulas::yaled_timed& x)
  {
    push(epsilon());
  }

  void leave(const state_formulas::delay& x)
  {
    push(epsilon());
  }

  void leave(const state_formulas::delay_timed& x)
  {
    push(epsilon());
  }

  void leave(const state_formulas::variable& x)
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
    data::data_expression_list e = xf + xp + Par(X, data::variable_list(), phi0);
    e = is_timed() ? T + e : e;
    propositional_variable v(X, e);
    data::set_identifier_generator id_generator;
    pbes_expression expr = detail::RHS(phi0, phi, lps, id_generator, T);
    pbes_equation eqn(sigma, v, expr);
    push(atermpp::vector<pbes_equation>() + eqn + E(phi0, phi, lps, T));
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

template <template <class> class Traverser>
struct apply_e_traverser: public Traverser<apply_e_traverser<Traverser> >
{
  typedef Traverser<apply_e_traverser<Traverser> > super;
  using super::enter;
  using super::leave;
  using super::operator();

  apply_e_traverser(const state_formulas::state_formula& phi0,
                   const lps::linear_process& lps,
                   const data::variable& T = data::variable()
                  )
    : super(phi0, lps, T)
  {}

#ifdef BOOST_MSVC
#include "mcrl2/core/detail/traverser_msvc.inc.h"
#endif
};

inline
atermpp::vector<pbes_equation> E(const state_formulas::state_formula& x0,
                                 const state_formulas::state_formula& x,
                                 const lps::linear_process& lps,
                                 const data::variable& T // = data::variable()
                                )
{
  apply_e_traverser<e_traverser> f(x0, lps, T);
  f(x);
  assert(f.result_stack.size() == 1);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_E_H
