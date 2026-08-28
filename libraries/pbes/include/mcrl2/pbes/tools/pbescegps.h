// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/tools/pbescegps.h
/// \brief This file provides a tool that can simplify PBESs by
///        substituting PBES equations for variables in the rhs,
///        simplifying the result, and keeping it when it can
///        eliminate PBES variables.

#ifndef MCRL2_PBES_TOOLS_PBESCEGPS_H
#define MCRL2_PBES_TOOLS_PBESCEGPS_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/constelm.h"
#include "mcrl2/pbes/detail/count_free_variables.h"
#include "mcrl2/pbes/detail/find_free_variables.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/pbescegps_refine_strategies.h"
#include "mcrl2/pbes/detail/pbescegps_utilities.h"
#include "mcrl2/pbes/detail/stategraph_local_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_local_reset_variables.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/parelm.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/pbesinst_structure_graph2.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/rewrite.h"
#include <algorithm>
#include <deque>
#include <iterator>
#include <ostream>
#ifdef MCRL2_ENABLE_SYLVAN
#include "mcrl2/pbes/pbesreach.h"
#include "mcrl2/pbes/tools/pbesstategraph_options.h"
#endif
#include "mcrl2/pbes/pbessolve_options.h"
#include "mcrl2/pbes/rewriters/abstraction_rewriter.h"
#include "mcrl2/pbes/rewriters/essential_variable_extractor.h"
#include "mcrl2/pbes/solve_structure_graph.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/execution_timer.h"
#include "mcrl2/utilities/logger.h"
#include <boost/asio.hpp>
#include <boost/asio/buffer.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/filesystem.hpp>
#include <boost/process.hpp>
#include <boost/process/search_path.hpp>
#include <cstddef>
#include <functional>
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <vector>

namespace bp = boost::process;

namespace mcrl2::pbes_system
{

struct pbescegps_iterator
{
private:
  utilities::indexed_set<data::data_expression> m_values;

  bp::child sym_process;

  // Cache for variable occurrence counts to avoid recomputing
  // Keyed by equation name instead of formula pointer for stability
  std::map<core::identifier_string, std::map<data::variable, std::size_t>> m_var_count_cache;

  // Shared data rewriter, created once from the data specification and reused throughout the tool
  std::optional<data::rewriter> m_datar;

  // Cache of approximation results, keyed by the parameters remaining in each equation
  // after simplification (constelm/parelm) together with the approximation type.
  // If the same set of parameters remains again, the result and structure graph are
  // reused without solving.
  std::map<std::pair<std::map<core::identifier_string, std::set<data::variable>>, bool>,
    std::pair<bool, structure_graph>>
    m_solution_cache;

  // Ruling relation: m_ruling_relation[X][dₘ] = { dⱼ | dⱼ ≽ dₘ in equation X }.
  // Computed once from the PBES, reused across all iterations.
  // dⱼ ≽ dₘ means: dⱼ appears in the guard of a transition that changes dₘ.
  std::map<core::identifier_string, std::map<data::variable, std::set<data::variable>>> m_ruling_relation;


public:
  // Read-only access to the computed ruling relation (used by tests).
  const std::map<core::identifier_string, std::map<data::variable, std::set<data::variable>>>&
  ruling_relation() const
  {
    return m_ruling_relation;
  }

  std::pair<bool, structure_graph> solve(const pbes& p, pbescegps_options options)
  {
    pbes p_copy(p);
    utilities::execution_timer timer;
    mcrl2::log::log_level_t saved_level = mcrl2::log::logger::get_reporting_level();
    if (saved_level == mcrl2::log::trace || saved_level == mcrl2::log::debug)
    {
      mcrl2::log::logger::set_reporting_level(mcrl2::log::verbose);
    }
    structure_graph m_solved_graph;

    bool result = false;
    timer.start("solving approximation");
    if (options.solve_symbolic)
    {
      try
      {
        bp::ipstream output_sym_stream;
        bp::opstream input_sym_stream;
        mCRL2log(log::debug) << "Solving symbolic with args: " << options.solve_symbolic_args << std::endl;
        sym_process = bp::child(("pbessolvesymbolic - " + options.solve_symbolic_args),
          bp::std_in<input_sym_stream, bp::std_out> output_sym_stream);

        std::ostringstream buffer(std::ios::binary);
        atermpp::binary_aterm_ostream(buffer) << p_copy;

        const std::string& data = buffer.str();
        input_sym_stream.write(data.data(), data.size());

        input_sym_stream.flush();

        std::vector<std::string> outline;
        std::string line;
        while (sym_process.running() && std::getline(output_sym_stream, line))
        {
          mCRL2log(log::debug) << "[symbolic]: " << line << std::endl;
          outline.push_back(line);
        }
        mCRL2log(log::verbose) << "Result: " << outline.back() << std::endl;
        sym_process.wait();

        result = outline.back() == "true";
      }
      catch (const std::exception& e)
      {
        sym_process.wait();
        throw mcrl2::runtime_error("symbolic solver failed: " + std::string(e.what()));
      }
    }
    else
    {
      pbessolve_options options2;
      options2.rewrite_strategy = options.rewrite_strategy;
      options2.optimization = options.optimization;
      options2.number_of_threads = options.number_of_threads;

      m_solved_graph = structure_graph();
      if (options.optimization <= partial_solve_strategy::remove_self_loops)
      {
        pbesinst_structure_graph_algorithm algorithm(options2, p_copy, m_solved_graph, m_datar);
        algorithm.run();
      }
      else
      {
        pbesinst_structure_graph_algorithm2 algorithm(options2, p_copy, m_solved_graph, m_datar);
        algorithm.run();
      }

      // Solve the structure graph
      result = solve_structure_graph(m_solved_graph);
      mCRL2log(log::verbose) << "Structure graph solver returned " << (result ? "TRUE" : "FALSE") << std::endl;
    }
    timer.finish("solving approximation");
    if (mcrl2::log::mCRL2logEnabled(log::verbose))
    {
      timer.report();
    }
    mcrl2::log::logger::set_reporting_level(saved_level);
    return {result, m_solved_graph};
  }

