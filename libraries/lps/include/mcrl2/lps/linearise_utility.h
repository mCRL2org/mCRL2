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

#include "mcrl2/data/bool.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/real.h"
#include "mcrl2/lps/deadlock_summand.h"
#include "mcrl2/lps/detail/configuration.h"
#include "mcrl2/lps/stochastic_action_summand.h"
#include "mcrl2/process/builder.h"
#include "mcrl2/process/process_expression.h"

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


inline
process::action_list sort_actions(const process::action_list& actions)
{
  return process::action_list(atermpp::sort_list(actions));
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

/// \brief The process delta@0, which the lineariser uses for a process that cannot do anything.
inline process::process_expression delta_at_zero()
{
  return process::at(process::delta(), data::sort_real::real_(0));
}

namespace detail
{

template<typename ConditionRewriter>
struct simplify_pcrl_builder : public process::process_expression_builder<simplify_pcrl_builder<ConditionRewriter>>
{
  using super = process::process_expression_builder<simplify_pcrl_builder<ConditionRewriter>>;
  using super::apply;
  using super::enter;
  using super::leave;
  using super::update;

  ConditionRewriter R;

  explicit simplify_pcrl_builder(ConditionRewriter R_)
    : R(R_)
  {}

  template<class T>
  void apply(T& result, const process::choice& x)
  {
    process::process_expression left;
    apply(left, x.left());
    if (left == delta_at_zero())
    {
      apply(result, x.right());
      return;
    }
    process::process_expression right;
    apply(right, x.right());
    if (right == delta_at_zero())
    {
      result = left;
      return;
    }
    process::make_choice(result, left, right);
  }

  template<class T>
  void apply(T& result, const process::seq& x)
  {
    process::process_expression left;
    apply(left, x.left());
    if (left == delta_at_zero())
    {
      result = left;
      return;
    }
    process::process_expression right;
    apply(right, x.right());
    process::make_seq(result, left, right);
  }

  template<class T>
  void apply(T& result, const process::if_then& x)
  {
    const data::data_expression condition = R(x.condition());
    if (condition == data::sort_bool::false_())
    {
      result = delta_at_zero();
      return;
    }
    if (condition == data::sort_bool::true_())
    {
      apply(result, x.then_case());
      return;
    }
    process::process_expression then_case;
    apply(then_case, x.then_case());
    process::make_if_then(result, condition, then_case);
  }

  template<class T>
  void apply(T& result, const process::if_then_else& x)
  {
    const data::data_expression condition = R(x.condition());
    if (condition == data::sort_bool::false_())
    {
      apply(result, x.else_case());
      return;
    }
    if (condition == data::sort_bool::true_())
    {
      apply(result, x.then_case());
      return;
    }
    process::process_expression then_case;
    apply(then_case, x.then_case());
    process::process_expression else_case;
    apply(else_case, x.else_case());
    process::make_if_then_else(result, condition, then_case, else_case);
  }
};

} // namespace detail

/// \brief Simplifies a pCRL process as the lineariser does after a substitution.
/// \details The conditions are rewritten with R. A condition that becomes true is removed, a condition that becomes
///          false replaces the process by delta@0 (or by the else case). A delta@0 is removed from a choice, and a
///          sequence that starts with delta@0 is replaced by delta@0. Other data expressions are not rewritten.
/// \param p A pCRL process.
/// \param R A function that rewrites a condition, for instance a data rewriter, or the identity.
template<typename ConditionRewriter>
process::process_expression simplify_pcrl(const process::process_expression& p, ConditionRewriter R)
{
  process::process_expression result;
  detail::simplify_pcrl_builder<ConditionRewriter>(R).apply(result, p);
  return result;
}

} // namespace mcrl2::lps





#endif // MCRL2_LPS_LINEARISE_UTILITY_H
