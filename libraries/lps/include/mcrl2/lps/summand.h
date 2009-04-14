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
#include <boost/iterator/transform_iterator.hpp>
#include "mcrl2/atermpp/aterm.h"
#include "mcrl2/atermpp/aterm_list.h"
#include "mcrl2/atermpp/algorithm.h"
#include "mcrl2/atermpp/utility.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/detail/data_assignment_functional.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/deadlock.h"
#include "mcrl2/lps/multi_action.h"
#include "mcrl2/lps/detail/action_utility.h"
#include "mcrl2/data/detail/sequence_algorithm.h"

namespace mcrl2 {

namespace lps {

/// \brief LPS summand.
// <LinearProcessSummand>   ::= LinearProcessSummand(<DataVarId>*, <DataExpr>, <MultActOrDelta>,
//                    <DataExprOrNil>, <Assignment>*)
//<MultActOrDelta>
//               ::= <MultAct>
//                 | Delta
// <MultAct>      ::= MultAct(<ParamId>*)                                   (- tc)
//                  | MultAct(<Action>*)                                    (+ tc)
class summand: public atermpp::aterm_appl
{
  protected:
    /// \brief The summation variables of the summand
    data::data_variable_list m_summation_variables;

    /// \brief The condition of the summand
    data::data_expression m_condition;

    /// \brief If m_delta is true the summand is a delta summand
    bool m_delta;

    /// \brief The actions of the summand
    action_list m_actions;

    /// \brief The time of the summand. If <tt>m_time == data::data_expression()</tt>
    /// the summand has no time.
    data::data_expression m_time;

    /// \brief The assignments of the summand. These assignments are an encoding of
    /// the 'next states' of the summand.
    data::data_assignment_list m_assignments;

  public:
    /// \brief Returns the multi-action of this summand.
    /// \pre The summand is no deadlock summand.
    lps::multi_action multi_action() const
    {
      return lps::multi_action(m_actions, m_time);
    }

    /// \brief Returns the deadlock of this summand.
    /// \pre The summand is a deadlock summand.
    lps::deadlock deadlock() const
    {
      return lps::deadlock(m_time);
    }

    /// \brief Constructor.
    summand()
      : atermpp::aterm_appl(mcrl2::core::detail::constructLinearProcessSummand())
    {}

    /// \brief Constructor.
    /// \param t A term
    summand(atermpp::aterm_appl t)
     : atermpp::aterm_appl(t)
    {
      assert(core::detail::check_rule_LinearProcessSummand(m_term));
      atermpp::aterm_appl::iterator i = t.begin();

      m_summation_variables = data::data_variable_list(*i++);
      m_condition           = data::data_expression(*i++);
      atermpp::aterm_appl x          = *i++;
      m_delta = core::detail::gsIsDelta(x);
      if (!m_delta)
      {
        assert(core::detail::gsIsMultAct(x));
        m_actions = action_list(x.argument(0));
      }
      m_time                = data::data_expression(*i++);
      m_assignments         = data::data_assignment_list(*i);
    }

    /// \brief Constructor.
    /// Constructs an untimed summand.
    /// \deprecated
    summand(data::data_variable_list   summation_variables,
            data::data_expression      condition,
            bool                       delta,
            action_list                actions,
            data::data_assignment_list assignments
           )
      : atermpp::aterm_appl(core::detail::gsMakeLinearProcessSummand(summation_variables,
               condition,
               (delta ? core::detail::gsMakeDelta() : core::detail::gsMakeMultAct(actions)),
               core::detail::gsMakeNil(),
               assignments)
        ),
        m_summation_variables(summation_variables),
        m_condition          (condition),
        m_delta              (delta),
        m_actions            (actions),
        m_time               (data::data_expression(core::detail::gsMakeNil())),
        m_assignments        (assignments)
    {}

    /// \brief Constructor.
    /// Constructs a timed summand.
    /// \deprecated
    summand(data::data_variable_list   summation_variables,
            data::data_expression      condition,
            bool                       delta,
            action_list                actions,
            data::data_expression      time,
            data::data_assignment_list assignments
           )
      : atermpp::aterm_appl(core::detail::gsMakeLinearProcessSummand(summation_variables,
               condition,
               (delta ? core::detail::gsMakeDelta() : core::detail::gsMakeMultAct(actions)),
               time,
               assignments)
        ),
        m_summation_variables(summation_variables),
        m_condition          (condition),
        m_delta              (delta),
        m_actions            (actions),
        m_time               (time),
        m_assignments        (assignments)
    {}