  // Solves the underapproximated PBES using structure graph solving
  std::pair<bool, structure_graph>
  solve_approximation(const pbes& p, pbescegps_options options, const bool& is_overapproximation)
  {
    data::mutable_map_substitution<> sigma;
    pbes p_copy(p);
    sigma = pbes_system::detail::instantiate_global_variables(p_copy);
    pbes_system::detail::replace_global_variables(p_copy, sigma);

    try
    {
      mCRL2log(log::verbose) << "Solving " << (is_overapproximation ? "over" : "under") << "approximated PBES"
                             << std::endl;
      return solve(p_copy, options);
    }
    catch (const std::exception& e)
    {
      throw mcrl2::runtime_error("Exception during structure graph solving: " + std::string(e.what()));
    }
  }

  // Computes, for each equation of the simplified PBES, the parameters that remain
  // after simplification (constelm/parelm). These determine the effective abstraction
  // state, so they are used as the cache key.
  std::map<core::identifier_string, std::set<data::variable>> compute_remaining_parameters(const pbes& simplified)
  {
    std::map<core::identifier_string, std::set<data::variable>> remaining_parameters;
    for (const pbes_equation& eq: simplified.equations())
    {
      remaining_parameters[eq.variable().name()] = as_set(eq.variable().parameters());
    }
    return remaining_parameters;
  }

  // Solves an under- or over-approximation of the PBES, reusing a previously cached
  // result and structure graph when the same set of equation parameters remains
  // after simplification.
  bool solve_approximation_cached(const pbes& p,
    abstract_param_state& state,
    bool is_overapproximation,
    const pbescegps_options& options,
    structure_graph& graph)
  {
    pbes p_approx = apply_abstraction_to_pbes(p, state, is_overapproximation, options);
    std::map<core::identifier_string, std::set<data::variable>> remaining_parameters
      = compute_remaining_parameters(p_approx);

    auto key = std::make_pair(remaining_parameters, is_overapproximation);
    auto cached = m_solution_cache.find(key);
    if (cached != m_solution_cache.end())
    {
      mCRL2log(log::verbose) << "Using cached " << (is_overapproximation ? "over" : "under")
                             << "-approximation with result: " << (cached->second.first ? "TRUE" : "FALSE")
                             << std::endl;
      mCRL2log(log::verbose) << "Remaining parameters:" << std::endl;
      for (const auto& [eq_name, variables]: remaining_parameters)
      {
        mCRL2log(log::verbose) << "  " << eq_name << ": " << core::detail::print_list(variables) << std::endl;
      }
      graph = cached->second.second;
      return cached->second.first;
    }

    auto [result, solved_graph] = solve_approximation(p_approx, options, is_overapproximation);
    graph = solved_graph;
    m_solution_cache[key] = {result, solved_graph};
    return result;
  }

  // Collects all parameters W = decl(E) from a PBES
  // This gathers all data variables that appear in PBES equations
  std::set<data::variable> extract_equation_parameters(const pbes& p)
  {
    std::set<data::variable> parameters;
    for (const pbes_equation& eq: p.equations())
    {
      for (const auto& param: eq.variable().parameters())
      {
        parameters.insert(atermpp::down_cast<data::variable>(param));
      }
    }
    return parameters;
  }

  // Calculate the indices of the parameters to abstract
  // Maps equation names to sets of parameter indices that should be abstracted
  std::map<core::identifier_string, std::set<std::size_t>> map_parameters_to_abstraction_indices(const pbes& p,
    const std::map<core::identifier_string, std::set<data::variable>>& abstraction_vars_per_eq)
  {
    std::map<core::identifier_string, std::set<std::size_t>> pbes_parameters_abstraction_indices;
    for (const auto& eq: p.binding_variables())
    {
      pbes_parameters_abstraction_indices[eq.name()] = std::set<std::size_t>();
      std::size_t i = 0;
      for (const auto& param: eq.parameters())
      {
        if (abstraction_vars_per_eq.at(eq.name()).contains(param))
        {
          pbes_parameters_abstraction_indices[eq.name()].insert(i);
        }
        ++i;
      }
    }
    return pbes_parameters_abstraction_indices;
  }

