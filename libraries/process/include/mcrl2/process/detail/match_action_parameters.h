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

#include "mcrl2/data/detail/data_typechecker.h"

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
bool is_contained_in(const data::sort_expression_list& l, const data::sorts_list& sorts)
{
  return std::find(sorts.begin(), sorts.end(), l) != sorts.end();
}

inline
bool is_allowed_sort(const data::sort_expression& sort, const data::sort_expression& allowed_sort)
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

inline
bool is_allowed_sort_list(const data::sort_expression_list& sorts, const data::sort_expression_list& allowed_sorts)
{
  assert(sorts.size() == allowed_sorts.size());
  auto j = allowed_sorts.begin();
  for (auto i = sorts.begin(); i != sorts.end(); ++i, ++j)
  {
    if (!is_allowed_sort(*i, *j))
    {
      return false;
    }
  }
  return true;
}

inline
data::sort_expression_list insert_type(const data::sort_expression_list& sorts, const data::sort_expression& sort)
{
  if (std::find(sorts.begin(), sorts.end(), sort) != sorts.end())
  {
    return sorts;
  }
  data::sort_expression_list result = sorts;
  result.push_front(sort);
  return result;
}

inline
data::sort_expression_list get_not_inferred_list(const data::sorts_list& sorts)
{
  if (sorts.size() == 1)
  {
    return sorts.front();
  }
  data::sort_expression_list result;
  size_t n = sorts.front().size();
  std::vector<data::sort_expression_list> parameter_lists(n, data::sort_expression_list());
  for (data::sort_expression_list s: sorts)
  {
    for (size_t i = 0; i < n; s = s.tail(), i++)
    {
      parameter_lists[i] = insert_type(parameter_lists[i], s.front());
    }
  }
  for (size_t i = n; i > 0; i--)
  {
    data::sort_expression sort;
    if (parameter_lists[i - 1].size() == 1)
    {
      sort = parameter_lists[i - 1].front();
    }
    else
    {
      sort = data::untyped_possible_sorts(data::sort_expression_list(atermpp::reverse(parameter_lists[i - 1])));
    }
    result.push_front(sort);
  }
  return result;
}

inline
std::pair<bool, data::sort_expression_list> adjust_not_inferred_list(const data::sort_expression_list& possible_sorts, const data::sorts_list& sorts)
{
  if (!contains_untyped_sorts(possible_sorts))
  {
    if (is_contained_in(possible_sorts, sorts))
    {
      return std::make_pair(true, possible_sorts);
    }
    else
    {
      return std::make_pair(false, data::sort_expression_list());
    }
  }

  data::sorts_list new_sorts;
  for (const data::sort_expression_list& s: sorts)
  {
    if (is_allowed_sort_list(s, possible_sorts))
    {
      new_sorts.push_front(s);
    }
  }
  new_sorts = atermpp::reverse(new_sorts);

  if (new_sorts.empty())
  {
    return std::make_pair(false, data::sort_expression_list());
  }
  if (new_sorts.size() == 1)
  {
    return std::make_pair(true, new_sorts.front());
  }
  return std::make_pair(true, get_not_inferred_list(new_sorts));
}

inline
data::sorts_list normalize_sorts(const data::sorts_list& x, const data::sort_specification& sortspec)
{
  std::vector<data::sort_expression_list> result;
  for (const data::sort_expression_list& s: x)
  {
    result.push_back(data::normalize_sorts(s, sortspec));
  }
  return data::sorts_list(result.begin(), result.end());
}

inline
std::pair<data::data_expression_list, data::sort_expression_list> match_action_parameters(const data::data_expression_list& parameters,
                                                                                          const data::sorts_list& parameter_list1,
                                                                                          const data::detail::variable_context& variable_context,
                                                                                          const core::identifier_string& name,
                                                                                          const std::string& msg,
                                                                                          data::detail::data_typechecker& typechecker
                                                                                         )
{
  data::sorts_list parameter_list = normalize_sorts(parameter_list1, typechecker.get_sort_type_checker().get_sort_specification());

  if (parameter_list.empty())
  {
    throw mcrl2::runtime_error("no " + msg + " " + core::pp(name)
                    + " with " + atermpp::to_string(parameters.size()) + " parameter" + ((parameters.size() != 1)?"s":"")
                    + " is declared (while typechecking " + core::pp(name) + "(" + data::pp(parameters) + "))");
  }
  data::sort_expression_list expected_sorts = get_not_inferred_list(parameter_list);
  data::sort_expression_list possible_sorts = expected_sorts;
  data::data_expression_vector new_parameters(parameters.begin(), parameters.end());
  auto p1 = new_parameters.begin();
  auto p2 = possible_sorts.begin();
  for (; p1 != new_parameters.end(); ++p1, ++p2)
  {
    data::data_expression& e = *p1;
    const data::sort_expression& expected_sort = *p2;
    e = typechecker.typecheck_data_expression_nothrow(e, expected_sort, variable_context.context(), name, parameters);
  }

  std::pair<bool, data::sort_expression_list> p = adjust_not_inferred_list(parameter_sorts(new_parameters), parameter_list);
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
      e = typechecker.upcast_numeric_type(e, expected_sort, variable_context.context(), name, parameters);
    }

    std::pair<bool, data::sort_expression_list> p = adjust_not_inferred_list(parameter_sorts(new_parameters), parameter_list);
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
  assert(data::normalize_sorts(result, typechecker.get_sort_type_checker().get_sort_specification()) == result);
  return std::make_pair(result, possible_sorts);
}

} // namespace detail

} // namespace process

} // namespace mcrl2

#endif // MCRL2_PROCESS_DETAIL_MATCH_ACTION_PARAMETERS_H