    /// \brief Constructor.
    /// Constructs a multi action summand.
    summand(data::data_variable_list   summation_variables,
            data::data_expression      condition,
            const lps::multi_action&   a,
            data::data_assignment_list assignments
           )
      : atermpp::aterm_appl(core::detail::gsMakeLinearProcessSummand(summation_variables,
               condition,
               core::detail::gsMakeMultAct(a.actions()),
               a.time(),
               assignments)
        ),
        m_summation_variables(summation_variables),
        m_condition          (condition),
        m_delta              (false),
        m_actions            (a.actions()),
  m_time               (a.time()),
        m_assignments        (assignments)
    {}

    /// \brief Constructor.
    /// Constructs a deadlock summand.
    summand(data::data_variable_list   summation_variables,
            data::data_expression      condition,
            const lps::deadlock&       d
           )
      : atermpp::aterm_appl(core::detail::gsMakeLinearProcessSummand(summation_variables,
               condition,
               core::detail::gsMakeDelta(),
               d.time(),
               data::data_assignment_list())
        ),
        m_summation_variables(summation_variables),
        m_condition          (condition),
        m_delta              (true),
        m_time               (d.time())
    {}

    /// \brief Returns the sequence of summation variables.
    /// \return The sequence of summation variables.
    data::data_variable_list summation_variables() const
    {
      return m_summation_variables;
    }

    /// \brief Returns true if the multi-action corresponding to this summand is equal to delta.
    /// \return True if the multi-action corresponding to this summand is equal to delta.
    bool is_delta() const
    {
      return m_delta;
    }

    /// \brief Returns true if the multi-action corresponding to this summand is equal to tau.
    /// \return True if the multi-action corresponding to this summand is equal to tau.
    bool is_tau() const
    {
      // return !is_delta() && actions().size() == 1 && core::detail::gsIsTau(actions().front());

      /*
      // Written by Luc Engelen; no idea what it does.
      ATermAppl v_multi_action_or_delta = ATAgetArgument(*this, 2);
      if (gsIsMultAct(v_multi_action_or_delta)) {
        return ATisEmpty(ATLgetArgument(v_multi_action_or_delta, 0));
      } else {
        return false;
      }
      */
      return !is_delta() && actions().empty();
    }

    /// \brief Returns true if time is available.
    /// \return True if time is available.
    bool has_time() const
    {
      return !data::data_expr::is_nil(m_time);
    }

    /// \brief Returns the condition expression.
    /// \return The condition expression.
    data::data_expression condition() const
    {
      return m_condition;
    }

    /// \brief Returns the sequence of actions. Returns an empty list if is_delta() holds.
    /// \return The sequence of actions. Returns an empty list if is_delta() holds.
    /// \deprecated
    action_list actions() const
    {
      return m_actions;
    }

    /// \brief Returns the time.
    /// \return The time.
    /// \deprecated
    data::data_expression time() const
    {
      return m_time;
    }

    /// \brief Returns the sequence of assignments.
    /// \return The sequence of assignments.
    data::data_assignment_list assignments() const
    {
      return m_assignments;
    }

    /// \brief Returns the next state corresponding to this summand.
    /// \details The next state is constructed out of the assignments in
    /// this summand, by substituting the assignments to the list of
    /// variables that are an argument of this function. In general this
    /// argument is the list with the process parameters of this process.
    /// \param process_parameters A sequence of data variables
    /// \return A symbolic representation of the next states
    data::data_expression_list next_state(const data::data_variable_list& process_parameters) const
    {
      const data::data_expression_list pp((atermpp::aterm)process_parameters);
      return pp.substitute(data::assignment_list_substitution(assignments()));
    }

    /// \brief Applies a low level substitution function to this term and returns the result.
    /// \param f A
    /// The function <tt>f</tt> must supply the method <tt>aterm operator()(aterm)</tt>.
    /// This function is applied to all <tt>aterm</tt> noded appearing in this term.
    /// \deprecated
    /// \return The substitution result.
    template <typename Substitution>
    summand substitute(Substitution f) const
    {
      action_list actions;
      data::data_expression condition = m_condition.substitute(f);
      if (!m_delta)
      {
        actions = m_actions.substitute(f);
      }
      data::data_expression time = m_time.substitute(f);

      return summand(m_summation_variables, condition, m_delta, actions, time, m_assignments);
    }

