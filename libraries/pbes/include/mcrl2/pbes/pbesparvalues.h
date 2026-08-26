// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbesparvalues.h
/// \brief Explore statically which parameter values may be reached in a PBES

#ifndef MCRL2_PBES_PBESPARVALUES_H
#define MCRL2_PBES_PBESPARVALUES_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/detail/parvalues.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_functions.h"
#include "mcrl2/pbes/rewriters/enumerate_quantifiers_rewriter.h"
#include "mcrl2/pbes/rewriters/pbes2data_rewriter.h"

#include <map>
#include <unordered_set>

namespace mcrl2::pbes_system
{

/// \brief The values that a single parameter of a predicate variable may reach.
struct pbes_parameter_domain
{
  core::identifier_string equation;
  data::variable parameter;
  std::unordered_set<data::data_expression> values;
  /// \brief True if not all reachable values could be enumerated; the reported
  ///        values form a partial domain and may be unbounded.
  bool unbounded = false;
};

/// \brief Algorithm that enumerates the values that the parameters of the
///        predicate variables of a PBES can take during exploration, starting
///        from the initial state.
template<typename DataRewriter>
class pbes_parvalues_algorithm : public data::detail::parvalues_algorithm<DataRewriter>
{
public:
  /// \brief The type of the results computed by the algorithm.
  using result_type = std::vector<pbes_parameter_domain>;

  using super = data::detail::parvalues_algorithm<DataRewriter>;
  using super::m_graph;
  using super::m_rewriter;

protected:
  pbes m_pbes;
  bool m_expand_finite_sorts;

  /// \brief Compute the names of the equations reachable from the initial state.
  std::set<core::identifier_string> reachable_equations(
    const std::map<core::identifier_string, const pbes_equation*>& declared) const
  {
    std::set<core::identifier_string> visited;
    std::vector<const pbes_equation*> todo;
    auto add = [&](const core::identifier_string& name)
    {
      if (visited.contains(name))
      {
        return;
      }
      const auto it = declared.find(name);
      if (it == declared.end())
      {
        throw mcrl2::runtime_error("Predicate variable " + pp(name) + " occurs in the PBES but is not declared.");
      }
      visited.insert(name);
      todo.push_back(it->second);
    };

    add(m_pbes.initial_state().name());
    while (!todo.empty())
    {
      const pbes_equation* eqn = todo.back();
      todo.pop_back();
      for (const propositional_variable_instantiation& pvi: find_propositional_variable_instantiations(eqn->formula()))
      {
        add(pvi.name());
      }
    }
    return visited;
  }

public:

  /// \brief Constructor for the pbes_parvalues algorithm
  /// \param spec Specification to which the algorithm should be applied
  /// \param r a rewriter for data
  /// \param qlimit limit on the size of the terms in enumeration
  /// \param max_rounds maximal number of propagation rounds
  /// \param expand_finite_sorts if true, quantifiers over finite sorts are expanded before analysis
  pbes_parvalues_algorithm(const pbes& spec,
    DataRewriter& r,
    const std::size_t qlimit,
    const std::size_t max_rounds,
    bool expand_finite_sorts)
    : super(r, spec.data(), qlimit, max_rounds),
      m_pbes(spec),
      m_expand_finite_sorts(expand_finite_sorts)
  {}

