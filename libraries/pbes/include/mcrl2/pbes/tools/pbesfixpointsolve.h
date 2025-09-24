// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesfixpointsolve.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESFIXPOINTSOLVE_H
#define MCRL2_PBES_TOOLS_PBESFIXPOINTSOLVE_H

#include "mcrl2/data/detail/prover/bdd_prover.h"
#include "mcrl2/data/merge_data_specifications.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/iteration_builders.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewriters/pbes2data_rewriter.h"
#include "mcrl2/smt/solver.h"

#include <map>
#include <optional>



namespace mcrl2::pbes_system
{

struct pbesfixpointsolve_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;

  bool check_global_invariant = false;
  bool smt = false;
  bool early_stopping = false;
};

inline void substitute(pbes_equation& into,
    const pbes_equation& by,
    detail::substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter)
{
  substituter.set_equation(by);
  substituter.set_name(into.variable().name());
  pbes_expression p;
  substituter.apply(p, into.formula());
  into.formula() = p;
}

inline data::data_expression pbestodata(pbes_equation& equation,
    detail::replace_other_propositional_variables_with_functions_builder<pbes_system::pbes_expression_builder>&
        replace_substituter)
{
  pbes_expression expr;
  replace_substituter.set_forward(true);
  replace_substituter.set_name(equation.variable().name());
  replace_substituter.apply(expr, equation.formula());

  data::data_expression data_expr = detail::pbes2data(expr);

  replace_substituter.set_forward(false);
  replace_substituter.apply(expr, equation.formula());
  return data_expr;
}

enum class InvResult
{
  INV_TRUE,
  INV_FALSE,
  EQ_FALSE
};

// Equation should be in full form.
inline InvResult global_invariant_check(pbes_equation& equation,
    detail::substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter,
    detail::replace_other_propositional_variables_with_functions_builder<pbes_system::pbes_expression_builder>&
        replace_substituter,
    data::data_specification data_spec,
    simplify_data_rewriter<data::rewriter> pbes_rewriter,
    std::set<data::variable>& global_variables)
{
  mCRL2log(log::debug) << "INV: " << equation.symbol() << "\n";
  if (equation.symbol().is_mu())
  {
    return InvResult::INV_FALSE;
  }

  // Base check: does it contain the right pvi
  std::set<propositional_variable_instantiation> set = find_propositional_variable_instantiations(equation.formula());
  if (set.size() == 0)
  {
    mCRL2log(log::debug) << "INV: " << equation.variable().name() << " does not contain any pvi." << "\n";
    return InvResult::INV_TRUE;
  }

  if (std::any_of(set.begin(),
          set.end(),
          [=](const propositional_variable_instantiation& v) { return v.name() != equation.variable().name(); }))
  {
    // The formula contains another pvi than the current equation, so skip
    mCRL2log(log::debug) << "INV: " << equation.variable().name() << " contains other pvi." << "\n";
    return InvResult::INV_FALSE;
  }

  // Calculate CC
  pbes_equation eq;
  eq.formula() = true_();
  eq.symbol() = equation.symbol();
  eq.variable() = equation.variable();
  substituter.set_equation(eq);
  substituter.set_name(equation.variable().name());
  pbes_expression cc;
  substituter.apply(cc, equation.formula());
  cc = pbes_rewrite(cc, pbes_rewriter);

  bool global_invariant = false;

  detail::substitute_propositional_variables_for_bools_builder<pbes_system::pbes_expression_builder> bool_substituter(
      pbes_rewriter);

  mcrl2::data::detail::BDD_Prover f_bdd_prover(data_spec, data::used_data_equation_selector(data_spec));
  mCRL2log(log::verbose) << "INV: PVI set size " << set.size() << "\n";
  int i = 0;
  for (const propositional_variable_instantiation& pvi: set)
  {
    if (i % 5 == 0)
    {
      mCRL2log(log::verbose) << "INV: PVI " << i << "begin added now. \n";
    }
    i++;
    // Calculate CC_i
    data::mutable_indexed_substitution sigma;
    data::data_expression_list pars = pvi.parameters();
    for (const data::variable& v : equation.variable().parameters())
    {
      data::data_expression par = pars.front();
      pars.pop_front();
      if (par.sort() != v.sort())
      {
        // Parameters do not match with variables. Ignore this substitution.
        mCRL2log(log::verbose) << "INV: No param match for " << v.name() << "\n";
        return InvResult::INV_FALSE;
      }
      sigma[v] = par;
    }
    pbes_expression cc_i = pbes_rewrite(cc, pbes_rewriter, sigma);

    // Calculate CC and C_i
    bool_substituter.set_pvi(pvi);
    bool_substituter.set_replacement(false_());
    pbes_expression c_i;
    bool_substituter.apply(c_i, equation.formula());
    c_i = pbes_rewrite(c_i, pbes_rewriter);

    pbes_equation cc_i_eq;
    cc_i_eq.formula() = cc_i;
    data::data_expression cc_i_data = pbestodata(cc_i_eq, replace_substituter);
    pbes_equation c_i_eq;
    c_i_eq.formula() = c_i;
    data::data_expression c_i_data = pbestodata(c_i_eq, replace_substituter);

    f_bdd_prover.set_formula(c_i_data);
    data::detail::Answer c_i_is_contradiction = f_bdd_prover.is_contradiction();
    if (c_i_is_contradiction == data::detail::answer_yes)
    {
      mCRL2log(log::verbose) << "Core constraint is not simple (contains pvi)." << std::endl;
      return InvResult::INV_FALSE;
    }

    // Check CC and C_i implies CC_i
    data::data_expression bdd_expr = data::imp(c_i_data, cc_i_data);
    // Add exists
    for (const data::variable& v : global_variables)
    {
      bdd_expr = make_exists_(data::variable_list({v}), bdd_expr);
    }

    f_bdd_prover.set_formula(bdd_expr);
    mCRL2log(log::verbose) << "INV?: " << std::endl;

    data::detail::Answer v_is_tautology = f_bdd_prover.is_tautology();
    data::detail::Answer v_is_contradiction = f_bdd_prover.is_contradiction();
    if (v_is_contradiction == data::detail::answer_yes)
    {
      mCRL2log(log::verbose) << "Contradiction for inv checker." << std::endl;
      equation.formula() = false_();
      return InvResult::EQ_FALSE;
    }
    else if (v_is_tautology == data::detail::answer_no || v_is_tautology == data::detail::answer_undefined)
    {
      mCRL2log(log::verbose) << "Found some transition that is not an invariant" << std::endl;
      return InvResult::INV_FALSE;
    }
    else if (v_is_tautology == data::detail::answer_yes)
    {
      mCRL2log(log::verbose) << "This transition is true" << std::endl;
      global_invariant = true;
    }
    else
    {
      mCRL2log(log::error) << "No contradiction and no tautology" << std::endl;
      throw mcrl2::runtime_error("TODO");
    }
  }

  mCRL2log(log::info) << "Is global invariant? " << global_invariant << std::endl;
  if (global_invariant)
  {
    equation.formula() = cc;
    return InvResult::INV_TRUE;
  }
  return InvResult::INV_FALSE;
}

