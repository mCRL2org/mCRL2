// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/summand.h
/// \brief The class summand.

#ifndef MCRL2_LPS_SUMMAND_H
#define MCRL2_LPS_SUMMAND_H

#include <string>
#include <cassert>
#include <algorithm>
#include <iterator>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_access.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/data/data_expression.h"
#include "mcrl2/data/print.h"
#include "mcrl2/data/real.h"
#include "mcrl2/data/bool.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/detail/assignment_functional.h"
#include "mcrl2/data/detail/sequence_algorithm.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/deadlock.h"
#include "mcrl2/lps/multi_action.h"

namespace mcrl2
{

namespace lps
{

/// \brief Base class for LPS summands.
class summand_base
{
  protected:
    /// \brief The summation variables of the summand
    data::variable_list m_summation_variables;

    /// \brief The condition of the summand
    data::data_expression m_condition;

  public:
    /// \brief Constructor.
    summand_base()
    {}

    /// \brief Constructor.
    summand_base(const data::variable_list& summation_variables, const data::data_expression& condition)
      : m_summation_variables(summation_variables),
        m_condition(condition)
    {}

    /// \brief Returns the sequence of summation variables.
    /// \return The sequence of summation variables.
    data::variable_list& summation_variables()
    {
      return m_summation_variables;
    }

    /// \brief Returns the sequence of summation variables.
    /// \return The sequence of summation variables.
    const data::variable_list& summation_variables() const
    {
      return m_summation_variables;
    }

    /// \brief Returns the condition expression.
    /// \return The condition expression.
    const data::data_expression& condition() const
    {
      return m_condition;
    }

    /// \brief Returns the condition expression.
    /// \return The condition expression.
    data::data_expression& condition()
    {
      return m_condition;
    }

    /// \brief Swaps the contents
    void swap(summand_base& other)
    {
      using std::swap;
      swap(m_summation_variables, other.m_summation_variables);
      swap(m_condition, other.m_condition);
    }
};

/// \brief LPS summand containing a deadlock.
class deadlock_summand: public summand_base
{
  protected:
    /// \brief The super class
    typedef summand_base super;

    /// \brief The deadlock of the summand
    lps::deadlock m_deadlock;

    /// \brief Returns true if time is available.
    /// \return True if time is available.
    bool has_time() const
    {
      return m_deadlock.has_time();
    }

  public:
    /// \brief Constructor.
    // TODO: check if the default constructor results in a deadlock summand
    deadlock_summand()
    {}

    /// \brief Constructor.
    deadlock_summand(const data::variable_list& summation_variables, const data::data_expression& condition, const lps::deadlock& delta)
      : summand_base(summation_variables, condition),
        m_deadlock(delta)
    {}

    /// \brief Returns the deadlock of this summand.
    const lps::deadlock& deadlock() const
    {
      return m_deadlock;
    }

    /// \brief Returns the deadlock of this summand.
    lps::deadlock& deadlock()
    {
      return m_deadlock;
    }

    /// \brief Swaps the contents
    void swap(deadlock_summand& other)
    {
      summand_base::swap(other);
      using std::swap;
      swap(m_deadlock, other.m_deadlock);
    }
};

/// \brief Vector of deadlock summands
typedef std::vector<deadlock_summand> deadlock_summand_vector;

/// \brief swap overload
inline void swap(deadlock_summand& t1, deadlock_summand& t2)
{
  t1.swap(t2);
}

/// \brief Conversion to atermappl.
/// \return The deadlock summand converted to aterm format.
inline
atermpp::aterm_appl deadlock_summand_to_aterm(const deadlock_summand& s)
{
  atermpp::aterm_appl result = core::detail::gsMakeLinearProcessSummand(
                       s.summation_variables(),
                       s.condition(),
                       core::detail::gsMakeDelta(),
                       s.deadlock().time(),
                       data::assignment_list()
                     );
  return result;
}

/// \brief LPS summand containing a multi-action.
class action_summand: public summand_base
{
  protected:
    /// \brief The super class
    typedef summand_base super;