  // // Applies abstraction to a PBES expression
  // // Replaces data expressions depending on abstracted variables with true/false
  pbes_expression apply_abstraction(const pbes_expression& expr,
    const std::set<data::variable>& abstraction_vars,
    const std::map<core::identifier_string, std::set<std::size_t>>& pbes_parameters_abstraction_indices,
    bool is_overapproximation);

  // Applies abstraction to all equations in a PBES
  pbes apply_abstraction_to_pbes(const pbes& p,
    const abstract_param_state& state,
    bool is_overapproximation,
    const pbescegps_options& options)
  {
    pbes result = p;

    // Filter the parameters of the PBES binding variables
    std::vector<pbes_equation> new_equations;
    for (const pbes_equation& eq: result.equations())
    {
      propositional_variable bnd_var = eq.variable();
      data::variable_list filtered_params;
      for (const auto& param: bnd_var.parameters())
      {
        if (!state.W.at(bnd_var.name()).contains(param))
        {
          filtered_params.push_front(atermpp::down_cast<data::variable>(param));
        }
      }
      filtered_params = reverse(filtered_params);
      propositional_variable new_bnd_var(bnd_var.name(), filtered_params);
      pbes_equation new_eq(eq.symbol(), new_bnd_var, eq.formula());
      new_equations.push_back(new_eq);
    }
    result.equations() = new_equations;

    // Update the PBES intitial state
    std::size_t i = 0;
    data::data_expression_list filtered_args_vec;
    for (const auto& param: result.initial_state().parameters())
    {
      if (!state.I.at(result.initial_state().name()).contains(i))
      {
        filtered_args_vec.push_front(atermpp::down_cast<data::data_expression>(param));
      }
      ++i;
    }
    filtered_args_vec = reverse(filtered_args_vec);
    result.initial_state() = propositional_variable_instantiation(result.initial_state().name(), filtered_args_vec);

    // Apply abstraction to the formulae
    for (pbes_equation& eq: result.equations())
    {
      // Find abstraction set for this equation
      auto it = state.W.find(eq.variable().name());
      if (it != state.W.end())
      {
        eq.formula() = apply_abstraction(eq.formula(), it->second, state.I, is_overapproximation);
      }
    }

    mCRL2log(log::trace) << pp(result) << std::endl;

    // Rewrite expressions for simplification
    simplify_data_rewriter<data::rewriter> pbesr(*m_datar);
    pbes_rewrite(result, pbesr);
    mcrl2::log::log_level_t saved_level = mcrl2::log::logger::get_reporting_level();
    if (saved_level == mcrl2::log::trace || saved_level == mcrl2::log::debug)
    {
      mcrl2::log::logger::set_reporting_level(mcrl2::log::verbose);
    }
    pbes_system::parelm(result, false);
    pbes_constelm_algorithm<data::rewriter, simplify_data_rewriter<data::rewriter>> constelm_algo(*m_datar, pbesr);
    constelm_algo.run(result);
    pbes_rewrite(result, pbesr);
    pbes_system::parelm(result, false);
    pbes_rewrite(result, pbesr);
    if (options.stategraph)
    {
      pbesstategraph_options opts;
      detail::local_reset_variables_algorithm algo(result, opts);
      algo.run();
      result = algo.result();

      constelm_algo.run(result);
    }
    mcrl2::log::logger::set_reporting_level(saved_level);

    mCRL2log(log::trace) << pp(result) << std::endl;

    return result;
  }

