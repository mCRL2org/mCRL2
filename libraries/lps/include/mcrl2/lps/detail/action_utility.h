// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/detail/action_utility.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_DETAIL_ACTION_UTILITY_H
#define MCRL2_LPS_DETAIL_ACTION_UTILITY_H

#include "mcrl2/data/detail/data_utility.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/utilities/detail/container_utility.h"
#include <set>

namespace mcrl2
{

namespace lps
{

namespace detail
{

/// \brief Returns true if the sorts of the given actions are contained in sorts.
/// \param actions A sequence of actions
/// \param sorts A set of sort expressions
/// \return True if the sorts of the given actions are contained in sorts.
inline
bool check_action_sorts(const process::action_list& actions, const std::set<data::sort_expression>& sorts)
{
  for (const process::action& a: actions)
  {
    for (const data::sort_expression& s: a.label().sorts())
    {
      if (!data::detail::check_sort(s, sorts))
      {
        return false;
      }
    }
  }
  return true;
}

/// \brief Returns true if the labels of the given actions are contained in labels.
/// \param actions A sequence of actions
/// \param labels A set of action labels
/// \return True if the labels of the given actions are contained in labels.
inline
bool check_action_labels(const process::action_list& actions, const std::set<process::action_label>& labels)
{
  using utilities::detail::contains;
  for (const process::action& a: actions)
  {
    if (!contains(labels, a.label()))
    {
      return false;
    }
  }
  return true;
}

/// \brief Returns true if the sorts of the given action labels are contained in sorts.
/// \param action_labels A sequence of action labels
/// \param sorts A set of sort expressions
/// \return True if the sorts of the given action labels are contained in sorts.
inline
bool check_action_label_sorts(const process::action_label_list& action_labels, const std::set<data::sort_expression>& sorts)
{
  for (const process::action_label& label: action_labels)
  {
    for (const data::sort_expression& s: label.sorts())
    {
      if (!data::detail::check_sort(s, sorts))
      {
        return false;
      }
    }
  }
  return true;
}

/// \brief Check whether some action name is defined multiple times with different sorts
/// \param actions_labels A sequence of action labels
/// \return True if there are two action label with the same name but with different sorts
inline
bool check_action_label_duplicates(const process::action_label_list& actions_labels)
{
  std::map< core::identifier_string, data::sort_expression_list > sort_map;
  for(const process::action_label& al: actions_labels)
  {
    auto find_result = sort_map.find(al.name());
    if(find_result != sort_map.end() && find_result->second != al.sorts())
    {
      return false;
    }
    sort_map.insert(std::make_pair(al.name(), al.sorts()));
  }
  return true;
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_ACTION_UTILITY_H
