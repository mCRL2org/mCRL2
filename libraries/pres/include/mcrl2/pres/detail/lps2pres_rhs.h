// Author(s): Jan Friso Groote. Based on pbes/lps2pbes_rhs.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/lps2pres_rhs.h
/// \brief Translate a modal formula to a pres

#ifndef MCRL2_PRES_DETAIL_LPS2PRES_RHS_H
#define MCRL2_PRES_DETAIL_LPS2PRES_RHS_H

#include "mcrl2/data/consistency.h"
#include "mcrl2/pres/detail/lps2pres_par.h"
#include "mcrl2/pres/detail/lps2pres_sat.h"
#include "mcrl2/pres/replace.h"





namespace mcrl2::pres_system::detail {

struct lps2pres_parameters
{
  const state_formulas::state_formula& phi0; // the original formula
  const lps::stochastic_linear_process& lps;
  data::set_identifier_generator& id_generator;
  const data::variable& T;

  lps2pres_parameters(const state_formulas::state_formula& phi0_,
                      const lps::stochastic_linear_process& lps_,
                      data::set_identifier_generator& id_generator_,
                      const data::variable& T_
                     )
    : phi0(phi0_), lps(lps_), id_generator(id_generator_), T(T_)
  {}

  bool is_timed() const
  {
    return T != data::undefined_real_variable();
  }

  template <typename TermTraits>
  pres_expression rhs_may_must(bool is_must,
                               const data::variable_list& y,
                               const pres_expression& left,  // Sat(ai(fi(x,y)) && ci(x,y) && (ti(x,y) > T)
                               const pres_expression& right, // RHS(phi)[T, x := ti(x,y), gi(x,y)]
                               const lps::multi_action& /* ai */,
                               const data::assignment_list& /* gi */,
                               const lps::stochastic_distribution& dist,
                               TermTraits
                              )
  {
    using tr = TermTraits;
    if (is_must)
    {
      return tr::infimum(y, tr::imp(left, tr::sum(dist.variables(), tr::const_multiply(dist.distribution(), right))));
    }
    else
    {
      return tr::supremum(y, tr::and_(left,  tr::sum(dist.variables(), tr::const_multiply(dist.distribution(), right))));
    }
  }
};

//--- RHS default variant ---//

template <typename TermTraits, typename Parameters>
pres_expression RHS(const state_formulas::state_formula& x, Parameters& parameters, TermTraits tr);

template <typename Derived, typename TermTraits, typename Parameters>
struct rhs_traverser: public state_formulas::state_formula_traverser<Derived>
{
  using super = state_formulas::state_formula_traverser<Derived>;
  using tr = TermTraits;
  using pres_expression = typename tr::term_type;

  using super::enter;
  using super::leave;
  using super::apply;

  Parameters& parameters;
  std::vector<pres_expression> result_stack;

  rhs_traverser(Parameters& parameters_, TermTraits)
    : parameters(parameters_)
  {}

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  void push(const pres_expression& x)
  {
    result_stack.push_back(x);
  }

  pres_expression& top()
  {
    return result_stack.back();
  }

  const pres_expression& top() const
  {
    return result_stack.back();
  }

  pres_expression pop()
  {
    pres_expression result = top();
    result_stack.pop_back();
    return result;
  }

  void push_variables(const data::variable_list& variables)
  {
    for (const data::variable& v: variables)
    {
      parameters.id_generator.add_identifier(v.name());
    }
  }

  void pop_variables(const data::variable_list& variables)
  {
    for (const data::variable& v: variables)
    {
      parameters.id_generator.remove_identifier(v.name());
    }
  }

