// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise_allow_block.h
/// \brief Apply the allow and block operators to summands.

#ifndef MCRL2_LPS_LINEARISE_ALLLOW_BLOCK_H
#define MCRL2_LPS_LINEARISE_ALLLOW_BLOCK_H

#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/lps/deadlock_summand.h"
#include "mcrl2/lps/stochastic_action_summand.h"
#include "mcrl2/lps/linearise_utility.h"
#include "mcrl2/process/process_expression.h"


namespace mcrl2
{

namespace lps
{

/**************** allow/block *************************************/

/// Determine if allow_action allows multi_action.
///
/// \param allow_action A sorted action_name_multiset a1|...|an
/// \param multi_action A multiaction b1(d1)|...|bm(dm)
/// \param termination_action The action that encodes successful termination (used only in debug mode)
/// \returns n == m and ai == bi for 1 <= i <= n
inline
bool allow_(const process::action_name_multiset& allow_action,
            const process::action_list& multi_action,
            const process::action& termination_action)
{
  /* The special cases where multiaction==tau and multiaction=={ Terminated } must have been
     dealt with separately. */
  assert(!multi_action.empty());
  assert(multi_action != process::action_list({ termination_action }));
  assert(std::is_sorted(allow_action.names().begin(), allow_action.names().end(), action_name_compare()));
  assert(std::is_sorted(multi_action.begin(), multi_action.end(), action_compare()));

  if (allow_action.size() != multi_action.size())
  {
    return false;
  }

  const core::identifier_string_list& names=allow_action.names();
  core::identifier_string_list::const_iterator names_it = names.begin();
  process::action_list::const_iterator multiaction_it = multi_action.begin();

  while (names_it != names.end())
  {
    assert(multiaction_it != multi_action.end());
    if (*names_it != multiaction_it->label().name())
    {
      return false;
    }
    ++names_it;
    ++multiaction_it;
  }

  return true;
}

/// \brief Determine if multi_action is allowed by an allow expression in allow_list
///
/// Calculates if the names of the action in multi_action match with an expression in allow_list.
/// If multi_action is the termination action, or multi_action is the empty multiaction, the result is also true.
inline
bool allow_(const process::action_name_multiset_list& allow_list,
            const process::action_list& multi_action,
            const process::action& termination_action)
{
  // The empty multiaction and the termination action can never be blocked by allows.
  if (multi_action.empty() || multi_action == process::action_list({ termination_action }))
  {
    return true;
  }

  for (const process::action_name_multiset& allow_action: allow_list)
  {
    if (allow_(allow_action, multi_action, termination_action))
    {
      return true;
    }
  }
  return false;
}

/// \brief Calculate if any of the actions in multiaction is blocked by encap_list.
///
/// \param encap_list is a list of action_name_multisets of size 1. Its single element contains all the blocked actions.
/// \param multi_action contains a multiaction a1(d1)|...|an(dn)
/// \returns true iff
inline
bool encap(const process::action_name_multiset_list& encaplist, const process::action_list& multiaction)
{
  assert(encaplist.size() == 1);
  assert(std::is_sorted(multiaction.begin(), multiaction.end(), action_compare()));

  const core::identifier_string_list& blocked_actions = encaplist.front().names();
  assert(std::is_sorted(blocked_actions.begin(), blocked_actions.end(), action_name_compare()));

  core::identifier_string_list::const_iterator blocked_actions_it = blocked_actions.begin();
  process::action_list::const_iterator multiaction_it = multiaction.begin();

  while (blocked_actions_it != blocked_actions.end() && multiaction_it != multiaction.end())
  {
    if (*blocked_actions_it == multiaction_it->label().name())
    {
      return true;
    }

    if (action_name_compare()(*blocked_actions_it, multiaction_it->label().name()))
    {
      ++blocked_actions_it;
    }
    else
    {
      // The following assertion should hold because blocked_actions_it is not less than or equal to the name
      // of multiaction_it
      assert(action_name_compare()(multiaction_it->label().name(), *blocked_actions_it));
      ++multiaction_it;
    }
  }

  return false;
}


void allowblockcomposition(
      const process::action_name_multiset_list& allowlist1,  // This is a list of list of identifierstring.
      const bool is_allow,
      stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const process::action& termination_action,
      bool ignore_time,
      bool nodeltaelimination)
    {
      /* This function calculates the allow or the block operator,
         depending on whether is_allow is true */

      stochastic_action_summand_vector sourcesumlist;
      action_summands.swap(sourcesumlist);

      deadlock_summand_vector resultdeltasumlist;
      deadlock_summand_vector resultsimpledeltasumlist;
      deadlock_summands.swap(resultdeltasumlist);

      process::action_name_multiset_list allowlist((is_allow)?sort_multi_action_labels(allowlist1):allowlist1);

      std::size_t sourcesumlist_length=sourcesumlist.size();
      if (sourcesumlist_length>2 || is_allow) // This condition prevents this message to be printed
        // when performing data elimination. In this case the
        // term delta is linearised, to determine which data
        // is essential for all processes. In these cases a
        // message about the block operator is very confusing.
      {
        mCRL2log(mcrl2::log::verbose) << "- calculating the " << (is_allow?"allow":"block") <<
              " operator on " << sourcesumlist.size() << " action summands and " << resultdeltasumlist.size() << " delta summands";
      }

      /* First add the resulting sums in two separate lists
         one for actions, and one for delta's. The delta's
         are added at the end to the actions, where for
         each delta summand it is determined whether it ought
         to be added, or is superseded by an action or another
         delta summand */
      for (const stochastic_action_summand& smmnd: sourcesumlist)
      {
        const data::variable_list& sumvars=smmnd.summation_variables();
        const process::action_list multiaction=smmnd.multi_action().actions();
        const data::data_expression& actiontime=smmnd.multi_action().time();
        const data::data_expression& condition=smmnd.condition();

        // Explicitly allow the termination action in any allow.
        if ((is_allow && allow_(allowlist,multiaction,termination_action)) ||
            (!is_allow && !encap(allowlist,multiaction)))
        {
          action_summands.push_back(smmnd);
        }
        else
        {
          if (smmnd.has_time())
          {
            resultdeltasumlist.push_back(deadlock_summand(sumvars, condition, deadlock(actiontime)));
          }
          else
          {
            // summand has no time.
            if (condition==data::sort_bool::true_())
            {
              resultsimpledeltasumlist.push_back(deadlock_summand(sumvars, condition, deadlock()));
            }
            else
            {
              resultdeltasumlist.push_back(deadlock_summand(sumvars, condition, deadlock()));
            }
          }
        }
      }

      if (nodeltaelimination)
      {
        deadlock_summands.swap(resultsimpledeltasumlist);
        copy(resultdeltasumlist.begin(),resultdeltasumlist.end(),back_inserter(deadlock_summands));
      }
      else
      {
        if (!ignore_time) /* if a delta summand is added, conditional, timed
                                   delta's are subsumed and do not need to be added */
        {
          for (const deadlock_summand& summand: resultsimpledeltasumlist)
          {
            insert_timed_delta_summand(action_summands,deadlock_summands,summand,ignore_time);
          }
          for (const deadlock_summand& summand: resultdeltasumlist)
          {
            insert_timed_delta_summand(action_summands,deadlock_summands,summand,ignore_time);
          }
        }
        else
        {
          // Add a true -> delta
          insert_timed_delta_summand(action_summands,deadlock_summands,deadlock_summand(data::variable_list(),data::sort_bool::true_(),deadlock()), ignore_time);
        }
      }
      if (mCRL2logEnabled(mcrl2::log::verbose) && (sourcesumlist_length>2 || is_allow))
      {
        mCRL2log(mcrl2::log::verbose) << ", resulting in " << action_summands.size() << " action summands and " << deadlock_summands.size() << " delta summands\n";
      }
    }

} // namespace lps

} // namespace mcrl2



#endif // MCRL2_LPS_LINEARISE_ALLLOW_BLOCK_H
