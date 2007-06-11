// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/utility.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_UTILITY_H
#define MCRL2_LPS_DETAIL_UTILITY_H

#include <vector>
#include <string>
#include <utility>
#include <iterator>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "mcrl2/data/data.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/data/function.h"
#include "libstruct.h"

namespace lps {

namespace detail {

/// Makes a set of the given container.
template <class Container>
std::set<typename Container::value_type> make_set(const Container& c)
{
  std::set<typename Container::value_type> result;
  std::copy(c.begin(), c.end(), std::inserter(result, result.begin()));
  return result;
}

template <typename T>
std::set<T> set_union(const std::set<T>& x, const std::set<T>& y)
{
  std::set<T> result;
  std::set_union(x.begin(), x.end(), y.begin(), y.end(), std::inserter(result, result.begin()));
  return result;
}

template <typename T>
std::set<T> set_difference(const std::set<T>& x, const std::set<T>& y)
{
  std::set<T> result;
  std::set_difference(x.begin(), x.end(), y.begin(), y.end(), std::inserter(result, result.begin()));
  return result;
}

// example: "X(d:D,e:E)"
inline
std::pair<std::string, data_expression_list> parse_variable(std::string s)
{
  using boost::algorithm::split;
  using boost::algorithm::is_any_of;

  std::string name;
  data_expression_list variables;

  std::string::size_type idx = s.find('(');
  if (idx == std::string::npos)
  {
    name = s;
  }
  else
  {
    name = s.substr(0, idx);
    assert(*s.rbegin() == ')');
    std::vector<std::string> v;
    std::string w = s.substr(idx + 1, s.size() - idx - 2);
    split(v, w, is_any_of(","));
    // This doesn't compile in combination with 'using namespace std::rel_ops'
    // for Visual C++ 8.0 (looks like a compiler bug)
    // for (std::vector<std::string>::reverse_iterator i = v.rbegin(); i != v.rend(); ++i)
    // {
    //   data_expression d = data_variable(*i);
    //   variables = push_front(variables, d);
    // }
    for (std::vector<std::string>::iterator i = v.begin(); i != v.end(); ++i)
    {
      data_expression d = data_variable(*i);
      variables = push_front(variables, d);
    }
  }
  return std::make_pair(name, atermpp::reverse(variables));
}

/// Returns true if the names of the given variables are unique.
inline
bool unique_names(data_variable_list variables)
{
  std::set<identifier_string> summation_variable_names;
  for (data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
  {
    summation_variable_names.insert(i->name());
  }
  if (summation_variable_names.size() != variables.size())
  {
    return false;
  }
  return true;
}

/// Returns true if the left hand sides of assignments are contained in variables.
inline
bool check_assignment_variables(data_assignment_list assignments, data_variable_list variables)
{
  std::set<data_variable> v;
  std::copy(variables.begin(), variables.end(), std::inserter(v, v.begin()));
  for (data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
  {
    if (v.find(i->lhs()) == v.end())
      return false;
  }
  return true;
}

/// Returns true if the domain sorts and the range sort of the given sort s are
/// contained in sorts.
inline bool check_sort(lps::sort s, const std::set<lps::sort>& sorts)
{
  if (sorts.find(s.range_sort()) == sorts.end())
    return false;
  sort_list domain_sorts = s.domain_sorts();
  for (sort_list::iterator i = domain_sorts.begin(); i != domain_sorts.end(); ++i)
  {
    if (sorts.find(*i) == sorts.end())
      return false;
  }
  return true;
}

/// Returns true if the domain sorts and the range sort of the given variables are contained
/// in sorts.
inline
bool check_variable_sorts(data_variable_list variables, const std::set<lps::sort>& sorts)
{
  for (data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
  {
    if (!check_sort(i->sort(), sorts))
      return false;
  }
  return true;
}

/// Returns true if the sorts of the given actions are contained in sorts.
inline
bool check_action_sorts(action_list actions, const std::set<lps::sort>& sorts)
{
  for (action_list::iterator i = actions.begin(); i != actions.end(); ++i)
  {
    const sort_list& s = i->label().sorts();
    for (sort_list::iterator j = s.begin(); j != s.end(); ++j)
    {
      if (!check_sort(*j, sorts))
        return false;
    }
  }
  return true;
}

/// Returns true if the labels of the given actions are contained in labels.
inline
bool check_action_labels(action_list actions, const std::set<action_label>& labels)
{
  for (action_list::iterator i = actions.begin(); i != actions.end(); ++i)
  {
    if (labels.find(i->label()) == labels.end())
      return false;
  }
  return true;
}

/// Returns true if the sorts of the given action labels are contained in sorts.
inline
bool check_action_label_sorts(action_label_list action_labels, const std::set<lps::sort>& sorts)
{
  for (action_label_list::iterator i = action_labels.begin(); i != action_labels.end(); ++i)
  {
    for (sort_list::iterator j = i->sorts().begin(); j != i->sorts().end(); ++j)
    {
      if (!check_sort(*j, sorts))
        return false;
    }
  }
  return true;
}

/// Returns true if names of the given variables are not contained in names.
inline
bool check_variable_names(data_variable_list variables, const std::set<identifier_string>& names)
{
  for (data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i)
  {
    if (names.find(i->name()) != names.end())
      return false;
  }
  return true;
}

/// Returns true if the domain sorts and range sort of the given functions are
/// contained in sorts.
inline
bool check_data_spec_sorts(function_list functions, const std::set<lps::sort>& sorts)
{
  for (function_list::iterator i = functions.begin(); i != functions.end(); ++i)
  {
    if (!check_sort(i->sort(), sorts))
      return false;
  }
  return true;
}

/// \brief Returns the names of the variables in t
inline
std::vector<std::string> variable_strings(data_variable_list t)
{
  std::vector<std::string> result;
  for (data_variable_list::iterator i = t.begin(); i != t.end(); ++i)
    result.push_back(i->name());
  return result;
}

} // namespace detail

} // namespace lps

#endif // MCRL2_LPS_DETAIL_UTILITY_H