  bool is_timed() const
  {
    return parameters.T != data::undefined_real_variable();
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

  void apply(const state_formulas::minus&)
  {
    throw mcrl2::runtime_error("rhs_traverser: minus is not supported!");
  }

  void leave(const state_formulas::and_&)
  {
    pres_expression right = pop();
    pres_expression left = pop();
    push(tr::and_(left, right));
  }

  void leave(const state_formulas::or_&)
  {
    pres_expression right = pop();
    pres_expression left = pop();
    push(tr::or_(left, right));
  }

  void leave(const state_formulas::plus&)
  {
    pres_expression right = pop();
    pres_expression left = pop();
    push(tr::plus(left, right));
  }

  void apply(const state_formulas::const_multiply& x)
  {
    derived().enter(x);
    derived().apply(x.right());
    make_const_multiply(top(), x.left(), top());
    derived().leave(x);
  }

  void leave(const state_formulas::const_multiply&)
  {
    /* Do nothing */
  }

  void apply(const state_formulas::const_multiply_alt& x)
  {
    derived().enter(x);
    derived().apply(x.left());
    make_const_multiply_alt(top(), top(), x.right());
    derived().leave(x);
  }

  void leave(const state_formulas::const_multiply_alt&)
  {
    /* Do nothing */
  }

  void apply(const state_formulas::imp&)
  {
    throw mcrl2::runtime_error("rhs_traverser: implication is not supported!");
  }

  void apply(const state_formulas::forall& x)
  {
    derived().enter(x);
    push_variables(x.variables());
    derived().apply(x.body());
    make_infimum(top(), x.variables(), top());
    derived().leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    derived().enter(x);
    push_variables(x.variables());
    derived().apply(x.body());
    tr::make_supremum(top(), x.variables(), top());
    derived().leave(x);
  }

  void apply(const state_formulas::infimum& x)
  {
    derived().enter(x);
    push_variables(x.variables());
    derived().apply(x.body());
    make_infimum(top(), x.variables(), top());
    derived().leave(x);
  }

  void apply(const state_formulas::supremum& x)
  {
    derived().enter(x);
    push_variables(x.variables());
    derived().apply(x.body());
    tr::make_supremum(top(), x.variables(), top());
    derived().leave(x);
  }

  void apply(const state_formulas::sum& x)
  {
    derived().enter(x);
    push_variables(x.variables());
    derived().apply(x.body());
    tr::make_sum(top(), x.variables(), top());
    derived().leave(x);
  }

  // This function is overridden in the structured variant of the algorithm
  template <typename MustMayExpression>
  pres_expression apply_may_must_rhs(const MustMayExpression& x)
  {
    return RHS(x.operand(), parameters, TermTraits());
  }

  // This function is overridden in the structured variant of the algorithm
  pres_expression apply_may_must_result(const pres_expression& p)
  {
    return p;
  }

  // share code between must and may
  template <typename MustMayExpression>
  void apply_may_must(const MustMayExpression& x, bool is_must)
  {
    bool timed = is_timed();
    std::vector<pres_expression> v;
    pres_expression rhs_phi = derived().apply_may_must_rhs(x);
    assert(action_formulas::is_action_formula(x.formula()));
    const action_formulas::action_formula& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());

    for (const lps::stochastic_action_summand& summand: parameters.lps.action_summands())
    {
      const data::variable_list& yi            = summand.summation_variables();
      const data::data_expression& ci          = summand.condition();
      const lps::multi_action& ai              = summand.multi_action();
      const data::assignment_list& gi          = summand.assignments();
      const lps::stochastic_distribution& dist = summand.distribution();

      pres_expression right = rhs_phi;
      const data::data_expression& ti = ai.time();
      pres_expression sat = Sat(ai, alpha, parameters.id_generator, TermTraits());
      data::mutable_map_substitution<> sigma;
      for (const data::assignment& a: gi)
      {
        sigma[a.lhs()] = a.rhs();
      }
      pres_expression left = tr::and_(sat, ci);

      if (timed)
      {
        sigma[parameters.T] = ti;
        left = tr::and_(left, data::greater(ti, parameters.T));
      }

      right = pres_system::replace_variables_capture_avoiding(right, sigma);

      pres_expression p = parameters.rhs_may_must(is_must, yi, left, right, ai, gi, dist, TermTraits());
      v.push_back(derived().apply_may_must_result(p));
    }

    pres_expression result = is_must ? tr::join_and(v.begin(), v.end()) : tr::join_or(v.begin(), v.end());
    push(result);
  }

  void apply(const state_formulas::must& x)
  {
    apply_may_must(x, true);
  }

  void apply(const state_formulas::may& x)
  {
    apply_may_must(x, false);
  }

  void leave(const state_formulas::yaled&)
  {
    throw mcrl2::runtime_error("rhs_traverser: yaled is not supported!");
  }

