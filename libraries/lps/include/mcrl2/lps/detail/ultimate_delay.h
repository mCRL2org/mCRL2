// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file ultimate_delay.h
/// \brief This file defines the class ultimate_delay.
///        An ultimate delay consists of a time variable t
///        and expression exp(t,x) and free variables x.
///        It can be read as a predicate on t of the form
///        exists x.exp(t,x). If this is true for certain t, this
///        indicates that a process can wait until time t.  


#ifndef MCRL2_LPS_DETAIL_ULTIMATE_DELAY_H
#define MCRL2_LPS_DETAIL_ULTIMATE_DELAY_H

#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/replace_capture_avoiding_with_an_identifier_generator.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/lps/deadlock_summand.h"
#include "mcrl2/lps/detail/merge_variables_by_sort.h"
#include "mcrl2/lps/linearise_utility.h"
#include "mcrl2/lps/stochastic_action_summand.h"

namespace mcrl2::lps::detail
{

using namespace data;

class ultimate_delay
{
  protected:
    variable m_time;
    variable_list m_vl;
    data_expression m_data;

  public:
    /// \brief Constructor.
    ultimate_delay()
     : m_data(sort_bool::false_())
     {}


    /// \brief Constructor.
    ultimate_delay(const variable& t)
     : m_time(t), m_vl(), m_data(sort_bool::true_())
    {
      assert(t.sort()==sort_real::real_());
    }

    /// \brief Constructor.
    ultimate_delay(const variable& t, const variable_list& vl, const data_expression& d)
     : m_time(t), m_vl(vl), m_data(d)
    {
      assert(d.sort()==sort_bool::bool_());
      assert(t.sort()==sort_real::real_());
    }

    /// \brief Obtain the variables. 
    const variable_list& variables() const
    {
      return m_vl;
    }

    /// \brief Reference to the variables. 
    variable_list& variables()
    {
      return m_vl;
    }

    /// \brief Obtain the constraint in the ultimate delay.
    const data_expression& constraint() const
    {
      return m_data;
    }

    /// \brief Obtain a reference to the constraint. 
    data_expression& constraint()
    {
      return m_data;
    }

    /// \brief Obtain the constraint in the ultimate delay.
    const variable& time_var() const
    {
      return m_time;
    }

    /// \brief Obtain a reference to the time variable in the ultimate delay.
    variable& time_var()
    {
      return m_time;
    }
};

/// \brief Returns true if time is of the form t1 + ... + tn, where some ti is a variable of sum_variables that does
///        not occur in condition.
/// \details A summand with such a time can happen at any time, as ti can be chosen freely.
inline bool time_can_be_chosen_freely(const data::variable_list& sum_variables,
  const data::data_expression& time,
  const data::data_expression& condition)
{
  if (data::is_variable(time))
  {
    const data::variable& t = atermpp::down_cast<data::variable>(time);
    return utilities::detail::contains(sum_variables, t) && !occursinterm(condition, t);
  }
  if (data::sort_real::is_plus_application(time))
  {
    const data::application& plus = atermpp::down_cast<data::application>(time);
    return time_can_be_chosen_freely(sum_variables, data::binary_left(plus), condition)
           || time_can_be_chosen_freely(sum_variables, data::binary_right(plus), condition);
  }
  return false;
}

/// \brief Returns the variables of variables that occur in e, in the same order.
inline data::variable_list variables_occurring_in(const data::variable_list& variables, const data::data_expression& e)
{
  std::vector<data::variable> result;
  std::copy_if(variables.begin(),
    variables.end(),
    std::back_inserter(result),
    [&](const data::variable& v) { return occursinterm(e, v); });
  return data::variable_list(result.begin(), result.end());
}

/// \brief Returns true if some summand is untimed and has condition true, so that the process can wait forever.
inline bool has_untimed_summand_with_condition_true(const stochastic_action_summand_vector& action_summands,
  const deadlock_summand_vector& deadlock_summands)
{
  const auto untimed_and_true = [](const auto& summand)
  {
    return !summand.has_time() && summand.condition() == data::sort_bool::true_();
  };
  return std::any_of(deadlock_summands.begin(), deadlock_summands.end(), untimed_and_true)
         || std::any_of(action_summands.begin(), action_summands.end(), untimed_and_true);
}

/// \brief Returns the condition under which a summand can wait until time_variable: condition && time_variable <
///        time, or only the condition if the summand is untimed or can happen at any time.
template<typename Rewriter>
data::data_expression summand_ultimate_delay_condition(const data::variable_list& sum_variables,
  const data::data_expression& condition,
  bool has_time,
  const data::data_expression& time,
  const data::variable& time_variable,
  Rewriter rewrite)
{
  if (!has_time || time_can_be_chosen_freely(sum_variables, time, condition))
  {
    return condition;
  }
  return rewrite(data::lazy::and_(condition, data::less(time_variable, time)));
}

/// \brief Builds the disjunction of the ultimate delay conditions of summands, in which the sum variables of the
///        summands are merged by sort (see merge_variables_by_sort).
class summand_delay_condition_merger
{
protected:
  const data::data_specification& m_data;
  data::set_identifier_generator& m_id_generator;

  /// \brief The merged variables.
  data::variable_list m_variables;
  /// \brief Per summand, its condition and the renaming of its variables into the merged variables.
  std::vector<data::data_expression> m_conditions;
  std::vector<variable_renaming> m_renamings;
  /// \brief Per summand, in reverse order, the condition that fixes the merged variables that do not occur in it to a
  ///        default value.
  data::data_expression_list m_default_value_conditions;

