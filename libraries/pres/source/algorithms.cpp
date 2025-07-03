// Author(s): Jan Friso Groote. Based on pbes/source/algorithms by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file algorithms.cpp
/// \brief

#include "mcrl2/pres/normalize.h"
#include "mcrl2/pres/presinst_finite_algorithm.h"
#include "mcrl2/pres/remove_equations.h"
#include "mcrl2/pres/remove_parameters.h"
#include "mcrl2/pres/significant_variables.h"





namespace mcrl2::pres_system::algorithms {

void remove_parameters(pres& x, const std::set<data::variable>& to_be_removed)
{
  pres_system::remove_parameters(x, to_be_removed);
}

void remove_parameters(pres& x, const std::map<core::identifier_string, std::vector<std::size_t> >& to_be_removed)
{
  pres_system::remove_parameters(x, to_be_removed);
} 

void normalize(pres& x)
{
  try
  {
    pres_system::normalize(x);
  }
  catch (const mcrl2::runtime_error&)
  {
    throw mcrl2::runtime_error("The PBES is not monotonic!");
  }
}

bool is_normalized(const pres& x)
{
  return pres_system::is_normalized(x);
}

void presinst_finite(pres& p, data::rewrite_strategy rewrite_strategy, const std::string& finite_parameter_selection)
{
  pres_system::presinst_finite(p, rewrite_strategy, finite_parameter_selection);
} 

std::string print_removed_equations(const std::vector<propositional_variable>& removed)
{
  return pres_system::detail::print_removed_equations(removed);
}

std::vector<propositional_variable> remove_unreachable_variables(pres& p)
{
  return pres_system::remove_unreachable_variables(p);
}

std::set<data::variable> significant_variables(const pres_expression& x)
{
  return pres_system::significant_variables(x);
} 

} // namespace mcrl2::pres_system::algorithms





