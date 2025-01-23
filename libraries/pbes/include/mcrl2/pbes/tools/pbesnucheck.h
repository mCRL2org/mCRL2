// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbesnucheck.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESNUCHECK_H
#define MCRL2_PBES_TOOLS_PBESNUCHECK_H

#include "mcrl2/data/detail/prover/bdd_prover.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/rewriters/pbes2data_rewriter.h"

#include <map>

namespace mcrl2
{

namespace pbes_system
{

struct pbesnucheck_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
};

/* template <template <class> class Builder, class Substitution>
struct substitute_propositional_variables_builder: public Builder<substitute_propositional_variables_builder<Builder,
Substitution> >
{
  typedef Builder<substitute_propositional_variables_builder<Builder, Substitution> > super;
  using super::apply; */

// Substitutor to replace predicate variables OTHER than the self-referencing onces to boolean variables
template <template <class> class Builder>
struct replace_propositional_variables_builder : public Builder<replace_propositional_variables_builder<Builder>>
{
  typedef Builder<replace_propositional_variables_builder<Builder>> super;
  using super::apply;

  core::identifier_string name;
  simplify_quantifiers_data_rewriter<data::rewriter> m_pbes_rewriter;
  data::data_specification data_spec;
  std::map<data::variable, propositional_variable_instantiation> m_instantiations;
  bool forward = true;

  explicit replace_propositional_variables_builder(simplify_quantifiers_data_rewriter<data::rewriter>& r)
      : m_pbes_rewriter(r)
  {}

  void set_name(const core::identifier_string& s) { name = s; }

  void set_forward(bool b) { forward = b; }

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
      return;
    }
    result = d;
  }

  template <class T>
  void apply(T& result, const propositional_variable_instantiation& x)
  {
    if (forward && x.name() != name)
    {
      std::string var_name = "a" + std::to_string(m_instantiations.size());
      data::variable vb1(var_name, data::sort_bool::bool_());
      m_instantiations.insert({vb1, x});
      result = vb1;
      return;
    }
    result = x;
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
  simplify_quantifiers_data_rewriter<data::rewriter> m_pbes_rewriter;
  data::data_specification data_spec;
  bool m_stable = false;

  explicit substitute_propositional_variables_builder(simplify_quantifiers_data_rewriter<data::rewriter>& r)
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
        // The result does not contain the variable m_eq.variable().name() and is therefore considered simpler.
        //        mCRL2log(log::verbose) << "Replaced in PBES equation for " << name << ":\n" << x << " --> " << p <<
        //        "\n";
        result = p;
        m_stable = false;
        return;
      }
      //      mCRL2log(log::debug) << "No Replacement in PBES equation for " << name << ":\n" << x << " --> " << p <<
      //      "\n";
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

void nu_iteration(pbes_equation& equation,
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder>& substituter,
    replace_propositional_variables_builder<pbes_system::pbes_expression_builder>& replace_substituter,
    data::data_specification data_spec)
{
  replace_substituter.set_forward(true);
  replace_substituter.set_name(equation.variable().name());
  replace_substituter.apply(equation.formula(), equation.formula());

  mcrl2::data::detail::BDD_Prover f_bdd_prover(data_spec, data::used_data_equation_selector(data_spec));
  pbes_equation eq;
  if (eq.symbol().is_nu())
  {
    eq.formula() = true_();
  }
  else
  {
    eq.formula() = false_();
  }
  eq.variable() = equation.variable();
  bool stable = false;
  int i = 0;
  while (!stable && i < 20)
  {
    mCRL2log(log::info) << eq.variable().name() << ":  " << i << std::endl;
    // Apply substitution
    substituter.set_equation(eq);
    substituter.set_name(equation.variable().name());
    pbes_expression p;
    substituter.apply(p, equation.formula());

    // Check if the iteration has reached a fixpoint
    data::data_expression eq_data = atermpp::down_cast<data::data_expression>(detail::pbes2data(eq.formula()));
    data::data_expression p_data = atermpp::down_cast<data::data_expression>(detail::pbes2data(p));
    f_bdd_prover.set_formula(data::and_(data::imp(eq_data, p_data), data::imp(p_data, eq_data)));
    data::detail::Answer v_is_tautology = f_bdd_prover.is_tautology();
    data::detail::Answer v_is_contradiction = f_bdd_prover.is_contradiction();
    if (v_is_tautology == data::detail::answer_yes)
    {
      mCRL2log(log::info) << eq.variable().name() << ": iteration " << i << " is equal to " << i + 1 << std::endl;
      stable = true;
      break;
    }
    else if (v_is_contradiction == data::detail::answer_yes)
    {
      mCRL2log(log::error) << "Contradiction" << std::endl;
      throw mcrl2::runtime_error("Iteration was not monotone.");
    }
    eq.formula() = p;
    i++;
  }

  replace_substituter.set_forward(false);
  replace_substituter.apply(equation.formula(), equation.formula());

  equation.formula() = eq.formula();
}

struct pbesnucheck_pbes_backward_substituter
{
  void run(pbes& p, pbesnucheck_options options)
  {
    data::rewriter data_rewriter(p.data(), options.rewrite_strategy);
    simplify_quantifiers_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(pbes_rewriter);

    replace_propositional_variables_builder<pbes_system::pbes_expression_builder> replace_substituter(pbes_rewriter);

    for (std::vector<pbes_equation>::reverse_iterator i = p.equations().rbegin(); i != p.equations().rend(); i++)
    {
      // Simplify the equation *i by substituting in itself.
      mCRL2log(log::verbose) << "Investigating the equation for " << i->variable().name() << "\n";
      nu_iteration(*i, substituter, replace_substituter, p.data());
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
    pbes_rewrite(p, pbes_rewriter);
  }
};

void pbesnucheck(const std::string& input_filename,
    const std::string& output_filename,
    const utilities::file_format& input_format,
    const utilities::file_format& output_format,
    pbesnucheck_options options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  complete_data_specification(p);
  algorithms::normalize(p);
  pbesnucheck_pbes_backward_substituter backward_substituter;
  backward_substituter.run(p, options);
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESnucheck_H
