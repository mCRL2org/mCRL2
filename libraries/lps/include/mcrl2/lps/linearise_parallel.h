// Author(s): Jan Friso Groote, Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/linearise_parallel.h
/// \brief The parallel composition of linear processes, as used by the lineariser.

#ifndef MCRL2_LPS_LINEARISE_PARALLEL_H
#define MCRL2_LPS_LINEARISE_PARALLEL_H

#include <optional>

#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/real_utilities.h"
#include "mcrl2/data/replace_capture_avoiding_with_an_identifier_generator.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/substitutions/maintain_variables_in_rhs.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/lps/deadlock_summand.h"
#include "mcrl2/lps/detail/ultimate_delay.h"
#include "mcrl2/lps/linearise_allow_block.h"
#include "mcrl2/lps/linearise_utility.h"
#include "mcrl2/lps/stochastic_action_summand.h"
#include "mcrl2/utilities/detail/container_utility.h"

namespace mcrl2::lps
{

namespace detail
{

/// \brief Restricts a summand of one process in the left merge to the times at which the other process can
///        still wait, given by its ultimate delay.
/// \details The variables of the ultimate delay become sum variables, renamed away from the sum variables of
///          the summand. A timed summand gets the ultimate delay at its time as an extra condition. An
///          untimed summand gets the time variable of the ultimate delay as a new sum variable and as its time,
///          and the ultimate delay as an extra condition, unless the ultimate delay is true. The resulting
///          condition is rewritten.
/// \param delay The ultimate delay of the other process.
/// \param sumvars The sum variables of the summand, extended.
/// \param condition The condition of the summand, extended.
/// \param time The time of the summand, data::undefined_real() if it has none (as in multi_action and
///             deadlock); set if the summand gets a time.
/// \param id_generator The generator of fresh names.
/// \param rewrite A function that rewrites a data expression.
template<typename Rewriter>
void add_ultimate_delay(const ultimate_delay& delay,
  data::variable_list& sumvars,
  data::data_expression& condition,
  data::data_expression& time,
  data::set_identifier_generator& id_generator,
  Rewriter rewrite)
{
  const data::variable_list summand_variables = sumvars;
  data::maintain_variables_in_rhs<data::mutable_map_substitution<>> sigma;
  const data::variable_list delay_variables = rename_variables_away(
    delay.variables(),
    [&](const data::variable& v) { return utilities::detail::contains(summand_variables, v); },
    sigma,
    [&](const data::variable& v) { return data::variable(id_generator(v.name()), v.sort()); });
  sumvars = summand_variables + delay_variables;

  if (time == data::undefined_real())
  {
    if (delay.constraint() == data::sort_bool::true_())
    {
      condition = rewrite(condition);
      return;
    }
    time = delay.time_var();
    sumvars.push_front(delay.time_var());
  }
  else
  {
    // Substitute the time of the summand for the time variable of the ultimate delay.
    sigma[delay.time_var()] = time;
  }
  data::make_optimized_and(condition,
    condition,
    data::replace_variables_capture_avoiding_with_an_identifier_generator(delay.constraint(), sigma, id_generator));
  condition = rewrite(condition);
}

} // namespace detail

/// \brief The summand of the left merge for an action summand of one process, with another process whose ultimate
///        delay is given: the summand can only happen at a time at which the other process can still wait.
/// \details The result may have condition false. The caller decides which summands take part in the left merge
///          (not the termination action, and only those kept by an inline allow or block).
/// \param summand An action summand of the first process.
/// \param delay The ultimate delay of the second process.
/// \param id_generator The generator of fresh names, which must contain the identifiers of both processes.
/// \param rewrite A function that rewrites a data expression.
template<typename Rewriter>
stochastic_action_summand left_merge_summand(const stochastic_action_summand& summand,
  const detail::ultimate_delay& delay,
  data::set_identifier_generator& id_generator,
  Rewriter rewrite)
{
  data::variable_list sumvars = summand.summation_variables();
  data::data_expression condition = summand.condition();
  data::data_expression time = summand.multi_action().time();
  detail::add_ultimate_delay(delay, sumvars, condition, time, id_generator, rewrite);
  return stochastic_action_summand(sumvars,
    condition,
    multi_action(summand.multi_action().actions(), time),
    summand.assignments(),
    summand.distribution());
}

/// \brief The summand of the left merge for a deadlock summand of one process, with another process whose ultimate
///        delay is given: the deadlock can only happen at a time at which the other process can still wait.
/// \details The result may have condition false.
/// \param summand A deadlock summand of the first process.
/// \param delay The ultimate delay of the second process.
/// \param id_generator The generator of fresh names, which must contain the identifiers of both processes.
/// \param rewrite A function that rewrites a data expression.
template<typename Rewriter>
deadlock_summand left_merge_summand(const deadlock_summand& summand,
  const detail::ultimate_delay& delay,
  data::set_identifier_generator& id_generator,
  Rewriter rewrite)
{
  data::variable_list sumvars = summand.summation_variables();
  data::data_expression condition = summand.condition();
  data::data_expression time = summand.deadlock().time();
  detail::add_ultimate_delay(delay, sumvars, condition, time, id_generator, rewrite);
  return deadlock_summand(sumvars, condition, deadlock(time));
}

/// \brief Adds the summands of the left merge of a first process with a second process to action_summands and
///        deadlock_summands.
/// \details Each summand of the first process is restricted to the times at which the second process can still
///          wait (left_merge_summand); summands with condition false are left out. The termination action does
///          not take part in the left merge, and filter applies an inline allow or block. If time is ignored, the
///          ultimate delay is not used and deadlock summands are left out; inline allow and block are only used
///          if time is ignored. Otherwise a deadlock summand is only added if no action or deadlock summand
///          subsumes it. The deadlock summands are added before the action summands.
/// \param action_summands1 The action summands of the first process.
/// \param deadlock_summands1 The deadlock summands of the first process.
/// \param delay2 The ultimate delay of the second process.
/// \param filter The inline allow or block, if any.
/// \param termination_action The termination action of the lineariser.
/// \param ignore_time Whether time is ignored.
/// \param id_generator The generator of fresh names, which must contain the identifiers of both processes.
/// \param rewrite A function that rewrites a data expression.
/// \param action_summands The action summands to which the result is added.
/// \param deadlock_summands The deadlock summands to which the result is added.
template<typename Rewriter>
void left_merge(const stochastic_action_summand_vector& action_summands1,
  const deadlock_summand_vector& deadlock_summands1,
  const detail::ultimate_delay& delay2,
  const allow_block_filter& filter,
  const process::action& termination_action,
  bool ignore_time,
  data::set_identifier_generator& id_generator,
  Rewriter rewrite,
  stochastic_action_summand_vector& action_summands,
  deadlock_summand_vector& deadlock_summands)
{
  // Without time the ultimate delay is never computed, so it has no variables to add to the summands.
  assert(!ignore_time || delay2.variables().empty());
  assert(ignore_time || (!filter.is_allow() && !filter.is_block()));

  if (!ignore_time)
  {
    for (const deadlock_summand& summand1: deadlock_summands1)
    {
      const deadlock_summand summand = left_merge_summand(summand1, delay2, id_generator, rewrite);
      if (summand.condition() != data::sort_bool::false_())
      {
        insert_timed_delta_summand(action_summands, deadlock_summands, summand, ignore_time);
      }
    }
  }

  const process::action_list termination({termination_action});
  for (const stochastic_action_summand& summand1: action_summands1)
  {
    if (summand1.multi_action().actions() == termination || !filter.keeps(summand1.multi_action()))
    {
      continue;
    }

    // Without time the ultimate delay is not computed (its constraint is false), so the summand stays as it is.
    stochastic_action_summand summand
      = ignore_time ? summand1 : left_merge_summand(summand1, delay2, id_generator, rewrite);
    if (summand.condition() != data::sort_bool::false_())
    {
      action_summands.push_back(std::move(summand));
    }
  }
}

namespace detail
{

inline const data::data_expression& summand_time(const stochastic_action_summand& summand)
{
  return summand.multi_action().time();
}

inline const data::data_expression& summand_time(const deadlock_summand& summand)
{
  return summand.deadlock().time();
}

} // namespace detail

/// \brief The actions of the combination of two multi-actions in the communication merge.
/// \details The termination action only combines with the termination action; the result is empty if exactly one
///          of the multi-actions is the termination action. Otherwise it is the (sorted) union of both.
/// \param actions1 The sorted actions of a multi-action of the first process.
/// \param actions2 The sorted actions of a multi-action of the second process.
/// \param termination_action The termination action of the lineariser.
inline std::optional<process::action_list> communication_merge_actions(const process::action_list& actions1,
  const process::action_list& actions2,
  const process::action& termination_action)
{
  const process::action_list termination({termination_action});
  const bool terminates1 = actions1 == termination;
  if (terminates1 != (actions2 == termination))
  {
    return std::nullopt;
  }
  return terminates1 ? termination : atermpp::merge_lists(actions1, actions2);
}

/// \brief The combination of two multi-actions in the communication merge.
/// \details The actions are combined by communication_merge_actions; the result is empty if they do not combine.
///          The time is that of the first multi-action if it has one, and otherwise that of the second (if any).
///          That both times are equal if both multi-actions have a time is a condition of the combined summand
///          (communication_merge_summand).
/// \param multi_action1 A multi-action of the first process.
/// \param multi_action2 A multi-action of the second process.
/// \param termination_action The termination action of the lineariser.
inline std::optional<multi_action> communication_merge_multi_action(const multi_action& multi_action1,
  const multi_action& multi_action2,
  const process::action& termination_action)
{
  const std::optional<process::action_list> actions
    = communication_merge_actions(multi_action1.actions(), multi_action2.actions(), termination_action);
  if (!actions)
  {
    return std::nullopt;
  }
  return multi_action(*actions, multi_action1.has_time() ? multi_action1.time() : multi_action2.time());
}

/// \brief The summand of the communication merge for an action summand of each process, with the given
///        multi-action (see communication_merge_multi_action).
/// \details Both summands happen together: the sum variables, conditions, next states and distributions are
///          joined. If both summands have a time, the times must be equal. The condition is rewritten and may be
///          false. The distributions of both summands must be reals (a summand without stochastic operator has
///          distribution 1 over no variables).
/// \param summand1 An action summand of the first process.
/// \param summand2 An action summand of the second process.
/// \param action The multi-action of the result.
/// \param rewrite A function that rewrites a data expression.
template<typename Rewriter>
stochastic_action_summand communication_merge_summand(const stochastic_action_summand& summand1,
  const stochastic_action_summand& summand2,
  const multi_action& action,
  Rewriter rewrite)
{
  data::data_expression condition = data::lazy::and_(summand1.condition(), summand2.condition());
  if (summand1.has_time() && summand2.has_time())
  {
    condition
      = data::lazy::and_(condition, data::equal_to(summand1.multi_action().time(), summand2.multi_action().time()));
  }

  const stochastic_distribution& distribution1 = summand1.distribution();
  const stochastic_distribution& distribution2 = summand2.distribution();
  return stochastic_action_summand(summand1.summation_variables() + summand2.summation_variables(),
    rewrite(condition),
    action,
    summand1.assignments() + summand2.assignments(),
    stochastic_distribution(distribution1.variables() + distribution2.variables(),
      data::sort_real::optimized_times(distribution1.distribution(), distribution2.distribution())));
}

/// \brief The deadlock summand of the communication merge for an action or deadlock summand of the first process
///        and a deadlock summand of the second.
/// \details The result can delay until the earliest of the times of both summands. The sum variables and
///          conditions are joined; the condition is not rewritten, and if it is false the time is not computed.
/// \param summand1 An action or deadlock summand of the first process.
/// \param summand2 A deadlock summand of the second process.
/// \param rewrite A function that rewrites a data expression, used for the minimum of two times.
template<typename Summand, typename Rewriter>
deadlock_summand
communication_merge_summand(const Summand& summand1, const deadlock_summand& summand2, Rewriter rewrite)
{
  const data::variable_list sumvars = summand1.summation_variables() + summand2.summation_variables();
  const data::data_expression condition = data::lazy::and_(summand1.condition(), summand2.condition());
  if (condition == data::sort_bool::false_())
  {
    return deadlock_summand(sumvars, condition, deadlock());
  }
  const data::data_expression& time1 = detail::summand_time(summand1);
  const data::data_expression& time2 = detail::summand_time(summand2);
  if (summand1.has_time() && summand2.has_time())
  {
    return deadlock_summand(sumvars, condition, deadlock(rewrite(data::sort_real::minimum(time1, time2))));
  }
  return deadlock_summand(sumvars, condition, deadlock(summand1.has_time() ? time1 : time2));
}

namespace detail
{

/// \brief Adds the deadlock summands of the communication merge of the action or deadlock summands summands1 of
///        the first process with the deadlock summands deadlock_summands2 of the second, each only if no action or
///        deadlock summand subsumes it. Only used if time is not ignored.
template<typename Summand, typename Rewriter>
void add_communication_merge_deadlock_summands(const std::vector<Summand>& summands1,
  const deadlock_summand_vector& deadlock_summands2,
  Rewriter rewrite,
  const stochastic_action_summand_vector& action_summands,
  deadlock_summand_vector& deadlock_summands)
{
  for (const Summand& summand1: summands1)
  {
    for (const deadlock_summand& summand2: deadlock_summands2)
    {
      const deadlock_summand summand = communication_merge_summand(summand1, summand2, rewrite);
      if (summand.condition() != data::sort_bool::false_())
      {
        insert_timed_delta_summand(action_summands, deadlock_summands, summand, false);
      }
    }
  }
}

} // namespace detail

/// \brief Adds the summands of the communication merge of two processes to action_summands and deadlock_summands.
/// \details Every action summand of the first process is combined with every action summand of the second
///          (communication_merge_multi_action, communication_merge_summand); filter applies an inline allow or block to
///          the resulting multi-actions, and summands with condition false are left out. If time is not ignored,
///          the deadlock summands for the combinations of an action or deadlock summand with a deadlock summand are
///          added after the action summands (action summands of the first with deadlock summands of the second,
///          action summands of the second with deadlock summands of the first, deadlock summands of both), each
///          only if no action or deadlock summand subsumes it. Inline allow and block are only used if time is
///          ignored.
/// \param action_summands1 The action summands of the first process.
/// \param deadlock_summands1 The deadlock summands of the first process.
/// \param action_summands2 The action summands of the second process.
/// \param deadlock_summands2 The deadlock summands of the second process.
/// \param filter The inline allow or block, if any.
/// \param termination_action The termination action of the lineariser.
/// \param ignore_time Whether time is ignored.
/// \param rewrite A function that rewrites a data expression.
/// \param action_summands The action summands to which the result is added.
/// \param deadlock_summands The deadlock summands to which the result is added.
template<typename Rewriter>
void communication_merge(const stochastic_action_summand_vector& action_summands1,
  const deadlock_summand_vector& deadlock_summands1,
  const stochastic_action_summand_vector& action_summands2,
  const deadlock_summand_vector& deadlock_summands2,
  const allow_block_filter& filter,
  const process::action& termination_action,
  bool ignore_time,
  Rewriter rewrite,
  stochastic_action_summand_vector& action_summands,
  deadlock_summand_vector& deadlock_summands)
{
  assert(ignore_time || (!filter.is_allow() && !filter.is_block()));

  for (const stochastic_action_summand& summand1: action_summands1)
  {
    for (const stochastic_action_summand& summand2: action_summands2)
    {
      const std::optional<multi_action> action
        = communication_merge_multi_action(summand1.multi_action(), summand2.multi_action(), termination_action);
      if (!action || !filter.keeps(*action))
      {
        continue;
      }
      stochastic_action_summand summand = communication_merge_summand(summand1, summand2, *action, rewrite);
      if (summand.condition() != data::sort_bool::false_())
      {
        action_summands.push_back(std::move(summand));
      }
    }
  }

  if (ignore_time)
  {
    return;
  }
  detail::add_communication_merge_deadlock_summands(action_summands1,
    deadlock_summands2,
    rewrite,
    action_summands,
    deadlock_summands);
  detail::add_communication_merge_deadlock_summands(action_summands2,
    deadlock_summands1,
    rewrite,
    action_summands,
    deadlock_summands);
  detail::add_communication_merge_deadlock_summands(deadlock_summands1,
    deadlock_summands2,
    rewrite,
    action_summands,
    deadlock_summands);
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_LINEARISE_PARALLEL_H
