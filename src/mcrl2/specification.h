///////////////////////////////////////////////////////////////////////////////
/// \file specification.h
/// 
/// Contains LPE data structures for the mcrl2 library.

#ifndef MCRL2_SPECIFICATION_H
#define MCRL2_SPECIFICATION_H

#include <iostream>
#include <vector>
#include <utility>

#include "atermpp/aterm.h"
#include "mcrl2/function.h"
#include "mcrl2/lpe.h"
#include "mcrl2/predefined_symbols.h"
#include "mcrl2/aterm_wrapper.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list_iterator;
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
class specification: public aterm_wrapper
{
  protected:
    sort_list       m_sorts;
    function_list   m_constructors;
    function_list   m_mappings;
    data_equation_list   m_equations;
    action_list     m_actions;
    data_variable_list   m_init_variables;
    data_assignment_list m_init_assignments;

    // DataDeclaration m_data_declaration;
    data_expression_list m_initial_state;
    LPE m_lpe;

    /// Create a list containing the right hand sides of the initial assignments.
    ///
    data_expression_list compute_initial_state(data_assignment_list assignments)
    {
      std::vector<data_expression> expressions;
      expressions.reserve(assignments.size());
      for (data_assignment_list::iterator i = assignments.begin(); i != assignments.end(); ++i)
      {
        expressions.push_back(i->rhs());
      }
      return data_expression_list(expressions.begin(), expressions.end());
    }

    /// Initialize the LPE with an aterm_appl.
    ///
    void init_term(aterm_appl t)
    {
      m_term = t;
      aterm_list_iterator i = m_term.argument_list().begin();
      m_sorts               = sort_list(aterm_appl(*i++).argument(0));
      m_constructors        = function_list(aterm_appl(*i++).argument(0));
      m_mappings            = function_list(aterm_appl(*i++).argument(0));
      m_equations           = data_equation_list(aterm_appl(*i++).argument(0));
      m_actions             = action_list(*i++);
      aterm_appl lpe        = *i++;
      aterm_appl lpe_init   = *i;

      m_lpe = LPE(lpe, m_actions);

      // unpack LPEInit(.,.) term
      // compute m_initial_state from lpe_init
      aterm_list_iterator k       = lpe_init.argument_list().begin();
      m_init_variables            = data_variable_list(*k++);
      m_init_assignments          = data_assignment_list(*k);
      data_expression_list d0(m_init_variables.to_ATermList());
      m_initial_state = compute_initial_state(m_init_assignments);
    }

    /// Returns the assignments of the initial state.
    ///
    data_assignment_list initial_assignments() const
    {
      return m_init_assignments;
    }

  public:
    specification()
    {}

    specification(aterm_appl t)
      : aterm_wrapper(t)
    {
      init_term(t);
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

    /// Returns the sequence of sorts.
    ///
    sort_list sorts()
    { return m_sorts; }

    /// Returns the sequence of constructors.
    ///
    function_list constructors()
    { return m_constructors; }

    /// Returns the sequence of mappings.
    ///
    function_list mappings()
    { return m_mappings; }

    /// Returns the sequence of data equations.
    ///
    data_equation_list equations()
    { return m_equations; }

    /// Returns the initial state of the LPE.
    ///
    data_expression_list initial_state() const
    {
      return data_expression_list(m_initial_state);
    }

    /// Returns the sequence of free variables of the initial state.
    ///
    data_variable_list initial_free_variables() const
    {
      return m_init_variables;
    }

    /// Returns the initial assingments of the LPE.
    ///
    data_assignment_list init_assignments() const
    {
      return data_assignment_list(m_init_assignments);
    }
};

} // namespace mcrl

#endif // MCRL2_SPECIFICATION_H
