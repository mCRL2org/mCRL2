// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SPLIT_ACTION_H_
#define MCRL2_SPLIT_ACTION_H_

#include "mcrl2/data/data_expression.h"

using namespace mcrl2;

std::pair<process::action_list, process::action_list> split_action(
  const process::action_list& actions,
  bool is_left_update_trivial,
  bool is_right_update_trivial,
  std::size_t index,
  const data::variable_list& left_parameters,
  const data::variable_list& right_parameters,
  std::list<std::size_t>& indices)
{
  process::action_list left_action;
  process::action_list right_action;

  for (const process::action& action : actions)
  {
    auto dependencies = data::find_free_variables(action.arguments());

    // If this action belongs to one process keep it there.
    if (is_subset(dependencies, left_parameters))
    {
      left_action.push_front(action);
    }
    else if (is_subset(dependencies, right_parameters))
    {
      right_action.push_front(action);
    }
    else
    {
      // If we can obtain an independent summand keep it in the non-trivial component
      if (is_right_update_trivial)
      {
        left_action.push_front(action);
      }
      else if (is_left_update_trivial)
      {
        right_action.push_front(action);
      }
      else
      {
        // Use the user-defined indices to indicate the choice.
        bool generate_left = (std::find(indices.begin(), indices.end(), index) != indices.end());

        if (generate_left)
        {
          left_action.push_front(action);
        }
        else
        {
          right_action.push_front(action);
        }
      }
    }
  }

  return std::make_pair(left_action, right_action);
}

#endif // MCRL2_SPLIT_ACTION_H_