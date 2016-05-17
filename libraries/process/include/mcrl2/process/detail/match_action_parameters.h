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

inline
bool contains_untyped_sorts(const data::sort_expression_list& sorts)
{
  for (const data::sort_expression& s: sorts)
  {
    if (data::is_untyped_sort(s) || data::is_untyped_possible_sorts(s))
    {
      return true;
    }
  }
  return false;
}

inline
bool is_matching_sort(const data::sort_expression& sort, const data::sort_expression& allowed_sort)
{
  if (is_untyped_sort(allowed_sort))
  {
    return true;
  }
  if (is_untyped_possible_sorts(allowed_sort))
  {
    const data::sort_expression_list& sorts = atermpp::down_cast<const data::untyped_possible_sorts>(allowed_sort).sorts();
    return std::find(sorts.begin(), sorts.end(), sort) != sorts.end();
  }
  return sort == allowed_sort;
}

// Returns true if sorts matches allowed_sorts. That is, the holes in allowed_sorts can be filled such that the lists become equal.
inline
bool is_matching_sort_list(const data::sort_expression_list& sorts, const data::sort_expression_list& allowed_sorts)
{
  assert(sorts.size() == allowed_sorts.size());
  auto j = allowed_sorts.begin();
  for (auto i = sorts.begin(); i != sorts.end(); ++i, ++j)
  {
    if (!is_matching_sort(*i, *j))
    {
      return false;
    }
  }
  return true;
}

inline
data::sort_expression make_untyped_possible_sorts(const std::set<data::sort_expression>& x)
{
  if (x.size() == 1)
  {
    return *(x.begin());
  }
  return data::untyped_possible_sorts(data::sort_expression_list(x.begin(), x.end()));
}

// Join the sort lists, by wrapping the set of i-th elements of the lists into an untyped_possible_sorts.
// If a set has size one, it is not wrapped into an untyped_possible_sorts.
inline
data::sort_expression_list join_sort_lists(const data::sorts_list& sorts)
{
  if (sorts.size() == 1)
  {
    return sorts.front();
  }
  std::size_t n = sorts.front().size();

  // use sets to remove duplicates
  std::vector<std::set<data::sort_expression> > sort_sets(n);
  for (const data::sort_expression_list& sort_list: sorts)
  {
    std::size_t i = 0;
    for (const data::sort_expression& s: sort_list)
    {
      sort_sets[i++].insert(s);
    }
  }

  data::sort_expression_vector result;
  for (const std::set<data::sort_expression>& x: sort_sets)
  {
    result.push_back(make_untyped_possible_sorts(x));
  }
  return data::sort_expression_list(result.begin(), result.end());
}

// Filter the possible sort lists.
// If there is no matching sort list, the first element of the result is false.
// The second element of the result contains the remaining alternatives. Multiple choices at a certain position are wrapped into an untyped_possible_sorts.
inline
std::pair<bool, data::sort_expression_list> filter_sort_lists(const data::sort_expression_list& possible_sorts, const data::sorts_list& possible_parameter_sorts)
{
  // If possible_sorts does not contain untyped sorts, look for a precise match.
  if (!contains_untyped_sorts(possible_sorts))
  {
    if (std::find(possible_parameter_sorts.begin(), possible_parameter_sorts.end(), possible_sorts) != possible_parameter_sorts.end())
    {
      return std::make_pair(true, possible_sorts);
    }
    else
    {
      return std::make_pair(false, data::sort_expression_list());
    }
  }

  data::sorts_list matching_sort_lists;
  for (const data::sort_expression_list& s: possible_parameter_sorts)
  {
    if (is_matching_sort_list(s, possible_sorts))
    {
      matching_sort_lists.push_front(s);
    }
  }
  matching_sort_lists = atermpp::reverse(matching_sort_lists);

  if (matching_sort_lists.empty())
  {
    return std::make_pair(false, data::sort_expression_list());
  }
  if (matching_sort_lists.size() == 1)
  {
    return std::make_pair(true, matching_sort_lists.front());
  }
  return std::make_pair(true, join_sort_lists(matching_sort_lists));
}

inline
data::data_expression typecheck_data_expression(const data::data_expression& x,
                                                const data::sort_expression& expected_sort,
                                                const data::detail::variable_context& variable_context,
                                                const core::identifier_string& name,
                                                const data::data_expression_list& parameters,
                                                data::data_type_checker& typechecker
                                               )

{
  data::data_expression result;
  try
  {
    result = typechecker.typecheck_data_expression(x, expected_sort, variable_context);
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(x) + " as type " + data::pp(typechecker.expand_numeric_types_down(expected_sort)) + " (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
  }
  return result;
}

inline
data::data_expression upcast_numeric_type(const data::data_expression& x,
                                          const data::sort_expression& expected_sort,
                                          const data::detail::variable_context& variable_context,
                                          const core::identifier_string& name,
                                          const data::data_expression_list& parameters,
                                          data::data_type_checker& typechecker
                                         )
{
  try
  {
    return typechecker.upcast_numeric_type(x, expected_sort, variable_context);
  }
  catch (mcrl2::runtime_error& e)
  {
    throw mcrl2::runtime_error(std::string(e.what()) + "\ncannot typecheck " + data::pp(x) + " as type " + data::pp(typechecker.expand_numeric_types_down(expected_sort)) + " (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
  }
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
  if (possible_parameter_sorts.empty())
  {
    throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                    + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                    + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
  }
  data::sort_expression_list expected_sorts = join_sort_lists(possible_parameter_sorts);
  data::sort_expression_list possible_sorts = expected_sorts;
  data::data_expression_vector new_parameters(parameters.begin(), parameters.end());
  auto p1 = new_parameters.begin();
  auto p2 = possible_sorts.begin();
  for (; p1 != new_parameters.end(); ++p1, ++p2)
  {
    data::data_expression& e = *p1;
    const data::sort_expression& expected_sort = *p2;
    e = typecheck_data_expression(e, expected_sort, variable_context, name, parameters, typechecker);
  }

  std::pair<bool, data::sort_expression_list> p = filter_sort_lists(parameter_sorts(new_parameters), possible_parameter_sorts);
  possible_sorts = p.second;

  if (!p.first)
  {
    possible_sorts = expected_sorts;
    auto q1 = new_parameters.begin();
    auto q2 = possible_sorts.begin();
    for (; q1 != new_parameters.end(); ++q1, ++q2)
    {
      data::data_expression& e = *q1;
      data::sort_expression expected_sort = *q2;
      e = upcast_numeric_type(e, expected_sort, variable_context, name, parameters, typechecker);
    }

    std::pair<bool, data::sort_expression_list> p = filter_sort_lists(parameter_sorts(new_parameters), possible_parameter_sorts);
    possible_sorts = p.second;

    if (!p.first)
    {
      throw mcrl2::runtime_error("no " + msg + " " + core::pp(name) + "with type " + data::pp(parameter_sorts(new_parameters)) + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
    }
  }
  if (contains_untyped_sorts(possible_sorts))
  {
    throw mcrl2::runtime_error("ambiguous " + msg + " " + core::pp(name));
  }
  data::data_expression_list result(new_parameters.begin(), new_parameters.end());
  assert(data::normalize_sorts(result, typechecker.get_sort_specification()) == result);
  return std::make_pair(result, possible_sorts);
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_MATCH_ACTION_PARAMETERS_H
