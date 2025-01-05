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

inline
bool allowsingleaction(const process::action_name_multiset& allowaction,
                       const process::action_list& multiaction)
{
  /* The special cases where multiaction==tau and multiaction=={ Terminated } must have been
     dealt with separately. */
  assert(multiaction.size()!=0 && multiaction != action_list({ terminationAction }));

  const core::identifier_string_list& names=allowaction.names();
  core::identifier_string_list::const_iterator i=names.begin();

  for (process::action_list::const_iterator walker=multiaction.begin();
       walker!=multiaction.end(); ++walker,++i)
  {
    if (i==names.end())
    {
      return false;
    }
    if (*i!=walker->label().name())
    {
      return false;
    }
  }
  return i==names.end();
}

/// \brief determine whether the multiaction has the same labels as the allow action,
//         in which case true is delivered. If multiaction is the action Terminate,
//         then true is also returned.
inline
bool allow_(const process::action_name_multiset_list& allowlist,
            const process::action_list& multiaction,
            const process::action termination_action)
{
  /* The empty multiaction, i.e. tau, is never blocked by allow */
  if (multiaction.empty())
  {
    return true;
  }

  /* The multiaction is equal to the special Terminate action. This action cannot be blocked. */
  if (multiaction == process::action_list({ termination_action }))
  {
    return true;
  }

  for (process::action_name_multiset_list::const_iterator i=allowlist.begin();
       i!=allowlist.end(); ++i)
  {
    if (allowsingleaction(*i,multiaction))
    {
      return true;
    }
  }
  return false;
}

inline
bool encap(const process::action_name_multiset_list& encaplist, const process::action_list& multiaction)
{
  for (const process::action& a: multiaction)
  {
    assert(encaplist.size()==1);
    for (const core::identifier_string& s1: encaplist.front().names())
    {
      const core::identifier_string s2=a.label().name();
      if (s1==s2)
      {
        return true;
      }
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
