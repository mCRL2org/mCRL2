// Author(s): Wieger Wesselink (2016)
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/process/detail/match_action_parameters.h
/// \brief add your file description here.

#ifndef MCRL2_PROCESS_DETAIL_MATCH_ACTION_PARAMETERS_H
#define MCRL2_PROCESS_DETAIL_MATCH_ACTION_PARAMETERS_H

#include "mcrl2/core/detail/print_utility.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/detail/data_utility.h"

namespace mcrl2 {

namespace process {

namespace detail {

inline
std::pair<bool, data::data_expression_vector> match_action_parameters(const data::data_expression_list& parameters,
                                                                      const data::sort_expression_list& expected_sorts,
                                                                      const data::detail::variable_context& variable_context,
                                                                      data::data_type_checker& typechecker
                                                                     )
{
  data::data_expression_vector result;
  auto i = parameters.begin();
  auto j = expected_sorts.begin();
  for (; i != parameters.end(); ++i, ++j)
  {
    try
    {
      result.push_back(typechecker.typecheck_data_expression(*i, *j, variable_context));
    }
    catch (mcrl2::runtime_error& e)
    {
      return { false, {} };
    }
  }
  return { true, result };
}

inline
std::pair<data::data_expression_list, data::sort_expression_list> match_action_parameters(const data::data_expression_list& parameters,
                                                                                          const data::sorts_list& possible_parameter_sorts,
                                                                                          const data::detail::variable_context& variable_context,
                                                                                          const core::identifier_string& name,
                                                                                          const std::string& msg,
                                                                                          data::data_type_checker& typechecker
                                                                                         )
{
  std::vector<data::data_expression_vector> matches;
  for (const data::sort_expression_list& sorts: possible_parameter_sorts)
  {
    auto p = match_action_parameters(parameters, sorts, variable_context, typechecker);
    if (p.first)
    {
      matches.push_back(p.second);
    }
  }
  if (matches.empty())
  {
    throw mcrl2::runtime_error("Could not find a matching " + msg + " declaration for " + core::pp(name) + core::detail::print_arguments(parameters) + ".");
  }
  if (matches.size() > 1)
  {
    throw mcrl2::runtime_error("Multiple matching " + msg + " declarations for " + core::pp(name) + core::detail::print_arguments(parameters) + ".");
  }
  const data::data_expression_vector& typechecked_parameters = matches.front();
  return { data::data_expression_list(typechecked_parameters.begin(), typechecked_parameters.end()), data::detail::parameter_sorts(typechecked_parameters) };
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_MATCH_ACTION_PARAMETERS_H