  // Helper: Calculate non-Control Flow Parameters (CFP) per equation
  // Populates the abstraction_state directly with W and indices
  void compute_initial_abstraction_set(pbes& p, const bool use_init_control_flow, abstract_param_state& state)
  {
    // Initialize W with ALL parameters for each equation using add_abstracted_variable
    for (const pbes_equation& eq: p.equations())
    {
      state.I[eq.variable().name()] = std::set<std::size_t>();
      state.W[eq.variable().name()] = std::set<data::variable>();
      for (const auto& param: eq.variable().parameters())
      {
        state.add_abstracted_variable(p, eq.variable().name(), atermpp::down_cast<data::variable>(param));
      }
    }

    if (!use_init_control_flow)
    {
      return;
    }

    mcrl2::log::log_level_t saved_level = mcrl2::log::logger::get_reporting_level();
    if (saved_level == mcrl2::log::trace || saved_level == mcrl2::log::debug)
    {
      mcrl2::log::logger::set_reporting_level(mcrl2::log::verbose);
    }

    detail::stategraph_pbes stategraph(p, *m_datar);
    pbesstategraph_options opts;
    detail::stategraph_local_algorithm algo(p, opts);

    for (detail::stategraph_equation& equation: stategraph.equations())
    {
      for (detail::predicate_variable& predvar: equation.predicate_variables())
      {
        predvar.simplify_guard();
      }
    }

    algo.run();

    // Get the GCFP vector for each equation
    const std::map<core::identifier_string, std::vector<bool>>& gcfp_map = algo.get_GCFP();
    mcrl2::log::logger::set_reporting_level(saved_level);

    for (const auto& [eq_name, cfp_vector]: gcfp_map)
    {
      // Find the corresponding equation to get parameter list
      auto eq_opt = detail::find_equation_by_name(p, eq_name);
      if (eq_opt)
      {
        const auto& params = as_vector(eq_opt->get().variable().parameters());

        // cfp_vector[i] == true means parameter i is a CFP
        // cfp_vector[i] == false means parameter i is NOT a CFP (non-CFP)
        for (std::size_t i = 0; i < cfp_vector.size() && i < params.size(); ++i)
        {
          if (cfp_vector[i]) // If IS a CFP (remove from non-CFP set)
          {
            state.remove_abstracted_variable(p, eq_name, i);
          }
        }
      }
    }
  }

  // Computes, per equation, the parameters that occur in the guards of predicate
  // variable instances that are in the scope of an infinite quantifier. These
  // parameters must not be abstracted, otherwise the structure graph may contain
  // infinitely many vertices.
  std::map<core::identifier_string, std::set<data::variable>> infinite_quantifier_guard_variables(const pbes& p)
  {
    std::map<core::identifier_string, std::set<data::variable>> result;
    for (const pbes_equation& eq: p.equations())
    {
      // Find the predicate variable instantiations that occur in the scope of an
      // infinite quantifier.
      detail::infinite_quantifier_pvi_traverser scope_traverser(p.data());
      scope_traverser.apply(eq.formula());
      if (scope_traverser.m_pvis.empty())
      {
        continue;
      }

      // Compute the guards of all predicate variable instantiations in the equation.
      detail::guard_traverser guards(*m_datar);
      guards.apply(eq.formula());

      std::set<data::variable> parameters = as_set(eq.variable().parameters());
      for (const auto& [pvi, guard]: guards.top().guards)
      {
        if (!scope_traverser.m_pvis.contains(pvi))
        {
          continue;
        }
        for (const data::variable& v: find_free_variables(guard))
        {
          if (parameters.contains(v))
          {
            result[eq.variable().name()].insert(v);
          }
        }
      }
    }
    return result;
  }

  // Ensures that parameters occurring in the guards of predicate variable instances
  // in the scope of an infinite quantifier are not abstracted, so that the
  // structure graph does not contain infinitely many vertices.
  void instantiate_infinite_quantifier_guards(pbes& p, abstract_param_state& state)
  {
    for (const auto& [eq_name, variables]: infinite_quantifier_guard_variables(p))
    {
      for (const data::variable& v: variables)
      {
        if (state.W[eq_name].contains(v))
        {
          mCRL2log(log::verbose) << "Not abstracting parameter " << v << " of equation " << eq_name
                                 << " because it occurs in the guard of a predicate variable instance in the "
                                    "scope of an infinite quantifier."
                                 << std::endl;
          state.remove_abstracted_variable(p, eq_name, v);
        }
      }
    }
  }

  // This can probably be optimized if you create a dependency graph a priori
  void make_data_closed(const pbes& p, abstract_param_state& state)
  {
    bool done = false;
    mCRL2log(log::debug) << "======== Closing the data ======" << std::endl;
    auto global_variables = p.global_variables();
    do
    {
      done = true;
      for (const auto& eq: p.equations())
      {
        std::set<propositional_variable_instantiation> pvis = find_propositional_variable_instantiations(eq.formula());
        for (const propositional_variable_instantiation& pvi: pvis)
        {
          std::size_t i = 0;
          mCRL2log(log::trace) << "Data-closed: eq " << eq.variable().name() << " pvi " << pvi
                               << " has abstracted parameters at indices "
                               << core::detail::print_list(state.I.at(pvi.name())) << " with "
                               << core::detail::print_list(state.W.at(pvi.name())) << std::endl;
          for (const auto& pvi_param: pvi.parameters())
          {
            // Check for the terms that we want to instantiate
            if (!contains(state.I.at(pvi.name()), i))
            {
              std::set<data::variable> free_vars = find_free_variables(pvi_param);
              mCRL2log(log::trace) << "Data-closed: free_vars " << core::detail::print_list(free_vars) << " in \""
                                   << pp(pvi_param) << "\" due to " << pp(pvi) << std::endl;
              for (const data::variable& v: free_vars)
              {
                if (contains(state.W[eq.variable().name()], v)
                    && std::find(global_variables.begin(), global_variables.end(), v) == global_variables.end())
                {
                  mCRL2log(log::debug) << "Data-closed: concrete param " << pp(v)
                                       << " in W=" << core::detail::print_list(state.W[eq.variable().name()])
                                       << " of equation " << pvi.name() << " due to " << pp(pvi) << std::endl;
                  // Find the parameter with the same index
                  bool found = false;
                  for (const auto& param: eq.variable().parameters())
                  {
                    if (param.name() == v.name())
                    {
                      state.remove_abstracted_variable(p, eq.variable().name(), v);
                      done = false;
                      found = true;
                      break;
                    }
                  }
                  if (!found)
                  {
                    throw mcrl2::runtime_error("Data-closed: Could not find parameter " + pp(v.name()) + " in equation "
                                               + pp(eq.variable().name()));
                  }
                  mCRL2log(log::debug) << "Data-closed: Updated W="
                                       << core::detail::print_list(state.W[eq.variable().name()]) << std::endl;
                }
              }
            }
            ++i;
          }
        }
      }
    }
    while (!done);
    mCRL2log(log::trace) << "======== Data closed ======" << std::endl;
    mCRL2log(log::trace) << "Data closed: W = " << std::endl;
    for (const auto& [eq_name, var_set]: state.W)
    {
      mCRL2log(log::trace) << "" << eq_name << ": " << core::detail::print_list(var_set) << std::endl;
    }
  }

