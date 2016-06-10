// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/lps2pbes_rhs.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_LPS2PBES_RHS_H
#define MCRL2_PBES_DETAIL_LPS2PBES_RHS_H

#include "mcrl2/atermpp/detail/aterm_list_utility.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/pbes/detail/lps2pbes_par.h"
#include "mcrl2/pbes/detail/lps2pbes_sat.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
data::variable_list rhs_structured_compute_variables(const state_formulas::state_formula& x, const std::multiset<data::variable>& variables)
{
  std::set<data::variable> fv = state_formulas::find_free_variables(x);
  fv.insert(variables.begin(), variables.end());
  return data::variable_list(fv.begin(), fv.end());
}

inline
void add_annotation(pbes_expression&, const lps::multi_action&, const data::data_expression&)
{
  // skip
}

//--- RHS default variant ---//

template <typename TermTraits>
typename TermTraits::term_type RHS(const state_formulas::state_formula& x0,
                                   const state_formulas::state_formula& x,
                                   const lps::linear_process& lps,
                                   data::set_identifier_generator& id_generator,
                                   const data::variable& T,
                                   TermTraits tr
                                  );

template <typename Derived, typename TermTraits>
struct rhs_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  typedef TermTraits tr;
  typedef typename tr::term_type expression_type;

  using super::enter;
  using super::leave;
  using super::apply;

  const state_formulas::state_formula& phi0; // the original formula
  const lps::linear_process& lps;
  data::set_identifier_generator& id_generator;
  const data::variable& T;
  std::vector<expression_type> result_stack;

  rhs_traverser(const state_formulas::state_formula& phi0_,
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

  void push(const expression_type& x)
  {
    result_stack.push_back(x);
  }

  expression_type& top()
  {
    return result_stack.back();
  }

  const expression_type& top() const
  {
    return result_stack.back();
  }

  expression_type pop()
  {
    expression_type result = top();
    result_stack.pop_back();
    return result;
  }

  void push_variables(const data::variable_list& variables)
  {
    for (const data::variable& v: variables)
    {
      id_generator.add_identifier(v.name());
    }
  }

  void pop_variables(const data::variable_list& variables)
  {
    for (const data::variable& v: variables)
    {
      id_generator.remove_identifier(v.name());
    }
  }

  bool is_timed() const
  {
    return T != data::undefined_real_variable();
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
    throw mcrl2::runtime_error("rhs_traverser: negation is not supported!");
  }

  void leave(const state_formulas::and_&)
  {
    expression_type right = pop();
    expression_type left = pop();
    push(tr::and_(left, right));
  }

  void leave(const state_formulas::or_&)
  {
    expression_type right = pop();
    expression_type left = pop();
    push(tr::or_(left, right));
  }

  void apply(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("rhs_traverser: implication is not supported!");
  }

  void apply(const state_formulas::forall& x)
  {
    push_variables(x.variables());
    derived().apply(x.body());
    top() = tr::forall(x.variables(), top());
    //pop_variables(x.variables());
  }

  void apply(const state_formulas::exists& x)
  {
    push_variables(x.variables());
    derived().apply(x.body());
    top() = tr::exists(x.variables(), top());
    //pop_variables(x.variables());
  }

  // This function is overridden in the structured variant of the algorithm
  template <typename MustMayExpression>
  expression_type apply_must_may_rhs(const MustMayExpression& x)
  {
    return RHS(phi0, x.operand(), lps, id_generator, T, TermTraits());
  }

  // This function is overridden in the structured variant of the algorithm
  expression_type apply_must_may_result(const expression_type& p)
  {
    return p;
  }

  // share code between must and may
  template <typename MustMayExpression>
  void apply_must_may(const MustMayExpression& x, bool is_must)
  {
    bool timed = is_timed();
    std::vector<expression_type> v;
    expression_type rhs0 = derived().apply_must_may_rhs(x);
    assert(action_formulas::is_action_formula(x.formula()));
    const action_formulas::action_formula& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());

    for (const lps::action_summand& summand: lps.action_summands())
    {
      data::data_expression ci = summand.condition();
      lps::multi_action ai     = summand.multi_action();
      data::assignment_list gi = summand.assignments();
      const data::variable_list& yi   = summand.summation_variables();

      expression_type rhs = rhs0;
      data::mutable_map_substitution<> sigma_yi = pbes_system::detail::make_fresh_variables(yi, id_generator);
      std::set<data::variable> sigma_yi_variables = data::substitution_variables(sigma_yi);
      ci = data::replace_variables_capture_avoiding(ci, sigma_yi, sigma_yi_variables);
      lps::replace_variables_capture_avoiding(ai, sigma_yi, sigma_yi_variables);
      gi = data::replace_variables_capture_avoiding(gi, sigma_yi, sigma_yi_variables);
      const data::data_expression& ti = ai.time();
      expression_type p1 = Sat(ai, alpha, id_generator, TermTraits());
      expression_type p2 = ci;
      data::mutable_map_substitution<> sigma_gi;
      for (const data::assignment& a: gi)
      {
        sigma_gi[a.lhs()] = a.rhs();
      }
      rhs = pbes_system::replace_variables_capture_avoiding(rhs, sigma_gi, data::substitution_variables(sigma_gi));
      expression_type p = tr::and_(p1, p2);
      if (timed)
      {
        data::mutable_map_substitution<> sigma_ti;
        sigma_ti[T] = ti;
        rhs = pbes_system::replace_variables_capture_avoiding(rhs, sigma_ti, data::substitution_variables(sigma_ti));
        p = tr::and_(p, data::greater(ti, T));
      }
      data::variable_list y = data::replace_variables(yi, sigma_yi);
      add_annotation(rhs, ai, ti);
      p = is_must ? tr::forall(y, tr::imp(p, rhs)) : tr::exists(y, tr::and_(p, rhs));
      v.push_back(derived().apply_must_may_result(p));
    }

    expression_type result = is_must ? tr::join_and(v.begin(), v.end()) : tr::join_or(v.begin(), v.end());
    push(result);
  }

  void apply(const state_formulas::must& x)
  {
    apply_must_may(x, true);
  }

  void apply(const state_formulas::may& x)
  {
    apply_must_may(x, false);
  }

  void leave(const state_formulas::yaled&)
  {
    throw mcrl2::runtime_error("rhs_traverser: yaled is not supported!");
  }

  void leave(const state_formulas::yaled_timed& x)
  {
    const data::data_expression& t = x.time_stamp();
    std::vector<expression_type> v;
    for (const lps::action_summand& i: lps.action_summands())
    {
      const data::data_expression& ci = i.condition();
      const data::data_expression& ti = i.multi_action().time();
      const data::variable_list&   yi = i.summation_variables();
      expression_type p = tr::forall(yi, tr::or_(data::sort_bool::not_(ci), data::greater(t, ti)));
      v.push_back(p);
    }
    for (const lps::deadlock_summand& j: lps.deadlock_summands())
    {
      const data::data_expression& cj = j.condition();
      const data::data_expression& tj = j.deadlock().time();
      const data::variable_list&   yj = j.summation_variables();
      expression_type p = tr::forall(yj, tr::or_(data::sort_bool::not_(cj), data::greater(t, tj)));
      v.push_back(p);
    }
    push(tr::and_(tr::join_or(v.begin(), v.end()), data::greater(t, T)));
  }

  void leave(const state_formulas::delay&)
  {
    throw mcrl2::runtime_error("rhs_traverser: delay is not supported!");
  }

  void leave(const state_formulas::delay_timed& x)
  {
    const data::data_expression& t = x.time_stamp();
    std::vector<expression_type> v;
    for (const lps::action_summand& i : lps.action_summands())
    {
      const data::data_expression& ci = i.condition();
      data::data_expression ti = i.multi_action().time();
      const data::variable_list&   yi = i.summation_variables();
      expression_type p = tr::exists(yi, tr::and_(ci, data::less_equal(t, ti)));
      v.push_back(p);
    }
    for (const lps::deadlock_summand& j: lps.deadlock_summands())
    {
      const data::data_expression& cj = j.condition();
      data::data_expression tj = j.deadlock().time();
      const data::variable_list&   yj = j.summation_variables();
      expression_type p = tr::exists(yj, tr::and_(cj, data::less_equal(t, tj)));
      v.push_back(p);
    }
    push(tr::or_(tr::join_or(v.begin(), v.end()), data::less_equal(t, T)));
  }

  void leave(const state_formulas::variable& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    const data::data_expression_list& d = x.arguments();
    data::variable_list xp = lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), phi0);
    if (is_timed())
    {
      e = T + e;
    }
    push(propositional_variable_instantiation(X, e));
  }

  void apply(const state_formulas::nu& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    data::data_expression_list d = detail::mu_expressions(x);
    data::variable_list xp = lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), phi0);
    if (is_timed())
    {
      e = T + e;
    }
    push(propositional_variable_instantiation(X, e));
  }

  void apply(const state_formulas::mu& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    data::data_expression_list d = detail::mu_expressions(x);
    data::variable_list xp = lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), phi0);
    if (is_timed())
    {
      e = T + e;
    }
    push(propositional_variable_instantiation(X, e));
  }
};

