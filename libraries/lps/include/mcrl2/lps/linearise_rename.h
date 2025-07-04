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

#include "mcrl2/lps/stochastic_action_summand.h"
#include "mcrl2/lps/linearise_utility.h"
#include "mcrl2/lps/detail/configuration.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2::lps
{

  inline
  std::string log_rename_application(const lps_statistics_t& lps_statistics_before,
                                   const lps_statistics_t& lps_statistics_after,
                                   const std::size_t num_rename_expressions,
                                   size_t indent = 0)
  {
    std::string indent_str(indent, ' ');
    std::ostringstream os;

    os << indent_str << "- operator: rename" << std::endl;

    indent += 2;
    indent_str = std::string(indent, ' ');
    os << indent_str << "number of rename expressions: " << num_rename_expressions << std::endl
       << indent_str << "before:" << std::endl << print(lps_statistics_before, indent+2)
       << indent_str << "after:" << std::endl << print(lps_statistics_after, indent+2);

    return os.str();
  }

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

    result = sort_actions(result);
    return result;
  }

  inline
  multi_action rename(const process::rename_expression_list& renamings,
                      const multi_action& multi_action)
  {
    return lps::multi_action(rename(renamings, multi_action.actions()), multi_action.time());
  }

  inline
  stochastic_action_summand rename(const process::rename_expression_list& renamings,
    const stochastic_action_summand& summand)
  {
    return stochastic_action_summand(summand.summation_variables(),
                             summand.condition(),
                             rename(renamings, summand.multi_action()),
                             summand.assignments(),
                             summand.distribution());
  }


  inline
  void rename(
    const process::rename_expression_list& renamings,
    lps::stochastic_action_summand_vector& action_summands)
  {
    [[maybe_unused]]    
    lps_statistics_t lps_statistics_before = get_statistics(action_summands);

    for (stochastic_action_summand& action_summand: action_summands)
    {
      action_summand = rename(renamings, action_summand);
    }

    if constexpr (detail::EnableLineariseStatistics)
    {
      lps_statistics_t lps_statistics_after = get_statistics(action_summands);
      std::cout << log_rename_application(lps_statistics_before, lps_statistics_after, renamings.size());
    }
  }

  } // namespace mcrl2::lps

#endif // MCRL2_LPS_LINEARISE_RENAME_H
