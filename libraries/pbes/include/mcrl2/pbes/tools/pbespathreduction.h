// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbespathreduction.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESPATHREDUCTION_H
#define MCRL2_PBES_TOOLS_PBESPATHREDUCTION_H

#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/rewriter.h"

namespace mcrl2
{

namespace pbes_system
{

struct pbespathreduction_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
};

// Substitutor to target specific path, replace our specific pvi with true/false
template <template <class> class Builder>
struct substitute_propositional_variables_for_true_false_builder
    : public Builder<substitute_propositional_variables_for_true_false_builder<Builder>>
{
  typedef Builder<substitute_propositional_variables_for_true_false_builder<Builder>> super;
  using super::apply;

  simplify_quantifiers_data_rewriter<data::rewriter> m_pbes_rewriter;
  propositional_variable_instantiation m_pvi;
  pbes_expression m_replacement;

  explicit substitute_propositional_variables_for_true_false_builder(
      simplify_quantifiers_data_rewriter<data::rewriter>& r)
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
      result = x;
    }
  }
};

template <template <class> class Builder>
struct substitute_propositional_variables_builder : public Builder<substitute_propositional_variables_builder<Builder>>
{
  typedef Builder<substitute_propositional_variables_builder<Builder>> super;
  using super::apply;

  pbes_equation m_eq;
  core::identifier_string name;
  simplify_quantifiers_data_rewriter<data::rewriter> m_pbes_rewriter;
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
        mCRL2log(log::verbose) << "Replaced in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
        result = p;
        m_stable = false;
        return;
      }
      mCRL2log(log::debug) << "No Replacement in PBES equation for " << name << ":\n" << x << " --> " << p << "\n";
      result = x;
      return;
    }
    result = x;
  }
};