  void leave(const state_formulas::yaled_timed& x)
  {
    const data::data_expression& t = x.time_stamp();
    std::vector<pres_expression> v;
    for (const lps::action_summand& i: parameters.lps.action_summands())
    {
      const data::data_expression& ci = i.condition();
      const data::data_expression& ti = i.multi_action().time();
      const data::variable_list&   yi = i.summation_variables();
      pres_expression p = tr::infimum(yi, tr::or_(data::not_(ci), data::greater(t, ti)));
      v.push_back(p);
    }
    for (const lps::deadlock_summand& j: parameters.lps.deadlock_summands())
    {
      const data::data_expression& cj = j.condition();
      const data::data_expression& tj = j.deadlock().time();
      const data::variable_list&   yj = j.summation_variables();
      pres_expression p = tr::infimum(yj, tr::or_(data::not_(cj), data::greater(t, tj)));
      v.push_back(p);
    }
    push(tr::and_(tr::join_or(v.begin(), v.end()), data::greater(t, parameters.T)));
  }

  void leave(const state_formulas::delay&)
  {
    throw mcrl2::runtime_error("rhs_traverser: delay is not supported!");
  }

  void leave(const state_formulas::delay_timed& x)
  {
    const data::data_expression& t = x.time_stamp();
    std::vector<pres_expression> v;
    for (const lps::action_summand& i : parameters.lps.action_summands())
    {
      const data::data_expression& ci = i.condition();
      data::data_expression ti = i.multi_action().time();
      const data::variable_list&   yi = i.summation_variables();
      pres_expression p = tr::supremum(yi, tr::and_(ci, data::less_equal(t, ti)));
      v.push_back(p);
    }
    for (const lps::deadlock_summand& j: parameters.lps.deadlock_summands())
    {
      const data::data_expression& cj = j.condition();
      data::data_expression tj = j.deadlock().time();
      const data::variable_list&   yj = j.summation_variables();
      pres_expression p = tr::supremum(yj, tr::and_(cj, data::less_equal(t, tj)));
      v.push_back(p);
    }
    push(tr::or_(tr::join_or(v.begin(), v.end()), data::less_equal(t, parameters.T)));
  }

  void leave(const state_formulas::variable& x)
  {
    const core::identifier_string& X = x.name();
    const data::data_expression_list& d = x.arguments();
    data::variable_list xp = parameters.lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), parameters.phi0);
    if (is_timed())
    {
      e.push_front(parameters.T);
    }
    push(propositional_variable_instantiation(X, e));
  }

  void apply(const state_formulas::nu& x)
  {
    const core::identifier_string& X = x.name();
    data::data_expression_list d = detail::mu_expressions(x);
    data::variable_list xp = parameters.lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), parameters.phi0);
    if (is_timed())
    {
      e.push_front(parameters.T);
    }
    push(propositional_variable_instantiation(X, e));
  }

  void apply(const state_formulas::mu& x)
  {
    const core::identifier_string& X = x.name();
    data::data_expression_list d = detail::mu_expressions(x);
    data::variable_list xp = parameters.lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), parameters.phi0);
    if (is_timed())
    {
      e.push_front(parameters.T);
    }
    push(propositional_variable_instantiation(X, e));
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_rhs_traverser: public Traverser<apply_rhs_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  using super = Traverser<apply_rhs_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_traverser(Parameters& parameters, TermTraits tr)
    : super(parameters, tr)
  {}
};

template <typename TermTraits, typename Parameters>
inline
pres_expression RHS(const state_formulas::state_formula& x, Parameters& parameters, TermTraits tr)
{
  apply_rhs_traverser<rhs_traverser, TermTraits, Parameters> f(parameters, tr);
  f.apply(x);
  return f.top();
}

//--- RHS_structured variant ---//

template <typename TermTraits, typename Parameters>
inline
typename TermTraits::term_type RHS_structured(const state_formulas::state_formula& x,
                                              Parameters& parameters,
                                              const data::variable_list& variables,
                                              const fixpoint_symbol& sigma,
                                              std::vector<pres_equation>& equations,
                                              TermTraits tr
                                             );

template <typename Derived, typename TermTraits, typename Parameters>
struct rhs_structured_traverser: public rhs_traverser<Derived, TermTraits, Parameters>
{
  using super = rhs_traverser<Derived, TermTraits, Parameters>;
  using tr = TermTraits;

  using super::enter;
  using super::leave;
  using super::apply;
  using super::push;
  using super::top;
  using super::pop;
  using super::is_timed;
  using super::parameters;
  using super::apply_may_must;
  using super::derived;

