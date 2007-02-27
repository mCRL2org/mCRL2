///////////////////////////////////////////////////////////////////////////////
/// \file linear_process.h
/// Contains LPE data structures for the LPE Library.

#ifndef LPE_LPE_H
#define LPE_LPE_H

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
#include "lpe/action.h"
#include "lpe/data.h"
#include "lpe/data_specification.h"
#include "lpe/pretty_print.h"
#include "lpe/detail/specification_utility.h"

namespace {
  /// INTERNAL ONLY
  /// Returns a set containing the names of the variables in l.
  inline
  std::set<std::string> make_string_set(lpe::data_variable_list l)
  {
    std::set<std::string> result;
    for (lpe::data_variable_list::iterator i = l.begin(); i != l.end(); ++i)
    {
      result.insert(i->name());
    }
    return result;
  }

  inline
  std::string print_string_set(const std::set<std::string>& v)
  {
    std::ostringstream out;
    std::copy(v.begin(), v.end(), std::ostream_iterator<std::string>(out, " "));
    return out.str();
  }
} // unnamed namespace

namespace lpe {

using namespace std::rel_ops; // for definition of operator!= in terms of operator==
using atermpp::aterm_appl;
using atermpp::read_from_named_file;

///////////////////////////////////////////////////////////////////////////////
// summand
/// \brief LPE summand.
///
// <LPESummand>   ::= LPESummand(<DataVarId>*, <DataExpr>, <MultActOrDelta>,
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
    {}

    summand(aterm_appl t)
     : aterm_appl(t)
    {
      assert(check_rule_LPESummand(m_term));
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
      : aterm_appl(gsMakeLPESummand(summation_variables,
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
      : aterm_appl(gsMakeLPESummand(summation_variables,
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
    /// 1) all data assignments are well typed
    /// 2) the (optional) time has sort Real
    /// 3) the condition has sort Bool
    bool is_well_typed() const
    {
      for (data_assignment_list::iterator i = m_assignments.begin(); i != m_assignments.end(); ++i)
      {
        if (!i->is_well_typed())
          return false;
      }
      if (has_time() && !data_expr::is_real(m_time))
      {
        return false;
      }
      if (!data_expr::is_bool(m_condition))
      {
        return false;
      }
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
// LPE
/// \brief linear process equation.
///
class linear_process: public aterm_appl
{
  protected:
    data_variable_list m_free_variables;
    data_variable_list m_process_parameters;
    summand_list       m_summands;

    typedef std::vector<std::pair<summand_list::iterator, std::set<std::string> > > name_clash_list;

    /// Returns all name clashes between variables names occurring in a sum operator
    /// of a summand and variable names occurring in the process parameters of the LPE.
    name_clash_list variable_name_clashes() const
    {
      std::vector<std::pair<summand_list::iterator, std::set<std::string> > > result;
      std::set<std::string> pv = make_string_set(process_parameters());
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        std::set<std::string> sv = make_string_set(i->summation_variables());
        std::set<std::string> v;
        std::set_intersection(pv.begin(), pv.end(), sv.begin(), sv.end(), std::inserter(v, v.end()));
        if (!v.empty())
        {
          result.push_back(make_pair(i, v));
        }
      }
      return result;
    }

  public:
    /// Returns true if all summands are well typed
    bool is_well_typed() const
    {
      for (summand_list::iterator i = m_summands.begin(); i != m_summands.end(); ++i)
      {
        if (!i->is_well_typed())
          return false;
      }
      return true;
    }

    /// Returns true if there are no name clashes between variables in all sum operators
    /// of the summands and the process parameters of the LPE. If print is true, an
    /// overview of the name clashes will be printed to cerr.
    bool is_name_clash_free(bool print=false) const
    {
      name_clash_list v = variable_name_clashes();
      if (print && !v.empty()) {
        for (name_clash_list::const_iterator i = v.begin(); i != v.end(); ++i)
        {
          std::cerr << "LPE " << i->first->to_string() << " has name clashes " << print_string_set(i->second) << std::endl;
        }
      }
      return v.empty();
    }

    linear_process()
    {}

    linear_process(data_variable_list free_variables,
        data_variable_list process_parameters,
        summand_list       summands
       )
     : aterm_appl(gsMakeLPE(free_variables, process_parameters, summands)),
       m_free_variables    (free_variables    ),
       m_process_parameters(process_parameters),
       m_summands          (summands          )
    {
      assert(is_well_typed());
      assert(is_name_clash_free(true));
    }

    linear_process(aterm_appl lpe)
      : aterm_appl(lpe)
    {
      assert(check_term_LPE(m_term));
      assert(is_well_typed());
      assert(is_name_clash_free(true));

      // unpack LPE(.,.,.) term     
      aterm_appl::iterator i = lpe.begin();
      m_free_variables     = data_variable_list(*i++);
      m_process_parameters = data_variable_list(*i++);
      m_summands           = summand_list(*i);
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

    /// Applies a substitution to this LPE and returns the result.
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
// LPEInit(<DataVarId>*, <DataVarIdInit>*)
class process_initializer: public aterm_appl
{
  protected:
    data_variable_list   m_free_variables;
    data_assignment_list m_assignments;

  public:
    process_initializer()
    {}

    process_initializer(data_variable_list free_variables,
                        data_assignment_list assignments
                       )
     : aterm_appl(gsMakeLPEInit(free_variables, assignments)),
       m_free_variables(free_variables),
       m_assignments(assignments)
    {
    }

    process_initializer(aterm_appl t)
      : aterm_appl(t)
    {
      assert(check_term_LPEInit(m_term));
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

    /// Returns the initial state of the LPE.
    ///
    data_expression_list state() const
    {
      return detail::compute_initial_state(m_assignments);
    }

    /// Applies a substitution to this LPE and returns the result.
    /// The Substitution object must supply the method aterm operator()(aterm).
    ///
    template <typename Substitution>
    process_initializer substitute(Substitution f)
    {
      return process_initializer(f(aterm(*this)));
    }     
};

} // namespace lpe

/// INTERNAL ONLY
namespace atermpp
{
using lpe::summand;
using lpe::linear_process;
using lpe::process_initializer;

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

#endif // LPE_LPE_H
