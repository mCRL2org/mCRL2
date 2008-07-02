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

#include <set>
#include "mcrl2/lps/action.h"
#include "mcrl2/old_data/detail/data_utility.h"

namespace mcrl2 {

namespace lps {

namespace detail {

/// Returns true if the sorts of the given actions are contained in sorts.
inline
bool check_action_sorts(action_list actions, const std::set<old_data::sort_expression>& sorts)
{
  for (action_list::iterator i = actions.begin(); i != actions.end(); ++i)
  {
    const old_data::sort_expression_list& s = i->label().sorts();
    for (old_data::sort_expression_list::iterator j = s.begin(); j != s.end(); ++j)
    {
      if (!old_data::detail::check_sort(*j, sorts))
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
bool check_action_label_sorts(action_label_list action_labels, const std::set<old_data::sort_expression>& sorts)
{
  for (action_label_list::iterator i = action_labels.begin(); i != action_labels.end(); ++i)
  {
    for (old_data::sort_expression_list::iterator j = i->sorts().begin(); j != i->sorts().end(); ++j)
    {
      if (!old_data::detail::check_sort(*j, sorts))
        return false;
    }
  }
  return true;
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_ACTION_UTILITY_H
