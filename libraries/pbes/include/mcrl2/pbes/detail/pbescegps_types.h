// Author(s): Jore Booy
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbescegps_types.h
/// \brief Shared types for the CEGPS algorithm.

#ifndef MCRL2_PBES_DETAIL_PBESCEGPS_TYPES_H
#define MCRL2_PBES_DETAIL_PBESCEGPS_TYPES_H

#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/core/identifier_string.h"
#include "mcrl2/data/rewrite_strategy.h"
#include "mcrl2/data/variable.h"
#include "mcrl2/pbes/pbes.h"

namespace mcrl2::pbes_system
{

enum class var_choice_strategy
{
  lhs, // variable order of the left-hand side of the equation
  rhs, // variable order of the right-hand side of the equation
  count // free variable that occurs most often (excluding data expressions in PVI)
};

struct pbescegps_options
{
  data::rewrite_strategy rewrite_strategy = data::rewrite_strategy::jitty;
  bool init_control_flow = false;
  bool solve_symbolic = false;
  var_choice_strategy var_choice = var_choice_strategy::lhs;
  std::string solve_symbolic_args = "";
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

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_DETAIL_PBESCEGPS_TYPES_H
