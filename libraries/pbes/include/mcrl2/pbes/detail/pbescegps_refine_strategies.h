// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbescegps_refine_strategies.h
/// \brief Refinement strategies for the CEGPS algorithm.
///        Walks counterexample/witness structure graphs to find
///        parameters that can be un-abstracted.

#ifndef MCRL2_PBES_DETAIL_PBESCEGPS_REFINE_STRATEGIES_H
#define MCRL2_PBES_DETAIL_PBESCEGPS_REFINE_STRATEGIES_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/detail/pbescegps_utilities.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/structure_graph.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include <algorithm>
#include <cstddef>
#include <optional>

namespace mcrl2::pbes_system
{

/// \brief Refinement strategies for the CEGPS algorithm.
///
/// Given an under-approximation structure graph (counterexample) and an
/// over-approximation structure graph (witness), this class walks strategy
/// paths in both graphs to find parameters that should be un-abstracted.
struct pbescegps_refine_strategies
{
  using decoration_type = structure_graph::decoration_type;
  using index_type = structure_graph::index_type;
  using vertex = structure_graph::vertex;

private:
  const pbes* m_p = nullptr;
  abstract_param_state* m_state = nullptr;
  const pbescegps_options* m_options = nullptr;
  const data::rewriter* m_datar = nullptr;

  // Maps equation names to the vector of remaining parameters in each approximation
  // This is in the order they appear in the approximated equation
  std::map<core::identifier_string, std::vector<data::variable>> m_under_params;
  std::map<core::identifier_string, std::vector<data::variable>> m_over_params;

  // Maps equation names to the original parameters for reference
  std::map<core::identifier_string, std::vector<data::variable>> m_original_params;

  std::map<core::identifier_string, std::map<data::variable, std::size_t>> m_var_count_cache;

  // Maps each parameter in a PVI to its original parameter name and compares
  // by finding common parameters in both under and over approximations.
  bool pvis_match_by_common_parameters(const propositional_variable_instantiation& a,
    const propositional_variable_instantiation& b,
    bool find_in_over) const
  {
    mCRL2log(log::trace) << "pvis_match_by_common_parameters comparing " << a << " with " << b << std::endl;

    if (a.name() != b.name())
    {
      mCRL2log(log::trace) << "  Different names => NO MATCH" << std::endl;
      return false;
    }

    auto orig_eq_opt = detail::find_equation_by_name(*m_p, a.name());
    if (!orig_eq_opt)
    {
      throw mcrl2::runtime_error("Could not find original equation for " + pp(a));
    }
    auto orig_it = m_original_params.find(a.name());
    if (orig_it == m_original_params.end())
    {
      throw mcrl2::runtime_error("Could not find original params for equation " + pp(a));
    }
    const std::vector<data::variable>& orig_params = orig_it->second;

    const std::vector<data::data_expression>& a_args = as_vector(a.parameters());
    const std::vector<data::data_expression>& b_args = as_vector(b.parameters());

    mCRL2log(log::trace) << "  Original params: ";
    for (const auto& p: orig_params)
      mCRL2log(log::trace) << p.name() << " ";
    mCRL2log(log::trace) << std::endl;

    auto it_under = m_under_params.find(a.name());
    auto it_over = m_over_params.find(a.name());
    if (it_under == m_under_params.end() || it_over == m_over_params.end())
    {
      throw mcrl2::runtime_error("Could not find remaining params for equation " + pp(a));
    }

    const std::vector<data::variable>& under_params = it_under->second;
    const std::vector<data::variable>& over_params = it_over->second;

    // Try matching with a as under, b as over
    bool match = find_in_over ? try_match(a_args, b_args, under_params, over_params)
                              : try_match(a_args, b_args, over_params, under_params);

    mCRL2log(log::trace) << (match ? "  MATCH" : "  NO MATCH") << std::endl;
    return match;
  }

