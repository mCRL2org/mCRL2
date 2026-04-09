// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise_hide.h
/// \brief Apply the hide operator to action summands.

#ifndef MCRL2_LPS_LINEARISE_HIDE_H
#define MCRL2_LPS_LINEARISE_HIDE_H

#include "mcrl2/lps/stochastic_action_summand.h"
#include "mcrl2/lps/linearise_utility.h"
#include "mcrl2/lps/detail/configuration.h"
#include "mcrl2/process/process_expression.h"

namespace mcrl2::lps
{

inline
std::string log_hide_application(const lps_statistics_t& lps_statistics_before,
  const lps_statistics_t& lps_statistics_after,
  const std::size_t num_hidden_actions,
  size_t indent = 0)
{
  std::string indent_str(indent, ' ');
  std::ostringstream os;

  os << indent_str << "- operator: hide" << std::endl;

  indent += 2;
  indent_str = std::string(indent, ' ');
  os << indent_str << "number of hidden actions: " << num_hidden_actions << std::endl
     << indent_str << "before:" << std::endl
     << print(lps_statistics_before, indent + 2) << indent_str << "after:" << std::endl
     << print(lps_statistics_after, indent + 2);

  return os.str();
}

inline
process::action_list hide_(const core::identifier_string_list& hidelist, const process::action_list& multiaction)
{
  process::action_list resultactionlist;

  for (const process::action& a: multiaction)
  {
    if (std::find(hidelist.begin(), hidelist.end(), a.label().name()) == hidelist.end())
    {
      resultactionlist.push_front(a);
    }
  }

  /* reverse the actionlist to maintain the ordering */
  resultactionlist = reverse(resultactionlist);
  return resultactionlist;
}

inline
void hidecomposition(const core::identifier_string_list& hidelist, stochastic_action_summand_vector& action_summands)
{
  [[maybe_unused]]
  lps_statistics_t lps_statistics_before = get_statistics(action_summands);

  for (auto& action_summand: action_summands)
  {
    const process::action_list acts = hide_(hidelist, action_summand.multi_action().actions());
    action_summand = stochastic_action_summand(action_summand.summation_variables(),
      action_summand.condition(),
      action_summand.has_time() ? multi_action(acts, action_summand.multi_action().time()) : multi_action(acts),
      action_summand.assignments(),
      action_summand.distribution());
  }

  if constexpr (detail::EnableLineariseStatistics)
  {
    lps_statistics_t lps_statistics_after = get_statistics(action_summands);
    std::cout << log_hide_application(lps_statistics_before, lps_statistics_after, hidelist.size());
  }
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_LINEARISE_HIDE_H
