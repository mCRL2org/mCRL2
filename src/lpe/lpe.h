///////////////////////////////////////////////////////////////////////////////
/// \file LPE.h
/// Contains LPE data structures for the LPE Library.

#ifndef LPE_LPE_H
#define LPE_LPE_H

#include <functional>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cassert>
#include "atermpp/aterm.h"
#include "lpe/aterm_wrapper.h"
#include "lpe/action.h"
#include "atermpp/aterm_list.h"
#include "lpe/pretty_print.h"
#include "lpe/substitute.h"

namespace lpe {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm_appl;
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
    bool                 m_delta;         // m_delta == true represents no multi-action
    action_list          m_actions;
    data_expression      m_time;          // m_time == data_expression() represents no time available
    data_assignment_list m_assignments;

  public:
    LPE_summand()
    {}

    LPE_summand(aterm_appl t)
     : aterm_wrapper(t)
    {
      aterm_list::iterator i = m_term.argument_list().begin();

      m_summation_variables = data_variable_list(*i++);
      m_condition           = data_expression(*i++);
      aterm_appl x          = *i++;
      m_delta = x.function().name() == "Delta";
      if (!m_delta)
      {
        assert(x.function().name() == "MultAct");
        m_actions = action_list(x.argument(0));
      }
      m_time                = data_expression(*i++);
      m_assignments         = data_assignment_list(*i);
    }

    /// Constructs an LPE_summand with a multi-action.
    ///
    LPE_summand(data_variable_list   summation_variables,
                data_expression      condition,
                bool                 delta,
                action_list          actions,
                data_expression      time,
                data_assignment_list assignments
               )
      : aterm_wrapper(gsMakeLPESummand(summation_variables,
               condition,
               (delta ? gsMakeDelta() : gsMakeMultAct(actions)),
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
      return m_delta;
    }

    /// Returns true if time is available.
    ///
    bool has_time() const
    {
      return !m_time.is_nil();
    }

    /// Returns the condition expression.
    ///
    data_expression condition() const
    {
      return m_condition;
    }

    /// Returns the sequence of actions. Returns an empty list if is_delta() holds.
    ///
    action_list actions() const
    {
      return m_actions;
    }

    /// Returns the time expression.
    ///
    data_expression time() const
    {
      return m_time;
    }

    /// Returns the sequence of assignments.
    ///
    data_assignment_list assignments() const
    {
      return m_assignments;
    }

    /// Applies a substitution to this LPE_summand and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    LPE_summand substitute(Substitution f) const
    {
      action_list actions;
      data_expression condition = m_condition.substitute(f);
      if (!m_delta)
      {
        actions = substitute(m_actions, f);
      }
      data_expression time = m_time.substitute(f);

      return LPE_summand(m_summation_variables, condition, m_delta, actions, time, m_assignments);
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
class LPE: public aterm_wrapper
{
  protected:
    data_variable_list m_free_variables;
    data_variable_list m_process_parameters;
    summand_list       m_summands;
    action_list        m_actions;

  public:
    LPE()
    {}

    LPE(data_variable_list free_variables,
        data_variable_list process_parameters,
        summand_list       summands,
        action_list        actions
       )
     : aterm_wrapper(gsMakeLPE(free_variables, process_parameters, summands)),
       m_free_variables    (free_variables    ),
       m_process_parameters(process_parameters),
       m_summands          (summands          ),
       m_actions           (actions           )
    {}

    LPE(aterm_appl lpe, action_list actions)
      : aterm_wrapper(lpe)
    {
      // unpack LPE(.,.,.) term     
      aterm_list::iterator i = lpe.argument_list().begin();
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

    /// Applies a substitution to this LPE and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    LPE substitute(Substitution f)
    {
      data_variable_list d = lpe::substitute(m_free_variables    , f);
      data_variable_list p = lpe::substitute(m_process_parameters, f);
      summand_list       s = lpe::substitute(m_summands          , f);
      action_list        a = lpe::substitute(m_actions           , f);
      return LPE(d, p, s, a);
    }     

    /// Returns a pretty print representation of the term.
    ///
    std::string pp() const
    {
      std::string s1 = lpe::pp(m_free_variables    );
      std::string s2 = lpe::pp(m_process_parameters);
      std::string s3 = lpe::pp(m_summands          );
      std::string s4 = lpe::pp(m_actions           );
      return s1 + "\n" + s2 + "\n" + s3 + "\n" + s4;
    }

    /// Returns a representation of the term.
    ///
    std::string to_string() const
    {
      std::string s1 = m_free_variables    .to_string();
      std::string s2 = m_process_parameters.to_string();
      std::string s3 = m_summands          .to_string();
      std::string s4 = m_actions           .to_string();
      return s1 + "\n" + s2 + "\n" + s3 + "\n" + s4;
    }
};

} // namespace mcrl

#endif // LPE_LPE_H