  std::multiset<data::variable> variables;
  const fixpoint_symbol& sigma;
  std::vector<pres_equation>& equations; // new equations that are generated on the fly

  rhs_structured_traverser(Parameters& parameters,
                           const data::variable_list& variables_,
                           const fixpoint_symbol& sigma_,
                           std::vector<pres_equation>& equations_,
                           TermTraits tr
               )
    : super(parameters, tr),
    	variables(variables_.begin(), variables_.end()),
    	sigma(sigma_),
    	equations(equations_)
  {}

  data::variable_list rhs_structured_compute_variables(const state_formulas::state_formula& x, const std::multiset<data::variable>& variables) const
  {
    std::set<data::variable> fv = state_formulas::find_free_variables(x);
    fv.insert(variables.begin(), variables.end());
    return data::variable_list(fv.begin(), fv.end());
  }

  void enter(const state_formulas::forall& x)
  {
    const data::variable_list& v = x.variables();
    variables.insert(v.begin(), v.end());
  }

  void leave(const state_formulas::forall& x)
  {
    for (const data::variable& var: x.variables())
    {
      variables.erase(var);
    }
  }

  void enter(const state_formulas::exists& x)
  {
    const data::variable_list& v = x.variables();
    variables.insert(v.begin(), v.end());
  }

  void leave(const state_formulas::exists& x)
  {
    for (const data::variable& var: x.variables())
    {
      variables.erase(var);
    }
  }

  void enter(const state_formulas::infimum& x)
  {
    const data::variable_list& v = x.variables();
    variables.insert(v.begin(), v.end());
  }

  void leave(const state_formulas::infimum& x)
  {
    for (const data::variable& var: x.variables())
    {
      variables.erase(var);
    }
  }

  void enter(const state_formulas::supremum& x)
  {
    const data::variable_list& v = x.variables();
    variables.insert(v.begin(), v.end());
  }

  void leave(const state_formulas::supremum& x)
  {
    for (const data::variable& var: x.variables())
    {
      variables.erase(var);
    }
  }

  void enter(const state_formulas::sum& x)
  {
    const data::variable_list& v = x.variables();
    variables.insert(v.begin(), v.end());
  }

  void leave(const state_formulas::sum& x)
  {
    for (const data::variable& var: x.variables())
    {
      variables.erase(var);
    }
  }

  // override
  template <typename MustMayExpression>
  pres_expression apply_may_must_rhs(const MustMayExpression& x)
  {
    return RHS_structured(x.operand(), parameters, rhs_structured_compute_variables(x.operand(), variables), sigma, equations, TermTraits());
  }

  // override
  pres_expression apply_may_must_result(const pres_expression& p)
  {
    // generate a new equation 'Y(d) = p', and add Y(d) to v
    core::identifier_string Y = parameters.id_generator("Y");
    data::variable_list d(variables.begin(), variables.end());
    propositional_variable Yd(Y, d);
    pres_equation eqn(sigma, Yd, p);
    equations.push_back(eqn);
    return propositional_variable_instantiation(Y, data::make_data_expression_list(d));
  }

  void apply(const state_formulas::must& x)
  {
    apply_may_must(x, true);
  }

  void apply(const state_formulas::may& x)
  {
    apply_may_must(x, false);
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_rhs_structured_traverser: public Traverser<apply_rhs_structured_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  using super = Traverser<apply_rhs_structured_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_structured_traverser(Parameters& parameters,
                                 const data::variable_list& variables,
                                 const fixpoint_symbol& sigma,
                                 std::vector<pres_equation>& equations,
                                 TermTraits tr
                                )
    : super(parameters, variables, sigma, equations, tr)
  {}
};

template <typename TermTraits, typename Parameters>
inline
typename TermTraits::term_type RHS_structured(const state_formulas::state_formula& x,
                                              Parameters& parameters,
                                              const data::variable_list& variables,
                                              const fixpoint_symbol& sigma,
                                              std::vector<pres_equation>& equations,
                                              TermTraits tr
                                             )
{
  apply_rhs_structured_traverser<rhs_structured_traverser, TermTraits, Parameters> f(parameters, variables, sigma, equations, tr);
  f.apply(x);
  return f.top();
}

} // namespace mcrl2::pres_system::detail





#endif // MCRL2_PRES_DETAIL_LPS2PRES_RHS_H
