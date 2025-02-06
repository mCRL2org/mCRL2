// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesiteration.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESITERATION_H
#define MCRL2_PBES_TOOLS_PBESITERATION_H

#include "mcrl2/data/detail/prover/bdd_prover.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewriters/pbes2data_rewriter.h"

#include <map>

namespace mcrl2
{

namespace pbes_system
{

struct pbesiteration_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;

  bool check_global_invariant = false;
};

// Substitutor to replace predicate variables OTHER than the self-referencing onces to boolean variables
template <template <class> class Builder>
struct replace_propositional_variables_builder : public Builder<replace_propositional_variables_builder<Builder>>
{
  typedef Builder<replace_propositional_variables_builder<Builder>> super;
  using super::apply;

  core::identifier_string name;
  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  data::data_specification data_spec;
  std::map<data::variable, propositional_variable_instantiation> m_instantiations;
  bool forward = true;

  explicit replace_propositional_variables_builder(simplify_data_rewriter<data::rewriter>& r)
      : m_pbes_rewriter(r)
  {}

  void set_name(const core::identifier_string& s) { name = s; }

  void set_forward(bool b) { forward = b; }

  void reset_instantiations() { m_instantiations.clear(); }
  std::map<data::variable, propositional_variable_instantiation> instantiations() const { return m_instantiations; }

  template <class T>
  void apply(T& result, const pbes_expression& d)
  {
    if (forward)
    {
      super::apply(result, d);
      return;
    }
    if (!data::is_variable(d))
    {
      super::apply(result, d);
      return;
    }

    auto it = m_instantiations.find(atermpp::down_cast<data::variable>(d));
    if (!forward && it != m_instantiations.end())
    {
      propositional_variable_instantiation pvi = it->second;
      result = pvi;
      m_instantiations.erase(it);
      return;
    }
    result = d;
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (forward && x.name() != name)
    {
      // Unsound probably!
      mCRL2log(log::error) << "Formula contains other (unsolved) PVI instances in the current equation " << std::endl;
      data::data_expression_list params = x.parameters();
      data::sort_expression_list sort_list(params);
      data::sort_expression asdf =  data::function_sort(sort_list, data::bool_());
      data::function_symbol vb1(x.name(), asdf);
      result = vb1;
      return;
    }
    result = x;
  }
};

// Substitutor to target specific path, replace our specific pvi with a substituted CC
template <template <class> class Builder>
struct substitute_propositional_variables_for_bools_builder
    : public Builder<substitute_propositional_variables_for_bools_builder<Builder>>
{
  typedef Builder<substitute_propositional_variables_for_bools_builder<Builder>> super;
  using super::apply;

  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  propositional_variable_instantiation m_pvi;
  pbes_expression m_replacement;

  explicit substitute_propositional_variables_for_bools_builder(simplify_data_rewriter<data::rewriter>& r)
      : m_pbes_rewriter(r)
  {}

  void set_pvi(const propositional_variable_instantiation x) { m_pvi = x; }
  void set_replacement(const pbes_expression x) { m_replacement = x; }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (x == m_pvi)
    {
      result = m_replacement;
    }
    else
    {
      result = true_();
    }
  }
};

// Substitutor for the self_substitution / iterations
template <template <class> class Builder>
struct substitute_propositional_variables_builder : public Builder<substitute_propositional_variables_builder<Builder>>
{
  typedef Builder<substitute_propositional_variables_builder<Builder>> super;
  using super::apply;

  pbes_equation m_eq;
  core::identifier_string name;
  simplify_data_rewriter<data::rewriter> m_pbes_rewriter;
  data::data_specification data_spec;
  bool m_stable = false;

  explicit substitute_propositional_variables_builder(simplify_data_rewriter<data::rewriter>& r)
      : m_pbes_rewriter(r)
  {}

  void set_stable(bool b) { m_stable = b; }

  bool stable() const { return m_stable; }

  void set_equation(const pbes_equation& eq) { m_eq = eq; }

  void set_name(const core::identifier_string& s) { name = s; }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (x.name() == m_eq.variable().name())
    {
      data::mutable_indexed_substitution sigma;
      data::data_expression_list pars = x.parameters();
      for (const data::variable& v : m_eq.variable().parameters())
      {
        data::data_expression par = pars.front();
        pars.pop_front();
        if (par.sort() != v.sort())
        {
          // Parameters do not match with variables. Ignore this substitution.
          result = x;
          return;
        }
        sigma[v] = par;
      }
      pbes_expression p = pbes_rewrite(m_eq.formula(), m_pbes_rewriter, sigma);
      std::set<propositional_variable_instantiation> set = find_propositional_variable_instantiations(p);
      if (std::all_of(set.begin(),
              set.end(),
              [this](const propositional_variable_instantiation& v) { return v.name() != m_eq.variable().name(); }))
      {
        result = p;
        m_stable = false;
        return;
      }
      result = x;
      return;
    }
    result = x;
  }
};

void substitute(pbes_equation& into,
    const pbes_equation& by,
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter)
{
  substituter.set_equation(by);
  substituter.set_name(into.variable().name());
  pbes_expression p;
  substituter.apply(p, into.formula());
  into.formula() = p;
}

