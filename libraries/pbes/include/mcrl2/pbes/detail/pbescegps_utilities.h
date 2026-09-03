// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbescegps_utilities.h
/// \brief Shared types and utility functions for the CEGPS algorithm.

#ifndef MCRL2_PBES_DETAIL_PBESCEGPS_UTILITIES_H
#define MCRL2_PBES_DETAIL_PBESCEGPS_UTILITIES_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/detail/count_free_variables.h"
#include "mcrl2/pbes/detail/find_free_variables.h"
#include "mcrl2/pbes/detail/guard_traverser.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/pbessolve_options.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>
#include <vector>

namespace mcrl2::pbes_system
{

enum class var_choice_strategy
{
  lhs, // variable order of the left-hand side of the equation
  rhs, // variable order of the right-hand side of the equation
  count, // free variable that occurs most often (excluding data expressions in PVI)
  all, // un-abstract all variables that occur
  ruling // prioritize based on the ruled-by ordering: pick the variable that rules the most others
};

inline std::string to_string(const var_choice_strategy& strategy)
{
  switch (strategy)
  {
    case var_choice_strategy::lhs:
      return "lhs";
    case var_choice_strategy::rhs:
      return "rhs";
    case var_choice_strategy::count:
      return "count";
    case var_choice_strategy::all:
      return "all";
    case var_choice_strategy::ruling:
      return "ruling";
  }
  return "lhs";
}

inline std::ostream& operator<<(std::ostream& os, const var_choice_strategy& strategy)
{
  return os << to_string(strategy);
}

inline std::istream& operator>>(std::istream& is, var_choice_strategy& strategy)
{
  std::string s;
  is >> s;
  if (s == "lhs")
  {
    strategy = var_choice_strategy::lhs;
  }
  else if (s == "rhs")
  {
    strategy = var_choice_strategy::rhs;
  }
  else if (s == "count")
  {
    strategy = var_choice_strategy::count;
  }
  else if (s == "all")
  {
    strategy = var_choice_strategy::all;
  }
  else if (s == "ruling")
  {
    strategy = var_choice_strategy::ruling;
  }
  else
  {
    is.setstate(std::ios_base::failbit);
  }
  return is;
}

struct pbescegps_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  partial_solve_strategy optimization = partial_solve_strategy::no_optimisation;
  bool init_control_flow = false;
  bool solve_symbolic = false;
  bool stategraph = false;
  var_choice_strategy var_choice = var_choice_strategy::lhs;
  std::string solve_symbolic_args = "";
  std::size_t number_of_threads = 1;
  std::string initial_state_file = ""; // if non-empty, read the initial abstraction state from this file
  bool instantiate_infinite_quantifier_guards = false; // if true, do not abstract parameters that occur in the
                                                       // guards of predicate variable instances in the scope of an
                                                       // infinite quantifier
  bool rules_ideal = false; // if true, enforce the order-ideal invariant after each refinement step
  std::string ruling_file = ""; // if non-empty, write the ruling relation to this file as text
};

// The ruled-by relation together with cached dominance information.
// ruled_by[eq][dₘ] = { dⱼ | dⱼ ≽ dₘ in equation eq }.
// tree_size[eq][dⱼ] = the number of parameters that dⱼ (transitively) rules in eq,
// i.e. the size of dⱼ's dominance subtree. Computed once by compute_tree_sizes()
// and used to rank roots instead of counting formula occurrences.
struct ruling_relation_type
{
  using equation_relation = std::map<data::variable, std::set<data::variable>>;
  using equation_tree_sizes = std::map<data::variable, std::size_t>;

  std::map<core::identifier_string, equation_relation> ruled_by;
  std::map<core::identifier_string, equation_tree_sizes> tree_size;