  // Computes the ruling relation from the PBES structure.
  // dⱼ ≽ dₘ (dⱼ rules dₘ) iff dⱼ appears in the guard of a self-recursive
  // transition that changes dₘ. For mutual pairs (A≽B and B≽A), only the
  // direction with more transition occurrences is kept. Longer cycles are broken
  // afterwards by removing the edge ruling the most dominant node of the cycle,
  // so the final relation is a strict order (DAG).
  void compute_ruling_relation(const pbes& p)
  {
    // Count occurrences: counts[eq][dₘ][dⱼ] = # transitions where dⱼ guards and dₘ changes
    std::map<core::identifier_string, std::map<data::variable, std::map<data::variable, std::size_t>>> counts;

    for (const pbes_equation& eq: p.equations())
    {
      const auto& eq_name = eq.variable().name();
      const auto& params = as_vector(eq.variable().parameters());

      detail::guard_traverser guard_trav(*m_datar);
      guard_trav.apply(eq.formula());

      for (const auto& [pvi, guard_expr]: guard_trav.expression_stack.back().guards)
      {
        if (pvi.name() != eq_name)
        {
          continue;
        }

        // Guard variables: free variables of guard_expr that are parameters of this equation
        std::set<data::variable> free_vars = pbes_system::find_free_variables(guard_expr);
        std::set<data::variable> params_set(params.begin(), params.end());
        std::set<data::variable> guard_vars;
        std::set_intersection(free_vars.begin(),
          free_vars.end(),
          params_set.begin(),
          params_set.end(),
          std::inserter(guard_vars, guard_vars.begin()));

        // Changed parameters: compare PVI args with bound variables
        auto pvi_args = as_vector(pvi.parameters());
        for (std::size_t j = 0; j < params.size() && j < pvi_args.size(); ++j)
        {
          if (pvi_args[j] != atermpp::down_cast<data::data_expression>(params[j]))
          {
            // params[j] changes in this transition — all guard variables rule it
            for (const auto& gv: guard_vars)
            {
              if (params[j] != gv)
              {
                counts[eq_name][params[j]][gv]++;
              }
            }
          }
        }
      }
    }

    // Build the ruling relation, pruning mutual pairs to keep only the stronger direction.
    m_ruling_relation.clear();
    for (const auto& [eq_name, ruled_by_counts]: counts)
    {
      for (const auto& [d_m, rulers_counts]: ruled_by_counts)
      {
        for (const auto& [d_j, count_j]: rulers_counts)
        {
          // Check if the reverse direction also exists
          auto rev_it = counts.find(eq_name);
          if (rev_it != counts.end())
          {
            auto ruled_it = rev_it->second.find(d_j);
            if (ruled_it != rev_it->second.end())
            {
              auto rev_count_it = ruled_it->second.find(d_m);
              if (rev_count_it != ruled_it->second.end())
              {
                // Both d_j ≽ d_m and d_m ≽ d_j exist.
                // Keep only the stronger direction (higher count).
                // When counts are equal, break the tie by name.
                // Longer cycles are removed by the cycle-breaking pass below.
                if (count_j < rev_count_it->second)
                {
                  continue; // d_m ≽ d_j is stronger, skip d_j ≽ d_m
                }
                else if (count_j == rev_count_it->second && d_j.name() > d_m.name())
                {
                  continue;
                }
              }
            }
          }
          m_ruling_relation[eq_name][d_m].insert(d_j);
        }
      }
    }

    // Break longer cycles (3 and up) so that the relation is a strict order.
    // For each cycle, remove the edge that rules the most dominant node, i.e.
    // drop the cycle-successor from the ruler set of the node with the highest
    // occurrence count in the equation formula (ties broken by name). This makes
    // the most dominant node of the cycle a root, which is what
    // choose_variable_by_ruling_order ranks highest anyway. Each removal
    // strictly decreases the number of edges, so the loop terminates.
    for (const pbes_equation& eq: p.equations())
    {
      const auto& eq_name = eq.variable().name();
      auto rel_it = m_ruling_relation.find(eq_name);
      if (rel_it == m_ruling_relation.end())
      {
        continue;
      }
      auto& ruled_by_map = rel_it->second;

      // Guard-count weight of a node: total number of self-recursive transitions
      // in which it is changed while guarded by something (sum of the incoming edge
      // weights already computed in `counts`). This reuses the same dominance evidence
      // the 2-cycle pruning relies on, so the more dominant node of a cycle becomes
      // the root when the cycle is broken.
      const auto& eq_counts = counts[eq_name];
      auto node_weight = [&](const data::variable& v) -> std::size_t
      {
        auto it = eq_counts.find(v);
        if (it == eq_counts.end())
        {
          return 0;
        }
        std::size_t sum = 0;
        for (const auto& [ruler, c]: it->second)
        {
          sum += c;
        }
        return sum;
      };

      while (true)
      {
        // Find one simple cycle via DFS along the ruled-by edges.
        // On success, `path` ends in the cycle: path = [..., c, ..., x, c].
        std::set<data::variable> done;
        std::vector<data::variable> path;
        std::function<bool(const data::variable&)> dfs = [&](const data::variable& current) -> bool
        {
          if (done.contains(current))
          {
            return false;
          }
          if (std::find(path.begin(), path.end(), current) != path.end())
          {
            path.push_back(current);
            return true;
          }
          path.push_back(current);
          auto rit = ruled_by_map.find(current);
          if (rit != ruled_by_map.end())
          {
            for (const data::variable& ruler: rit->second)
            {
              if (dfs(ruler))
              {
                return true;
              }
            }
          }
          path.pop_back();
          done.insert(current);
          return false;
        };

        bool cycle_found = false;
        for (const auto& [d_m, rulers]: ruled_by_map)
        {
          if (dfs(d_m))
          {
            cycle_found = true;
            break;
          }
        }
        if (!cycle_found)
        {
          break;
        }

        // Extract the cycle: suffix of path starting at the first occurrence of
        // the repeated last element. Edge order: cycle[i] is ruled by cycle[i+1].
        const std::vector<data::variable> cycle(path.begin() + (std::find(path.begin(), path.end(), path.back()) - path.begin()),
          path.end() - 1);

        // Find the most dominant node on the cycle and its cycle-successor.
        const data::variable* v_max = &cycle.front();
        for (const data::variable& v: cycle)
        {
          std::size_t w = node_weight(v);
          std::size_t w_max = node_weight(*v_max);
          if (w > w_max || (w == w_max && v.name() < v_max->name()))
          {
            v_max = &v;
          }
        }
        std::size_t idx = static_cast<std::size_t>(std::find(cycle.begin(), cycle.end(), *v_max) - cycle.begin());
        const data::variable& succ = cycle[(idx + 1) % cycle.size()];

        ruled_by_map[*v_max].erase(succ);
        if (ruled_by_map[*v_max].empty())
        {
          ruled_by_map.erase(*v_max);
        }
      }
    }

    mCRL2log(log::debug) << "=== Ruling relation ===" << std::endl;
    for (const auto& [eq_name, ruled_by_map]: m_ruling_relation)
    {
      for (const auto& [d_m, parameters]: ruled_by_map)
      {
        mCRL2log(log::debug) << eq_name << ": " << pp(d_m) << " ruled by " << core::detail::print_list(parameters)
                             << std::endl;
      }
    }
  }