inline void perform_iteration(pbes_equation& equation,
    detail::substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter,
    detail::replace_other_propositional_variables_with_functions_builder<pbes_system::pbes_expression_builder>&
        replace_substituter,
    data::data_specification data_spec,
    pbesfixpointsolve_options options,
    propositional_variable_instantiation initial_state)
{
  std::optional<smt::smt_solver> solv;
  std::optional<mcrl2::data::detail::BDD_Prover> f_bdd_prover;
  if (options.smt)
  {
    solv.emplace(data_spec);
  }
  else
  {
    f_bdd_prover.emplace(data_spec, data::used_data_equation_selector(data_spec));
  }
  pbes_equation eq;
  if (equation.symbol().is_nu())
  {
    eq.formula() = true_();
  }
  else
  {
    eq.formula() = false_();
  }
  eq.symbol() = equation.symbol();
  eq.variable() = equation.variable();
  bool stable = false;
  int i = 0;
  while (!stable)
  {
    mCRL2log(log::info) << eq.variable().name() << ":  " << i << std::endl;

    // Apply substitution
    substituter.set_equation(eq);
    substituter.set_name(equation.variable().name());
    pbes_expression p;
    substituter.apply(p, equation.formula());

    // Check if the iteration has reached a fixpoint
    pbes_equation p_eq;
    p_eq.formula() = p;
    p_eq.variable() = eq.variable();
    replace_substituter.reset_variable_list();
    data::data_expression eq_data = pbestodata(eq, replace_substituter);
    data::data_expression p_data = pbestodata(p_eq, replace_substituter);
    data::data_expression formula;
    // Due to mononicity, only one way implication needs to be tested
    if (equation.symbol().is_nu())
    {
      formula = data::imp(eq_data, p_data);
    }
    else
    {
      formula = data::imp(p_data, eq_data);
    }

    if (options.smt)
    {
      data::variable_list var_list = p_eq.variable().parameters();
      for (const auto& x : as_set(replace_substituter.get_variable_list()))
      {
        var_list.push_front(x);
      }
      mCRL2log(log::verbose) << eq.variable().name() << ": SMT check for " << i << " started." << std::endl;
      smt::answer result = solv->solve(var_list, data::not_(formula), std::chrono::seconds(0));
      switch (result)
      {
      case smt::answer::UNSAT:
        mCRL2log(log::info) << eq.variable().name() << ": iteration " << i << " is equal to " << i + 1 << std::endl;
        stable = true;
        break;
      case smt::answer::SAT:
        break;
      case smt::answer::UNKNOWN:
        break;
      }
    }
    else
    {
      f_bdd_prover->set_formula(formula);
      mCRL2log(log::verbose) << eq.variable().name() << ": EQ-BDD check for " << i << " started." << std::endl;
      data::detail::Answer v_is_tautology = f_bdd_prover->is_tautology();
      if (v_is_tautology == data::detail::answer_yes)
      {
        mCRL2log(log::info) << eq.variable().name() << ": iteration " << i << " is equal to " << i + 1 << std::endl;
        stable = true;
      }
    }

    // Stop iterating if (mu) the equation is true for the initial variable or (nu) the equation is false for the
    // initial variable
    if (!stable && options.early_stopping && initial_state.name() == eq.variable().name())
    {
      pbes_equation init_eq = eq;
      init_eq.formula() = initial_state;
      substitute(init_eq, eq, substituter);
      data::data_expression init_data = pbestodata(init_eq, replace_substituter);

      if (options.smt)
      {
        mCRL2log(log::verbose) << eq.variable().name() << ": SMT init check for " << i << " started." << std::endl;
        data::data_expression smt_data = init_eq.symbol().is_mu() ? data::not_(init_data) : init_data;
        smt::answer result = solv->solve({}, smt_data, std::chrono::seconds(0));
        switch (result)
        {
        case smt::answer::UNSAT:
          mCRL2log(log::info) << eq.variable().name() << ": iteration " << i << " is equal to " << i + 1 << std::endl;
          stable = true;
          break;
        case smt::answer::SAT:
          break;
        case smt::answer::UNKNOWN:
          break;
        }
      }
      else
      {
        f_bdd_prover->set_formula(init_data);
        mCRL2log(log::verbose) << eq.variable().name() << ": EQ-BDD init check for " << i << " started." << std::endl;
        data::detail::Answer v_is_tautology = f_bdd_prover->is_tautology();
        data::detail::Answer v_is_contradiction = f_bdd_prover->is_contradiction();

        mCRL2log(log::verbose) << eq.variable().name() << ": EQ-BDD tauto? "
                               << (v_is_tautology == data::detail::answer_yes) << std::endl;
        mCRL2log(log::verbose) << eq.variable().name() << ": EQ-BDD contra? "
                               << (v_is_contradiction == data::detail::answer_yes) << std::endl;
        if (init_eq.symbol().is_mu() ? v_is_tautology == data::detail::answer_yes
                                     : v_is_contradiction == data::detail::answer_yes)
        {
          mCRL2log(log::info) << eq.variable().name() << ": iteration " << i << " is equal to " << i + 1 << std::endl;
          stable = true;
        }
      }
    }

    if (stable)
    {
      break;
    }
    eq.formula() = p;
    i++;
  }

  equation.formula() = eq.formula();
}