  // (Re)computes tree_size from ruled_by. Must be called after mutating ruled_by.
  void compute_tree_sizes()
  {
    tree_size.clear();
    for (const auto& [eq_name, ruled_by_map]: ruled_by)
    {
      auto& sizes = tree_size[eq_name];
      for (const auto& [d_m, rulers]: ruled_by_map)
      {
        sizes.try_emplace(d_m, 0);
        for (const data::variable& d_j: rulers)
        {
          sizes.try_emplace(d_j, 0);
        }
      }

      // For each parameter, walk upward through its rulers; every ancestor is a
      // dominator and gains exactly one descendant. visited ensures a shared
      // descendant is counted once even when reached through several children.
      for (const auto& [d_m, rulers]: ruled_by_map)
      {
        std::set<data::variable> visited;
        std::vector<data::variable> todo;
        for (const data::variable& d_j: rulers)
        {
          if (visited.insert(d_j).second)
          {
            todo.push_back(d_j);
          }
        }
        while (!todo.empty())
        {
          const data::variable d_j = todo.back();
          todo.pop_back();
          ++sizes[d_j];
          auto it = ruled_by_map.find(d_j);
          if (it != ruled_by_map.end())
          {
            for (const data::variable& ancestor: it->second)
            {
              if (visited.insert(ancestor).second)
              {
                todo.push_back(ancestor);
              }
            }
          }
        }
      }
    }
  }
};

namespace detail
{

// Helper to find an equation by name in a PBES
inline std::optional<std::reference_wrapper<const pbes_equation>> find_equation_by_name(const pbes& p,
  const core::identifier_string& eq_name)
{
  for (const pbes_equation& eq: p.equations())
  {
    if (eq.variable().name() == eq_name)
    {
      return std::ref(eq);
    }
  }
  return std::nullopt;
}

// Traverser that collects the propositional variable instantiations that occur
// in the scope of an infinite quantifier, i.e. a forall/exists whose bound
// variables have a sort that cannot be determined to be finite.
struct infinite_quantifier_pvi_traverser : public pbes_expression_traverser<infinite_quantifier_pvi_traverser>
{
  using super = pbes_expression_traverser<infinite_quantifier_pvi_traverser>;
  using super::apply;
  using super::enter;
  using super::leave;

  const data::data_specification& m_data_spec;

  // The propositional variable instantiations found in the scope of an infinite quantifier.
  std::set<propositional_variable_instantiation> m_pvis;

  // Stack that tracks whether the current subexpression is in the scope of an infinite quantifier.
  std::vector<bool> m_scope;

  explicit infinite_quantifier_pvi_traverser(const data::data_specification& data_spec)
    : m_data_spec(data_spec)
  {}

  // Returns true if the current subexpression is in the scope of an infinite quantifier.
  bool in_infinite_scope() const
  {
    return !m_scope.empty() && m_scope.back();
  }

  // Returns true if at least one bound variable has a sort that is not certainly finite.
  bool is_infinite(const data::variable_list& variables) const
  {
    for (const data::variable& v: variables)
    {
      if (!m_data_spec.is_certainly_finite(v.sort()))
      {
        return true;
      }
    }
    return false;
  }

  void enter(const forall& x)
  {
    m_scope.push_back(in_infinite_scope() || is_infinite(x.variables()));
  }

  void leave(const forall&)
  {
    m_scope.pop_back();
  }

  void enter(const exists& x)
  {
    m_scope.push_back(in_infinite_scope() || is_infinite(x.variables()));
  }

  void leave(const exists&)
  {
    m_scope.pop_back();
  }

  void apply(const propositional_variable_instantiation& x)
  {
    if (in_infinite_scope())
    {
      m_pvis.insert(x);
    }
  }
};

} // namespace detail

struct abstract_param_state
{
  std::map<core::identifier_string, std::set<data::variable>> W;
  std::map<core::identifier_string, std::set<std::size_t>> I;

  void remove_abstracted_variable(const pbes& p, const core::identifier_string& eq_name, const std::size_t& i)
  {
    I[eq_name].erase(i);
    auto eq_opt = detail::find_equation_by_name(p, eq_name);
    if (eq_opt)
    {
      auto asdf = atermpp::down_cast<data::variable>(as_vector(eq_opt->get().variable().parameters())[i]);
      W[eq_name].erase(asdf);
      mCRL2log(log::debug) << "removed " << std::to_string(i) << " (parameter " << pp(asdf) << ") from " << pp(eq_name)
                           << std::endl;
    }
  }

