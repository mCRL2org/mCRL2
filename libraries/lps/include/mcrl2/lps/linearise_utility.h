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

inline
bool action_name_compare(const core::identifier_string& s1, const core::identifier_string& s2)
{
  return std::string(s1) < std::string(s2);
}

/// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
inline
bool action_label_compare(const process::action_label& a1, const process::action_label& a2)
{
  /* first compare the strings in the actions */
  const core::identifier_string a1_name = a1.name();
  const core::identifier_string a2_name = a2.name();

  if (action_name_compare(a1_name, a2_name))
  {
    return true;
  }

  if (a1_name == a2_name)
  {
    /* the strings are equal; the sorts are used to
       determine the ordering */
    return a1.sorts() < a2.sorts();
  }

  return false;
}

/// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
///
/// \returns true iff the label of a1 is less than the label of a2 (w.r.t. action_label_compare), or the labels are equal and the arguments of a1 are less than the arguments of a2.
/// for the latter, we use the standard < comparison.
inline
bool action_compare(const process::action& a1, const process::action& a2)
{
  const process::action_label a1_label = a1.label();
  const process::action_label a2_label = a2.label();

  if (action_label_compare(a1_label, a2_label))
  {
    return true;
  };

  if  (a1_label == a2_label)
  {
    return a1.arguments() < a2.arguments();
  }

  return false;
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

/// insert an action name into the list, while preserving the sorting of action names.
inline
core::identifier_string_list insert(
  const core::identifier_string& s,
  core::identifier_string_list l)
{
  if (l.empty())
  {
    return core::identifier_string_list({ s });
  }
  const core::identifier_string& head = l.front();

  if (action_name_compare(s, head))
  {
    l.push_front(s);
    return l;
  }

  core::identifier_string_list result = insert(s, l.tail());
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

/// Sort the left-hand sides of the communication expressions in communications
///
/// Sorting is done using sort_action_labels, so by default, the left-hand sides of the communications are sorted by names of the actions.
inline
process::communication_expression_list sort_communications(const process::communication_expression_list& communications)
{
  process::communication_expression_list result;

  for (const process::communication_expression& comm: communications)
  {
    result.push_front(process::communication_expression(sort_action_labels(comm.action_name()),comm.name()));
  }

  return result;
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

/// Determine if action summand as subsumes delta summand ds.
///
/// The action summand subsumes the deadlock summand if its condition is implied by that of the deadlock summand,
/// and either the action summand is not timed, or the timestamp of the deadlock summand and the action summand coincide.
inline
bool subsumes(const stochastic_action_summand& as, const deadlock_summand& ds)
{
  return (!as.multi_action().has_time() || ds.deadlock().time() == as.multi_action().time())
      && implies_condition(ds.condition(), as.condition());
}

/// Determine if delta summand ds1 subsumes delta summand ds2.
inline
bool subsumes(const deadlock_summand& ds1, const deadlock_summand& ds2)
{
  return (!ds1.deadlock().has_time() || ds2.deadlock().time() == ds1.deadlock().time())
      && implies_condition(ds2.condition(), ds1.condition());
}


inline
void insert_timed_delta_summand(
      const stochastic_action_summand_vector& action_summands,
      deadlock_summand_vector& deadlock_summands,
      const deadlock_summand& s,
      const bool ignore_time)
{
  if (ignore_time)
  {
    deadlock_summands.push_back(s);
    return;
  }

  assert(!ignore_time);

  // First check whether the delta summand is subsumed by an action summand.
  if (std::any_of(action_summands.begin(), action_summands.end(),
    [&s](const stochastic_action_summand& as) { return subsumes(as, s); }))
  {
    return;
  }

  deadlock_summand_vector result;

  for (deadlock_summand_vector::iterator i=deadlock_summands.begin(); i!=deadlock_summands.end(); ++i)
  {
    if (subsumes(*i, s))
    {
      /* put the summand that was effective in removing
         this delta summand to the front, such that it
         is encountered early later on, removing a next
         delta summand */

      copy(i,deadlock_summands.end(),back_inserter(result));
      deadlock_summands.swap(result);
      return;
    }
    if (!subsumes(s, *i))
    {
      result.push_back(*i);
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
