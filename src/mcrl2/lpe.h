///////////////////////////////////////////////////////////////////////////////
/// \file lpe.h
/// Contains LPE data structures for the mcrl2 library.

#ifndef MCRL2_LPE_H
#define MCRL2_LPE_H

#include <iostream>
#include <vector>
#include <utility>
#include "atermpp/aterm.h"
#include "mcrl2/action.h"
#include "mcrl2/data_declaration.h"
#include "mcrl2/list_iterator.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list_iterator;

///////////////////////////////////////////////////////////////////////////////
// LPEAssignment
/// \brief LPEAssignment to a data variable.
///
class LPEAssignment
{
  protected:
    aterm_appl                      m_term;        // keep the original aterm for reference
    aterm_appl                      m_lhs;         // left hand side of the assignment
    aterm_appl                      m_rhs;         // right hand side of the assignment

  public:
    LPEAssignment(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      m_lhs = *i++;
      m_rhs = *i;
    }
    
    /// Returns the left hand side of the LPEAssignment.
    ///
    DataVariable lhs() const
    {
      return DataVariable(m_lhs);
    }

    /// Returns the right hand side of the LPEAssignment.
    ///
    DataExpression rhs() const
    {
      return DataExpression(m_rhs);
    }

    /// Returns the internal representation of the LPEAssignment.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};


///////////////////////////////////////////////////////////////////////////////
// LPEInit
/// \brief initialization fo an LPE.
///
class LPEInit
{
  protected:
    aterm_appl m_term;           // keep the original aterm for reference
    aterm_list m_free_variables; // elements are of type DataVariable
    aterm_list m_assignments;    // elements are of type LPEAssignment

  public:
    typedef list_iterator<DataVariable>   variable_iterator;
    typedef list_iterator<LPEAssignment>  assignment_iterator;

    LPEInit(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      m_free_variables = *i++;
      m_assignments    = *i;
    }

    /// Returns a begin iterator to the sequence of free variables.
    ///
    variable_iterator free_variables_begin() const
    {
      return variable_iterator(m_free_variables);
    }

    /// Returns an end iterator to the sequence of free variables.
    ///
    variable_iterator free_variables_end() const
    {
      return variable_iterator();
    }

    /// Returns a begin iterator to the sequence of assignments.
    ///
    assignment_iterator assignments_begin() const
    {
      return assignment_iterator(m_assignments);
    }

    /// Returns an end iterator to the sequence of assignments.
    ///
    assignment_iterator assignments_end() const
    {
      return assignment_iterator();
    }

    /// Returns the internal representation of the LPEInit.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};

///////////////////////////////////////////////////////////////////////////////
// LPEState
/// \brief state of an LPE.
//
// LPEState
// 
// P(true,0)
// 
// free_variables -\> \<empty\>
// expressions -\> true, 0
// 
// 
// LPESummand
// 
// sum c: Bool. b -\> e@1.P(b := c)
// 
// sum_variables -\> c
// condition -\> b
// action -\> e		(kan delta zijn)
// time -\> 1		(kan nil zijn)
// state_change -\> b := c
// 
// next_states([true,0]) -\> (e,[true,0]), (e,[false,0])	(hier mist tijd nog)
//
class LPEState
{
  protected:
    aterm_appl m_term;              // keep the original aterm for reference
    aterm_list m_expressions;       // elements are of type DataExpression
    aterm_list m_free_variables;    // elements are of type DataVariable

  public:
    typedef list_iterator<DataExpression> expression_iterator;
    typedef list_iterator<DataVariable>   variable_iterator;

    LPEState()
    {}

    LPEState(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      m_expressions    = *i++;
      m_free_variables = *i;
    }

    /// Returns a begin iterator to the sequence of expressions.
    ///
    expression_iterator expressions_begin() const
    {
      return expression_iterator(m_expressions);
    }

    /// Returns an end iterator to the sequence of expressions.
    ///
    expression_iterator expressions_end() const
    {
      return expression_iterator();
    }

    /// Returns a begin iterator to the sequence of free variables.
    ///
    variable_iterator free_variables_begin() const
    {
      return variable_iterator(m_free_variables);
    }

    /// Returns an end iterator to the sequence of free variables.
    ///
    variable_iterator free_variables_end() const
    {
      return variable_iterator();
    }

    /// Returns the internal representation of the LPEState.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};

///////////////////////////////////////////////////////////////////////////////
// LPESummand
/// \brief LPE summand.
///
class LPESummand
{
  protected:
    aterm_appl m_term;                 // keep the original aterm for reference
    aterm_list m_summation_variables;
    aterm_appl m_condition;
    aterm_list m_actions;              // !m_actions represents delta
    aterm_appl m_time;                 // empty appl represents nil
    aterm_list m_assignments;

  public:
    typedef list_iterator<DataVariable>   variable_iterator;
    typedef list_iterator<Action>         action_iterator;
    typedef list_iterator<LPEAssignment>  assignment_iterator;

    LPESummand(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();

      m_summation_variables = *i++;
      m_condition           = *i++;
      aterm_appl x          = *i++;
      if (x.function().name() != "Delta")
        m_actions = x.argument(0).to_list();
      m_time                = *i++;
      m_assignments         = *i;
    }

    /// Returns a begin iterator to the sequence of summation variables.
    ///
    variable_iterator summation_variables_begin() const
    {
      return variable_iterator(m_summation_variables);
    }

    /// Returns an end iterator to the sequence of summation variables.
    ///
    variable_iterator summation_variables_end() const
    {
      return variable_iterator();
    }