data::data_expression pbestodata(pbes_equation& equation,
    replace_propositional_variables_builder<pbes_system::pbes_expression_builder>& replace_substituter)
{
  pbes_expression expr;
  replace_substituter.set_forward(true);
  replace_substituter.set_name(equation.variable().name());
  replace_substituter.apply(expr, equation.formula());

  data::data_expression data_expr = atermpp::down_cast<data::data_expression>(detail::pbes2data(expr));

  replace_substituter.set_forward(false);
  replace_substituter.apply(expr, equation.formula());
  replace_substituter.reset_instantiations();
  return data_expr;
}

enum class InvResult
{
  INV_TRUE,
  INV_FALSE,
  EQ_FALSE
};

// Equation should be in full form.
InvResult global_invariant_check(pbes_equation& equation,
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter,
    replace_propositional_variables_builder<pbes_system::pbes_expression_builder>& replace_substituter,
    data::data_specification data_spec,
    simplify_data_rewriter<data::rewriter> pbes_rewriter,
    std::set<data::variable>& global_variables)
{
  mCRL2log(log::verbose) << "INV: " << equation.symbol() << "\n";
  if (equation.symbol().is_mu())
  {
    return InvResult::INV_FALSE;
  }

  // Base check: does it contain the right pvi
  std::set<propositional_variable_instantiation> set = find_propositional_variable_instantiations(equation.formula());
  if (set.size() == 0)
  {
    mCRL2log(log::verbose) << "INV: " << equation.variable().name() << " does not contain any pvi." << "\n";
    return InvResult::INV_FALSE;
  }

  if (std::any_of(set.begin(),
          set.end(),
          [=](const propositional_variable_instantiation& v) { return v.name() != equation.variable().name(); }))
  {
    // The formula contains another pvi than the current equation, so skip
    mCRL2log(log::verbose) << "INV: " << equation.variable().name() << " contains other pvi." << "\n";
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

  substitute_propositional_variables_for_bools_builder<pbes_system::pbes_expression_builder> bool_substituter(
      pbes_rewriter);

  mcrl2::data::detail::BDD_Prover f_bdd_prover(data_spec, data::used_data_equation_selector(data_spec));
  mCRL2log(log::verbose) << "INV: PVI set size " << set.size() << "\n";
  int i = 0;
  for (propositional_variable_instantiation pvi : set)
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

void nu_iteration(pbes_equation& equation,
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter,
    replace_propositional_variables_builder<pbes_system::pbes_expression_builder>& replace_substituter,
    data::data_specification data_spec,
    simplify_data_rewriter<data::rewriter> pbes_rewriter,
    std::set<data::variable>& global_variables)
{
  mcrl2::data::detail::BDD_Prover f_bdd_prover(data_spec, data::used_data_equation_selector(data_spec));
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
    data::data_expression eq_data = pbestodata(eq, replace_substituter);
    data::data_expression p_data = pbestodata(p_eq, replace_substituter);
    f_bdd_prover.set_formula(data::and_(data::imp(eq_data, p_data), data::imp(p_data, eq_data)));
    data::detail::Answer v_is_tautology = f_bdd_prover.is_tautology();
    if (v_is_tautology == data::detail::answer_yes)
    {
      mCRL2log(log::info) << eq.variable().name() << ": iteration " << i << " is equal to " << i + 1 << std::endl;
      stable = true;
      break;
    }
    eq.formula() = p;
    i++;
  }

  equation.formula() = eq.formula();
}

struct pbesiteration_pbes_fixpoint_iterator
{
  void run(pbes& p, pbesiteration_options options)
  {
    data::rewriter data_rewriter(p.data(), options.rewrite_strategy);
    simplify_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(pbes_rewriter);

    replace_propositional_variables_builder<pbes_system::pbes_expression_builder> replace_substituter(pbes_rewriter);

    for (std::vector<pbes_equation>::reverse_iterator i = p.equations().rbegin(); i != p.equations().rend(); i++)
    {
      mCRL2log(log::verbose) << "Investigating the equation for " << i->variable().name() << "\n";
      //  Turn this into a option flag
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
          nu_iteration(*i, substituter, replace_substituter, p.data(), pbes_rewriter, p.global_variables());
        }
      }
      else
      {
        nu_iteration(*i, substituter, replace_substituter, p.data(), pbes_rewriter, p.global_variables());
      }

      for (std::vector<pbes_equation>::reverse_iterator j = i + 1; j != p.equations().rend(); j++)
      {
        substitute(*j, *i, substituter);
      }
    }
    if (!p.equations().empty())
    {
      // Remove all but the first equation, as they have been solved
      p.equations().erase(p.equations().begin() + 1, p.equations().end());
    }
  }
};

void pbesiteration(const std::string& input_filename,
    const std::string& output_filename,
    const utilities::file_format& input_format,
    const utilities::file_format& output_format,
    pbesiteration_options options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  complete_data_specification(p);
  algorithms::normalize(p);
  pbesiteration_pbes_fixpoint_iterator fixpoint_iterator;
  fixpoint_iterator.run(p, options);
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESITERATION_H