  void remove_abstracted_variable(const pbes& p, const core::identifier_string& eq_name, const data::variable& var)
  {
    W[eq_name].erase(var);
    auto eq_opt = detail::find_equation_by_name(p, eq_name);
    bool found = false;
    if (eq_opt)
    {
      std::size_t i = 0;
      for (const auto& param: as_vector(eq_opt->get().variable().parameters()))
      {
        if (param.name() == var.name())
        {
          I[eq_name].erase(i);
          mCRL2log(log::debug) << "removed " << std::to_string(i) << " (parameter " << pp(var) << ") from "
                               << pp(eq_name) << std::endl;
          found = true;
          break;
        }
        ++i;
      }
    }
    if (!found)
    {
      throw mcrl2::runtime_error("parameter " + pp(var) + " not found in equation");
    }
  }

  void add_abstracted_variable(const pbes& p, const core::identifier_string& eq_name, const data::variable& var)
  {
    W[eq_name].insert(var);
    auto eq_opt = detail::find_equation_by_name(p, eq_name);
    bool found = false;
    if (eq_opt)
    {
      std::size_t i = 0;
      for (const auto& param: as_vector(eq_opt->get().variable().parameters()))
      {
        if (param.name() == var.name())
        {
          I[eq_name].insert(i);
          found = true;
          break;
        }
        ++i;
      }
    }
    if (!found)
    {
      throw mcrl2::runtime_error("parameter " + pp(var) + " not found in equation");
    }
  }
};