    /// \brief The summation variables of the summand
    lps::multi_action m_multi_action;

    /// \brief The assignments of the next state
    data::assignment_list m_assignments;

  public:
    /// \brief Constructor.
    // TODO: check if the default constructor results in a deadlock summand
    action_summand()
    {}

    /// \brief Constructor.
    action_summand(const data::variable_list& summation_variables, const data::data_expression& condition, const lps::multi_action& action, const data::assignment_list& assignments)
      : summand_base(summation_variables, condition),
        m_multi_action(action),
        m_assignments(assignments)
    {}

    /// \brief Returns the multi-action of this summand.
    const lps::multi_action& multi_action() const
    {
      return m_multi_action;
    }

    /// \brief Returns the multi-action of this summand.
    lps::multi_action& multi_action()
    {
      return m_multi_action;
    }

    /// \brief Returns the sequence of assignments.
    /// \return The sequence of assignments.
    const data::assignment_list& assignments() const
    {
      return m_assignments;
    }

    /// \brief Returns the sequence of assignments.
    /// \return The sequence of assignments.
    data::assignment_list &assignments()
    {
      return m_assignments;
    }

    // TODO: check if this is correct (where is the documentation of the internal format?)
    /// \brief Returns true if the multi-action corresponding to this summand is equal to tau.
    /// \return True if the multi-action corresponding to this summand is equal to tau.
    bool is_tau() const
    {
      return multi_action().actions().empty();
    }

    /// \brief Returns true if time is available.
    /// \return True if time is available.
    bool has_time() const
    {
      return m_multi_action.has_time();
    }

    /// \brief Returns the next state corresponding to this summand.
    /// \details The next state is constructed out of the assignments in
    /// this summand, by substituting the assignments to the list of
    /// variables that are an argument of this function. In general this
    /// argument is the list with the process parameters of this process.
    /// \param process_parameters A sequence of data variables
    /// \return A symbolic representation of the next states
    data::data_expression_list next_state(const data::variable_list& process_parameters) const
    {
      return data::replace_variables(atermpp::convert<data::data_expression_list>(process_parameters), data::assignment_sequence_substitution(assignments()));
    }

    /// \brief Swaps the contents
    void swap(action_summand& other)
    {
      summand_base::swap(other);
      using std::swap;
      swap(m_multi_action, other.m_multi_action);
      swap(m_assignments, other.m_assignments);
    }
};

/// \brief Comparison operator for action summands.
inline
bool operator<(const action_summand& x, const action_summand& y)
{
  if (x.summation_variables() != y.summation_variables())
  {
    return x.summation_variables() < y.summation_variables();
  }
  if (x.condition() != y.condition())
  {
    return x.condition() < y.condition();
  }
  if (x.assignments() != y.assignments())
  {
    return x.assignments() < y.assignments();
  }
  return x.multi_action() < y.multi_action();
}

/// \brief Vector of action summands
typedef std::vector<action_summand> action_summand_vector;

/// \brief Equality operator of action summands
inline
bool operator==(const action_summand& x, const action_summand& y)
{
  return x.condition() == y.condition() && x.multi_action() == y.multi_action() && x.assignments() == y.assignments();
}

/// \brief swap overload
inline void swap(action_summand& t1, action_summand& t2)
{
  t1.swap(t2);
}

/// \brief Conversion to atermAppl.
/// \return The action summand converted to aterm format.
inline
atermpp::aterm_appl action_summand_to_aterm(const action_summand& s)
{
  atermpp::aterm_appl result = core::detail::gsMakeLinearProcessSummand(
                       s.summation_variables(),
                       s.condition(),
                       lps::detail::multi_action_to_aterm(s.multi_action()),
                       s.multi_action().time(),
                       s.assignments()
                     );
  return result;
}

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUMMAND_H
