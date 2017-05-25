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
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/assignment_sequence_substitution.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/replace.h"
#include "mcrl2/modal_formula/find.h"
#include "mcrl2/modal_formula/traverser.h"
#include "mcrl2/pbes/detail/lps2pbes_par.h"
#include "mcrl2/pbes/detail/lps2pbes_sat.h"
#include "mcrl2/pbes/detail/lps2pbes_utility.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/replace.h"
#include "mcrl2/utilities/text_utility.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct lps2pbes_parameters
{
  const state_formulas::state_formula& phi0; // the original formula
  const lps::linear_process& lps;
  data::set_identifier_generator& id_generator;
  const data::variable& T;

  lps2pbes_parameters(const state_formulas::state_formula& phi0_,
                      const lps::linear_process& lps_,
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
  pbes_expression rhs_may_must(bool is_must,
                               const data::variable_list& y,
                               const pbes_expression& left,  // Sat(ai(fi(x,y)) && ci(x,y) && (ti(x,y) > T)
                               const pbes_expression& right, // RHS(phi)[T, x := ti(x,y), gi(x,y)]
                               const lps::multi_action& /* ai */,
                               const data::assignment_list& /* gi */,
                               TermTraits
                              )
  {
    typedef TermTraits tr;
    if (is_must)
    {
      return tr::forall(y, tr::imp(left, right));
    }
    else
    {
      return tr::exists(y, tr::and_(left, right));
    }
  }
};

struct lps2pbes_counter_example_parameters: public lps2pbes_parameters
{
  data::variable_list d1;                                   // d'
  data::mutable_map_substitution<> sigma;                   // the substitution [d := d'], where d is the sequence of process parameters of lps
  std::set<data::variable> sigma_variables;
  std::map<lps::multi_action, propositional_variable> Zpos; // represents the additional equations { nu Zpos_ai(d, fi(x,y), d') = true }
  std::map<lps::multi_action, propositional_variable> Zneg; // represents the additional equations { nu Zneg_ai(d, fi(x,y), d') = false }

  // creates variables corresponding to the action label sorts in actions
  data::variable_list action_variables(const process::action_list& actions) const
  {
    std::vector<data::variable> result;
    for (const process::action& a: actions)
    {
      for (const data::sort_expression& s: a.label().sorts())
      {
        data::variable v(id_generator("var"), s);
        result.push_back(v);
      }
    }
    return data::variable_list(result.begin(), result.end());
  }

  // returns the concatenation of the arguments of the list of actions
  data::data_expression_list action_expressions(const process::action_list& actions) const
  {
    std::vector<data::data_expression> result;
    for (const process::action& a: actions)
    {
      auto const& args = a.arguments();
      result.insert(result.end(), args.begin(), args.end());
    }
    return data::data_expression_list(result.begin(), result.end());
  }

  // returns the equations needed for counter example generation
  std::vector<pbes_equation> equations() const
  {
    std::vector<pbes_equation> result;
    for (auto const& p: Zneg)
    {
      pbes_equation eqn(fixpoint_symbol::nu(), p.second, false_());
      result.push_back(eqn);
    }
    for (auto const& p: Zpos)
    {
      pbes_equation eqn(fixpoint_symbol::nu(), p.second, true_());
      result.push_back(eqn);
    }
    return result;
  }

  std::string multi_action_name(const lps::multi_action& a) const
  {
    std::vector<std::string> v;
    for (const process::action& ai: a.actions())
    {
      v.push_back(std::string(ai.label().name()));
    }
    return utilities::string_join(v, "_");
  }

  lps2pbes_counter_example_parameters(const state_formulas::state_formula& phi0,
                                      const lps::linear_process& lps,
                                      data::set_identifier_generator& id_generator,
                                      const data::variable& T
                                     )
    : lps2pbes_parameters(phi0, lps, id_generator, T)
  {
    const data::variable_list& d = lps.process_parameters();
    sigma = detail::make_fresh_variable_substitution(d, id_generator);
    sigma_variables = data::substitution_variables(sigma);
    d1 = data::replace_variables(d, sigma);

    for (const lps::action_summand& summand: lps.action_summands())
    {
      const lps::multi_action& ai = summand.multi_action();
      core::identifier_string pos = id_generator("Z" + multi_action_name(ai) + "_pos");
      core::identifier_string neg = id_generator("Z" + multi_action_name(ai) + "_neg");
      Zpos[ai] = propositional_variable(pos, d + action_variables(ai.actions()) + d1);
      Zneg[ai] = propositional_variable(neg, d + action_variables(ai.actions()) + d1);
    }
  }

  data::data_expression equal_to(const data::variable_list& d, const data::data_expression_list& e) const
  {
    std::vector<data::data_expression> v;
    auto i = d.begin();
    auto j = e.begin();
    for (; i != d.end(); ++i, ++j)
    {
      v.push_back(data::equal_to(*i, *j));
    }
    return data::lazy::join_and(v.begin(), v.end());
  }

  template <typename TermTraits>
  pbes_expression rhs_may_must(bool is_must,
                               const data::variable_list& y,
                               const pbes_expression& left,  // Sat(ai(fi(x,y)) && ci(x,y) && (ti(x,y) > T)
                               const pbes_expression& right, // RHS(phi)[T, x := ti(x,y), gi(x,y)]
                               const lps::multi_action& ai,
                               const data::assignment_list& gi,
                               TermTraits
                              )
  {
    typedef TermTraits tr;
    const data::variable_list& d = lps.process_parameters();
    data::data_expression_list gi1 = data::replace_variables(atermpp::container_cast<data::data_expression_list>(d), data::assignment_sequence_substitution(gi));
    auto fi = action_expressions(ai.actions());
    data::data_expression_list da = atermpp::container_cast<data::data_expression_list>(d) + fi + gi1;
    propositional_variable_instantiation Pos(Zpos.at(ai).name(), da);
    propositional_variable_instantiation Neg(Zneg.at(ai).name(), da);
    auto right1 = right;

    if (is_must)
    {
      right1 = tr::or_(tr::and_(right, Pos), Neg);
      return tr::forall(y, tr::imp(left, right1));
    }
    else
    {
      right1 = tr::and_(tr::or_(right, Neg), Pos);
      return tr::exists(y, tr::and_(left, right1));
    }
  }
};

//--- RHS default variant ---//

template <typename TermTraits, typename Parameters>
pbes_expression RHS(const state_formulas::state_formula& x, Parameters& parameters, TermTraits tr);

template <typename Derived, typename TermTraits, typename Parameters>
struct rhs_traverser: public state_formulas::state_formula_traverser<Derived>
{
  typedef state_formulas::state_formula_traverser<Derived> super;
  typedef TermTraits tr;
  typedef typename tr::term_type pbes_expression;

  using super::enter;
  using super::leave;
  using super::apply;

  Parameters& parameters;
  std::vector<pbes_expression> result_stack;

  rhs_traverser(Parameters& parameters_, TermTraits)
    : parameters(parameters_)
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
    throw mcrl2::runtime_error("rhs_traverser: implication is not supported!");
  }

  void apply(const state_formulas::forall& x)
  {
    derived().enter(x);
    push_variables(x.variables());
    derived().apply(x.body());
    top() = tr::forall(x.variables(), top());
    //pop_variables(x.variables());
    derived().leave(x);
  }

  void apply(const state_formulas::exists& x)
  {
    derived().enter(x);
    push_variables(x.variables());
    derived().apply(x.body());
    top() = tr::exists(x.variables(), top());
    //pop_variables(x.variables());
    derived().leave(x);
  }

  // This function is overridden in the structured variant of the algorithm
  template <typename MustMayExpression>
  pbes_expression apply_may_must_rhs(const MustMayExpression& x)
  {
    return RHS(x.operand(), parameters, TermTraits());
  }

  // This function is overridden in the structured variant of the algorithm
  pbes_expression apply_may_must_result(const pbes_expression& p)
  {
    return p;
  }

  // share code between must and may
  template <typename MustMayExpression>
  void apply_may_must(const MustMayExpression& x, bool is_must)
  {
    bool timed = is_timed();
    std::vector<pbes_expression> v;
    pbes_expression rhs_phi = derived().apply_may_must_rhs(x);
    assert(action_formulas::is_action_formula(x.formula()));
    const action_formulas::action_formula& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());

    for (const lps::action_summand& summand: parameters.lps.action_summands())
    {
      const data::data_expression& ci = summand.condition();
      const lps::multi_action& ai     = summand.multi_action();
      const data::assignment_list& gi = summand.assignments();
      const data::variable_list& yi   = summand.summation_variables();

      pbes_expression right = rhs_phi;
      const data::data_expression& ti = ai.time();
      pbes_expression sat = Sat(ai, alpha, parameters.id_generator, TermTraits());
      data::mutable_map_substitution<> sigma;
      for (const data::assignment& a: gi)
      {
        sigma[a.lhs()] = a.rhs();
      }
      pbes_expression left = tr::and_(sat, ci);

      if (timed)
      {
        sigma[parameters.T] = ti;
        left = tr::and_(left, data::greater(ti, parameters.T));
      }

      right = pbes_system::replace_variables_capture_avoiding(right, sigma, data::substitution_variables(sigma));

      pbes_expression p = parameters.rhs_may_must(is_must, yi, left, right, ai, gi, TermTraits());
      v.push_back(derived().apply_may_must_result(p));
    }

    pbes_expression result = is_must ? tr::join_and(v.begin(), v.end()) : tr::join_or(v.begin(), v.end());
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
    std::vector<pbes_expression> v;
    for (const lps::action_summand& i: parameters.lps.action_summands())
    {
      const data::data_expression& ci = i.condition();
      const data::data_expression& ti = i.multi_action().time();
      const data::variable_list&   yi = i.summation_variables();
      pbes_expression p = tr::forall(yi, tr::or_(data::sort_bool::not_(ci), data::greater(t, ti)));
      v.push_back(p);
    }
    for (const lps::deadlock_summand& j: parameters.lps.deadlock_summands())
    {
      const data::data_expression& cj = j.condition();
      const data::data_expression& tj = j.deadlock().time();
      const data::variable_list&   yj = j.summation_variables();
      pbes_expression p = tr::forall(yj, tr::or_(data::sort_bool::not_(cj), data::greater(t, tj)));
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
    std::vector<pbes_expression> v;
    for (const lps::action_summand& i : parameters.lps.action_summands())
    {
      const data::data_expression& ci = i.condition();
      data::data_expression ti = i.multi_action().time();
      const data::variable_list&   yi = i.summation_variables();
      pbes_expression p = tr::exists(yi, tr::and_(ci, data::less_equal(t, ti)));
      v.push_back(p);
    }
    for (const lps::deadlock_summand& j: parameters.lps.deadlock_summands())
    {
      const data::data_expression& cj = j.condition();
      data::data_expression tj = j.deadlock().time();
      const data::variable_list&   yj = j.summation_variables();
      pbes_expression p = tr::exists(yj, tr::and_(cj, data::less_equal(t, tj)));
      v.push_back(p);
    }
    push(tr::or_(tr::join_or(v.begin(), v.end()), data::less_equal(t, parameters.T)));
  }

  void leave(const state_formulas::variable& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    const data::data_expression_list& d = x.arguments();
    data::variable_list xp = parameters.lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), parameters.phi0);
    if (is_timed())
    {
      e = parameters.T + e;
    }
    push(propositional_variable_instantiation(X, e));
  }

  void apply(const state_formulas::nu& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    data::data_expression_list d = detail::mu_expressions(x);
    data::variable_list xp = parameters.lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), parameters.phi0);
    if (is_timed())
    {
      e = parameters.T + e;
    }
    push(propositional_variable_instantiation(X, e));
  }

  void apply(const state_formulas::mu& x)
  {
    using atermpp::detail::operator+;
    const core::identifier_string& X = x.name();
    data::data_expression_list d = detail::mu_expressions(x);
    data::variable_list xp = parameters.lps.process_parameters();
    data::data_expression_list e = d + xp + Par(X, data::variable_list(), parameters.phi0);
    if (is_timed())
    {
      e = parameters.T + e;
    }
    push(propositional_variable_instantiation(X, e));
  }
};