  // Enforces the order-ideal invariant: abstracted gate → abstracted data.
  // Contrapositive: if dₘ is concrete and dⱼ ≽ dₘ and dⱼ is abstracted,
  // then dⱼ must be made concrete too. Uses a TODO set for efficiency:
  // only parameters affected by newly-concrete parameters are checked.
  void make_rules_ideal(const pbes& p, abstract_param_state& state)
  {
    // Phase 1: find all violations — concrete parameters with abstracted rulers.
    // Each violation requires un-abstracting the ruler (making it concrete).
    std::deque<std::pair<core::identifier_string, data::variable>> todo;

    for (const auto& [eq_name, ruled_by_map]: m_ruling_relation)
    {
      for (const auto& [d_m, rulers]: ruled_by_map)
      {
        if (!state.W[eq_name].contains(d_m)) // dₘ is concrete
        {
          for (const auto& d_j: rulers)
          {
            if (state.W[eq_name].contains(d_j)) // dⱼ is abstracted → violation
            {
              todo.emplace_back(eq_name, d_j);
            }
          }
        }
      }
    }

    // Phase 2: cascade. When dⱼ is un-abstracted (made concrete), its own
    // rulers must also be concrete (by the order ideal applied to dⱼ as data).
    std::set<std::pair<core::identifier_string, data::variable>> seen;
    while (!todo.empty())
    {
      auto key = todo.front();
      todo.pop_front();
      if (!seen.insert(key).second)
      {
        continue;
      }

      auto& [eq_name, d_j] = key;
      if (!state.W[eq_name].contains(d_j))
      {
        continue; // already concrete
      }

      state.remove_abstracted_variable(p, eq_name, d_j);
      mCRL2log(log::debug) << "Rules ideal: un-abstracted " << d_j.name() << " from " << eq_name << std::endl;

      // dⱼ is now concrete — its rulers must be concrete too.
      auto it = m_ruling_relation[eq_name].find(d_j);
      if (it != m_ruling_relation[eq_name].end())
      {
        for (const auto& d_k: it->second) // dₖ ≽ dⱼ
        {
          if (state.W[eq_name].contains(d_k)) // dₖ is abstracted
          {
            todo.emplace_back(eq_name, d_k);
          }
        }
      }
    }
  }