template <template <class, class> class Traverser, typename TermTraits>
struct apply_rhs_traverser: public Traverser<apply_rhs_traverser<Traverser, TermTraits>, TermTraits>
{
  typedef Traverser<apply_rhs_traverser<Traverser, TermTraits>, TermTraits> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_traverser(const state_formulas::state_formula& phi0,
                      const lps::linear_process& lps,
                      data::set_identifier_generator& id_generator,
                      const data::variable& T,
                      TermTraits tr
                     )
    : super(phi0, lps, id_generator, T, tr)
  {}
};

template <typename TermTraits>
inline
typename TermTraits::term_type RHS(const state_formulas::state_formula& x0,
                                   const state_formulas::state_formula& x,
                                   const lps::linear_process& lps,
                                   data::set_identifier_generator& id_generator,
                                   const data::variable& T,
                                   TermTraits tr
                                  )
{
  apply_rhs_traverser<rhs_traverser, TermTraits> f(x0, lps, id_generator, T, tr);
  f.apply(x);
  return f.top();
}

//--- RHS_structured variant ---//

template <typename TermTraits>
inline
typename TermTraits::term_type RHS_structured(const state_formulas::state_formula& x0,
                                              const state_formulas::state_formula& x,
                                              const lps::linear_process& lps,
                                              data::set_identifier_generator& id_generator,
                                              data::set_identifier_generator& propvar_generator,
                                              const data::variable_list& variables,
                                              const fixpoint_symbol& sigma,
                                              std::vector<pbes_equation>& Z,
                                              const data::variable& T,
                                              TermTraits tr
                                             );

