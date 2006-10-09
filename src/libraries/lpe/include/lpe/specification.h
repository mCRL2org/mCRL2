///////////////////////////////////////////////////////////////////////////////
/// \file specification.h
/// 
/// Contains LPE data structures for the LPE Library.

#ifndef LPE_SPECIFICATION_H
#define LPE_SPECIFICATION_H

#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

#include "atermpp/aterm.h"
#include "lpe/function.h"
#include "lpe/lpe.h"
#include "lpe/pretty_print.h"
#include "lpe/aterm_wrapper.h"
#include "lpe/data_declaration.h"

namespace lpe {

using atermpp::aterm_appl;
using atermpp::read_from_named_file;

///////////////////////////////////////////////////////////////////////////////
// Specification
/// \brief mCRL2 specification
///
// sort ...;
// 
// cons ...;
// 
// map ...;
// 
// eqn ...;
// 
// proc P(b: Bool, n: Nat) = a(b).P() + sum c: Bool. b -\> e@1.P(b := c);
// 
// init P(true, 0);
// 
// parameters -\> b, n
// initial_state -\> true, 0
// data -\> ...
// summands -\> [[],true,a(b),nil,[]], [[c],b,e,1,[b := c]]
//
// syntax: SpecV1(SortSpec(list<sort> sorts),
//                ConsSpec(list<operation> constructors),
//                MapSpec(list<operation> mappings),
//                DataEqnSpec(list<data_equation> equations),
//                ActSpec(list<action> actions),
//                LPE(list<data_variable> free_variables, list<data_variable> process_parameters, list<LPESummand> lpe_summands),
//         )
class specification: public aterm_appl_wrapper
{
  protected:
    data_declaration     m_data;
    action_list          m_actions;
    LPE                  m_lpe;
    data_variable_list   m_initial_free_variables;
    data_assignment_list m_initial_assignments;