template <template <class, class, class> class Traverser, typename TermTraits, typename Parameters>
struct apply_rhs_traverser: public Traverser<apply_rhs_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters>
{
  typedef Traverser<apply_rhs_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_traverser(Parameters& parameters, TermTraits tr)
    : super(parameters, tr)
  {}
};

template <typename TermTraits, typename Parameters>
inline
pbes_expression RHS(const state_formulas::state_formula& x, Parameters& parameters, TermTraits tr)
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
                                              std::vector<pbes_equation>& equations,
                                              TermTraits tr
                                             );

template <typename Derived, typename TermTraits, typename Parameters>
struct rhs_structured_traverser: public rhs_traverser<Derived, TermTraits, Parameters>
{
  typedef rhs_traverser<Derived, TermTraits, Parameters> super;
  typedef TermTraits tr;

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
  std::vector<pbes_equation>& equations; // new equations that are generated on the fly

  rhs_structured_traverser(Parameters& parameters,
                           const data::variable_list& variables_,
                           const fixpoint_symbol& sigma_,
                           std::vector<pbes_equation>& equations_,
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

  // override
  template <typename MustMayExpression>
  pbes_expression apply_may_must_rhs(const MustMayExpression& x)
  {
    return RHS_structured(x.operand(), parameters, rhs_structured_compute_variables(x.operand(), variables), sigma, equations, TermTraits());
  }

  // override
  pbes_expression apply_may_must_result(const pbes_expression& p)
  {
    // generate a new equation 'Y(d) = p', and add Y(d) to v
    core::identifier_string Y = parameters.id_generator("Y");
    data::variable_list d(variables.begin(), variables.end());
    propositional_variable Yd(Y, d);
    pbes_equation eqn(sigma, Yd, p);
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
  typedef Traverser<apply_rhs_structured_traverser<Traverser, TermTraits, Parameters>, TermTraits, Parameters> super;
  using super::enter;
  using super::leave;
  using super::apply;

  apply_rhs_structured_traverser(Parameters& parameters,
                                 const data::variable_list& variables,
                                 const fixpoint_symbol& sigma,
                                 std::vector<pbes_equation>& equations,
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
                                              std::vector<pbes_equation>& equations,
                                              TermTraits tr
                                             )
{
  apply_rhs_structured_traverser<rhs_structured_traverser, TermTraits, Parameters> f(parameters, variables, sigma, equations, tr);
  f.apply(x);
  return f.top();
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_LPS2PBES_RHS_H
