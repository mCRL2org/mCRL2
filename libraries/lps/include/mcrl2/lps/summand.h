// Copyright (c) 2007 Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/summand.h
/// \brief Add your file description here.

#ifndef MCRL2_LPS_SUMMAND_H
#define MCRL2_LPS_SUMMAND_H

#include <string>
#include <cassert>
#include <algorithm>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/algorithm.h"
#include "atermpp/utility.h"
#include "mcrl2/data/data.h"
#include "mcrl2/lps/action.h"
#include "mcrl2/lps/detail/utility.h"

namespace lps {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm_appl;

///////////////////////////////////////////////////////////////////////////////
// summand
/// \brief LPS summand.
///
// <LinearProcessSummand>   ::= LinearProcessSummand(<DataVarId>*, <DataExpr>, <MultActOrDelta>,
//                    <DataExprOrNil>, <Assignment>*)
//<MultActOrDelta>
//               ::= <MultAct>
//                 | Delta
// <MultAct>      ::= MultAct(<ParamId>*)                                   (- tc)
//                  | MultAct(<Action>*)                                    (+ tc)
class summand: public aterm_appl
{
  protected:
    data_variable_list   m_summation_variables;
    data_expression      m_condition;
    bool                 m_delta;         // m_delta == true represents no multi-action
    action_list          m_actions;
    data_expression      m_time;          // m_time == data_expression() represents no time available
    data_assignment_list m_assignments;

  public:
    summand()
      : aterm_appl(detail::constructLinearProcessSummand())
    {}

    summand(aterm_appl t)
     : aterm_appl(t)
    {
      assert(detail::check_rule_LinearProcessSummand(m_term));
      aterm_appl::iterator i = t.begin();

      m_summation_variables = data_variable_list(*i++);
      m_condition           = data_expression(*i++);
      aterm_appl x          = *i++;
      m_delta = gsIsDelta(x);
      if (!m_delta)
      {
        assert(gsIsMultAct(x));
        m_actions = action_list(x.argument(0));
      }
      m_time                = data_expression(*i++);
      m_assignments         = data_assignment_list(*i);
    }

    /// Constructs an untimed summand.
    ///
    summand(data_variable_list   summation_variables,
                data_expression      condition,
                bool                 delta,
                action_list          actions,
                data_assignment_list assignments
               )
      : aterm_appl(gsMakeLinearProcessSummand(summation_variables,
               condition,
               (delta ? gsMakeDelta() : gsMakeMultAct(actions)),
               gsMakeNil(),
               assignments)
        ),
        m_summation_variables(summation_variables),
        m_condition          (condition),
        m_delta              (delta),
        m_actions            (actions),
        m_time               (data_expression(gsMakeNil())),
        m_assignments        (assignments)
    {}

    /// Constructs a timed summand.
    ///
    summand(data_variable_list   summation_variables,
                data_expression      condition,
                bool                 delta,
                action_list          actions,
                data_expression      time,
                data_assignment_list assignments
               )
      : aterm_appl(gsMakeLinearProcessSummand(summation_variables,
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

    /// Returns true if the multi-action corresponding to this summand is 
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
      return !data_expr::is_nil(m_time);
    }

    /// Returns the condition expression.
    ///
    data_expression condition() const
    {
      return m_condition;
    }

    /// Returns the sequence of actions.
    /// Returns an empty list if is_delta() holds.
    ///
    action_list actions() const
    {
      return m_actions;
    }

    /// Returns the time.
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

    /// Applies a substitution to this summand and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    summand substitute(Substitution f) const
    {
      action_list actions;
      data_expression condition = m_condition.substitute(f);
      if (!m_delta)
      {
        actions = m_actions.substitute(f);
      }
      data_expression time = m_time.substitute(f);

      return summand(m_summation_variables, condition, m_delta, actions, time, m_assignments);
    }     

    /// Returns true if
    /// <ul>
    /// <li>the data assignments are well typed</li>
    /// <li>the (optional) time has sort Real</li>
    /// <li>the condition has sort Bool</li>
    /// <li>the summation variables have unique names</li>
    /// </ul>
    ///
    bool is_well_typed() const
    {
      // check 1)
      for (data_assignment_list::iterator i = m_assignments.begin(); i != m_assignments.end(); ++i)
      {
        if (!i->is_well_typed())
          return false;
      }

      // check 2)
      if (has_time() && !data_expr::is_real(m_time))
      {
        std::cerr << "summand::is_well_typed() failed: time " << pp(m_time) << " doesn't have type real." << std::endl;
        return false;
      }

      // check 3)
      if (!data_expr::is_bool(m_condition))
      {
        std::cerr << "summand::is_well_typed() failed: condition " << pp(m_condition) << " doesn't have type bool." << std::endl;
        return false;
      }

      // check 4)
      if (!detail::unique_names(m_summation_variables))
      {
        std::cerr << "summand::is_well_typed() failed: summation variables " << pp(m_summation_variables) << " don't have unique names." << std::endl;
        return false;
      }

      return true;
    }
};

/// \brief Sets the summation variables of s and returns the result
inline
summand set_summation_variables(summand s, data_variable_list summation_variables)
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
inline
summand set_condition(summand s, data_expression condition)
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
inline
summand set_time(summand s, data_expression time)
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
inline
summand set_assignments(summand s, data_assignment_list assignments)
{
  return summand(s.summation_variables(),
                 s.condition          (),
                 s.is_delta           (),
                 s.actions            (),
                 s.time               (),
                 assignments 
                );
}

///////////////////////////////////////////////////////////////////////////////
// summand_list
/// \brief singly linked list of summands
typedef term_list<summand> summand_list;

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::summand;

template<>
struct aterm_traits<summand>
{
  typedef ATermAppl aterm_type;
  static void protect(summand t)   { t.protect(); }
  static void unprotect(summand t) { t.unprotect(); }
  static void mark(summand t)      { t.mark(); }
  static ATerm term(summand t)     { return t.term(); }
  static ATerm* ptr(summand& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // MCRL2_LPS_SUMMAND_H
