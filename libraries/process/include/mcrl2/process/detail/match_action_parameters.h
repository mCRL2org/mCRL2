// Author(s): Yaroslav Usenko, Jan Friso Groote, Wieger Wesselink (2016)
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

#include "mcrl2/data/normalize_sorts.h"
#include "mcrl2/data/typecheck.h"
#include "mcrl2/data/undefined.h"

namespace mcrl2 {

namespace process {

namespace detail {

template <typename Container>
data::sort_expression_list parameter_sorts(const Container& parameters)
{
  data::sort_expression_list sorts;
  for (const data::data_expression& e: parameters)
  {
    sorts.push_front(e.sort());
  }
  return atermpp::reverse(sorts);
}

// This function is introduced to hide the exception based interface of the data type checker.
inline
data::data_expression typecheck_data_expression(const data::data_expression& x,
                                                const data::sort_expression& expected_sort,
                                                const data::detail::variable_context& variable_context,
                                                data::data_type_checker& typechecker
                                               )
{
  try
  {
    return typechecker.typecheck_data_expression(x, expected_sort, variable_context);
  }
  catch (mcrl2::runtime_error& e)
  {
    return data::undefined_data_expression();
  }
}

// This function is introduced to hide the exception based interface of the data type checker.
inline
data::data_expression upcast_numeric_type(const data::data_expression& x,
                                          const data::sort_expression& expected_sort,
                                          const data::detail::variable_context& variable_context,
                                          data::data_type_checker& typechecker
                                         )
{
  try
  {
    return typechecker.upcast_numeric_type(x, expected_sort, variable_context);
  }
  catch (mcrl2::runtime_error& e)
  {
    return data::undefined_data_expression();
  }
}

inline
data::data_expression match_action_parameter(const data::data_expression& x,
                                             const data::sort_expression& expected_sort,
                                             const data::detail::variable_context& variable_context,
                                             data::data_type_checker& typechecker
                                            )
{
  data::data_expression result = typecheck_data_expression(x, expected_sort, variable_context, typechecker);
  if (result != data::undefined_data_expression() && (result.sort() != expected_sort))
  {
    result = upcast_numeric_type(result, expected_sort, variable_context, typechecker);
  }
  if (data::is_untyped_sort(result.sort()) || data::is_untyped_possible_sorts(result.sort()))
  {
    result = data::undefined_data_expression();
  }
  return result;
}

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
    data::data_expression x = match_action_parameter(*i, *j, variable_context, typechecker);
    if (x == data::undefined_data_expression())
    {
      return { false, {} };
    }
    else
    {
      result.push_back(x);
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
    throw mcrl2::runtime_error("no " + msg + " " + core::pp(name) + "with type " + data::pp(parameter_sorts(parameters)) + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
  }
  if (matches.size() > 1)
  {
    throw mcrl2::runtime_error("ambiguous " + msg + " " + core::pp(name) + "(" + data::pp(parameters) + ")");
  }
  const data::data_expression_vector& typechecked_parameters = matches.front();
  return { data::data_expression_list(typechecked_parameters.begin(), typechecked_parameters.end()), parameter_sorts(typechecked_parameters) };
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_MATCH_ACTION_PARAMETERS_H