  // Helper function to try matching PVI arguments against parameter sets
  bool try_match(const std::vector<data::data_expression>& a_args,
    const std::vector<data::data_expression>& b_args,
    const std::vector<data::variable>& a_params,
    const std::vector<data::variable>& b_params) const
  {
    // Build maps from parameter name to its value
    std::map<std::string, data::data_expression> a_values;
    std::map<std::string, data::data_expression> b_values;

    // Map arguments to parameter names using the parameter ordering
    if (a_args.size() != a_params.size())
    {
      throw std::runtime_error(
        "Arg count mismatch: " + std::to_string(a_args.size()) + " vs " + std::to_string(a_params.size()));
    }

    if (b_args.size() != b_params.size())
    {
      throw std::runtime_error(
        "Arg count mismatch: " + std::to_string(b_args.size()) + " vs " + std::to_string(b_params.size()));
    }

    for (std::size_t i = 0; i < a_params.size(); ++i)
    {
      a_values[a_params[i].name()] = a_args[i];
      mCRL2log(log::trace) << "    a[" << a_params[i].name() << "]=" << pp(a_args[i]) << std::endl;
    }

    for (std::size_t i = 0; i < b_params.size(); ++i)
    {
      b_values[b_params[i].name()] = b_args[i];
      mCRL2log(log::trace) << "    b[" << b_params[i].name() << "]=" << pp(b_args[i]) << std::endl;
    }

    // Find common parameters and check they have the same values
    for (const auto& [param_name, a_val]: a_values)
    {
      auto it_b = b_values.find(param_name);
      if (it_b != b_values.end())
      {
        const data::data_expression& b_val = it_b->second;
        mCRL2log(log::trace) << "    Common param " << param_name << ": a=" << pp(a_val) << " vs b=" << pp(b_val);

        if (a_val != b_val)
        {
          mCRL2log(log::trace) << " => NOT EQUAL" << std::endl;
          return false;
        }
        mCRL2log(log::trace) << " => EQUAL" << std::endl;
      }
    }

    return true;
  }

  // Determines whether two vertex formulae should be considered equal for the
  // purpose of matching vertices between the under- and over-approximation
  // structure graphs. When parameters are removed from an equation via abstraction
  // or parelm, matching is done by original parameter position (see
  // pvis_match_by_original_position), comparing only at positions that are
  // concrete in both PVIs.
  bool formulae_match(const pbes_expression& a, const pbes_expression& b, bool find_in_over) const
  {
    return a == b
           || pvis_match_by_common_parameters(atermpp::down_cast<propositional_variable_instantiation>(a),
             atermpp::down_cast<propositional_variable_instantiation>(b),
             find_in_over);
  }

  index_type find_vertex_index_by_formula(const structure_graph& g, const pbes_expression& formula, bool find_in_over)
  {
    for (index_type idx = 0; idx < g.extent(); ++idx)
    {
      if (formulae_match(formula, g.find_vertex(idx).formula(), find_in_over))
      {
        return idx;
      }
    }
    return undefined_vertex();
  }

  static bool has_edge(const structure_graph& g, index_type from, index_type to)
  {
    if (from >= g.extent() || to >= g.extent())
    {
      return false;
    }
    const std::vector<index_type>& successors = g.all_successors(from);
    return std::find(successors.begin(), successors.end(), to) != successors.end();
  }

