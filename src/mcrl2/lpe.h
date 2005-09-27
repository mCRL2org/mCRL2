///////////////////////////////////////////////////////////////////////////////
/// \file LPE.h
/// Contains LPE data structures for the mcrl2 library.

#ifndef MCRL2_LPE_H
#define MCRL2_LPE_H

#include <functional>
#include <iostream>
#include <vector>
#include <utility>
#include "atermpp/aterm.h"
#include "mcrl2/aterm_wrapper.h"
#include "mcrl2/action.h"
#include "mcrl2/list_iterator.h"
#include "mcrl2/term_list.h"
#include "mcrl2/predefined_symbols.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list_iterator;
using atermpp::read_from_named_file;

///////////////////////////////////////////////////////////////////////////////
// LPE_summand
/// \brief LPE summand.
///
class LPE_summand: public aterm_wrapper
{
  protected:
    data_variable_list   m_summation_variables;
    data_expression      m_condition;
    action_list          m_actions;       // m_actions == action_list() represents delta
    data_expression      m_time;          // empty aterm_appl represents no time
    data_assignment_list m_assignments;

  public:
    LPE_summand()
    {}

    LPE_summand(aterm_appl t)
     : aterm_wrapper(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();

      m_summation_variables = data_variable_list(*i++);
      m_condition           = data_expression(*i++);
      aterm_appl x          = *i++;
      if (x.function().name() != "Delta")
        m_actions = action_list(x.argument(0).to_list());
      m_time                = data_expression(*i++);
      m_assignments         = data_assignment_list(*i);
    }

    LPE_summand(data_variable_list   summation_variables,
                data_expression      condition,
                action_list          actions,
                data_expression      time,
                data_assignment_list assignments
               )
      :
        m_summation_variables(summation_variables),
        m_condition          (condition),
        m_actions            (actions),
        m_time               (time),
        m_assignments        (assignments)
    {}

    /// Returns the sequence of summation variables.
    ///
    data_variable_list summation_variables() const
    {
      return m_summation_variables;
    }

    /// Returns true if the multi-action corresponding to this LPE_summand is 
    /// equal to delta.
    ///
    bool is_delta() const
    {
      return m_actions == action_list();
    }

    /// Returns true if time is available.
    ///
    bool has_time()
    {
      return m_time.term() != aterm_appl();
    }

    /// Returns the time expression. This function may only be called if has_time
    /// returns true.
    ///
    data_expression time() const
    {
      return m_time;
    }

    /// Returns the condition expression.
    ///
    data_expression condition() const
    {
      return m_condition;
    }

    /// Returns the sequence of assignments.
    ///
    data_assignment_list assignments() const
    {
      return m_assignments;
    }

    /// Applies a substitution to this LPE_summand and returns the result.
    /// The Substitution object must supply the method aterm_appl operator()(aterm_appl).
    ///
    template <typename Substitution>
    LPE_summand substitute(Substitution f)
    {
      aterm_appl           condition           = f(m_condition);
      action_list          actions             = m_actions.substitute(f);
      aterm_appl           time                = f(m_time);

      return LPE_summand(summation_variables, condition, actions, time, assignments);
    }     

    /// Applies a sequence of substitutions to this LPE_summand and returns the result.
    ///
    template <typename SubstIter>
    LPE_summand substitute(SubstIter first, SubstIter last) const
    {
      data_expression      condition           = m_condition.substitute(first, last);
      action_list          actions             = m_actions  .substitute(first, last);
      data_expression      time                = m_time     .substitute(first, last);

      return LPE_summand(m_summation_variables, condition, actions, time, m_assignments);
    }

/*
    Iter next_states_begin(data_expression_list d)
    { 
    }
*/
};

typedef term_list<LPE_summand> summand_list;

///////////////////////////////////////////////////////////////////////////////
// LPE
/// \brief linear process equation.
///
class LPE
{
  protected:
    action_list   m_actions;
    data_variable_list m_free_variables;
    data_variable_list m_process_parameters;
    summand_list  m_summands;

  public:
    LPE()
    {}

    LPE(aterm_appl LPE, action_list actions)
    {
      // unpack LPE(.,.,.) term     
      aterm_list_iterator i = LPE.argument_list().begin();
      m_free_variables     = data_variable_list(*i++);
      m_process_parameters = data_variable_list(*i++);
      m_summands           = summand_list(*i);
      m_actions            = actions;
    }

    /// Returns the sequence of actions.
    ///
    action_list actions() const
    {
      return m_actions;
    }

    /// Returns the sequence of LPE summands.
    ///
    summand_list summands() const
    {
      return m_summands;
    }

    /// Returns the sequence of free variables.
    ///
    data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the sequence of process parameters.
    ///
    data_variable_list process_parameters() const
    {
      return m_process_parameters;
    }

    /// Applies a sequence of substitutions to this LPE and returns the result.
    ///
    template <typename SubstIter>
    LPE substitute(SubstIter first, SubstIter last) const
    {
      return LPE();
    }
};

} // namespace mcrl

#endif // MCRL2_LPE_H