    /// Create a list containing the left hand sides of the initial assignments.
    ///
    data_variable_list compute_initial_variables(data_assignment_list assignments) const
    {
      std::vector<data_variable> variables;
      variables.reserve(assignments.size());
      for (data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
      {
        variables.push_back(i->lhs());
      }
      return data_variable_list(variables.begin(), variables.end());
    }

    /// Create a list containing the right hand sides of the initial assignments.
    ///
    data_expression_list compute_initial_state(data_assignment_list assignments) const
    {
      std::vector<data_expression> expressions;
      expressions.reserve(assignments.size());
      for (data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
      {
        expressions.push_back(i->rhs());
      }
      return data_expression_list(expressions.begin(), expressions.end());
    }

    /// Create assignments for the initial state.
    ///
    data_assignment_list compute_initial_assignments(data_variable_list variables, data_expression_list initial_state) const
    {
      std::vector<data_assignment> assignments;
      assignments.reserve(variables.size());
      data_expression_list::iterator j = initial_state.begin();
      for (data_variable_list::iterator i = variables.begin(); i != variables.end(); ++i, ++j)
      {
        assignments.push_back(data_assignment(*i, *j));
      }
      return data_assignment_list(assignments.begin(), assignments.end());
    }

    /// Initialize the LPE with an aterm_appl.
    ///
    void init_term(aterm_appl t)
    {
      m_term = t;
      aterm_list::iterator i = arguments().begin();
      sort_list          sorts        = sort_list(aterm_appl(*i++).argument(0));
      function_list      constructors = function_list(aterm_appl(*i++).argument(0));
      function_list      mappings     = function_list(aterm_appl(*i++).argument(0));
      data_equation_list equations    = data_equation_list(aterm_appl(*i++).argument(0));
      m_data = data_declaration(sorts, constructors, mappings, equations);
      m_actions                       = action_list(aterm_appl(*i++).argument(0));
      aterm_appl lpe                  = *i++;
      aterm_appl lpe_init             = *i;

      m_lpe = LPE(lpe, m_actions);

      // unpack LPEInit(.,.) term
      aterm_list::iterator k         = lpe_init.argument_list().begin();
      m_initial_free_variables       = data_variable_list(*k++);
      m_initial_assignments          = data_assignment_list(*k);
      data_expression_list d0(m_initial_free_variables);
    }

  public:
    specification()
    {}

    specification(aterm_appl t)
      : aterm_appl_wrapper(t)
    {
      assert(gsIsSpecV1(t));
      init_term(t);
    }

    specification(
        sort_list            sorts            ,
        function_list        constructors     ,
        function_list        mappings         ,
        data_equation_list   equations        ,
        action_list          actions          ,
        LPE                  lpe              ,
        data_variable_list   initial_free_variables,
        data_variable_list   initial_variables,
        data_expression_list initial_state)
      :
        m_data(sorts, constructors, mappings, equations),
        m_actions       (actions       ),
        m_lpe           (lpe           ),
        m_initial_free_variables(initial_free_variables),        
        m_initial_assignments(compute_initial_assignments(initial_variables, initial_state))
    {
      assert(initial_variables.size() == initial_state.size());
      m_term = gsMakeSpecV1(
          gsMakeSortSpec(sorts),
          gsMakeConsSpec(constructors),
          gsMakeMapSpec(mappings),
          gsMakeDataEqnSpec(equations),
          gsMakeActSpec(actions),
          lpe,
          gsMakeLPEInit(initial_free_variables, m_initial_assignments)
      );        
    }

    specification(
        data_declaration     data             ,
        action_list          actions          ,
        LPE                  lpe              ,
        data_variable_list   initial_free_variables,
        data_variable_list   initial_variables,
        data_expression_list initial_state)
      :
        m_data(data),
        m_actions(actions),
        m_lpe(lpe),
        m_initial_free_variables(initial_free_variables),        
        m_initial_assignments(compute_initial_assignments(initial_variables, initial_state))
    {
      assert(initial_variables.size() == initial_state.size());
      m_term = gsMakeSpecV1(
          gsMakeSortSpec(data.sorts()),
          gsMakeConsSpec(data.constructors()),
          gsMakeMapSpec(data.mappings()),
          gsMakeDataEqnSpec(data.equations()),
          gsMakeActSpec(actions),
          lpe,
          gsMakeLPEInit(initial_free_variables, m_initial_assignments)
      );        
    }

    /// Reads the LPE from file. Returns true if the operation succeeded.
    ///
    bool load(const std::string& filename)
    {
      aterm_appl t = atermpp::read_from_named_file(filename);
      if (!t)
        return false;
      init_term(t);
      return true;
    }

    /// Writes the LPE to file. Returns true if the operation succeeded.
    ///
    bool save(const std::string& filename, bool binary = true)
    {
      if (binary)
      {
        return atermpp::write_to_named_binary_file(m_term, filename);
      }
      else
      {
        return atermpp::write_to_named_text_file(m_term, filename);
      }
    }

    /// Returns the LPE of the specification.
    ///
    LPE lpe() const
    {
      return m_lpe;
    }

    /// Returns the data declaration.
    ///
    data_declaration data() const
    { return m_data; }

    /// Returns the data declaration.
    ///
    data_declaration& data()
    { return m_data; }

    /// Returns the sequence of sorts.
    ///
    sort_list sorts() const
    { return m_data.sorts(); }

    /// Sets the sequence of sorts.
    ///
    void set_sorts(sort_list sorts)
    { m_data.set_sorts(sorts); }

    /// Returns the sequence of constructors.
    ///
    function_list constructors() const
    { return m_data.constructors(); }

    /// Sets the sequence of constructors.
    ///
    void set_constructors(function_list constructors)
    { m_data.set_constructors(constructors); }

    /// Returns the sequence of mappings.
    ///
    function_list mappings() const
    { return m_data.mappings(); }

    /// Sets the sequence of mappings.
    ///
    void set_mappings(function_list mappings)
    { m_data.set_mappings(mappings); }

    /// Returns the sequence of data equations.
    ///
    data_equation_list equations() const
    { return m_data.equations(); }

    /// Sets the sequence of data equations.
    ///
    void set_equations(data_equation_list equations)
    { m_data.set_equations(equations); }

    /// Returns the sequence of actions.
    ///
    action_list actions() const
    { return m_actions; }

    /// Returns the initial state of the LPE.
    ///
    data_expression_list initial_state() const
    {
      return compute_initial_state(m_initial_assignments);
    }

    /// Returns the variables of the initial state of the LPE.
    ///
    data_variable_list initial_variables() const
    {
      return compute_initial_variables(m_initial_assignments);
    }

    /// Returns the sequence of free variables of the initial state.
    ///
    data_variable_list initial_free_variables() const
    {
      return m_initial_free_variables;
    }

    /// Returns the assignments of the initial state.
    ///
    data_assignment_list initial_assignments() const
    {
      return m_initial_assignments;
    }
};


inline
specification set_data_declaration(specification spec, data_declaration data)
{
  return specification(data,
                       spec.actions(),
                       spec.lpe(),
                       spec.initial_free_variables(),
                       spec.initial_variables(),
                       spec.initial_state()
                      );
}

inline
specification set_actions(specification spec, action_list actions)
{
  return specification(spec.data(),
                       actions,
                       spec.lpe(),
                       spec.initial_free_variables(),
                       spec.initial_variables(),
                       spec.initial_state()
                      );
}

inline
specification set_lpe(specification spec, LPE lpe)
{
  return specification(spec.data(),
                       spec.actions(),
                       lpe,
                       spec.initial_free_variables(),
                       spec.initial_variables(),
                       spec.initial_state()
                      );
}

inline
specification set_initial_free_variables(specification spec, data_variable_list initial_free_variables)
{
  return specification(spec.data(),
                       spec.actions(),
                       spec.lpe(),
                       initial_free_variables,
                       spec.initial_variables(),
                       spec.initial_state()
                      );
}

inline
specification set_initial_variables(specification spec, data_variable_list initial_variables)
{
  return specification(spec.data(),
                       spec.actions(),
                       spec.lpe(),
                       spec.initial_free_variables(),
                       initial_variables,
                       spec.initial_state()
                      );
}

inline
specification set_initial_state(specification spec, data_expression_list initial_state)
{
  return specification(spec.data(),
                       spec.actions(),
                       spec.lpe(),
                       spec.initial_free_variables(),
                       spec.initial_variables(),
                       initial_state
                      );
}

} // namespace lpe

#endif // LPE_SPECIFICATION_H