  data::variable fresh_variable(const data::variable& v)
  {
    return data::variable(m_id_generator(v.name()), v.sort());
  }

  // A new representative generator is used for each sort, as the data specification can change in between.
  data::data_expression representative(const data::sort_expression& s) const
  {
    return data::representative_generator(m_data)(s);
  }

public:
  /// \brief Fresh names are taken from id_generator.
  summand_delay_condition_merger(const data::data_specification& data_spec,
    data::set_identifier_generator& id_generator)
    : m_data(data_spec),
      m_id_generator(id_generator)
  {}

  /// \brief Adds the condition of a summand, of which the sum variables that occur in it are merged.
  void add_summand_delay_condition(const data::variable_list& sum_variables, const data::data_expression& condition)
  {
    const auto fresh = [this](const data::variable& v)
    {
      return fresh_variable(v);
    };
    const auto default_value = [this](const data::sort_expression& s)
    {
      return representative(s);
    };
    m_conditions.push_back(condition);
    m_renamings.push_back(merge_variables_by_sort(variables_occurring_in(sum_variables, condition),
      m_variables,
      m_default_value_conditions,
      data::variable_list(),
      fresh,
      default_value));
  }

  /// \brief The merged variables.
  const data::variable_list& merged_variables() const
  {
    return m_variables;
  }

  /// \brief Returns the disjunction over the summands of their condition, in which the variables are renamed into
  ///        the merged variables, and the merged variables that do not occur in it are fixed to default values.
  data::data_expression merged_delay_condition()
  {
    assert(m_conditions.size() == m_default_value_conditions.size());
    const data::data_expression_list default_value_conditions = atermpp::reverse(m_default_value_conditions);
    data::data_expression result = data::sort_bool::false_();
    std::size_t i = 0;
    for (const data::data_expression& default_value_condition: default_value_conditions)
    {
      const data::data_expression condition = data::lazy::and_(m_conditions[i], default_value_condition);
      result = data::lazy::or_(result,
        data::replace_variables_capture_avoiding_with_an_identifier_generator(condition,
          m_renamings[i],
          m_id_generator));
      ++i;
    }
    return result;
  }
};

/// \brief Returns the ultimate delay of a linear process with the given summands: the disjunction over the summands
///        of the condition under which the summand can wait until the time variable.
/// \details The sum variables that occur in these conditions become the variables of the ultimate delay, where the
///          variables of the same sort of different summands are merged (see merge_variables_by_sort). The
///          ultimate delay is true if a summand is untimed and has condition true. Fresh names, also for the time
///          variable, are taken from id_generator, which must contain the identifiers of the linear process and its
///          data specification.
/// \param rewrite A function that rewrites a data expression.
template<typename Rewriter>
ultimate_delay compute_ultimate_delay(const stochastic_action_summand_vector& action_summands,
  const deadlock_summand_vector& deadlock_summands,
  const data::data_specification& data_spec,
  data::set_identifier_generator& id_generator,
  Rewriter rewrite)
{
  const data::variable time_variable(id_generator("timevar"), data::sort_real::real_());
  if (has_untimed_summand_with_condition_true(action_summands, deadlock_summands))
  {
    return ultimate_delay(time_variable);
  }

  summand_delay_condition_merger merger(data_spec, id_generator);
  for (const deadlock_summand& s: deadlock_summands)
  {
    merger.add_summand_delay_condition(s.summation_variables(),
      summand_ultimate_delay_condition(s.summation_variables(),
        s.condition(),
        s.has_time(),
        s.deadlock().time(),
        time_variable,
        rewrite));
  }
  for (const stochastic_action_summand& s: action_summands)
  {
    merger.add_summand_delay_condition(s.summation_variables(),
      summand_ultimate_delay_condition(s.summation_variables(),
        s.condition(),
        s.has_time(),
        s.multi_action().time(),
        time_variable,
        rewrite));
  }
  return ultimate_delay(time_variable, merger.merged_variables(), rewrite(merger.merged_delay_condition()));
}

/// \brief Returns the conjunction of the two delay conditions and the join of the variables, where
///        the variables in delay2 are renamed to avoid conflict with those in delay1, and the time variable of
///        delay2 is replaced by that of delay1. Fresh names are taken from id_generator.
inline ultimate_delay combine_ultimate_delays(const ultimate_delay& delay1,
  const ultimate_delay& delay2,
  data::set_identifier_generator& id_generator)
{
  const auto in_delay1 = [&](const data::variable& v)
  {
    return utilities::detail::contains(delay1.variables(), v);
  };
  const auto fresh = [&](const data::variable& v)
  {
    return data::variable(id_generator(v.name()), v.sort());
  };

  variable_renaming sigma;
  const data::variable_list renamed_variables = rename_variables_away(delay2.variables(), in_delay1, sigma, fresh);
  sigma[delay2.time_var()] = delay1.time_var();

  data::data_expression constraint;
  data::make_optimized_and(constraint,
    delay1.constraint(),
    data::replace_variables_capture_avoiding_with_an_identifier_generator(delay2.constraint(), sigma, id_generator));
  // TODO: The new constraint can be simplified, as two conditions sharing the timed variable have been merged.
  return ultimate_delay(delay1.time_var(), delay1.variables() + renamed_variables, constraint);
}

} // namespace mcrl2::lps::detail

#endif // MCRL2_LPS_DETAIL_ULTIMATE_DELAY_H

