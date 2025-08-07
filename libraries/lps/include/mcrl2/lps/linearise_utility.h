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
#include "mcrl2/lps/detail/configuration.h"
#include "mcrl2/process/process_expression.h"
#include "mcrl2/lps/deadlock_summand.h"
#include "mcrl2/lps/stochastic_action_summand.h"

#include <optional>

namespace mcrl2::lps
{

/// Data structure to store the statistics about summands in a linear process
struct lps_statistics_t
{
  size_t action_summand_count = 0;
  std::optional<size_t> deadlock_summand_count;
  size_t total_action_count = 0; // The sum of the number of actions in all the multiactions in the action summands.
};

/// Print statistics of lps as indented block in YAML format
inline
std::string print(const lps_statistics_t& stats, std::size_t indent = 0)
{
  std::stringstream ss;
  std::string indent_str = std::string(indent, ' ');

  ss << indent_str << "action_summand_count: " << stats.action_summand_count << std::endl;
  ss << indent_str << "deadlock_summand_count: " << (stats.deadlock_summand_count.has_value()?std::to_string(*stats.deadlock_summand_count):"n/a") << std::endl;
  ss << indent_str << "total_action_count: " << stats.total_action_count << std::endl;
  if (stats.action_summand_count > 0)
  {
    ss << indent_str << "average multiaction size (total_action_count / action_summand_count): "
       << static_cast<double>(stats.total_action_count) / static_cast<double>(stats.action_summand_count) << std::endl;
  }
  return ss.str();
}

/// Utility function to calculate the number of summands and the sizes of multiactions in those summands for printing
/// statistics
inline
lps_statistics_t get_statistics(const stochastic_action_summand_vector& action_summands)
{
  lps_statistics_t statistics;
  if constexpr (detail::EnableLineariseStatistics)
  {
    statistics.action_summand_count = action_summands.size();

    for (const action_summand& s: action_summands)
    {
      statistics.total_action_count += s.multi_action().actions().size();
    }
  }

  return statistics;
}

/// Get statistics for action and deadlock summands
inline
lps_statistics_t get_statistics(const stochastic_action_summand_vector& action_summands, const deadlock_summand_vector& deadlock_summands)
{
  lps_statistics_t statistics = get_statistics(action_summands);
  statistics.deadlock_summand_count = deadlock_summands.size();

  return statistics;
}

struct action_name_compare
{
  bool operator()(const core::identifier_string& s1, const core::identifier_string& s2) const
  {
    return std::string(s1) < std::string(s2);
  }
};

/// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
struct action_label_compare
{

  bool operator()(const process::action_label& a1, const process::action_label& a2) const
  {
    /* first compare the strings in the actions */
    const core::identifier_string& a1_name = a1.name();
    const core::identifier_string& a2_name = a2.name();

    return action_name_compare()(a1_name, a2_name) ||
                       (a1_name == a2_name && a1.sorts() < a2.sorts());
  }
};


/// Determine if a1 < a2; the key requirement is that orderings of action labels and the actions in multiactions are
/// consistent.
///
/// \returns true iff the label of a1 is less than the label of a2.
/// The arguments are ignored in this comparison.
/// The sort order is used for efficient application of process operators such as allow and comm
/// which are defined in terms of  action names.
struct action_compare
{
  bool operator()(const process::action& a1, const process::action& a2) const
  {
    const process::action_label& a1_label = a1.label();
    const process::action_label& a2_label = a2.label();

    return action_label_compare()(a1_label, a2_label);
  }
};

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

  if (action_compare()(act, head))
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

  if (action_name_compare()(s, head))
  {
    l.push_front(s);
    return l;
  }

  core::identifier_string_list result = insert(s, l.tail());
  result.push_front(head);
  return result;
}

inline
process::action_name_multiset sort_action_names(const process::action_name_multiset& action_labels,
  const std::function<bool(const core::identifier_string&, const core::identifier_string&)>& cmp
                                    = [](const core::identifier_string& t1, const core::identifier_string& t2){ return action_name_compare()(t1, t2);})
{
  return process::action_name_multiset(atermpp::sort_list(action_labels.names(), cmp));
}

inline
process::action_name_multiset_list sort_multi_action_labels(const process::action_name_multiset_list& l)
{
  return process::action_name_multiset_list(l.begin(),l.end(),[](const process::action_name_multiset& al){ return sort_action_names(al); });
}

inline
process::action_list sort_actions(const process::action_list& actions,
  const std::function<bool(const process::action&, const process::action&)>& cmp
                                    = [](const process::action& t1, const process::action& t2){ return action_compare()(t1, t2);})
{
  return process::action_list(atermpp::sort_list(actions, cmp));
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
    result.push_front(process::communication_expression(sort_action_names(comm.action_name()),comm.name()));
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

} // namespace mcrl2::lps





#endif // MCRL2_LPS_LINEARISE_RENAME_H
