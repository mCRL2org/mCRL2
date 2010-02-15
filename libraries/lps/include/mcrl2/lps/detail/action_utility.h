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
#include "mcrl2/data/detail/data_utility.h"

namespace mcrl2 {

namespace lps {

namespace detail {

/// \brief Returns true if the sorts of the given actions are contained in sorts.
/// \param actions A sequence of actions
/// \param sorts A set of sort expressions
/// \return True if the sorts of the given actions are contained in sorts.
inline
bool check_action_sorts(action_list actions, const std::set<data::sort_expression>& sorts)
{
  for (action_list::iterator i = actions.begin(); i != actions.end(); ++i)
  {
    const data::sort_expression_list& s = i->label().sorts();
    for (data::sort_expression_list::const_iterator j = s.begin(); j != s.end(); ++j)
    {
      if (!data::detail::check_sort(*j, sorts))
        return false;
    }
  }
  return true;
}

/// \brief Returns true if the labels of the given actions are contained in labels.
/// \param actions A sequence of actions
/// \param labels A set of action labels
/// \return True if the labels of the given actions are contained in labels.
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

/// \brief Returns true if the sorts of the given action labels are contained in sorts.
/// \param action_labels A sequence of action labels
/// \param sorts A set of sort expressions
/// \return True if the sorts of the given action labels are contained in sorts.
inline
bool check_action_label_sorts(action_label_list action_labels, const std::set<data::sort_expression>& sorts)
{
  for (action_label_list::iterator i = action_labels.begin(); i != action_labels.end(); ++i)
  {
    data::sort_expression_list i_sorts(i->sorts());

    for (data::sort_expression_list::const_iterator j = i_sorts.begin(); j != i_sorts.end(); ++j)
    {
      if (!data::detail::check_sort(*j, sorts))
        return false;
    }
  }
  return true;
}

} // namespace detail

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_DETAIL_ACTION_UTILITY_H