namespace detail
{

inline std::optional<data::variable> choose_variable_by_count(const core::identifier_string& var_name,
  const pbes_expression& equation_formula,
  const std::set<data::variable>& abstracted_vars,
  std::map<core::identifier_string, std::map<data::variable, std::size_t>>& cache)
{
  auto var_count_it = cache.find(var_name);
  if (var_count_it == cache.end())
  {
    mCRL2log(log::debug) << "Cache miss for " << var_name << std::endl;
    auto var_counts = count_free_variable_occurrences(equation_formula, false);
    cache[var_name] = var_counts;
    var_count_it = cache.find(var_name);
  }

  const std::map<data::variable, std::size_t>& var_counts = var_count_it->second;
  std::size_t best_count = 0;
  std::optional<data::variable> best_var;
  for (const data::variable& var: abstracted_vars)
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

inline std::optional<data::variable> choose_variable_by_lhs_order(const data::variable_list& all_parameters,
  const std::set<data::variable>& abstracted_vars,
  const std::optional<pbes_expression>& formula)
{
  std::set<data::variable> free_vars
    = formula ? find_free_variables(*formula, data::variable_list(), false) : abstracted_vars;
  for (const data::variable& param: all_parameters)
  {
    if (abstracted_vars.contains(param) && free_vars.contains(param))
    {
      return param;
    }
  }
  return std::nullopt;
}

inline std::optional<data::variable> choose_variable_by_lhs_order(const propositional_variable& bound_variable,
  const std::set<data::variable>& abstracted_vars,
  const std::optional<pbes_expression>& formula)
{
  return choose_variable_by_lhs_order(bound_variable.parameters(), abstracted_vars, formula);
}

inline std::optional<data::variable> choose_variable_by_rhs_order(const pbes_expression& formula,
  const std::set<data::variable>& essential_vars)
{
  const std::vector<data::variable> vars = find_free_variables_in_order(formula, data::variable_list(), false);
  mCRL2log(log::debug) << "vars: " << core::detail::print_list(vars);
  for (const data::variable& var: vars)
  {
    if (essential_vars.contains(var))
    {
      return var;
    }
  }
  return std::nullopt;
}

// Roots reachable from `start` via the ruling relation, ordered by dominance.
// `tree_sizes` supplies the dominance weight per root (number of descendants);
// `visited` breaks cycles; `cache` memoizes per-node results so each node is
// expanded once (avoids exponential re-expansion on shared sub-paths).
using ruling_roots_cache = std::map<data::variable, std::vector<std::pair<data::variable, std::size_t>>>;

inline std::vector<std::pair<data::variable, std::size_t>> find_dominant_roots_impl(const data::variable& start,
  const ruling_relation_type::equation_relation& ruled_by_map,
  const ruling_relation_type::equation_tree_sizes& tree_sizes,
  std::set<data::variable>& visited,
  ruling_roots_cache& cache)
{
  auto cached = cache.find(start);
  if (cached != cache.end())
  {
    return cached->second;
  }
  if (visited.contains(start))
  {
    return {}; // cycle: no new root on this path
  }
  visited.insert(start);

  std::map<data::variable, std::size_t> root_weights;
  auto it = ruled_by_map.find(start);
  if (it == ruled_by_map.end())
  {
    root_weights[start] = tree_sizes.contains(start) ? tree_sizes.at(start) : 0; // start is a root
  }
  else
  {
    for (const data::variable& ruler: it->second)
    {
      for (const auto& [root, weight]: find_dominant_roots_impl(ruler, ruled_by_map, tree_sizes, visited, cache))
      {
        root_weights[root] = std::max(root_weights[root], weight);
      }
    }

    // No ruler reached a root: start is its own representative root (all rulers
    // are part of a cycle through start).
    if (root_weights.empty())
    {
      root_weights[start] = tree_sizes.contains(start) ? tree_sizes.at(start) : 0;
    }
  }

  visited.erase(start);

  std::vector<std::pair<data::variable, std::size_t>> roots(root_weights.begin(), root_weights.end());
  std::sort(roots.begin(),
    roots.end(),
    [](const auto& lhs, const auto& rhs)
    {
      if (lhs.second != rhs.second)
      {
        return lhs.second > rhs.second;
      }
      return lhs.first.name() < rhs.first.name();
    });
  return cache[start] = roots;
}

inline std::vector<std::pair<data::variable, std::size_t>> find_dominant_roots(const data::variable& start,
  const ruling_relation_type::equation_relation& ruled_by_map,
  const ruling_relation_type::equation_tree_sizes& tree_sizes,
  std::set<data::variable>& visited)
{
  ruling_roots_cache cache;
  return find_dominant_roots_impl(start, ruled_by_map, tree_sizes, visited, cache);
}

// Chooses the most dominant variable to make concrete.
// Traverses the ruling relation backwards from each essential variable to find
// the "roots" — parameters that rule others but are not ruled by anyone.
// Per starting variable, picks the most dominant root that is itself essential;
// among those, picks the root with the largest tree size, i.e. the one that
// (transitively) rules the most parameters.
inline std::optional<data::variable> choose_variable_by_ruling_order(const core::identifier_string& eq_name,
  const std::set<data::variable>& essential_vars,
  const ruling_relation_type& ruling_relation)
{
  auto eq_it = ruling_relation.ruled_by.find(eq_name);
  if (eq_it == ruling_relation.ruled_by.end())
  {
    return std::nullopt;
  }

  auto sizes_it = ruling_relation.tree_size.find(eq_name);
  if (sizes_it == ruling_relation.tree_size.end())
  {
    return std::nullopt; // no cached tree sizes — cannot rank roots
  }
  const ruling_relation_type::equation_tree_sizes& tree_sizes = sizes_it->second;

  std::size_t best_tree_size = 0;
  std::optional<data::variable> best_var;

  // Roots depend only on the (static) ruling relation, not on the start node,
  // so reuse one cache across all essential variables of this equation.
  ruling_roots_cache cache;
  for (const data::variable& var: essential_vars)
  {
    std::set<data::variable> visited;
    auto roots = find_dominant_roots_impl(var, eq_it->second, tree_sizes, visited, cache);

    // Pick the most dominant root that is actually in essential_vars (abstracted).
    // The traversal may reach parameters that are already concrete.
    std::optional<data::variable> root;
    std::size_t size = 0;
    for (const auto& [candidate, weight]: roots)
    {
      if (essential_vars.contains(candidate))
      {
        root = candidate;
        size = weight;
        break;
      }
    }

    // Fallback: no essential root found, use the starting variable itself.
    if (!root.has_value())
    {
      root = var;
      size = tree_sizes.contains(var) ? tree_sizes.at(var) : 0;
    }

    mCRL2log(log::debug) << "  - " << var.name() << " -> root " << root->name() << " (tree size: " << size << ")"
                         << std::endl;

    if (size > best_tree_size)
    {
      best_tree_size = size;
      best_var = root;
    }
  }

  return best_var;
}

// Helper to extract equation name from "Abstracted parameters for X0:" format
inline std::string parse_verbose_equation_name(std::istringstream& iss, const std::string& line)
{
  std::string word, eq_name;
  if (!(iss >> word) || word != "parameters")
  {
    throw mcrl2::runtime_error("initial-state: expected 'parameters' in line \"" + line + "\"");
  }
  if (!(iss >> word) || word != "for")
  {
    throw mcrl2::runtime_error("initial-state: expected 'for' in line \"" + line + "\"");
  }
  if (!(iss >> eq_name))
  {
    throw mcrl2::runtime_error("initial-state: missing equation name in line \"" + line + "\"");
  }
  if (eq_name.size() > 1 && eq_name.back() == ':')
  {
    eq_name.pop_back();
  }
  else
  {
    char colon = '\0';
    if (!(iss >> colon) || colon != ':')
    {
      throw mcrl2::runtime_error("initial-state: expected ':' after the equation name in line \"" + line + "\"");
    }
  }
  return eq_name;
}

// Helper to extract equation name from "X0:" or "X0 :" format
inline std::string parse_bare_equation_name(std::istringstream& iss, const std::string& token, const std::string& line)
{
  std::string eq_name = token;
  if (eq_name.size() > 1 && eq_name.back() == ':')
  {
    eq_name.pop_back();
  }
  else
  {
    std::string colon;
    if (!(iss >> colon) || colon != ":")
    {
      throw mcrl2::runtime_error("initial-state: expected ':' after the equation name in line \"" + line + "\"");
    }
  }
  return eq_name;
}

// Helper to process a parameter token (index or name) and add it to state
inline void add_parameter_by_token(const pbes& p,
  abstract_param_state& state,
  const core::identifier_string& eq_name,
  const std::string& token,
  const std::vector<data::variable>& params)
{
  if (!token.empty() && std::isdigit(static_cast<unsigned char>(token[0])))
  {
    // Token is a parameter index.
    const std::size_t idx = std::stoul(token);
    if (idx >= params.size())
    {
      throw mcrl2::runtime_error(
        "initial-state: parameter index " + token + " out of bounds in equation " + core::pp(eq_name));
    }
    state.add_abstracted_variable(p, eq_name, params[idx]);
  }
  else
  {
    // Token is a parameter name.
    for (const auto& param: params)
    {
      if (param.name() == core::identifier_string(token))
      {
        state.add_abstracted_variable(p, eq_name, param);
        return;
      }
    }
    throw mcrl2::runtime_error("initial-state: parameter " + token + " not found in equation " + core::pp(eq_name));
  }
}

// Parses one line describing the abstracted parameters of a single equation and
// adds them to the initial abstraction state. The accepted formats are:
//   Abstracted parameters for X0: value_ValueBool57 value_ValueReal5
//   X0: value_ValueBool57 value_ValueReal5
//   X0: 0 3 7     (parameter indices)
inline void parse_initial_state_line(const pbes& p, abstract_param_state& state, const std::string& line)
{
  std::istringstream iss(line);
  std::string token;

  if (!(iss >> token))
  {
    return; // empty line
  }

  // Parse equation name
  std::string eq_name
    = (token == "Abstracted") ? parse_verbose_equation_name(iss, line) : parse_bare_equation_name(iss, token, line);

  auto eq_opt = find_equation_by_name(p, eq_name);
  if (!eq_opt)
  {
    throw mcrl2::runtime_error("initial-state: equation " + core::pp(eq_name) + " not found in the PBES");
  }
  const std::vector<data::variable> params = as_vector(eq_opt->get().variable().parameters());

  // Parse and add parameters
  while (iss >> token)
  {
    if (token[0] == '(')
    {
      break; // ignore trailing parenthetical remarks, e.g. "(indices: 0 1 2)"
    }
    add_parameter_by_token(p, state, eq_name, token, params);
  }
}

// Initializes the abstraction state from a file containing lines as printed by
// print_abstraction_summary. Equations not mentioned in the file get an empty
// abstraction set, i.e. they are solved fully concretely.
inline void
initialize_initial_abstraction_state(const pbes& p, abstract_param_state& state, const std::string& filename)
{
  for (const pbes_equation& eq: p.equations())
  {
    state.I[eq.variable().name()] = std::set<std::size_t>();
    state.W[eq.variable().name()] = std::set<data::variable>();
  }

  std::ifstream file(filename);
  if (!file.is_open())
  {
    throw mcrl2::runtime_error("initial-state: cannot open file " + filename);
  }

  std::string line;
  while (std::getline(file, line))
  {
    if (line.empty() || line[0] == '#')
    {
      continue;
    }
    parse_initial_state_line(p, state, line);
  }
}
// counts[eq][dₘ][dⱼ] = # recursive transitions in eq where dₘ changes while guarded by dⱼ.
using ruling_counts_type
  = std::map<core::identifier_string, std::map<data::variable, std::map<data::variable, std::size_t>>>;

// changes[eq][dₘ] = # recursive transitions in eq where dₘ changes, regardless of guard.
using pvi_change_counts_type = std::map<core::identifier_string, std::map<data::variable, std::size_t>>;

// Raw evidence for the ruling relation: how often one parameter change is guarded
// by another parameter, and how often each parameter changes at all.
struct ruling_statistics_type
{
  ruling_counts_type counts;
  pvi_change_counts_type changes;
};

inline ruling_statistics_type count_rulings(const pbes& p, const data::rewriter& datar)
{
  ruling_statistics_type stats;

  for (const pbes_equation& eq: p.equations())
  {
    const auto& eq_name = eq.variable().name();
    const auto params = as_vector(eq.variable().parameters());
    const auto params_set = std::set<data::variable>(params.begin(), params.end());

    detail::guard_traverser guard_trav(datar);
    guard_trav.apply(eq.formula());

    for (const auto& [pvi, guard_expr]: guard_trav.expression_stack.back().guards)
    {
      if (pvi.name() != eq_name)
      {
        continue;
      }

      std::set<data::variable> free_vars = pbes_system::find_free_variables(guard_expr);
      std::set<data::variable> guard_vars;
      std::set_intersection(free_vars.begin(),
        free_vars.end(),
        params_set.begin(),
        params_set.end(),
        std::inserter(guard_vars, guard_vars.begin()));

      auto pvi_args = as_vector(pvi.parameters());
      for (std::size_t j = 0; j < params.size() && j < pvi_args.size(); ++j)
      {
        if (pvi_args[j] != atermpp::down_cast<data::data_expression>(params[j]))
        {
          ++stats.changes[eq_name][params[j]];
          for (const auto& gv: guard_vars)
          {
            if (params[j] != gv)
            {
              ++stats.counts[eq_name][params[j]][gv];
            }
          }
        }
      }
    }
  }

  return stats;
}

// percentages[eq][dₘ][dⱼ] = # transitions where dⱼ guards a change of dₘ in eq divided
// by the total # transitions in which dₘ changes — i.e. how large a share of dₘ's
// changes is ruled by dⱼ. This lets parameters that change rarely and parameters
// that change often be compared on the same scale.
using ruling_percentages_type
  = std::map<core::identifier_string, std::map<data::variable, std::map<data::variable, double>>>;

inline ruling_percentages_type compute_ruling_percentages(const ruling_statistics_type& stats)
{
  ruling_percentages_type percentages;
  for (const auto& [eq_name, ruled_by_counts]: stats.counts)
  {
    for (const auto& [d_m, rulers_counts]: ruled_by_counts)
    {
      // Every counts entry was created in the same branch as a changes increment,
      // so the denominator exists and is positive.
      const std::size_t total = stats.changes.at(eq_name).at(d_m);
      for (const auto& [d_j, count_j]: rulers_counts)
      {
        percentages[eq_name][d_m][d_j] = static_cast<double>(count_j) / static_cast<double>(total);
      }
    }
  }
  return percentages;
}

// Builds the relation from the ruling percentages, keeping only the stronger direction
// of mutual pairs: dⱼ ≻ dₘ survives when dⱼ's percentage of ruling dₘ's changes is at
// least dₘ's percentage of ruling dⱼ's changes (ties broken by name).
inline ruling_relation_type build_ruling_relation(const ruling_percentages_type& percentages)
{
  ruling_relation_type relation;

  for (const auto& [eq_name, ruled_by_percentages]: percentages)
  {
    const auto& eq_percentages = percentages.at(eq_name);
    for (const auto& [d_m, rulers_percentages]: ruled_by_percentages)
    {
      for (const auto& [d_j, pct_j]: rulers_percentages)
      {
        auto ruled_it = eq_percentages.find(d_j);
        if (ruled_it != eq_percentages.end())
        {
          auto rev_pct_it = ruled_it->second.find(d_m);
          if (rev_pct_it != ruled_it->second.end())
          {
            const double pct_m = rev_pct_it->second;
            if (pct_j < pct_m)
            {
              continue;
            }
            else if (pct_j == pct_m && d_j.name() > d_m.name())
            {
              continue;
            }
          }
        }
        relation.ruled_by[eq_name][d_m].insert(d_j);
      }
    }
  }

  return relation;
}

// Removes cycles of length 3+ from relation, turning it into a strict order.
// Each cycle is broken by dropping the edge out of its most dominant node
// (highest summed ruling percentage, ties broken by name). Dominance is the same
// evidence used for the 2-cycle pruning, so the dominant node becomes the cycle root.
inline void break_ruling_cycles(const pbes& p, const ruling_percentages_type& percentages, ruling_relation_type& relation)
{
  for (const pbes_equation& eq: p.equations())
  {
    const auto& eq_name = eq.variable().name();
    auto rel_it = relation.ruled_by.find(eq_name);
    if (rel_it == relation.ruled_by.end())
    {
      continue;
    }
    auto& ruled_by_map = rel_it->second;

    auto pct_it = percentages.find(eq_name);
    auto node_weight = [&](const data::variable& v) -> double
    {
      if (pct_it == percentages.end())
      {
        return 0.0;
      }
      auto it = pct_it->second.find(v);
      if (it == pct_it->second.end())
      {
        return 0.0;
      }
      double sum = 0.0;
      for (const auto& [ruler, pct]: it->second)
      {
        sum += pct;
      }
      return sum;
    };

    while (true)
    {
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

      const std::vector<data::variable> cycle(path.begin()
                                                + (std::find(path.begin(), path.end(), path.back()) - path.begin()),
        path.end() - 1);

      const data::variable* v_max = &cycle.front();
      for (const data::variable& v: cycle)
      {
        double w = node_weight(v);
        double w_max = node_weight(*v_max);
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
}

// Anti-transitivity: drops ruling edges that are already implied by a longer
// chain. If d_m is ruled by d_k and d_k is (transitively) ruled by d_j, then the
// direct edge "d_m ruled by d_j" carries no new information and is removed,
// keeping only the covering pairs of the order. Does not filter branching dependencies,
// i.e. d_m -> d_k -> d_j and d_m -> d_k' -> d_j edges are not removed.
inline void remove_transitive_rulings(ruling_relation_type& relation)
{
  for (auto& [eq_name, ruled_by_map]: relation.ruled_by)
  {
    // Returns true when d_start is (transitively) ruled by d_target.
    std::function<bool(const data::variable&, const data::variable&, std::set<data::variable>&)> rules_transitively
      = [&](const data::variable& d_start, const data::variable& d_target, std::set<data::variable>& visited) -> bool
    {
      auto it = ruled_by_map.find(d_start);
      if (it == ruled_by_map.end())
      {
        return false;
      }
      for (const data::variable& d_ruler: it->second)
      {
        if (d_ruler == d_target || (visited.insert(d_ruler).second && rules_transitively(d_ruler, d_target, visited)))
        {
          return true;
        }
      }
      return false;
    };

    // Collect removals first so every edge is judged against the original relation.
    std::vector<std::pair<data::variable, data::variable>> removals;
    for (const auto& [d_m, rulers]: ruled_by_map)
    {
      for (const data::variable& d_j: rulers)
      {
        for (const data::variable& d_k: rulers)
        {
          if (d_k == d_j)
          {
            continue;
          }
          std::set<data::variable> visited;
          if (rules_transitively(d_k, d_j, visited))
          {
            removals.emplace_back(d_m, d_j);
            break;
          }
        }
      }
    }

    for (const auto& [d_m, d_j]: removals)
    {
      ruled_by_map[d_m].erase(d_j);
    }
    for (auto it = ruled_by_map.begin(); it != ruled_by_map.end();)
    {
      if (it->second.empty())
      {
        it = ruled_by_map.erase(it);
      }
      else
      {
        ++it;
      }
    }
  }
}

inline void log_ruling_relation(const ruling_relation_type& relation)
{
  mCRL2log(log::debug) << "=== Ruling relation ===" << std::endl;
  for (const auto& [eq_name, ruled_by_map]: relation.ruled_by)
  {
    for (const auto& [d_m, parameters]: ruled_by_map)
    {
      mCRL2log(log::debug) << eq_name << ": " << pp(d_m) << " ruled by " << core::detail::print_list(parameters)
                           << std::endl;
    }
  }
  for (const auto& [eq_name, sizes]: relation.tree_size)
  {
    for (const auto& [d_j, size]: sizes)
    {
      if (size > 0)
      {
        mCRL2log(log::debug) << eq_name << ": " << pp(d_j) << " rules " << size << " parameters (tree size)"
                             << std::endl;
      }
    }
  }
}

// Saves the ruling relation to a file in text format.
inline void save_ruling_relation(const ruling_relation_type& relation, const std::string& filename)
{
  std::ofstream out(filename);
  if (!out.is_open())
  {
    throw mcrl2::runtime_error("Could not open file '" + filename + "' for writing the ruling relation.");
  }
  out << "=== Ruling relation ===" << std::endl;
  for (const auto& [eq_name, ruled_by_map]: relation.ruled_by)
  {
    for (const auto& [d_m, parameters]: ruled_by_map)
    {
      out << eq_name << ": " << pp(d_m) << " ruled by " << core::detail::print_list(parameters) << std::endl;
    }
  }
}

} // namespace detail

// Computes the ruled-by relation for a PBES: relation[eq][dₘ] = { dⱼ | dⱼ ≽ dₘ in eq }.
// A variable dⱼ rules dₘ when some recursive transition changes dₘ while guarded by dⱼ.
// Ruling strength is measured as a percentage: # transitions where dⱼ guards a change of
// dₘ divided by # transitions in which dₘ changes. Mutual pairs are pruned to the
// stronger direction, and longer cycles are broken so the relation is a strict order
// (the most dominant cycle node becomes a root).
// Finally, transitively implied edges are removed: if C is ruled by B and B by A, the
// edge "C ruled by A" is redundant and dropped (anti-transitivity).
// The tree sizes — per equation the number of parameters each variable (transitively)
// rules — are computed once from the final relation and cached for the ruling strategy.
inline ruling_relation_type compute_ruling_relation(const pbes& p, const data::rewriter& datar)
{
  detail::ruling_statistics_type stats = detail::count_rulings(p, datar);
  detail::ruling_percentages_type percentages = detail::compute_ruling_percentages(stats);
  ruling_relation_type relation = detail::build_ruling_relation(percentages);
  detail::break_ruling_cycles(p, percentages, relation);
  detail::remove_transitive_rulings(relation);
  relation.compute_tree_sizes();
  detail::log_ruling_relation(relation);
  return relation;
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_DETAIL_PBESCEGPS_UTILITIES_H