template <typename Derived, typename TermTraits>
struct rhs_structured_traverser: public rhs_traverser<Derived, TermTraits>
{
  typedef rhs_traverser<Derived, TermTraits> super;
  typedef TermTraits tr;
  typedef typename tr::term_type expression_type;

  using super::enter;
  using super::leave;
  using super::apply;
  using super::push;
  using super::top;
  using super::pop;
  using super::is_timed;
  using super::phi0;
  using super::lps;
  using super::id_generator;
  using super::T;
  using super::apply_must_may;

  std::multiset<data::variable> variables;
  const fixpoint_symbol& sigma;
  data::set_identifier_generator& propvar_generator;
  std::vector<pbes_equation>& Z; // new equations that are generated on the fly

  rhs_structured_traverser(const state_formulas::state_formula& phi0,
                           const lps::linear_process& lps,
                           data::set_identifier_generator& id_generator,
                           data::set_identifier_generator& propvar_generator_,
                           const data::variable_list& variables_,
                           const fixpoint_symbol& sigma_,
                           std::vector<pbes_equation>& Z_,
                           const data::variable& T,
                           TermTraits tr
               )
    : super(phi0, lps, id_generator, T, tr),
    	variables(variables_.begin(), variables_.end()),
    	sigma(sigma_),
    	propvar_generator(propvar_generator_),
    	Z(Z_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void apply(const state_formulas::forall& x)
  {
  	const data::variable_list& v = x.variables();
  	variables.insert(v.begin(), v.end());
  	super::apply(x);
  	for (const data::variable& var: v)
    {
      variables.erase(var);
    }
  }

  void apply(const state_formulas::exists& x)
  {
  	const data::variable_list& v = x.variables();
  	variables.insert(v.begin(), v.end());
  	super::apply(x);
  	for (const data::variable& var: v)
    {
      variables.erase(var);
    }
  }

  // override
  template <typename MustMayExpression>
  expression_type apply_must_may_rhs(const MustMayExpression& x)
  {
    return RHS_structured(phi0, x.operand(), lps, id_generator, propvar_generator, rhs_structured_compute_variables(x.operand(), variables), sigma, Z, T, TermTraits());
  }

  // override
  expression_type apply_must_may_result(const expression_type& p)
  {
    // generate a new equation 'Y(d) = p', and add Y(d) to v
    core::identifier_string Y = propvar_generator("Y");
    data::variable_list d(variables.begin(), variables.end());
    propositional_variable Yd(Y, d);
    pbes_equation eqn(sigma, Yd, p);
    Z.push_back(eqn);
    return propositional_variable_instantiation(Y, data::make_data_expression_list(d));
  }

  void apply(const state_formulas::must& x)
  {
    apply_must_may(x, true);
  }

  void apply(const state_formulas::may& x)
  {
    apply_must_may(x, false);
  }
};

template <template <class, class> class Traverser, typename TermTraits>
struct apply_rhs_structured_traverser: public Traverser<apply_rhs_structured_traverser<Traverser, TermTraits>, TermTraits>
{
  typedef Traverser<apply_rhs_structured_traverser<Traverser, TermTraits>, TermTraits> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_structured_traverser(const state_formulas::state_formula& phi0,
                                 const lps::linear_process& lps,
                                 data::set_identifier_generator& id_generator,
                                 data::set_identifier_generator& propvar_generator,
                                 const data::variable_list& variables,
                                 const fixpoint_symbol& sigma,
                                 std::vector<pbes_equation>& Z,
                                 const data::variable& T,
                                 TermTraits tr
                                )
    : super(phi0, lps, id_generator, propvar_generator, variables, sigma, Z, T, tr)
  {}
};

template <typename TermTraits>
inline
typename TermTraits::term_type RHS_structured(const state_formulas::state_formula& x0,
                                              const state_formulas::state_formula& x,
                                              const lps::linear_process& lps,
                                              data::set_identifier_generator& id_generator,
                                              data::set_identifier_generator& propvar_generator,
                                              const data::variable_list& variables,
                                              const fixpoint_symbol& sigma,
                                              std::vector<pbes_equation>& Z,
                                              const data::variable& T,
                                              TermTraits tr
                                             )
{
  apply_rhs_structured_traverser<rhs_structured_traverser, TermTraits> f(x0, lps, id_generator, propvar_generator, variables, sigma, Z, T, tr);
  f.apply(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_RHS_H
