///////////////////////////////////////////////////////////////////////////////
/// \file linear_process.h
/// Contains LPS data structures for the LPS Library.

#ifndef LPS_LPS_H
#define LPS_LPS_H

#include <functional>
#include <iostream>
#include <vector>
#include <utility>
#include <string>
#include <cassert>
#include <set>
#include <iterator>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "atermpp/algorithm.h"
#include "atermpp/utility.h"
#include "lps/action.h"
#include "lps/data.h"
#include "lps/data_specification.h"
#include "lps/data_utility.h"
#include "lps/pretty_print.h"
#include "lps/detail/specification_utility.h"
#include "lps/detail/utility.h" // unique_names

namespace lps {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm_appl;
using atermpp::read_from_named_file;

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

    /// Constructs an summand with a multi-action.
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

    /// Constructs an summand with a multi-action.
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
    /// <li>the left hand sides of the assignments are contained in the set of summation variables</li>
    /// </ul>
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
        return false;
      }

      // check 3)
      if (!data_expr::is_bool(m_condition))
      {
        return false;
      }

      // check 4)
      if (!detail::unique_names(m_summation_variables))
      {
        return false;
      }

      // check 5)
      // if (!detail::check_assignment_variables(m_assignments, m_summation_variables))
      // {
      //   return false;
      // }

      return true;
    }
};

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

inline
summand set_delta(summand s, bool delta)
{
  return summand(s.summation_variables(),
                     s.condition          (),
                     delta,
                     s.actions            (),
                     s.time               (),
                     s.assignments        () 
                    );
}

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

typedef term_list<summand> summand_list;

///////////////////////////////////////////////////////////////////////////////
// LPS
/// \brief linear process.
///
class linear_process: public aterm_appl
{
  protected:
    data_variable_list m_free_variables;
    data_variable_list m_process_parameters;
    summand_list       m_summands;

  public:
    linear_process()
      : aterm_appl(detail::constructLinearProcess())
    {}

    linear_process(data_variable_list free_variables,
        data_variable_list process_parameters,
        summand_list       summands
       )
     : aterm_appl(gsMakeLinearProcess(free_variables, process_parameters, summands)),
       m_free_variables    (free_variables    ),
       m_process_parameters(process_parameters),
       m_summands          (summands          )
    { }

    linear_process(aterm_appl lps)
      : aterm_appl(lps)
    {
      assert(detail::check_term_LinearProcess(m_term));

      // unpack LPS(.,.,.) term     
      aterm_appl::iterator i = lps.begin();
      m_free_variables     = data_variable_list(*i++);
      m_process_parameters = data_variable_list(*i++);
      m_summands           = summand_list(*i);
    }

    /// Returns the sequence of LPS summands.
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

    /// Returns true if time is available in at least one of the summands.
    ///
    bool has_time() const
    {
      for (summand_list::iterator i = summands().begin(); i != summands().end(); ++i)
      {
        if(i->has_time()) return true;
      }
      return false;
    }

    /// Applies a substitution to this LPS and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    linear_process substitute(Substitution f)
    {
      data_variable_list d = m_free_variables    .substitute(f);
      data_variable_list p = m_process_parameters.substitute(f);
      summand_list       s = m_summands          .substitute(f);
      return linear_process(d, p, s);
    }     

    /// Returns a representation of the term.
    ///
    std::string to_string() const
    {
      std::string s1 = m_free_variables    .to_string();
      std::string s2 = m_process_parameters.to_string();
      std::string s3 = m_summands          .to_string();
      return s1 + "\n" + s2 + "\n" + s3;
    }