  bool select_variable(const structure_graph& g,
    index_type current_idx,
    const structure_graph& g_prime,
    index_type matching_idx,
    const std::string& phase)
  {
    const vertex& current_vertex = g.find_vertex(current_idx);
    const propositional_variable_instantiation& pvi
      = atermpp::down_cast<propositional_variable_instantiation>(current_vertex.formula());
    core::identifier_string var_name = pvi.name();

    abstract_param_state& state = *m_state;
    const pbescegps_options& options = *m_options;
    const pbes& p = *m_p;

    auto wit = state.W.find(var_name);
    if (wit == state.W.end() || wit->second.empty())
    {
      return false;
    }

    pbes_expression equation_formula;
    propositional_variable bound_variable;
    for (const pbes_equation& equation: p.equations())
    {
      if (equation.variable().name() == var_name)
      {
        equation_formula = equation.formula();
        bound_variable = equation.variable();
        break;
      }
    }

    data::mutable_indexed_substitution sigma;
    data::data_expression_list current_pvi_args = pvi.parameters();
    for (const data::variable& abstracted_param: bound_variable.parameters())
    {
      if (!state.W[var_name].contains(abstracted_param))
      {
        if (current_pvi_args.empty())
          break;
        sigma[abstracted_param] = current_pvi_args.front();
        current_pvi_args.pop_front();
      }
    }

    simplify_data_rewriter<data::rewriter> pbes_rewriter(*m_datar);
    pbes_expression instantiated_formula = pbes_rewrite(equation_formula, pbes_rewriter, sigma);
    mCRL2log(log::debug) << "Phase " << phase << ": Instantiated " << current_vertex << std::endl
                         << "to " << instantiated_formula << std::endl;

    std::set<data::variable> essential_vars = wit->second;
    pbes_expression guard_formula = instantiated_formula;

    std::optional<vertex> matching_vertex;

    if (matching_idx != undefined_vertex())
    {
      matching_vertex = g_prime.find_vertex(matching_idx);
    }

    if (current_vertex.strategy != undefined_vertex()
        || (matching_vertex.has_value() && matching_vertex->strategy != undefined_vertex()))
    {
      auto all_pvis = find_propositional_variable_instantiations(instantiated_formula);
      const propositional_variable_instantiation succ_pvi
        = current_vertex.strategy != undefined_vertex()
            ? atermpp::down_cast<propositional_variable_instantiation>(g.find_vertex(current_vertex.strategy).formula())
            : atermpp::down_cast<propositional_variable_instantiation>(
                g_prime.find_vertex(matching_vertex->strategy).formula());

      std::set<propositional_variable_instantiation> candidate_pvis;
      for (const auto& candidate: all_pvis)
      {
        if (candidate.name() != succ_pvi.name())
        {
          continue;
        }

        bool matches = true;
        auto cand_it = candidate.parameters().begin();
        auto succ_it = succ_pvi.parameters().begin();
        auto cand_end = candidate.parameters().end();
        auto succ_end = succ_pvi.parameters().end();

        for (const pbes_equation& equation: p.equations())
        {
          if (equation.variable().name() == candidate.name())
          {
            for (const auto& param: equation.variable().parameters())
            {
              if (cand_it == cand_end || succ_it == succ_end)
              {
                matches = true;
                break;
              }
              data::variable var = atermpp::down_cast<data::variable>(param);
              if (!state.W[candidate.name()].contains(var) && find_free_variables(*cand_it).empty())
              {
                data::data_expression eq_expr = data::lazy::equal_to(*cand_it, *succ_it);
                data::data_expression rewritten = (*m_datar)(eq_expr);
                if (rewritten != data::sort_bool::true_())
                {
                  matches = false;
                  break;
                }
              }
              ++cand_it;
              ++succ_it;
            }
            break;
          }
        }

        if (matches)
        {
          candidate_pvis.insert(candidate);
        }
      }

      mCRL2log(log::debug) << "Candidate PVIs: " << core::detail::print_list(candidate_pvis) << std::endl;
      if (!candidate_pvis.empty())
      {
        detail::guard_traverser guard_trav(*m_datar);
        guard_trav.apply(instantiated_formula);
        const std::vector<std::pair<propositional_variable_instantiation, pbes_expression>>& guards
          = guard_trav.expression_stack.back().guards;

        for (const auto& [pvi, guard_expr]: guards)
        {
          std::set<data::variable> guard_vars = find_free_variables(guard_expr);
          std::set<data::variable> common_vars;
          std::set_intersection(state.W[bound_variable.name()].begin(),
            state.W[bound_variable.name()].end(),
            guard_vars.begin(),
            guard_vars.end(),
            std::inserter(common_vars, common_vars.begin()));
          if (!common_vars.empty())
          {
            essential_vars = std::move(common_vars);
            guard_formula = guard_expr;
            mCRL2log(log::debug) << "Guard vars: " << core::detail::print_list(guard_vars) << std::endl;
            mCRL2log(log::debug) << "Guard formula: " << guard_formula << std::endl;
            break;
          }
        }
      }
    }

    if (options.var_choice == var_choice_strategy::all && guard_formula != instantiated_formula)
    {
      mCRL2log(log::debug) << "Phase " << phase << ": Un-abstracting " << core::detail::print_list(essential_vars)
                           << " from " << var_name << std::endl;
      for (const data::variable& var: essential_vars)
      {
        state.remove_abstracted_variable(p, var_name, var);
      }
      return !essential_vars.empty();
    }

    std::optional<data::variable> selected_var;
    if (options.var_choice == var_choice_strategy::count)
    {
      selected_var = detail::choose_variable_by_count(var_name, guard_formula, essential_vars, m_var_count_cache);
    }
    else if (options.var_choice == var_choice_strategy::rhs)
    {
      selected_var = detail::choose_variable_by_rhs_order_reverse(guard_formula, essential_vars);
    }
    else
    {
      selected_var = detail::choose_variable_by_lhs_order(bound_variable, essential_vars, guard_formula);
    }

    if (selected_var)
    {
      mCRL2log(log::debug) << "Phase " << phase << ": " << std::endl;
      mCRL2log(log::verbose) << "Un-abstracting " << selected_var->name() << " from " << var_name << std::endl;
      state.remove_abstracted_variable(p, var_name, *selected_var);
      return true;
    }
    return false;
  }