  void print_abstraction_summary(const abstract_param_state& state)
  {
    for (const auto& [eq_name, var_set]: state.W)
    {
      std::string param_names;
      for (const data::variable& var: var_set)
      {
        param_names += var.name();
        param_names += " ";
      }
      std::string indices;
      for (const std::size_t& i: state.I.at(eq_name))
      {
        indices += std::to_string(i);
        indices += " ";
      }
      mCRL2log(log::verbose) << "Abstracted parameters for " << eq_name << ": " << param_names;
      mCRL2log(log::debug) << " (indices: " << indices << ")" << std::endl;
      mCRL2log(log::verbose) << std::endl;
    }
  }

  // Removes one parameter from one equation's abstraction set
  void unabstract_one_parameter(const pbes& p, abstract_param_state& state, const pbescegps_options& options)
  {
    mCRL2log(log::debug) << "Updating parameters for refinement..." << std::endl;

    // Find the first non-empty equation's abstraction set
    bool found = false;
    for (auto it = state.W.rbegin(); it != state.W.rend(); it++)
    {
      if (!it->second.empty())
      {
        core::identifier_string eq_name = it->first;
        auto eq_opt = detail::find_equation_by_name(p, eq_name);
        if (eq_opt)
        {
          pbes_expression eq_formula = eq_opt->get().formula();
          propositional_variable bound_variable = eq_opt->get().variable();
          // TODO: I am not convinced the current calculation makes sense at all.
          // std::set<data::variable> essential_vars = find_essential_variables(eq_formula, state.W[eq_name], state.I);
          std::set<data::variable> essential_vars = state.W[eq_name];
          mCRL2log(log::debug) << "Essential variables: " << eq_name << ": " << essential_vars.size() << " ("
                               << core::detail::print_list(essential_vars) << ")" << std::endl;

          std::optional<data::variable> selected_var;

          if (options.var_choice == var_choice_strategy::all)
          {
            mCRL2log(log::debug) << "Un-abstracted all parameters " << core::detail::print_list(essential_vars)
                                 << " from equation " << eq_name << std::endl;
            for (const data::variable& var: essential_vars)
            {
              state.remove_abstracted_variable(p, eq_name, var);
            }
            found = true;
            return;
          }
          else if (options.var_choice == var_choice_strategy::count)
          {
            selected_var = detail::choose_variable_by_count(eq_name, eq_formula, essential_vars, m_var_count_cache);
          }
          else if (options.var_choice == var_choice_strategy::rhs)
          {
            selected_var = detail::choose_variable_by_rhs_order(eq_formula, essential_vars);
          }
          else if (options.var_choice == var_choice_strategy::ruling)
          {
            selected_var
              = detail::choose_variable_by_ruling_order(eq_name, essential_vars, m_ruling_relation, eq_formula);
            if (!selected_var)
            {
              selected_var = detail::choose_variable_by_rhs_order(eq_formula, essential_vars);
            }
          }
          else if (options.var_choice == var_choice_strategy::lhs)
          {
            selected_var = detail::choose_variable_by_lhs_order(bound_variable, essential_vars, std::nullopt);
          }
          else
          {
            throw mcrl2::runtime_error("Unknown var-choice option; this should not happen.");
          }

          if (selected_var)
          {
            mCRL2log(log::debug) << "Un-abstracted parameter " << selected_var->name() << " from equation " << eq_name
                                 << std::endl;
            state.remove_abstracted_variable(p, eq_name, *selected_var);
            found = true;
            return;
          }
        }
      }
    }

    if (!found)
      throw mcrl2::runtime_error("No essential variable found for un-abstracting parameter.");
  }

  bool run_cegps_algorithm(pbes& p, pbescegps_options options)
  {
    abstract_param_state unused_state;
    return run_cegps_algorithm(p, options, unused_state);
  }