    /// Returns the condition of the summand (must be of type bool).
    ///
    DataExpression condition() const                        // conditie (van type bool)
    {
      return DataExpression(m_condition);
    }

    /// Returns true if the multi-action corresponding to this LPESummand is 
    /// equal to delta.
    /// This function may only be called if is_delta returns false.
    /// Only in debug mode an assertion check is done for this.
    ///
    bool is_delta() const
    {
      return !m_actions;
    }

    /// Returns a begin iterator to the sequence of actions.
    /// This function may only be called if is_delta returns false.
    /// Only in debug mode an assertion check is done for this.
    ///
    action_iterator actions_begin() const
    {
      assert(!is_delta());
      return action_iterator(m_actions);
    }

    /// Returns an end iterator to the sequence of actions.
    /// This function may only be called if is_delta returns false.
    /// Only in debug mode an assertion check is done for this.
    ///
    action_iterator actions_end() const
    {
      assert(!is_delta());
      return action_iterator();
    }

    /// Returns the time.
    ///
    DataExpression time()
    {
      return DataExpression(m_time);
    }

    /// Returns a begin iterator to the sequence of assignments.
    ///
    assignment_iterator assignments_begin() const
    {
      return assignment_iterator(m_assignments);
    }

    /// Returns an end iterator to the sequence of assignments.
    ///
    assignment_iterator assignments_end() const
    {
      return assignment_iterator();
    }

    /// Returns the internal representation of the LPE.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }

    /// NOT YET IMPLEMENTED!
    /// Returns the state if the condition is true, otherwise ???
    ///
    std::vector<LPEAssignment> result_state(LPEState s); // geeft state terug als de conditie waar is, anders ?
};


///////////////////////////////////////////////////////////////////////////////
// LPE
/// \brief linear proces equation.
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
class LPE
{
  protected:
    aterm_appl m_term;                  // keep the original aterm for reference
    DataDeclaration m_data_declaration;
    aterm_list m_actions;               // elements are of type Action
    aterm_list m_free_variables;        // elements are of type DataVariable
    aterm_list m_process_parameters;    // elements are of type DataVariable
    aterm_list m_lpe_summands;          // elements are of type LPESummand
    LPEState   m_initial_state;
    aterm_appl m_lpe_init;

  public:
    typedef list_iterator<LPESummand>     summand_iterator;
    typedef list_iterator<DataVariable>   variable_iterator;
    typedef list_iterator<Action>         action_iterator;

    LPE()
    {}

    LPE(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      aterm_list sorts        = aterm_appl(*i++).argument(0);
      aterm_list constructors = aterm_appl(*i++).argument(0);
      aterm_list mappings     = aterm_appl(*i++).argument(0);
      aterm_list equations    = aterm_appl(*i++).argument(0);
      m_actions               = *i++;
      aterm_appl lpe          = *i++;
      m_lpe_init              = *i;

      m_data_declaration = DataDeclaration(sorts, constructors, mappings, equations);

      // unpack LPE(.,.,.) term
      aterm_list_iterator j = lpe.argument_list().begin();
      m_free_variables     = *j++;
      m_process_parameters = *j++;
      m_lpe_summands       = *j;
      
      // initial state
      // TODO: compute m_initial_state from lpe_init
    }

    /// Returns the data declaration part of the LPE.
    ///
    DataDeclaration data_declaration()
    {
      return m_data_declaration;
    } 

    /// Returns a begin iterator to the sequence of actions.
    ///
    action_iterator actions_begin() const
    {
      return action_iterator(m_actions);
    }

    /// Returns an end iterator to the sequence of actions.
    ///
    action_iterator actions_end() const
    {
      return action_iterator();
    }

    /// Returns a begin iterator to the sequence of LPE summands.
    ///
    summand_iterator summands_begin() const
    {
      return summand_iterator(m_lpe_summands);
    }

    /// Returns an end iterator to the sequence of LPE summands.
    ///
    summand_iterator summands_end() const
    {
      return summand_iterator();
    }

    /// NOT YET IMPLEMENTED!
    /// Returns the initial state of the LPE.
    ///
    LPEState initial_state() const
    {
      return m_initial_state;
    }

    /// Returns the internal representation of the LPE.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }

    /// Returns a begin iterator to the sequence of free variables.
    ///
    variable_iterator free_variables_begin() const
    {
      return variable_iterator(m_free_variables);
    }

    /// Returns an end iterator to the sequence of variables.
    ///
    variable_iterator free_variables_end() const
    {
      return variable_iterator();
    }

    /// Returns a begin iterator to the sequence of process parameters.
    ///
    variable_iterator process_parameters_begin() const
    {
      return variable_iterator(m_process_parameters);
    }

    /// Returns an end iterator to the sequence of variables.
    ///
    variable_iterator process_parameters_end() const
    {
      return variable_iterator();
    }

    LPEInit lpe_init() const
    {
      return LPEInit(m_lpe_init);
    }

    /// NOT YET IMPLEMENTED!
    /// Returns a sequence of all possible transitions.
    ///
    std::vector<std::pair<Action, LPEState> > next_state(LPEState s)
    {
    }

    /// NOT YET IMPLEMENTED!
    /// Implicitly returns a sequence of all possible transitions. The transitions
    /// are computed by applying the given assignments.
    ///
    std::vector<std::pair<Action, LPEAssignment> > next_state_difference(LPEState s)
    {
    }
};

} // namespace mcrl

#endif // MCRL2_LPE_H
