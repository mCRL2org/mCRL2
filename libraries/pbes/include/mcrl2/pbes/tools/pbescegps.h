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

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/container_sort.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/sort_expression.h"
#include "mcrl2/data/structured_sort.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/builder.h"
#include "mcrl2/pbes/detail/count_free_variables.h"
#include "mcrl2/pbes/detail/instantiate_global_variables.h"
#include "mcrl2/pbes/detail/pbessolve_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_global_algorithm.h"
#include "mcrl2/pbes/detail/stategraph_pbes.h"
#include "mcrl2/pbes/find.h"
#include "mcrl2/pbes/io.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbesinst_structure_graph.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/rewrite.h"
#include <algorithm>
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
#include <iostream>
#include <map>
#include <ranges>
#include <set>
#include <vector>

namespace bp = boost::process;

namespace mcrl2::pbes_system
{

struct pbescegps_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  bool init_control_flow = false;
  bool solve_symbolic = false;
  std::string var_choice = "first";
  std::string solve_symbolic_args = "";
};

struct abstract_param_state
{
  std::map<core::identifier_string, std::set<data::variable>> W;
  std::map<core::identifier_string, std::set<std::size_t>> I;

  // Remove a variable from W for a specific equation and update indices
  void remove_abstracted_variable(const pbes& p, const core::identifier_string& eq_name, const data::variable& var)
  {
    W[eq_name].erase(var);
    for (const auto& eq: p.equations())
    {
      if (eq.variable().name() == eq_name)
      {
        std::size_t i = 0;
        for (const auto& param: eq.variable().parameters())
        {
          if (param.name() == var.name())
          {
            I[eq_name].erase(i);
            break;
          }
          ++i;
        }
        break;
      }
    }
  }

  // Add a variable to W for a specific equation and update indices
  void add_abstracted_variable(const pbes& p, const core::identifier_string& eq_name, const data::variable& var)
  {
    W[eq_name].insert(var);
    for (const auto& eq: p.equations())
    {
      if (eq.variable().name() == eq_name)
      {
        std::size_t i = 0;
        for (const auto& param: eq.variable().parameters())
        {
          if (param.name() == var.name())
          {
            I[eq_name].insert(i);
            break;
          }
          ++i;
        }
        break;
      }
    }
  }
};

struct pbescegps_iterator
{
private:
  utilities::indexed_set<data::data_expression> m_values;

  bp::child sym_process;

