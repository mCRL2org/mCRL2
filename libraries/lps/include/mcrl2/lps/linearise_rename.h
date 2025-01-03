// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise_rename.h
/// \brief Apply the rename operator to action summands.

#ifndef MCRL2_LPS_LINEARISE_RENAME_H
#define MCRL2_LPS_LINEARISE_RENAME_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/lps/stochastic_action_summand.h"
#include "mcrl2/lps/linearise_utility.h"
#include "mcrl2/process/process_expression.h"


namespace mcrl2
{

namespace lps
{

  /// Apply renamings to a single action
  inline
  process::action rename(const process::rename_expression_list& renamings, const process::action& action)
  {
    const process::action_label& action_label = action.label();
    const core::identifier_string& action_name = action_label.name();
    for (const process::rename_expression& renaming: renamings)
    {
      if (action_name == renaming.source())
      {
        return process::action(process::action_label(renaming.target(), action_label.sorts()), action.arguments());
      }
    }
    return action;
  }

  inline
  process::action_list rename(const process::rename_expression_list& renamings,
                              const process::action_list& actions)
  {
    process::action_list result;

    for (const process::action& a: actions)
    {
      result.push_front(rename(renamings, a));
    }

    result = atermpp::sort_list(result, std::function<bool(const process::action&, const process::action&)>(action_compare));
    return result;
  }

  inline
  multi_action rename(const process::rename_expression_list& renamings,
                      const multi_action& multi_action)
  {
    return lps::multi_action(rename(renamings, multi_action.actions()), multi_action.time());
  }

  inline
  void rename(
    const process::rename_expression_list& renamings,
    lps::stochastic_action_summand_vector& action_summands)
  {
      for (lps::stochastic_action_summand_vector::iterator i=action_summands.begin(); i!=action_summands.end(); ++i)
      {
          *i = lps::stochastic_action_summand(i->summation_variables(),
                             i->condition(),
                             rename(renamings, i->multi_action()),
                             i->assignments(),
                             i->distribution());
      }
  }

} // namespace lps

} // namespace mcrl2



#endif // MCRL2_LPS_LINEARISE_RENAME_H
