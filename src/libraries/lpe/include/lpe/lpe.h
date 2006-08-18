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
#include <set>
#include <iterator>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "atermpp/aterm.h"
#include "atermpp/aterm_list.h"
#include "lpe/aterm_wrapper.h"
#include "lpe/action.h"
#include "lpe/data.h"
#include "lpe/data_declaration.h"
#include "lpe/pretty_print.h"
#include "lpe/substitute.h"

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
      assert(gsIsLPESummand(t));
      aterm_list::iterator i = m_term.argument_list().begin();

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
      if (has_time() && !is_real(m_time))
      {
        return false;
      }
      if (!is_bool(m_condition))
      {
        return false;
      }
      return true;
    }
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
    {
      assert(is_well_typed());
      assert(is_name_clash_free(true));
    }

    LPE(aterm_appl lpe, action_list actions)
      : aterm_wrapper(lpe)
    {
      assert(gsIsLPE(lpe));
      assert(is_well_typed());
      assert(is_name_clash_free(true));

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