  // Cache for variable occurrence counts to avoid recomputing
  // Keyed by equation name instead of formula pointer for stability
  std::map<core::identifier_string, std::map<data::variable, std::size_t>> m_var_count_cache;

public:
  bool solve(const pbes& p, pbescegps_options options)
  {
    pbes p_copy(p);
    utilities::execution_timer timer;
    mcrl2::log::log_level_t saved_level = mcrl2::log::logger::get_reporting_level();
    mcrl2::log::logger::set_reporting_level(mcrl2::log::error);

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
        mcrl2::runtime_error("symbolic solver failed: " + std::string(e.what()));
      }
    }
    else
    {
      pbessolve_options options2;
      options2.rewrite_strategy = options.rewrite_strategy;

      structure_graph G;
      pbesinst_structure_graph_algorithm algorithm(options2, p_copy, G);
      algorithm.run();

      // Solve the structure graph
      result = solve_structure_graph(G);
      mCRL2log(log::verbose) << "Structure graph solver returned " << (result ? "TRUE" : "FALSE") << std::endl;
    }
    timer.finish("solving approximation");
    if (mcrl2::log::mCRL2logEnabled(log::verbose))
    {
      timer.report();
    }
    mcrl2::log::logger::set_reporting_level(saved_level);
    return result;
  }

  // Solves the underapproximated PBES using structure graph solving
  bool solve_approximation(const pbes& p, pbescegps_options options, const bool& is_overapproximation)
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
    pbescegps_options options)
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
    data::rewriter datar(p.data(), options.rewrite_strategy);
    simplify_data_rewriter<data::rewriter> pbesr(datar);
    pbes_rewrite(result, pbesr);

    return result;
  }

  // Helper: Calculate non-Control Flow Parameters (CFP) per equation
  // Populates the abstraction_state directly with W and indices
  void compute_initial_abstraction_set(pbes& p,
    pbescegps_options& options,
    const bool use_init_control_flow,
    abstract_param_state& state)
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

    data::rewriter datar(p.data(), options.rewrite_strategy);
    detail::stategraph_pbes stategraph(p, datar);
    pbesstategraph_options opts;
    detail::stategraph_algorithm algo(p, opts);

    for (detail::stategraph_equation& equation: stategraph.equations())
    {
      for (detail::predicate_variable& predvar: equation.predicate_variables())
      {
        predvar.simplify_guard();
      }
    }

    stategraph.compute_source_target_copy();
    algo.run();

    // Get the GCFP vector for each equation
    const auto& gcfp_map = algo.get_GCFP();

    for (const auto& [eq_name, cfp_vector]: gcfp_map)
    {
      // Find the corresponding equation to get parameter list
      for (const pbes_equation& eq: p.equations())
      {
        if (eq.variable().name() == eq_name)
        {
          const data::variable_list& params = eq.variable().parameters();

          // cfp_vector[i] == true means parameter i is a CFP
          // cfp_vector[i] == false means parameter i is NOT a CFP (non-CFP)
          for (std::size_t i = 0; i < cfp_vector.size() && i < params.size(); ++i)
          {
            if (cfp_vector[i]) // If IS a CFP (remove from non-CFP set)
            {
              state.remove_abstracted_variable(p, eq_name, atermpp::down_cast<data::variable>(params[i]));
            }
          }
          break;
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
          mCRL2log(log::debug) << "Data-closed: eq " << eq.variable().name() << " pvi " << pvi
                               << " has abstracted parameters at indices "
                               << core::detail::print_list(state.I.at(pvi.name())) << " with "
                               << core::detail::print_list(state.W.at(pvi.name())) << std::endl;
          for (const auto& pvi_param: pvi.parameters())
          {
            // Check for the terms that we want to instantiate
            if (!contains(state.I.at(pvi.name()), i))
            {
              std::set<data::variable> free_vars = find_free_variables(pvi_param);
              mCRL2log(log::debug) << "Data-closed: free_vars " << core::detail::print_list(free_vars) << " in \""
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
    mCRL2log(log::debug) << "======== Data closed ======" << std::endl;
    mCRL2log(log::debug) << "Data closed: W = " << std::endl;
    for (const auto& [eq_name, var_set]: state.W)
    {
      mCRL2log(log::debug) << "" << eq_name << ": " << core::detail::print_list(var_set) << std::endl;
    }
  }

  void print_abstraction_summary(const std::map<core::identifier_string, std::set<data::variable>>& W_map)
  {
    for (const auto& [eq_name, var_set]: W_map)
    {
      std::string param_names;
      for (const auto& var: var_set)
      {
        param_names += var.name();
        param_names += " ";
      }
      mCRL2log(log::verbose) << "Abstracted parameters for " << eq_name << ": " << param_names << std::endl;
    }
  }

  std::optional<data::variable> choose_variable_by_count(const core::identifier_string& eq_name,
    const pbes_expression& eq_formula,
    const std::set<data::variable>& essential_vars)
  {
    // Check if we have a cached result for this equation
    auto cache_it = m_var_count_cache.find(eq_name);
    if (cache_it == m_var_count_cache.end())
    {
      // Compute and cache the results
      mCRL2log(log::verbose) << "ache miss for " << eq_name << std::endl;
      auto var_counts = detail::count_free_variable_occurrences(eq_formula, false);
      m_var_count_cache[eq_name] = var_counts;
      cache_it = m_var_count_cache.find(eq_name);
    }

    const auto& var_counts = cache_it->second;
    std::size_t best_count = 0;
    std::optional<data::variable> best_var;
    for (const data::variable& var: essential_vars)
    {
      mCRL2log(log::debug) << "  - " << var.name() << " -> " << var_counts.at(var) << std::endl;
      if (var_counts.find(var) != var_counts.end())
      {
        std::size_t count = var_counts.at(var);
        if (count > best_count)
        {
          best_count = count;
          best_var = var;
        }
      }
    }
    return best_var;
  }

  std::optional<data::variable> choose_variable_by_first_occurrence(const propositional_variable& bnd_var,
    const std::set<data::variable>& essential_vars)
  {
    for (const auto& param: bnd_var.parameters())
    {
      data::variable var = atermpp::down_cast<data::variable>(param);
      if (essential_vars.contains(var))
      {
        return var;
      }
    }
    return std::nullopt;
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
        pbes_expression eq_formula;
        propositional_variable bnd_var;
        for (const pbes_equation& eq: p.equations())
        {
          if (eq.variable().name() == eq_name)
          {
            eq_formula = eq.formula();
            bnd_var = eq.variable();
            break;
          }
        }
        // TODO: I am not convinced the current calculation makes sense at all.
        // std::set<data::variable> essential_vars = find_essential_variables(eq_formula, state.W[eq_name], state.I);
        std::set<data::variable> essential_vars = state.W[eq_name];
        mCRL2log(log::debug) << "Essential variables: " << eq_name << ": " << essential_vars.size() << " ("
                             << core::detail::print_list(essential_vars) << ")" << std::endl;

        std::optional<data::variable> selected_var;

        if (options.var_choice == "count")
        {
          selected_var = choose_variable_by_count(eq_name, eq_formula, essential_vars);
        }
        else if (options.var_choice == "first")
        {
          selected_var = choose_variable_by_first_occurrence(bnd_var, essential_vars);
        }
        else
        {
          mCRL2log(log::warning) << "Unknown var-choice option '" << options.var_choice << "'; using 'first'."
                                 << std::endl;
          selected_var = choose_variable_by_first_occurrence(bnd_var, essential_vars);
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

    if (!found)
      throw mcrl2::runtime_error("No essential variable found for un-abstracting parameter.");
  }

  bool run_cegps_algorithm(pbes& p, pbescegps_options options)
  {
    // Calculate non-Control Flow Parameters (parameters to abstract) per equation
    abstract_param_state state;
    compute_initial_abstraction_set(p, options, options.init_control_flow, state);

    pbes original_p = p;

    // Ensure W is data-closed
    make_data_closed(p, state);

    // Collect sorts to abstract (non-CFP parameters)
    print_abstraction_summary(state.W);

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
        return solve(p, options);
      }

      // Try under-approximation
      pbes p_under = apply_abstraction_to_pbes(p, state, false, options);
      mCRL2log(log::verbose) << "Trying under-approximation..." << std::endl;
      bool under_result = solve_approximation(p_under, options, false);

      if (under_result)
      {
        mCRL2log(log::verbose) << "Under-approximation solved to TRUE" << std::endl;
        print_abstraction_summary(state.W);
        return true;
      }

      // Try over-approximation
      pbes p_over = apply_abstraction_to_pbes(p, state, true, options);
      bool over_result = solve_approximation(p_over, options, true);
      mCRL2log(log::verbose) << "Trying over-approximation..." << std::endl;

      if (!over_result)
      {
        mCRL2log(log::verbose) << "Over-approximation solved to FALSE" << std::endl;
        print_abstraction_summary(state.W);
        return false;
      }

      // Both approximations are inconclusive, refine by un-abstracting one parameter
      mCRL2log(log::verbose) << "Both approximations inconclusive, refining..." << std::endl;
      p = original_p;
      unabstract_one_parameter(p, state, options);
      make_data_closed(p, state);
      print_abstraction_summary(state.W);
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