    /// \brief Checks if the summand is well typed
    /// \return Returns true if
    /// <ul>
    /// <li>the data assignments are well typed</li>
    /// <li>the (optional) time has sort Real</li>
    /// <li>the condition has sort Bool</li>
    /// <li>the summation variables have unique names</li>
    //  <li>the left hand sides of the data assignments are unique</li>
    /// </ul>
    bool is_well_typed() const
    {
      using namespace std::rel_ops; // for definition of operator!= in terms of operator==

      // check 1)
      for (data::data_assignment_list::iterator i = m_assignments.begin(); i != m_assignments.end(); ++i)
      {
        if (!i->is_well_typed())
          return false;
      }

      // check 2)
      if (has_time() && !data::data_expr::is_real(m_time))
      {
        std::cerr << "summand::is_well_typed() failed: time " << mcrl2::core::pp(m_time) << " doesn't have type real." << std::endl;
        return false;
      }

      // check 3)
      if (!data::data_expr::is_bool(m_condition))
      {
        std::cerr << "summand::is_well_typed() failed: condition " << mcrl2::core::pp(m_condition) << " doesn't have type bool." << std::endl;
        return false;
      }

      // check 4)
      if (!mcrl2::data::detail::unique_names(m_summation_variables))
      {
        std::cerr << "summand::is_well_typed() failed: summation variables " << mcrl2::core::pp(m_summation_variables) << " don't have unique names." << std::endl;
        return false;
      }

      // check 5)
      if (sequence_contains_duplicates(
               boost::make_transform_iterator(m_assignments.begin(), data::detail::data_assignment_lhs()),
               boost::make_transform_iterator(m_assignments.end()  , data::detail::data_assignment_lhs())
              )
         )
      {
        std::cerr << "summand::is_well_typed() failed: data assignments " << mcrl2::core::pp(m_assignments) << " don't have unique left hand sides." << std::endl;
        return false;
      }

      return true;
    }
};

/// \brief Sets the summation variables of s and returns the result
/// \param s A linear process summand
/// \param summation_variables A sequence of data variables
/// \return The updated summand
inline
summand set_summation_variables(summand s, data::data_variable_list summation_variables)
{
  return summand(summation_variables,
                     s.condition          (),
                     s.is_delta           (),
                     s.actions            (),
                     s.time               (),
                     s.assignments        ()
                    );
}

/// \brief Sets the condition of s and returns the result
/// \param s A linear process summand
/// \param condition A data expression
/// \return The updated summand
inline
summand set_condition(summand s, data::data_expression condition)
{
  return summand(s.summation_variables(),
                 condition,
                 s.is_delta           (),
                 s.actions            (),
                 s.time               (),
                 s.assignments        ()
                );
}

/// \brief Sets the actions of s to delta and returns the result
/// \param s A linear process summand
/// \return The updated summand
inline
summand set_delta(summand s)
{
  return summand(s.summation_variables(),
                 s.condition          (),
                 false,
                 s.actions            (),
                 s.time               (),
                 s.assignments        ()
                );
}

/// \brief Sets the actions of s and returns the result
/// \param s A linear process summand
/// \param actions A sequence of actions
/// \return The updated summand
inline
summand set_actions(summand s, action_list actions)
{
  return summand(s.summation_variables(),
                 s.condition          (),
                 s.is_delta           (),
                 actions,
                 s.time               (),
                 s.assignments        ()
                );
}

/// \brief Sets the time of s and returns the result
/// \param s A linear process summand
/// \param time A data expression
/// \return The updated summand
inline
summand set_time(summand s, data::data_expression time)
{
  return summand(s.summation_variables(),
                     s.condition          (),
                     s.is_delta           (),
                     s.actions            (),
                     time,
                     s.assignments        ()
                    );
}

/// \brief Sets the assignments of s and returns the result
/// \param s A linear process summand
/// \param assignments A sequence of assignments to data variables
/// \return The updated summand
inline
summand set_assignments(summand s, data::data_assignment_list assignments)
{
  return summand(s.summation_variables(),
                 s.condition          (),
                 s.is_delta           (),
                 s.actions            (),
                 s.time               (),
                 assignments
                );
}

/// \brief Read-only singly linked list of summands
typedef atermpp::term_list<summand> summand_list;

} // namespace lps

} // namespace mcrl2

#endif // MCRL2_LPS_SUMMAND_H
