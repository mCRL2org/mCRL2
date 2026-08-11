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
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/detail/count_free_variables.h"
#include "mcrl2/pbes/detail/find_free_variables.h"
#include "mcrl2/pbes/pbes.h"
#include "mcrl2/pbes/pbes_expression.h"
#include "mcrl2/pbes/propositional_variable.h"
#include "mcrl2/utilities/exception.h"
#include "mcrl2/utilities/logger.h"
#include <cctype>
#include <cstddef>
#include <fstream>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <sstream>

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
  bool stategraph = false;
  var_choice_strategy var_choice = var_choice_strategy::lhs;
  std::string solve_symbolic_args = "";
  std::size_t number_of_threads = 1;
  std::string initial_state_file = ""; // if non-empty, read the initial abstraction state from this file
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
  std::set<data::variable> vars = find_free_variables(formula, data::variable_list(), false);
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
  const std::set<data::variable>& abstracted_vars)
{
  std::set<data::variable> vars = find_free_variables(formula, data::variable_list(), false);
  for (const auto& var: std::ranges::reverse_view(vars))
  {
    if (abstracted_vars.contains(var))
    {
      return var;
    }
  }
  return std::nullopt;
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
inline void add_parameter_by_token(const pbes& p, abstract_param_state& state, 
  const core::identifier_string& eq_name, const std::string& token, 
  const std::vector<data::variable>& params)
{
  if (!token.empty() && std::isdigit(static_cast<unsigned char>(token[0])))
  {
    // Token is a parameter index.
    const std::size_t idx = std::stoul(token);
    if (idx >= params.size())
    {
      throw mcrl2::runtime_error("initial-state: parameter index " + token + " out of bounds in equation " + core::pp(eq_name));
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
  std::string eq_name = (token == "Abstracted") 
    ? parse_verbose_equation_name(iss, line)
    : parse_bare_equation_name(iss, token, line);

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

} // namespace detail

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_DETAIL_PBESCEGPS_UTILITIES_H