  std::vector<pbes_parameter_domain> run()
  {
    algorithms::instantiate_global_variables(m_pbes);

    // Index the declared equations by predicate variable name.
    std::map<core::identifier_string, const pbes_equation*> declared;
    for (const pbes_equation& eqn: m_pbes.equations())
    {
      if (!declared.emplace(eqn.variable().name(), &eqn).second)
      {
        throw mcrl2::runtime_error(
          "Predicate variable " + core::pp(eqn.variable().name()) + " is declared more than once.");
      }
    }

    // Only equations that are reachable from the initial state are considered.
    const std::set<core::identifier_string> reachable = reachable_equations(declared);
    if (reachable.size() < declared.size())
    {
      mCRL2log(log::verbose) << "Skipping " << declared.size() - reachable.size()
                             << " equation(s) that are not reachable from the initial state.\n";
    }

    // The influence graph identifies parameters by (equation name, parameter).
    // This is unambiguous under the following assumptions, which are checked
    // here: the parameters of each equation have distinct names, and the only
    // data variables occurring freely in the right hand side of an equation
    // are its own parameters.
    for (const core::identifier_string& name: reachable)
    {
      const pbes_equation& eqn = *declared.at(name);
      std::set<core::identifier_string> parameter_names;
      for (const data::variable& d: eqn.variable().parameters())
      {
        if (!parameter_names.insert(d.name()).second)
        {
          throw mcrl2::runtime_error(
            "Parameter " + core::pp(d.name()) + " occurs more than once in predicate variable " + core::pp(name) + ".");
        }
      }
      std::set<data::variable> stray_variables = find_free_variables(eqn.formula());
      for (const data::variable& d: eqn.variable().parameters())
      {
        stray_variables.erase(d);
      }
      if (!stray_variables.empty())
      {
        throw mcrl2::runtime_error("The right hand side of predicate variable " + core::pp(name)
                                   + " contains free data variable(s) that are not parameters of the equation.\n"
                                   + core::detail::print_list(stray_variables) + "\n"
                                   + core::detail::print_list(m_pbes.global_variables()));
      }
    }

    // Optionally instantiate quantifiers over finite sorts.
    std::map<core::identifier_string, pbes_expression> formulas;
    for (const core::identifier_string& name: reachable)
    {
      formulas.emplace(name, declared.at(name)->formula());
    }
    if (m_expand_finite_sorts)
    {
      const enumerate_quantifiers_rewriter enumerator(m_rewriter,
        m_pbes.data(),
        mcrl2::pbes_system::expand_finite_sorts);
      for (auto& [name, formula]: formulas)
      {
        formula = enumerator(formula);
      }
    }

    // Add an influence graph edge for each guarded occurrence of a predicate
    // variable instantiation in the right hand side of an equation.
    for (const core::identifier_string& name: reachable)
    {
      const pbes_equation& source = *declared.at(name);

      detail::guard_traverser guard_traverser(m_rewriter);
      guard_traverser.apply(formulas.at(name));
      mCRL2log(log::debug) << "Equation " << core::pp(name) << " has "
                           << guard_traverser.expression_stack.back().guards.size() << " guarded PVI(s)." << std::endl;
      for (const auto& [pvi, guard]: guard_traverser.expression_stack.back().guards)
      {
        const pbes_equation* target = declared.at(pvi.name());

        assert(pvi.parameters().size() == target->variable().parameters().size());
        std::vector<std::pair<data::detail::parameter, data::data_expression>> updates;
        updates.reserve(target->variable().parameters().size());
        for (const auto& [var, expr]: utilities::zip(target->variable().parameters(), pvi.parameters()))
        {
          updates.emplace_back(data::detail::parameter(pvi.name(), var), expr);
        }

        // Variables occurring freely in the guard and the updates either are
        // parameters of the source equation or were bound by enclosing
        // quantifiers whose binders the guard traverser discards.
        std::set<data::variable> qvars = find_free_variables(guard);
        for (const auto& [_, expr]: updates)
        {
          qvars.merge(find_free_variables(expr));
        }
        for (const data::variable& d: source.variable().parameters())
        {
          qvars.erase(d);
        }

        // The guard is a simple expression, i.e., it does not contain
        // propositional variable instantiations, hence it can be converted
        // to a genuine data expression.
        assert(is_simple_expression(guard, false));
        m_graph.add_edge(name, qvars, detail::pbes2data(guard), updates);
      }
    }

    // Register the parameters. Parameters of the initial equation receive
    // their initial values; all other parameters are declared without values
    // and obtain them once updates reach them.
    {
      const propositional_variable_instantiation& init = m_pbes.initial_state();
      const pbes_equation& init_eqn = *declared.at(init.name());
      assert(init.parameters().size() == init_eqn.variable().parameters().size());
      for (const auto& [var, expr]: utilities::zip(init_eqn.variable().parameters(), init.parameters()))
      {
        m_graph.new_parameter(data::detail::parameter(init.name(), var), m_rewriter(expr));
      }
      for (const core::identifier_string& name: reachable)
      {
        if (name == init.name())
        {
          continue;
        }
        for (const data::variable& var: declared.at(name)->variable().parameters())
        {
          m_graph.declare_parameter(data::detail::parameter(name, var));
        }
      }
    }

    super::run();

    std::vector<pbes_parameter_domain> result;
    for (const core::identifier_string& name: reachable)
    {
      for (const data::variable& var: declared.at(name)->variable().parameters())
      {
        const data::detail::parameter par(name, var);
        result.push_back({name, var, m_graph.at(par).stable, m_graph.is_unbounded(par)});
      }
    }
    return result;
  }
};

} // namespace mcrl2::pbes_system

#endif