struct pbesfixpointsolve_pbes_fixpoint_iterator
{
  void run(pbes& p, pbesfixpointsolve_options options)
  {
    data::rewriter data_rewriter(p.data(), options.rewrite_strategy);
    simplify_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);
    detail::substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(pbes_rewriter);

    detail::replace_other_propositional_variables_with_functions_builder<pbes_system::pbes_expression_builder>
        replace_substituter(pbes_rewriter);

    for (std::vector<pbes_equation>::reverse_iterator i = p.equations().rbegin(); i != p.equations().rend(); i++)
    {
      mCRL2log(log::verbose) << "Investigating the equation for " << i->variable().name() << "\n";
      if (options.check_global_invariant)
      {
        InvResult global_inv = global_invariant_check(*i,
            substituter,
            replace_substituter,
            p.data(),
            pbes_rewriter,
            p.global_variables());
        if (global_inv == InvResult::INV_FALSE)
        {
          perform_iteration(*i, substituter, replace_substituter, p.data(), options, p.initial_state());
        }
      }
      else
      {
        perform_iteration(*i, substituter, replace_substituter, p.data(), options, p.initial_state());
      }

      for (std::vector<pbes_equation>::reverse_iterator j = i + 1; j != p.equations().rend(); j++)
      {
        substitute(*j, *i, substituter);
      }
    }
  }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_TOOLS_PBESFIXPOINTSOLVE_H