void self_substitute(pbes_equation& equation,
    substitute_propositional_variables_for_true_false_builder<pbes_system::pbes_expression_builder>& pvi_substituter,
    simplify_quantifiers_data_rewriter<data::rewriter>& pbes_rewriter)
{
  bool stable = false;
  while (!stable)
  {
    stable = true;
    std::set<propositional_variable_instantiation> set = find_propositional_variable_instantiations(equation.formula());
    for (propositional_variable_instantiation x : set)
    {
      // Check if during the substitution of the other pvi this one got cancelled out
      std::set<propositional_variable_instantiation> path = {x};
      propositional_variable_instantiation cur_x = x;
      pbes_expression result = x;

      mCRL2log(log::verbose) << " -  -  -  -  -  -  -  -  -  -  -  -  -  -  -\n"
                             << equation.formula() << "\n\nStart " << (cur_x) << "\n";
      bool pvi_done = false;
      while (!pvi_done)
      {
        // (1) simplify
        data::mutable_indexed_substitution sigma;
        data::data_expression_list pars = cur_x.parameters();
        for (const data::variable& v : equation.variable().parameters())
        {
          data::data_expression par = pars.front();
          pars.pop_front();
          if (par.sort() != v.sort())
          {
            // Parameters do not match with variables. Ignore this substitution.
            result = x;
            break;
          }
          sigma[v] = par;
        }
        pbes_expression phi = pbes_rewrite(equation.formula(), pbes_rewriter, sigma);
        std::set<propositional_variable_instantiation> phi_set = find_propositional_variable_instantiations(phi);

        // (2) replace all reoccuring with true (nu) and false (mu)
        if (auto it = phi_set.find(cur_x); it != phi_set.end())
        {
          mCRL2log(log::debug) << "Need to replace this with true/false" << pp(cur_x) << "\n";
          mCRL2log(log::debug) << phi << "\n";

          // pbes_expression p_;
          pvi_substituter.set_pvi(cur_x);
          pvi_substituter.set_replacement(equation.symbol().is_nu() ? true_() : false_());
          pvi_substituter.apply(phi, phi);
          phi_set.erase(cur_x);

          mCRL2log(log::debug) << phi << "\n";

          mCRL2log(log::debug) << "- - - - - - - - - - - - - - - - - - - - \n";
        }

        // (3) check if simpler
        if (phi_set.size() == 1)
        {
          propositional_variable_instantiation new_x = *phi_set.begin();

          mCRL2log(log::debug) << "Trying loop " << new_x << " in path with \n";
          for (auto itr : path)
          {
            mCRL2log(log::verbose) << itr << "\n";
          }

          if (auto it = path.find(new_x); it != path.end())
          // if (std::any_of(path.begin(),
          //         path.end(),
          //         [new_x](const propositional_variable_instantiation& v)
          //         { return v.name() == new_x.name() && v.parameters() == new_x.parameters(); }))
          {
            // We have already seen this, so we are in a loop.
            mCRL2log(log::debug) << "Loop, seen " << new_x << " in path after " << cur_x << "    " << phi << "\n";
            for (auto itr : path)
            {
              mCRL2log(log::debug) << itr << "\n";
            }
            pvi_substituter.set_pvi(new_x);
            pvi_substituter.set_replacement(equation.symbol().is_nu() ? true_() : false_());
            pvi_substituter.apply(result, phi);

            mCRL2log(log::verbose) << "new_phi " << result << "\n";
            stable = false;
            pvi_done = true;
          }
          else
          {
            // The result does not contain the variable m_eq.variable().name() and is therefore considered simpler.
            mCRL2log(log::debug) << "Replaced in PBES equation for " << cur_x << "\n-->\n"
                                 << phi << "\n[" << new_x << "]\n";
            result = phi;
            stable = false;
            pvi_substituter.set_pvi(cur_x);
            pvi_substituter.set_replacement(result);
            pvi_substituter.apply(equation.formula(), equation.formula());
            cur_x = new_x;
            path.insert(new_x);
          }
        }
        else if (phi_set.size() == 0)
        {
          result = phi;
          stable = false;
          mCRL2log(log::debug) << "Replaced in PBES equation for " << cur_x << ":\n" << x << " \n-->\n " << phi << "\n";
          pvi_done = true;
        }
        else
        {
          pvi_done = true;
        }
        if (pvi_done)
        {
          mCRL2log(log::debug) << "Nothing further to do\n";
          break;
        }
      }

      mCRL2log(log::verbose) << "Ultimate result: replace \n" << x << "\n with \n" << result << "\n";
      mCRL2log(log::debug) << "in \n" << equation.formula() << "\n";
      pvi_substituter.set_pvi(cur_x);
      pvi_substituter.set_replacement(result);
      pvi_substituter.apply(equation.formula(), equation.formula());

      // Simplify
      equation.formula() = pbes_rewrite(equation.formula(), pbes_rewriter);
    }
  }
  mCRL2log(log::verbose) << "\n" << equation.variable() << "\n is stable! \n\n\n";
}

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

struct pbespathreduction_pbes_backward_substituter
{
  void run(pbes& p, pbespathreduction_options options)
  {
    data::rewriter data_rewriter(p.data(), options.rewrite_strategy);
    simplify_quantifiers_data_rewriter<data::rewriter> pbes_rewriter(data_rewriter);
    substitute_propositional_variables_builder<pbes_system::pbes_expression_builder> substituter(pbes_rewriter);
    substitute_propositional_variables_for_true_false_builder<pbes_system::pbes_expression_builder> pvi_substituter(
        pbes_rewriter);
    for (std::vector<pbes_equation>::reverse_iterator i = p.equations().rbegin(); i != p.equations().rend(); i++)
    {
      mCRL2log(log::verbose) << "Investigating the equation for " << i->variable().name() << "\n";
      self_substitute(*i, pvi_substituter, pbes_rewriter);

      std::set<propositional_variable_instantiation> pvi_set
          = find_propositional_variable_instantiations((*i).formula());
      if (pvi_set.size() == 0)
      {
        for (std::vector<pbes_equation>::reverse_iterator j = i + 1; j != p.equations().rend(); j++)
        {
          substitute(*j, *i, substituter);
        }
      }
    }
    pbes_rewrite(p, pbes_rewriter);
  }
};

void pbespathreduction(const std::string& input_filename,
    const std::string& output_filename,
    const utilities::file_format& input_format,
    const utilities::file_format& output_format,
    pbespathreduction_options options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);
  pbespathreduction_pbes_backward_substituter backward_substituter;
  backward_substituter.run(p, options);
  save_pbes(p, output_filename, output_format);
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_TOOLS_PBESPATHREDUCTION_H
