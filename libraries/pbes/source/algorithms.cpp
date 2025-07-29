// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes.cpp
/// \brief

#include "mcrl2/pbes/normalize.h"
#include "mcrl2/pbes/pbesinst_finite_algorithm.h"
#include "mcrl2/pbes/remove_equations.h"
#include "mcrl2/pbes/remove_parameters.h"
#include "mcrl2/pbes/significant_variables.h"





namespace mcrl2::pbes_system::algorithms {

void remove_parameters(pbes& x, const std::set<data::variable>& to_be_removed)
{
  pbes_system::remove_parameters(x, to_be_removed);
}

void remove_parameters(pbes& x, const std::map<core::identifier_string, std::vector<std::size_t> >& to_be_removed)
{
  pbes_system::remove_parameters(x, to_be_removed);
}

void normalize(pbes& x)
{
  try
  {
    pbes_system::normalize(x);
  }
  catch (const mcrl2::runtime_error&)
  {
    throw mcrl2::runtime_error("The PBES is not monotonic!");
  }
}

bool is_normalized(const pbes& x)
{
  return pbes_system::is_normalized(x);
}

void pbesinst_finite(pbes& p, data::rewrite_strategy rewrite_strategy, const std::string& finite_parameter_selection)
{
  pbes_system::pbesinst_finite(p, rewrite_strategy, finite_parameter_selection);
}

std::string print_removed_equations(const std::vector<propositional_variable>& removed)
{
  return pbes_system::detail::print_removed_equations(removed);
}

std::vector<propositional_variable> remove_unreachable_variables(pbes& p)
{
  return pbes_system::remove_unreachable_variables(p);
}

std::set<data::variable> significant_variables(const pbes_expression& x)
{
  return pbes_system::significant_variables(x);
}

} // namespace mcrl2::pbes_system::algorithms





