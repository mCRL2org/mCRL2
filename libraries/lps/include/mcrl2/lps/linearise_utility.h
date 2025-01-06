// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise_utility.h
/// \brief Utilities used in linearisation. Mainly for actions and multiactions.

#ifndef MCRL2_LPS_LINEARISE_UTILITY_H
#define MCRL2_LPS_LINEARISE_UTILITY_H

#include "mcrl2/data/data_expression.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/lps/deadlock_summand.h"
#include "mcrl2/lps/stochastic_action_summand.h"


namespace mcrl2
{

namespace lps
{

/// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
inline
bool action_label_compare(const process::action_label& a1, const process::action_label& a2)
{
  /* first compare the strings in the actions */
  if (std::string(a1.name())<std::string(a2.name()))
  {
    return true;
  }

  if (a1.name()==a2.name())
  {
    /* the strings are equal; the sorts are used to
       determine the ordering */
    return a1.sorts() < a2.sorts();
  }

  return false;
}

/// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
inline
bool action_compare(const process::action& a1, const process::action& a2)
{
  return action_label_compare(a1.label(), a2.label());
}

/// Insert action into an action_list, keeping the action list sorted w.r.t. action_compare.
/// Complexity: O(n) for an action_list of length n.
inline
process::action_list insert(
  const process::action& act,
  process::action_list l)
{
  if (l.empty())
  {
    return process::action_list({ act });
  }
  const process::action& head = l.front();

  if (action_compare(act, head))
  {
    l.push_front(act);
    return l;
  }

  process::action_list result = insert(act, l.tail());
  result.push_front(head);
  return result;
}

inline
process::action_name_multiset sort_action_labels(const process::action_name_multiset& action_labels,
  const std::function<bool(const core::identifier_string&, const core::identifier_string&)>& cmp
                                    = [](const core::identifier_string& t1, const core::identifier_string& t2){ return std::string(t1)<std::string(t2);})
{
  return process::action_name_multiset(atermpp::sort_list(action_labels.names(), cmp));
}

inline
process::action_name_multiset_list sort_multi_action_labels(const process::action_name_multiset_list& l)
{
  return process::action_name_multiset_list(l.begin(),l.end(),[](const process::action_name_multiset& al){ return sort_action_labels(al); });
}

inline
bool implies_condition(const data::data_expression& c1, const data::data_expression& c2)
{
  if (c2==data::sort_bool::true_())
  {
    return true;
  }

  if (c1==data::sort_bool::false_())
  {
    return true;
  }

  if (c1==data::sort_bool::true_())
  {
    return false;
  }

  if (c2==data::sort_bool::false_())
  {
    return false;
  }

  if (c1==c2)
  {
    return true;
  }

  /* Dealing with the conjunctions (&&) first and then the disjunctions (||)
     yields a 10-fold speed increase compared to the case where first the
     || occur, and then the &&. This result was measured on the alternating
     bit protocol, with --regular. */

  if (data::sort_bool::is_and_application(c2))
  {
    return implies_condition(c1,data::binary_left(atermpp::down_cast<data::application>(c2))) &&
           implies_condition(c1,data::binary_right(atermpp::down_cast<data::application>(c2)));
  }

  if (data::sort_bool::is_or_application(c1))
  {
    return implies_condition(data::binary_left(atermpp::down_cast<data::application>(c1)),c2) &&
           implies_condition(data::binary_right(atermpp::down_cast<data::application>(c1)),c2);
  }

  if (data::sort_bool::is_and_application(c1))
  {
    return implies_condition(data::binary_left(atermpp::down_cast<data::application>(c1)),c2) ||
           implies_condition(data::binary_right(atermpp::down_cast<data::application>(c1)),c2);
  }

  if (data::sort_bool::is_or_application(c2))
  {
    return implies_condition(c1,data::binary_left(atermpp::down_cast<data::application>(c2))) ||
           implies_condition(c1,data::binary_right(atermpp::down_cast<data::application>(c2)));
  }

  return false;
}

inline
void insert_timed_delta_summand(
      const stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const deadlock_summand& s,
      bool ignore_time)
{
  deadlock_summand_vector result;

  // const variable_list sumvars=s.summation_variables();
  const data::data_expression& cond=s.condition();
  const data::data_expression& actiontime=s.deadlock().time();

  // First check whether the delta summand is subsumed by an action summands.
  if (!ignore_time)
  {
    for (const stochastic_action_summand& as: action_summands)
    {
      const data::data_expression& cond1=as.condition();
      if (((actiontime==as.multi_action().time()) || (!as.multi_action().has_time())) &&
          (implies_condition(cond,cond1)))
      {
        /* De delta summand is subsumed by action summand as. So, it does not
           have to be added. */

        return;
      }
    }
  }

  for (deadlock_summand_vector::iterator i=deadlock_summands.begin(); i!=deadlock_summands.end(); ++i)
  {
    const deadlock_summand& smmnd=*i;
    const data::data_expression& cond1=i->condition();
    if ((!ignore_time) &&
        ((actiontime==i->deadlock().time()) || (!i->deadlock().has_time())) &&
        (implies_condition(cond,cond1)))
    {
      /* put the summand that was effective in removing
         this delta summand to the front, such that it
         is encountered early later on, removing a next
         delta summand */

      copy(i,deadlock_summands.end(),back_inserter(result));
      deadlock_summands.swap(result);
      return;
    }
    if (((ignore_time)||
         (((actiontime==smmnd.deadlock().time())|| (!s.deadlock().has_time())) &&
          (implies_condition(cond1,cond)))))
    {
      /* do not add summand to result, as it is superseded by s */
    }
    else
    {
      result.push_back(smmnd);
    }
  }

  result.push_back(s);
  deadlock_summands.swap(result);
}

inline
bool occursinterm(const data::data_expression& t, const data::variable& var)
{
  return data::search_free_variable(t, var);
}

} // namespace lps

} // namespace mcrl2



#endif // MCRL2_LPS_LINEARISE_RENAME_H
