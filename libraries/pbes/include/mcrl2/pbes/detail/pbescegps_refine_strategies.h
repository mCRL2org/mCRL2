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
#include "mcrl2/atermpp/aterm_list.h"
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

  // Multi-level index key for formula indexing. Currently keyed by equation name only,
  // but extensible for additional filtering criteria (e.g., first argument, arity).
  struct formula_key
  {
    core::identifier_string eq_name;

    bool operator<(const formula_key& other) const
    {
      return eq_name < other.eq_name;
    }

    bool operator==(const formula_key& other) const
    {
      return eq_name == other.eq_name;
    }
  };

  // Index structure: maps (eq_name) → sorted vector of (formula_pp_string, vertex_index) pairs
  // The pairs are sorted by formula string to enable binary search
  struct indexed_vertices
  {
    std::vector<std::pair<std::string, index_type>> sorted_by_formula;
  };

  std::map<formula_key, indexed_vertices> m_under_index;
  std::map<formula_key, indexed_vertices> m_over_index;

  // Helper to build the sorted index for a graph
  void build_formula_index(const structure_graph& g, std::map<formula_key, indexed_vertices>& index)
  {
    for (index_type idx = 0; idx < g.extent(); ++idx)
    {
      const pbes_expression& formula = g.find_vertex(idx).formula();
      const auto& pvi = atermpp::down_cast<propositional_variable_instantiation>(formula);
      formula_key key{pvi.name()};

      std::string formula_str = core::pp(formula);
      index[key].sorted_by_formula.emplace_back(formula_str, idx);
    }

    // Sort each bucket by formula string for binary search
    for (auto& [key, vertices]: index)
    {
      std::sort(vertices.sorted_by_formula.begin(),
        vertices.sorted_by_formula.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    }
  }

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
    const auto& pvi = atermpp::down_cast<propositional_variable_instantiation>(formula);
    formula_key key{pvi.name()};

    // Select the appropriate index based on which graph we're searching
    const auto& index = find_in_over ? m_over_index : m_under_index;

    auto key_it = index.find(key);
    if (key_it == index.end())
    {
      return undefined_vertex();
    }

    const indexed_vertices& candidates = key_it->second;
    const std::string formula_str = core::pp(formula);

    // Binary search for vertices with matching formula string.
    // Note: This should typically find at least one match, since we built the index
    // by converting formulas to strings. However, if no exact match is found,
    // the formula may have been rewritten or normalized differently.
    auto range = std::equal_range(candidates.sorted_by_formula.begin(),
      candidates.sorted_by_formula.end(),
      std::make_pair(formula_str, index_type(0)),
      [](const auto& a, const auto& b) { return a.first < b.first; });

    assert(range.first != range.second);

    // Among exact string matches, find the first one that passes formulae_match
    // (in case pp() produces identical strings for semantically different formulas)
    for (auto it = range.first; it != range.second; ++it)
    {
      if (formulae_match(formula, g.find_vertex(it->second).formula(), find_in_over))
      {
        return it->second;
      }
    }

#ifndef NDEBUG
    // If no exact string match was found via binary search, fall back to sequential
    // search through all candidates for this equation.
    for (const auto& [candidate_str, idx]: candidates.sorted_by_formula)
    {
      if (formulae_match(formula, g.find_vertex(idx).formula(), find_in_over))
      {
        mcrl2::runtime_error("Our indexing did not find the right formula");
        return idx;
      }
    }
#endif

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
    const std::string& phase,
    bool g_is_under)
  {
    const vertex& current_vertex = g.find_vertex(current_idx);
    const propositional_variable_instantiation& pvi
      = atermpp::down_cast<propositional_variable_instantiation>(current_vertex.formula());
    core::identifier_string var_name = pvi.name();
    const std::optional<vertex> matching_vertex
      = (matching_idx == undefined_vertex()) ? std::nullopt : std::optional<vertex>(g_prime.find_vertex(matching_idx));
    const std::optional<propositional_variable_instantiation>& matching_pvi
      = (matching_vertex.has_value())
          ? std::optional<propositional_variable_instantiation>(
              atermpp::down_cast<propositional_variable_instantiation>(matching_vertex->formula()))
          : std::nullopt;

    abstract_param_state& state = *m_state;
    const pbescegps_options& options = *m_options;
    const pbes& p = *m_p;

    auto wit = state.W.find(var_name);
    if (wit == state.W.end() || wit->second.empty())
    {
      return false;
    }

    pbes_expression equation_formula = detail::find_equation_by_name(p, var_name)->get().formula();

    data::mutable_indexed_substitution sigma;
    std::vector<data::variable> g_params(g_is_under ? m_under_params[var_name] : m_over_params[var_name]);
    std::vector<data::variable> g_prime_params = matching_pvi.has_value()
                                                   ? (!g_is_under ? m_under_params[var_name] : m_over_params[var_name])
                                                   : std::vector<data::variable>();
    std::vector<data::data_expression> pvi_values = atermpp::as_vector(pvi.parameters());
    std::vector<data::data_expression> matching_pvi_values = matching_pvi.has_value()
                                                               ? atermpp::as_vector(matching_pvi->parameters())
                                                               : std::vector<data::data_expression>();
    // Combine the under and over approximation parameters
    std::size_t ig = 0, ig_prima = 0;
    for (const data::variable& param: m_original_params[var_name])
    {
      if (!state.W[var_name].contains(param))
      {
        if (ig < g_params.size() && g_params[ig] == param)
        {
          sigma[param] = pvi_values[ig];
          mCRL2log(log::debug) << "sigma[" << param << "] = " << pvi_values[ig] << " (regular)" << std::endl;
          ++ig;
        }
        if (ig_prima < g_prime_params.size() && g_prime_params[ig_prima] == param)
        {
          sigma[param] = matching_pvi_values[ig_prima];
          mCRL2log(log::trace) << "sigma[" << param << "] = " << matching_pvi_values[ig_prima] << " (matching)"
                               << std::endl;
          ++ig_prima;
        }
      }
    }
    assert(ig == g_params.size() && ig_prima == g_prime_params.size());

    simplify_data_rewriter<data::rewriter> pbes_rewriter(*m_datar);
    pbes_expression instantiated_formula = pbes_rewrite(equation_formula, pbes_rewriter, sigma);
    mCRL2log(log::debug) << "Phase " << phase << ": Instantiated " << current_vertex << std::endl
                         << "to " << instantiated_formula << std::endl;

    std::set<data::variable> essential_vars = wit->second;
    pbes_expression guard_formula = instantiated_formula;

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
          if (!candidate_pvis.contains(pvi))
          {
            continue;
          }
          mCRL2log(log::trace) << "Guard for " << pvi << ": " << guard_expr << std::endl;
          std::set<data::variable> guard_vars = find_free_variables(guard_expr);
          std::set<data::variable> common_vars;
          std::set_intersection(state.W[var_name].begin(),
            state.W[var_name].end(),
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
      data::variable_list args(m_original_params[var_name]);
      selected_var = detail::choose_variable_by_lhs_order(args, essential_vars, guard_formula);
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
              || current_vertex.decoration != other.find_vertex(matching_idx).decoration)
          && (current_vertex.rank % 2 == 0 || current_vertex.decoration == decoration_type::d_true))
      {
        mCRL2log(log::debug) << "Phase " << phase << " choose vertex " << std::endl;
        if (select_variable(primary, current_idx, other, matching_idx, phase, primary_is_under))
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
      std::set<index_type> todo;
      todo.insert(primary.initial_vertex());
      std::set<index_type> visited;
      while (!todo.empty())
      {
        const index_type current_idx = *todo.begin();
        todo.erase(todo.begin());
        const vertex& current_vertex = primary.find_vertex(current_idx);
        const index_type strategy_idx = current_vertex.strategy;

        if (strategy_idx == undefined_vertex())
        {
          // If a strategy is undefined, it could be a conjunction and overapproximation or disjunction and
          // underapproximation.
          if (current_vertex.decoration == structure_graph::d_none
              || (primary_is_under ? current_vertex.decoration == structure_graph::d_disjunction
                                   : current_vertex.decoration == structure_graph::d_conjunction))
          {
            mCRL2log(log::trace) << "Special case: strategy undefined for vertex " << current_vertex << std::endl;
            const index_type matching_idx
              = find_vertex_index_by_formula(other, current_vertex.formula(), primary_is_under);
            mCRL2log(log::trace) << "Some index found " << matching_idx << std::endl;
            if (matching_idx == undefined_vertex())
            {
              mcrl2::runtime_error("matching_idx == undefined_vertex() for " + pp(current_vertex.formula()));
            }

            const index_type other_strategy_idx = other.find_vertex(matching_idx).strategy;
            mCRL2log(log::trace) << "Strategy index found " << other_strategy_idx << std::endl;
            if (other_strategy_idx == undefined_vertex())
            {
              mcrl2::runtime_error("The other strategy index is also undefined! " + pp(current_vertex.formula()));
            }

            const vertex& other_strategy_vertex = other.find_vertex(other_strategy_idx);
            mCRL2log(log::trace) << "Other strategy vertex found " << other_strategy_vertex << std::endl;
            const propositional_variable_instantiation& current_pvi
              = atermpp::down_cast<propositional_variable_instantiation>(current_vertex.formula());
            const propositional_variable_instantiation& other_strategy_pvi
              = atermpp::down_cast<propositional_variable_instantiation>(other_strategy_vertex.formula());
            const bool cross_equation = current_pvi.name() != other_strategy_pvi.name();
            mCRL2log(log::trace) << "Cross equation " << cross_equation << std::endl;
            mCRL2log(log::trace) << " Finding the strat location in primary " << std::endl;
            const index_type& other_strategy_in_primary_idx
              = find_vertex_index_by_formula(primary, other_strategy_vertex.formula(), !primary_is_under);
            if (cross_equation_only ? cross_equation : !cross_equation)
            {
              mCRL2log(log::trace) << " Index for other strat " << other_strategy_in_primary_idx << std::endl;
              if (other_strategy_in_primary_idx == undefined_vertex()
                  || !has_edge(primary, current_idx, other_strategy_in_primary_idx))
              {
                mCRL2log(log::debug) << " found other edge for vertex " << current_vertex << std::endl;
                if (select_variable(primary, current_idx, other, matching_idx, phase, primary_is_under))
                  return true;
              }
            }

            visited.insert(current_idx);
            if (other_strategy_in_primary_idx != undefined_vertex()
                && visited.find(other_strategy_in_primary_idx) == visited.end())
            {
              todo.insert(other_strategy_in_primary_idx);
            }
          }
          else
          {
            mCRL2log(log::debug) << "No special case: strategy undefined for vertex " << current_vertex << std::endl;
            break;
          }
        }
        else
        {
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
              if (select_variable(primary, current_idx, other, matching_idx, phase, primary_is_under))
                return true;
            }
          }

          visited.insert(current_idx);
          if (visited.find(strategy_idx) == visited.end())
          {
            todo.insert(strategy_idx);
          }
        }
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

    // Build formula indices for faster lookup
    build_formula_index(under_graph, m_under_index);
    build_formula_index(over_graph, m_over_index);

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