  bool step_decorations(const structure_graph& primary,
    const structure_graph& other,
    const std::string& phase,
    bool primary_is_under)
  {
    mCRL2log(log::debug) << "Phase " << phase << std::endl;
    index_type current_idx = primary.initial_vertex();
    std::set<index_type> visited;
    while (current_idx != undefined_vertex())
    {
      mCRL2log(log::debug) << "Find first index " << current_idx << std::endl;
      const vertex& current_vertex = primary.find_vertex(current_idx);
      index_type matching_idx = find_vertex_index_by_formula(other, current_vertex.formula(), primary_is_under);
      mCRL2log(log::debug) << "Phase " << phase << " vertex " << current_vertex;
      if (matching_idx != undefined_vertex())
      {
        mCRL2log(log::debug) << " trying other dec " << other.find_vertex(matching_idx);
      }
      mCRL2log(log::debug) << std::endl;
      if ((current_vertex.decoration == decoration_type::d_false
            || current_vertex.decoration == decoration_type::d_true)
          && (matching_idx == undefined_vertex()
              || current_vertex.decoration != other.find_vertex(matching_idx).decoration))
      {
        mCRL2log(log::debug) << "Phase " << phase << " choose vertex " << std::endl;
        if (select_variable(primary, current_idx, other, matching_idx, phase))
          return true;
      }

      visited.insert(current_idx);
      index_type strategy_idx = current_vertex.strategy;
      index_type other_strategy_idx
        = matching_idx != undefined_vertex() ? other.find_vertex(matching_idx).strategy : undefined_vertex();

      if (strategy_idx != undefined_vertex() && visited.find(strategy_idx) == visited.end())
      {
        current_idx = strategy_idx;
      }
      else if (other_strategy_idx != undefined_vertex() && visited.find(other_strategy_idx) == visited.end())
      {
        current_idx
          = find_vertex_index_by_formula(primary, other.find_vertex(other_strategy_idx).formula(), !primary_is_under);
      }
      else
      {
        break;
      }
    }
    return false;
  }

