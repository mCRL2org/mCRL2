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
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/detail/count_free_variables.h"
#include "mcrl2/pbes/detail/find_free_variables.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/utilities/logger.h"
#include <cstddef>
#include <map>
#include <optional>
#include <ranges>
#include <set>

namespace mcrl2::pbes_system
{

enum class var_choice_strategy
{
  lhs, // variable order of the left-hand side of the equation
  rhs, // variable order of the right-hand side of the equation
  count, // free variable that occurs most often (excluding data expressions in PVI)
  all // un-abstract all variables that occur
};

struct pbescegps_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  bool init_control_flow = false;
  bool solve_symbolic = false;
  var_choice_strategy var_choice = var_choice_strategy::lhs;
  std::string solve_symbolic_args = "";
  std::size_t number_of_threads = 1;
};

struct abstract_param_state
{
  std::map<core::identifier_string, std::set<data::variable>> W;
  std::map<core::identifier_string, std::set<std::size_t>> I;

  void remove_abstracted_variable(const pbes& p, const core::identifier_string& eq_name, const std::size_t& i)
  {
    I[eq_name].erase(i);
    for (const auto& eq: p.equations())
    {
      if (eq.variable().name() == eq_name)
      {
        W[eq_name].erase(atermpp::down_cast<data::variable>(eq.variable().parameters()[i]));
        break;
      }
    }
  }

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

namespace detail
{

inline std::optional<data::variable> choose_variable_by_count(const core::identifier_string& var_name,
  const pbes_expression& equation_formula,
  const std::set<data::variable>& essential_vars,
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

inline std::optional<data::variable> choose_variable_by_lhs_order(const propositional_variable& bound_variable,
  const std::set<data::variable>& essential_vars,
  const pbes_expression& formula = pbes_expression())
{
  for (const data::variable& param: bound_variable.parameters())
  {
    if (essential_vars.contains(param) && (formula == pbes_expression() || search_variable(formula, param)))
    {
      return param;
    }
  }
  return std::nullopt;
}

inline std::optional<data::variable> choose_variable_by_rhs_order(const pbes_expression& formula,
  const std::set<data::variable>& essential_vars)
{
  find_free_variables_traverser f(data::variable_list(), false);
  f.apply(formula);
  std::set<data::variable> vars = f.result;
  for (const auto& var: vars)
  {
    if (essential_vars.contains(var))
    {
      return var;
    }
  }
  return std::nullopt;
}

inline std::optional<data::variable> choose_variable_by_rhs_order_reverse(const pbes_expression& formula,
  const std::set<data::variable>& essential_vars)
{
  find_free_variables_traverser f(data::variable_list(), false);
  f.apply(formula);
  std::set<data::variable> vars = f.result;
  for (const auto& var: std::ranges::reverse_view(vars))
  {
    if (essential_vars.contains(var))
    {
      return var;
    }
  }
  return std::nullopt;
}

} // namespace detail

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_DETAIL_PBESCEGPS_UTILITIES_H