    /// Returns the set of free variables that appear in the process. This set
    /// is a subset of <tt>free_variables()</tt>.
    std::set<data_variable> find_free_variables()
    {
      // TODO: the efficiency of this implementation is not optimal
      std::set<data_variable> result;
      std::set<data_variable> parameters = detail::make_set(process_parameters());
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        std::set<data_variable> summation_variables = detail::make_set(i->summation_variables());
        std::set<data_variable> used_variables = find_variables(make_list(i->condition(), i->actions(), i->time()));
        std::set<data_variable> bound_variables = detail::set_union(parameters, summation_variables);
        std::set<data_variable> free_variables = detail::set_difference(used_variables, bound_variables);
        result.insert(free_variables.begin(), free_variables.end());
      }
      return result;
    }
    /// Returns true if
    /// <ul>
    /// <li>the summands are well typed</li>
    /// <li>the process parameters have unique names</li>
    /// <li>the free variables have unique names</li>
    /// <li>the names of the process parameters do not appear as the name of a summation variable</li>
    /// </ul>
    bool is_well_typed() const
    {
      // check 1)
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!i->is_well_typed())
          return false;
      }

      // check 2)
      if (!detail::unique_names(m_process_parameters))
      {
        return false;
      }

      // check 3)
      if (!detail::unique_names(m_free_variables))
      {
        return false;
      }

      // check 4)
      std::set<identifier_string> names;
      for (data_variable_list::iterator i = m_process_parameters.begin(); i != m_process_parameters.end(); ++i)
      {
        names.insert(i->name());
      }
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!detail::check_variable_names(i->summation_variables(), names))
          return false;
      }

      return true;
    }
  };

inline
linear_process set_free_variables(linear_process l, data_variable_list free_variables)
{
  return linear_process(free_variables,
             l.process_parameters(),
             l.summands          ()
            );
}

inline
linear_process set_process_parameters(linear_process l, data_variable_list process_parameters)
{
  return linear_process(l.free_variables    (),
             process_parameters,
             l.summands          ()
            );
}

inline
linear_process set_summands(linear_process l, summand_list summands)
{
  return linear_process(l.free_variables    (),
             l.process_parameters(),
             summands
            );
}

/// \brief initial linear process
// LinearProcessInit(<DataVarId>*, <DataVarIdInit>*)
class process_initializer: public aterm_appl
{
  protected:
    data_variable_list   m_free_variables;
    data_assignment_list m_assignments;

  public:
    process_initializer()
      : aterm_appl(detail::constructLinearProcessInit())
    {}

    process_initializer(data_variable_list free_variables,
                        data_assignment_list assignments
                       )
     : aterm_appl(gsMakeLinearProcessInit(free_variables, assignments)),
       m_free_variables(free_variables),
       m_assignments(assignments)
    {
    }

    process_initializer(aterm_appl t)
      : aterm_appl(t)
    {
      assert(detail::check_term_LinearProcessInit(m_term));
      aterm_appl::iterator i   = t.begin();
      m_free_variables = *i++;
      m_assignments    = *i;
    }

    /// Returns the sequence of free variables.
    ///
    data_variable_list free_variables() const
    {
      return m_free_variables;
    }

    /// Returns the sequence of assignments.
    ///
    data_assignment_list assignments() const
    {
      return m_assignments;
    }

    /// Returns the initial state of the LPS.
    ///
    data_expression_list state() const
    {
      return detail::compute_initial_state(m_assignments);
    }

    /// Applies a substitution to this LPS and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    process_initializer substitute(Substitution f)
    {
      return process_initializer(f(aterm(*this)));
    }     
};

} // namespace lps

/// INTERNAL ONLY
namespace atermpp
{
using lps::summand;
using lps::linear_process;
using lps::process_initializer;

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

template<>
struct aterm_traits<linear_process>
{
  typedef ATermAppl aterm_type;
  static void protect(linear_process t)   { t.protect(); }
  static void unprotect(linear_process t) { t.unprotect(); }
  static void mark(linear_process t)      { t.mark(); }
  static ATerm term(linear_process t)     { return t.term(); }
  static ATerm* ptr(linear_process& t)    { return &t.term(); }
};

template<>
struct aterm_traits<process_initializer>
{
  typedef ATermAppl aterm_type;
  static void protect(process_initializer t)   { t.protect(); }
  static void unprotect(process_initializer t) { t.unprotect(); }
  static void mark(process_initializer t)      { t.mark(); }
  static ATerm term(process_initializer t)     { return t.term(); }
  static ATerm* ptr(process_initializer& t)    { return &t.term(); }
};

} // namespace atermpp

#endif // LPS_LPS_H