  bool step_edges(const structure_graph& primary,
    const structure_graph& other,
    const std::string& phase,
    bool primary_is_under)
  {
    mCRL2log(log::debug) << "Phase " << phase << std::endl;

    // Walk the strategy path and try to refine on edges that miss a counterpart
    // in the other structure graph. Edges that go from one equation to a
    // different equation (i.e. the formula names differ) are checked first.
    auto check_edges = [this, &primary, &other, &phase, &primary_is_under](bool cross_equation_only)
    {
      index_type current_idx = primary.initial_vertex();
      std::set<index_type> visited;
      while (true)
      {
        const vertex& current_vertex = primary.find_vertex(current_idx);
        index_type strategy_idx = current_vertex.strategy;
        if (strategy_idx == undefined_vertex() || visited.find(strategy_idx) != visited.end())
        {
          break;
        }

        const vertex& strategy_vertex = primary.find_vertex(strategy_idx);
        const propositional_variable_instantiation& current_pvi
          = atermpp::down_cast<propositional_variable_instantiation>(current_vertex.formula());
        const propositional_variable_instantiation& strategy_pvi
          = atermpp::down_cast<propositional_variable_instantiation>(strategy_vertex.formula());
        const bool cross_equation = current_pvi.name() != strategy_pvi.name();

        if (!cross_equation_only || cross_equation)
        {
          index_type matching_idx = find_vertex_index_by_formula(other, current_vertex.formula(), primary_is_under);
          index_type strategy_match_idx
            = find_vertex_index_by_formula(other, strategy_vertex.formula(), primary_is_under);
          mCRL2log(log::debug) << "Phase " << phase << " vertex " << current_vertex;
          if (matching_idx != undefined_vertex())
          {
            mCRL2log(log::debug) << " trying other edge if " << strategy_match_idx << " is in "
                                 << core::detail::print_list(other.find_vertex(matching_idx).successors);
          }
          mCRL2log(log::debug) << std::endl;

          if (matching_idx != undefined_vertex()
              && (strategy_match_idx == undefined_vertex() || !has_edge(other, matching_idx, strategy_match_idx)))
          {
            mCRL2log(log::debug) << " found other edge for vertex " << current_vertex << std::endl;
            if (select_variable(primary, current_idx, other, matching_idx, phase))
              return true;
            break;
          }
        }

        visited.insert(current_idx);
        current_idx = strategy_idx;
      }
      return false;
    };

    if (check_edges(true))
      return true;

    return check_edges(false);
  }

public:
  bool refine_using_strategies(const pbes& p,
    const pbes& under_pbes,
    const pbes& over_pbes,
    abstract_param_state& state,
    const pbescegps_options& options,
    const structure_graph& under_graph,
    const structure_graph& over_graph,
    const data::rewriter& data_rewriter)
  {
    if (under_graph.is_empty() || over_graph.is_empty())
    {
      mCRL2log(log::warning) << "Counterexample or witness information missing, falling back to random selection."
                             << std::endl;
      return false;
    }

    m_p = &p;
    m_state = &state;
    m_options = &options;
    m_datar = &data_rewriter;

    // Store original parameters
    for (const pbes_equation& eq: p.equations())
    {
      m_original_params[eq.variable().name()] = as_vector(eq.variable().parameters());
    }

    // Store remaining parameters for each approximation
    for (const pbes_equation& eq: under_pbes.equations())
    {
      m_under_params[eq.variable().name()] = as_vector(eq.variable().parameters());
    }
    for (const pbes_equation& eq: over_pbes.equations())
    {
      m_over_params[eq.variable().name()] = as_vector(eq.variable().parameters());
    }

    mCRL2log(log::debug) << "Refining using strategies" << std::endl;
    mCRL2log(log::trace) << "Under: " << under_graph << std::endl;
    mCRL2log(log::trace) << "Over: " << over_graph << std::endl;

    if (step_decorations(under_graph, over_graph, "dec-cex", true))
      return true;

    if (step_edges(under_graph, over_graph, "edge-cex", true))
      return true;

    if (step_decorations(over_graph, under_graph, "dec-wit", false))
      return true;

    if (step_edges(over_graph, under_graph, "edge-wit", false))
      return true;

    return false;
  }
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_DETAIL_PBESCEGPS_REFINE_STRATEGIES_H
