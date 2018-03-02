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

  template <typename TermTraits>
  pbes_expression rhs_may_must(bool is_must,
                               const pbes_expression& left,  // Sat(a(x), alpha)
                               const pbes_expression& right, // RHS(phi, t)
                               std::size_t, /* i */
                               const lps::multi_action& /* a */,
                               lts2pbes_state_type /* s */,
                               lts2pbes_state_type /* t */,
                               TermTraits
                              )
  {
    typedef TermTraits tr;
    if (is_must)
    {
      return tr::imp(left, right);
    }
    else
    {
      return tr::and_(left, right);
    }
  }
};

// TODO: reuse code from lps2pbes_counter_example_parameters
struct lts2pbes_counter_example_parameters: public lts2pbes_parameters
{
  std::vector<propositional_variable> Zpos; // represents the additional equations { nu Zpos_i(s, a(i), s) = true }
  std::vector<propositional_variable> Zneg; // represents the additional equations { nu Zneg_i(s, a(i), t) = false }
  data::variable_list s_list;                               // contains the process parameter s: Nat
  data::variable_list t_list;                               // contains the process parameter t: Nat

  // creates variables corresponding to the action label sorts in actions
  data::variable_list action_variables(const process::action_list& actions) const
  {
    std::vector<data::variable> result;
    for (const process::action& a: actions)
    {
      for (const data::sort_expression& s: a.label().sorts())
      {
        data::variable v(id_generator("v"), s);
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
      pbes_equation eqn(fixpoint_symbol::nu(), p, false_());
      result.push_back(eqn);
    }
    for (auto const& p: Zpos)
    {
      pbes_equation eqn(fixpoint_symbol::nu(), p, true_());
      result.push_back(eqn);
    }
    return result;
  }

  std::string multi_action_name(const lps::multi_action& a) const
  {
    std::vector<std::string> v;
    for (const process::action& ai: a.actions())
    {
      v.emplace_back(ai.label().name());
    }
    return utilities::string_join(v, "_");
  }

  lts2pbes_counter_example_parameters(const state_formulas::state_formula& phi0,
                                      const lts::lts_lts_t& lts0,
                                      const lts2pbes_lts& lts1,
                                      data::set_identifier_generator& id_generator,
                                      utilities::progress_meter& pm
                                     )
    : lts2pbes_parameters(phi0, lts0, lts1, id_generator, pm),
      s_list { data::variable(id_generator("s"), data::sort_nat::nat()) },
      t_list { data::variable(id_generator("t"), data::sort_nat::nat()) }
  {
    for (const auto& p: lts1.state_map())
    {
      for (const lts2pbes_lts::edge& e: p.second)
      {
        const lps::multi_action& ai = lts1.action_labels()[e.label];
        data::variable_list actvars = action_variables(ai.actions());
        std::string suffix = utilities::number2string(e.index) + "_" + multi_action_name(ai);
        core::identifier_string pos = id_generator("Zpos_" + suffix);
        core::identifier_string neg = id_generator("Zneg_" + suffix);
        Zpos.push_back(propositional_variable(pos, s_list + actvars + t_list));
        Zneg.push_back(propositional_variable(neg, s_list + actvars + t_list));
      }
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
                               const pbes_expression& left,  // Sat(a(x), alpha)
                               const pbes_expression& right, // RHS(phi, t)
                               std::size_t i,                // The index of the transition s --a--> t
                               const lps::multi_action& a,
                               lts2pbes_state_type s,
                               lts2pbes_state_type t,
                               TermTraits
                              )
  {
    typedef TermTraits tr;
    auto f = action_expressions(a.actions());
    data::data_expression_list dx = data::data_expression_list({ data::sort_nat::nat(s) }) + f + data::data_expression_list({ data::sort_nat::nat(t) });
    propositional_variable_instantiation Pos(Zpos[i].name(), dx);
    propositional_variable_instantiation Neg(Zneg[i].name(), dx);
    auto right1 = right;

    if (is_must)
    {
      right1 = tr::or_(tr::and_(right, Pos), Neg);
      return tr::imp(left, right1);
    }
    else
    {
      right1 = tr::and_(tr::or_(right, Neg), Pos);
      return tr::and_(left, right1);
    }
  }
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
    const auto& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());
    const state_formulas::state_formula& phi = x.operand();

    // traverse all transitions s --a--> t
    for (const lts2pbes_lts::edge& e: lts1.edges(s))
    {
      lts2pbes_state_type t = e.state;
      const lps::multi_action& a = lts1.action_labels()[e.label];
      pbes_expression left = detail::Sat(a, alpha, parameters.id_generator, TermTraits());
      pbes_expression right = RHS(phi, t, parameters, TermTraits());
      pbes_expression p = parameters.rhs_may_must(true, left, right, e.index, a, s, t, TermTraits());
      v.push_back(p);
    }
    push(tr::join_and(v.begin(), v.end()));
  }

  void apply(const state_formulas::may& x)
  {
    const auto& lts1 = parameters.lts1;
    std::vector<pbes_expression> v;
    assert(action_formulas::is_action_formula(x.formula()));
    const auto& alpha = atermpp::down_cast<const action_formulas::action_formula>(x.formula());
    const state_formulas::state_formula& phi = x.operand();

    // traverse all transitions s --a--> t
    for (const lts2pbes_lts::edge& e: lts1.edges(s))
    {
      lts2pbes_state_type t = e.state;
      const lps::multi_action& a = lts1.action_labels()[e.label];
      pbes_expression left = detail::Sat(a, alpha, parameters.id_generator, TermTraits());
      pbes_expression right = RHS(phi, t, parameters, TermTraits());
      pbes_expression p = parameters.rhs_may_must(false, left, right, e.index, a, s, t, TermTraits());
      v.push_back(p);
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
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    const data::data_expression_list& e = x.arguments();
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }

  void apply(const state_formulas::nu& x)
  {
    const core::identifier_string& X = x.name();
    core::identifier_string X_s = make_identifier(X, s);
    data::data_expression_list e = detail::mu_expressions(x);
    push(propositional_variable_instantiation(X_s, e + detail::Par(X, data::variable_list(), parameters.phi0)));
  }

  void apply(const state_formulas::mu& x)
  {
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