  bool run_cegps_algorithm(pbes& p, pbescegps_options options, abstract_param_state& final_state)
  {
    // Create the data rewriter once and reuse it throughout the tool
    m_datar.emplace(p.data(), options.rewrite_strategy);

    // Compute the ruling relation once from the PBES structure
    compute_ruling_relation(p);

    // Calculate non-Control Flow Parameters (parameters to abstract) per equation
    abstract_param_state state;
    if (options.initial_state_file.empty())
    {
      compute_initial_abstraction_set(p, options.init_control_flow, state);
    }
    else
    {
      detail::initialize_initial_abstraction_state(p, state, options.initial_state_file);
    }

    if (options.instantiate_infinite_quantifier_guards)
    {
      instantiate_infinite_quantifier_guards(p, state);
    }

    pbes original_p = p;

    // Ensure W is data-closed
    make_data_closed(p, state);

    // Ensure W is rule-ideal: abstracted gate → abstracted data
    if (options.rules_ideal)
    {
      make_rules_ideal(p, state);
      make_data_closed(p, state);
    }

    // Collect sorts to abstract (non-CFP parameters)
    print_abstraction_summary(state);

    // Iterative refinement loop
    do
    {
      // Check if all equations have empty abstraction sets
      bool all_empty = true;
      for (const auto& [eq_name, var_set]: state.W)
      {
        if (!var_set.empty())
        {
          all_empty = false;
          break;
        }
      }

      if (all_empty)
      {
        mCRL2log(log::debug) << "No parameters to abstract, solving normally." << std::endl;
        auto [result, graph] = solve(p, options);
        final_state = state;
        return result;
      }

      // Try under-approximation
      mCRL2log(log::verbose) << "Trying under-approximation..." << std::endl;
      structure_graph under_graph;
      bool under_result = solve_approximation_cached(p, state, false, options, under_graph);

      if (under_result)
      {
        mCRL2log(log::verbose) << "Under-approximation solved to TRUE" << std::endl;
        print_abstraction_summary(state);
        final_state = state;
        return true;
      }

      // Try over-approximation
      mCRL2log(log::verbose) << "Trying over-approximation..." << std::endl;
      structure_graph over_graph;
      bool over_result = solve_approximation_cached(p, state, true, options, over_graph);

      if (!over_result)
      {
        mCRL2log(log::verbose) << "Over-approximation solved to FALSE" << std::endl;
        print_abstraction_summary(state);
        final_state = state;
        return false;
      }

      // Both approximations are inconclusive, refine by un-abstracting one parameter
      mCRL2log(log::verbose) << "Both approximations inconclusive, refining..." << std::endl;
      p = original_p;

      // Create the approximated PBES instances for matching
      pbes under_pbes = apply_abstraction_to_pbes(p, state, false, options);
      pbes over_pbes = apply_abstraction_to_pbes(p, state, true, options);

      pbescegps_refine_strategies refine;
      if (!refine.refine_using_strategies(p,
            under_pbes,
            over_pbes,
            state,
            options,
            under_graph,
            over_graph,
            *m_datar,
            m_ruling_relation))
      {
        unabstract_one_parameter(p, state, options);
      }
      make_data_closed(p, state);
      if (options.rules_ideal)
      {
        make_rules_ideal(p, state);
        make_data_closed(p, state);
      }
      print_abstraction_summary(state);
    }
    while (true);

    throw mcrl2::runtime_error("Could not find a solution");
  }
};

// Abstraction builder implementation
// Must be defined outside the struct due to template constraints
pbes_expression pbescegps_iterator::apply_abstraction(const pbes_expression& expr,
  const std::set<data::variable>& abstraction_vars,
  const std::map<core::identifier_string, std::set<std::size_t>>& pbes_parameters_abstraction_indices,
  bool is_overapproximation)
{
  mCRL2log(log::trace) << "=== Entering apply_abstraction ===" << std::endl;
  mCRL2log(log::trace) << "Abstraction mode: " << (is_overapproximation ? "OVER-approximation" : "UNDER-approximation")
                       << std::endl;
  mCRL2log(log::trace) << "Number of variables to abstract: " << abstraction_vars.size() << std::endl;
  for (const auto& var: abstraction_vars)
  {
    mCRL2log(log::trace) << "  - " << var.name() << std::endl;
  }

  pbes_expression result;
  abstraction_rewriter<> rewriter(abstraction_vars, pbes_parameters_abstraction_indices, is_overapproximation);
  mCRL2log(log::trace) << "Created abstraction_rewriter, now applying to expression" << std::endl;
  rewriter.apply(result, expr);
  mCRL2log(log::trace) << "=== Exiting apply_abstraction ===" << std::endl;
  return result;
}

inline bool pbescegps(const std::string& input_filename,
  const utilities::file_format& input_format,
  const pbescegps_options options)
{
  pbes p;
  load_pbes(p, input_filename, input_format);
  algorithms::normalize(p);

  pbescegps_iterator iterator;
  bool result = iterator.run_cegps_algorithm(p, options);

  mCRL2log(log::info) << (result ? "true" : "false") << std::endl;
  return result;
}
}; // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_TOOLS_PBESCEGPS_H
