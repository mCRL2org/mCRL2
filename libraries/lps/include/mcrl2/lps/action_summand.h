// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/action_summand.h
/// \brief add your file description here.

#ifndef MCRL2_LPS_ACTION_SUMMAND_H
#define MCRL2_LPS_ACTION_SUMMAND_H

#include "mcrl2/lps/summand.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/multi_action.h"

namespace mcrl2 {

namespace lps {

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
    data::data_expression_list next_state(const data::variable_list& process_parameters) const;

    /// \brief Swaps the contents
    void swap(action_summand& other)
    {
      summand_base::swap(other);
      using std::swap;
      swap(m_multi_action, other.m_multi_action);
      swap(m_assignments, other.m_assignments);
    }
};

//--- start generated class action_summand ---//
/// \brief list of action_summands
typedef atermpp::term_list<action_summand> action_summand_list;

/// \brief vector of action_summands
typedef std::vector<action_summand>    action_summand_vector;

// prototype declaration
std::string pp(const action_summand& x);

/// \brief Outputs the object to a stream
/// \param out An output stream
/// \return The output stream
inline
std::ostream& operator<<(std::ostream& out, const action_summand& x)
{
  return out << lps::pp(x);
}

/// \brief swap overload
inline void swap(action_summand& t1, action_summand& t2)
{
  t1.swap(t2);
}
//--- end generated class action_summand ---//

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

/// \brief Equality operator of action summands
inline
bool operator==(const action_summand& x, const action_summand& y)
{
  return x.condition() == y.condition() && x.multi_action() == y.multi_action() && x.assignments() == y.assignments();
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

#endif // MCRL2_LPS_ACTION_SUMMAND_H
