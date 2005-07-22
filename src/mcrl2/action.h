///////////////////////////////////////////////////////////////////////////////
/// \file sort.h
/// Contains action data structures for the mcrl2 library.

#ifndef MCRL2_ACTION_H
#define MCRL2_ACTION_H

#include "atermpp/aterm.h"
#include "mcrl2/data.h"
#include "mcrl2/list_iterator.h"

namespace mcrl2 {

using atermpp::aterm_appl;
using atermpp::aterm_list;
using atermpp::aterm_list_iterator;

///////////////////////////////////////////////////////////////////////////////
// Action
/// \brief action id.
//
// An action id can not be modified.
//
// Example:
//
// Action
// 
// a(n,m)
// 
// Action(ActId("a",[Nat,Nat]),
//  	[DataVarId("n",SortId("Nat")),DataVarId("n",SortId("Nat"))])
// 
// identifier -\> ActId("a",[Nat,Nat])
// arguments -\> DataVarId("n",SortId("Nat")), DataVarId("n",SortId("Nat"))

class Action
{
  protected:
    aterm_appl m_term;         // keep the original aterm for reference
    aterm_appl m_name;
    aterm_list m_arguments;    // elements are of type DataExpression

  public:
    typedef list_iterator<DataExpression> expression_iterator;

    Action()
    {}

    Action(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      m_name      = *i++;
      m_arguments = *i;
    }

    /// Returns the name of the action.
    ///
    std::string name() const
    {
      return m_name.to_string();
    }

    /// Returns a begin iterator to the sequence of arguments.
    ///
    expression_iterator arguments_begin() const
    {
      return expression_iterator(m_arguments);
    }

    /// Returns an end iterator to the sequence of arguments.
    ///
    expression_iterator arguments_end() const
    {
      return expression_iterator();
    }

    /// Returns the internal representation of the Action.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};


///////////////////////////////////////////////////////////////////////////////
// ActionExpression
/// \brief action expression.
//
// An action expression is a sequence of actions.
//
// a
// 
// ActId("a",[Nat,Nat])
// 
// name -\> "a"
// type -\> Nat, Nat
// 
class ActionExpression
{
  protected:
    aterm_appl m_term;         // keep the original aterm for reference
    aterm_appl m_name;
    aterm_list m_sorts;        // elements are of type Sort

  public:
    typedef list_iterator<Sort>   sort_iterator;

    ActionExpression()
    {}

    ActionExpression(aterm_appl t)
     : m_term(t)
    {
      aterm_list_iterator i = m_term.argument_list().begin();
      m_name  = *i++;
      m_sorts = *i;
    }

    /// Returns the name of the action.
    ///
    std::string name() const
    {
      return m_name.to_string();
    }

    /// Returns a begin iterator to the sequence of arguments.
    ///
    sort_iterator sorts_begin() const
    {
      return sort_iterator(m_sorts);
    }

    /// Returns an end iterator to the sequence of arguments.
    ///
    sort_iterator sorts_end() const
    {
      return sort_iterator();
    }

    /// Returns the internal representation of the ActionExpression.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};


///////////////////////////////////////////////////////////////////////////////
// ActionDeclaration
/// \brief action declaration.
//
// An action declaration is a sequence of action identifiers.
//
// Example:
//
// ActionDeclaration
// 
// act a: Nat#Nat;
//      b;
// 
// ActSpec([ActId("a",[Nat,Nat]),ActId("b",[])])
// 
// identifiers -\> ActId("a",[Nat,Nat]), ActId("b",[])
class ActionDeclaration
{
  protected:
    aterm_appl m_term;         // keep the original aterm for reference
    aterm_list m_identifiers;  // elements are of type ActionExpression

  public:
    typedef list_iterator<ActionExpression> action_expression_iterator;

    ActionDeclaration()
    {}

    ActionDeclaration(aterm_appl t)
     : m_term(t)
    {
      m_identifiers = m_term.argument(0).to_list();
    }

    /// Returns a begin iterator to the sequence of identifiers.
    ///
    action_expression_iterator identifiers_begin() const
    {
      return action_expression_iterator(m_identifiers);
    }

    /// Returns an end iterator to the sequence of identifiers.
    ///
    action_expression_iterator identifiers_end() const
    {
      return action_expression_iterator();
    }

    /// Returns the internal representation of the ActionExpression.
    ///
    std::string to_string() const
    {
      return m_term.to_string();
    }
};


} // namespace mcrl

#endif // MCRL2_ACTION_H
