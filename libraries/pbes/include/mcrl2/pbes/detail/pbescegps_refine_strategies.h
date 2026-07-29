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
#include "mcrl2/pbes/detail/count_free_variables.h"
#include "mcrl2/pbes/detail/find_free_variables.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/detail/pbescegps_types.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_equation.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/pbes/rewrite.h"
#include "mcrl2/pbes/structure_graph.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
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

  // Cache for variable occurrence counts to avoid recomputing
  std::map<core::identifier_string, std::map<data::variable, std::size_t>> m_var_count_cache;

  static index_type find_vertex_index_by_formula(const structure_graph& g, const pbes_expression& formula)
  {
    mCRL2log(log::debug) << "Find by form" << formula << std::endl;
    for (index_type i = 0; i < g.extent(); ++i)
    {
      mCRL2log(log::debug) << "Find by form" << i << " in " << g.extent() << std::endl;
      if (g.find_vertex(i).formula() == formula)
      {
        return i;
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
    const std::vector<index_type>& succs = g.all_successors(from);
    return std::find(succs.begin(), succs.end(), to) != succs.end();
  }

  std::optional<data::variable> choose_variable_by_count(const core::identifier_string& eq_name,
    const pbes_expression& eq_formula,
    const std::set<data::variable>& essential_vars)
  {
    auto cache_it = m_var_count_cache.find(eq_name);
    if (cache_it == m_var_count_cache.end())
    {
      mCRL2log(log::debug) << "Cache miss for " << eq_name << std::endl;
      auto var_counts = detail::count_free_variable_occurrences(eq_formula, false);
      m_var_count_cache[eq_name] = var_counts;
      cache_it = m_var_count_cache.find(eq_name);
    }

    const std::map<data::variable, size_t>& var_counts = cache_it->second;
    std::size_t best_count = 0;
    std::optional<data::variable> best_var;
    for (const data::variable& var: essential_vars)
    {
      if (var_counts.find(var) != var_counts.end())
      {
        mCRL2log(log::debug) << "  - " << var.name() << " -> " << var_counts.at(var) << std::endl;
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

  static std::optional<data::variable> choose_variable_by_lhs_order(const propositional_variable& bnd_var,
    const std::set<data::variable>& essential_vars)
  {
    for (const data::variable& param: bnd_var.parameters())
    {
      if (essential_vars.contains(param))
      {
        return param;
      }
    }
    return std::nullopt;
  }

  static std::optional<data::variable> choose_variable_by_rhs_order(const pbes_expression& formula,
    const std::set<data::variable>& essential_vars)
  {
    detail::find_free_variables_traverser f(data::variable_list(), false);
    f.apply(formula);
    std::set<data::variable> vars = f.result;
    for (auto it = vars.rbegin(); it != vars.rend(); ++it)
    {
      if (essential_vars.contains(*it))
      {
        return *it;
      }
    }
    return std::nullopt;
  }

  bool select_variable(const structure_graph& g,
    index_type v,
    const structure_graph& g_prime,
    index_type v_other_idx,
    const std::string& phase)
  {
    const vertex& vertex_data = g.find_vertex(v);
    const propositional_variable_instantiation& pvi
      = atermpp::down_cast<propositional_variable_instantiation>(vertex_data.formula());
    core::identifier_string eq_name = pvi.name();

    abstract_param_state& state = *m_state;
    const pbescegps_options& options = *m_options;
    const pbes& p = *m_p;

    auto wit = state.W.find(eq_name);
    if (wit == state.W.end() || wit->second.empty())
    {
      return false;
    }

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

    data::mutable_indexed_substitution sigma;
    data::data_expression_list current_pvi_args = pvi.parameters();
    for (const data::variable& explicit_: bnd_var.parameters())
    {
      if (!state.W[eq_name].contains(explicit_))
      {
        if (current_pvi_args.empty())
          break;
        sigma[explicit_] = current_pvi_args.front();
        current_pvi_args.pop_front();
      }
    }

    data::rewriter datar(p.data(), options.rewrite_strategy);
    simplify_data_rewriter<data::rewriter> pbesr(datar);
    pbes_expression instantiated = pbes_rewrite(eq_formula, pbesr, sigma);
    mCRL2log(log::debug) << "Phase " << phase << ": Instantiated " << vertex_data << std::endl
                         << "to " << instantiated << std::endl;

    std::set<data::variable> essential_vars = wit->second;
    pbes_expression guard_formula = instantiated;

    std::optional<vertex> v_other;

    if (v_other_idx != undefined_vertex())
    {
      v_other = g_prime.find_vertex(v_other_idx);
    }

    if (vertex_data.strategy != undefined_vertex() || (v_other.has_value() && v_other->strategy != undefined_vertex()))
    {
      auto all_pvis = find_propositional_variable_instantiations(instantiated);
      const propositional_variable_instantiation succ_pvi
        = vertex_data.strategy != undefined_vertex()
            ? atermpp::down_cast<propositional_variable_instantiation>(g.find_vertex(vertex_data.strategy).formula())
            : atermpp::down_cast<propositional_variable_instantiation>(
                g_prime.find_vertex(v_other->strategy).formula());

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

        for (const pbes_equation& eq: p.equations())
        {
          if (eq.variable().name() == candidate.name())
          {
            for (const auto& param: eq.variable().parameters())
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
                data::data_expression rewritten = datar(eq_expr);
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
        detail::guard_traverser gt(datar);
        gt.apply(instantiated);
        const std::vector<std::pair<propositional_variable_instantiation, pbes_expression>>& guards
          = gt.expression_stack.back().guards;

        for (const auto& [pvi, guard_result]: guards)
        {
          std::set<data::variable> guard_vars = find_free_variables(guard_result);
          std::set<data::variable> v_intersection;
          std::set_intersection(state.W[bnd_var.name()].begin(),
            state.W[bnd_var.name()].end(),
            guard_vars.begin(),
            guard_vars.end(),
            std::inserter(v_intersection, v_intersection.begin()));
          if (!v_intersection.empty())
          {
            essential_vars = std::move(v_intersection);
            guard_formula = guard_result;
            mCRL2log(log::debug) << "Guard vars: " << core::detail::print_list(guard_vars) << std::endl;
            break;
          }
        }
      }
    }

    std::optional<data::variable> selected_var;
    if (options.var_choice == var_choice_strategy::count)
    {
      selected_var = choose_variable_by_count(eq_name, guard_formula, essential_vars);
    }
    else if (options.var_choice == var_choice_strategy::rhs)
    {
      selected_var = choose_variable_by_rhs_order(guard_formula, essential_vars);
    }
    else
    {
      selected_var = choose_variable_by_lhs_order(bnd_var, essential_vars);
    }

    if (selected_var)
    {
      mCRL2log(log::debug) << "Phase " << phase << ": Un-abstracting " << selected_var->name() << " from " << eq_name
                           << std::endl;
      state.remove_abstracted_variable(p, eq_name, *selected_var);
      return true;
    }
    return false;
    // throw mcrl2::runtime_error(
    //   "No abstracted variable selected in instantiated formula for equation " + pp(eq_name) + " at phase " + phase);
  }

  bool step_decorations(const structure_graph& primary, const structure_graph& other, const std::string& phase)
  {
    mCRL2log(log::debug) << "Phase " << phase << std::endl;
    index_type v = primary.initial_vertex();
    std::set<index_type> visited;
    // TODO: Also prioritize based on the "next equation" heuristic
    // possibly make that an option in the tool as well (that disables it)
    while (v != undefined_vertex())
    {
      mCRL2log(log::debug) << "Find first index " << v << std::endl;
      const vertex& vertex_v = primary.find_vertex(v);
      index_type v_other_idx = find_vertex_index_by_formula(other, vertex_v.formula());
      mCRL2log(log::debug) << "Phase " << phase << " vertex " << vertex_v;
      if (v_other_idx != undefined_vertex())
      {
        mCRL2log(log::debug) << " trying other dec " << other.find_vertex(v_other_idx);
      }
      mCRL2log(log::debug) << std::endl;
      if ((vertex_v.decoration == decoration_type::d_false || vertex_v.decoration == decoration_type::d_true)
          && (v_other_idx == undefined_vertex() || vertex_v.decoration != other.find_vertex(v_other_idx).decoration))
      {
        mCRL2log(log::debug) << "Phase " << phase << " choose vertex " << std::endl;
        if (select_variable(primary, v, other, v_other_idx, phase))
          return true;
      }

      visited.insert(v);
      index_type strat = vertex_v.strategy;
      if (strat != undefined_vertex() && visited.find(strat) == visited.end())
      {
        v = strat;
      }
      else
      {
        if (v_other_idx != undefined_vertex() && other.find_vertex(v_other_idx).strategy != undefined_vertex())
        {
          v = find_vertex_index_by_formula(primary,
            other.find_vertex(other.find_vertex(v_other_idx).strategy).formula());
        }
        else
        {
          break;
        }
      }
    }
    return false;
  }

  bool step_edges(const structure_graph& primary, const structure_graph& other, const std::string& phase)
  {
    mCRL2log(log::debug) << "Phase " << phase << std::endl;

    index_type v = primary.initial_vertex();
    std::set<index_type> visited;
    while (true)
    {
      const vertex& vertex_v = primary.find_vertex(v);
      index_type strat = vertex_v.strategy;
      if (strat == undefined_vertex() || visited.find(strat) != visited.end())
      {
        break;
      }

      const vertex& vertex_strat = primary.find_vertex(strat);
      index_type v_other_idx = find_vertex_index_by_formula(other, vertex_v.formula());
      index_type strat_other_idx = find_vertex_index_by_formula(other, vertex_strat.formula());
      mCRL2log(log::debug) << "Phase " << phase << " vertex " << vertex_v;
      if (v_other_idx != undefined_vertex())
      {
        mCRL2log(log::debug) << " trying other edge if " << strat_other_idx << " is in "
                             << core::detail::print_list(other.find_vertex(v_other_idx).successors);
      }
      mCRL2log(log::debug) << std::endl;

      if (v_other_idx != undefined_vertex() && strat_other_idx != undefined_vertex()
          && !has_edge(other, v_other_idx, strat_other_idx))
      {
        mCRL2log(log::debug) << " found other edge for vertex " << vertex_v;
        if (select_variable(primary, v, other, v_other_idx, phase))
          return true;
        break;
      }

      visited.insert(v);
      v = strat;
    }
    return false;
  }

public:
  bool refine_using_strategies(const pbes& p,
    abstract_param_state& state,
    const pbescegps_options& options,
    const structure_graph& under_graph,
    const structure_graph& over_graph)
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

    mCRL2log(log::debug) << "Refining using strategies" << std::endl;
    mCRL2log(log::debug) << "Under: " << under_graph << std::endl;
    mCRL2log(log::debug) << "Over: " << over_graph << std::endl;

    if (step_decorations(under_graph, over_graph, "dec-cex"))
      return true;

    if (step_decorations(over_graph, under_graph, "dec-wit"))
      return true;

    if (step_edges(under_graph, over_graph, "edge-cex"))
      return true;

    if (step_edges(over_graph, under_graph, "edge-wit"))
      return true;

    return false;
  }
};

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_DETAIL_PBESCEGPS_REFINE_STRATEGIES_H
