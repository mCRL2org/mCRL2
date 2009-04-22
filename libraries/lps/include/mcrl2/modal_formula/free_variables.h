// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_variables.h
/// \brief Add your file description here.

#ifndef MCRL2_MODAL_FREE_VARIABLES_H
#define MCRL2_MODAL_FREE_VARIABLES_H

#include <set>
#include <vector>
#include "mcrl2/modal_formula/state_formula_visitor.h"
#include "mcrl2/data/data.h"
#include "mcrl2/data/find.h"

namespace mcrl2 {

namespace modal {

namespace state_frm {

/// \brief Visitor class for finding free variables in a state formula
struct free_variable_visitor: public state_formula_visitor
{
  /// \brief A sequence of bound variables
  data::variable_list bound_variables;

  /// \brief A stack with quantifier variables
  std::vector<data::variable_list> quantifier_stack;

  /// \brief Contains the solution
  std::set<data::variable> result;

  /// \brief Constructor
  free_variable_visitor()
  {}

  /// \brief Constructor
  /// \param bound_variables_ A sequence of data variables
  free_variable_visitor(data::variable_list bound_variables_)
    : bound_variables(bound_variables_)
  {}

  /// \brief Returns true if v is an element of bound_variables or quantifier_stack
  /// \return True if v is an element of bound_variables or quantifier_stack
  /// \param v A data variable
  bool is_bound(const data::variable& v) const
  {
    if (std::find(bound_variables.begin(), bound_variables.end(), v) != bound_variables.end())
    {
      return true;
    }
    for (std::vector<data::variable_list>::const_iterator i = quantifier_stack.begin(); i != quantifier_stack.end(); ++i)
    {
      if (std::find(i->begin(), i->end(), v) != i->end())
      {
        return true;
      }
    }
    return false;
  }

  /// \brief Pushes a sequence of data variables on the stack
  /// \param v A sequence of data variables
  void push(const data::variable_list& v)
  {
    quantifier_stack.push_back(v);
  }

  /// \brief Pops the stack
  void pop()
  {
    quantifier_stack.pop_back();
  }

  /// \brief Visit forall node
  /// \param e A modal formula
  /// \param v A sequence of data variables
  /// \return The result of visiting the node
  bool visit_forall(const state_formula& e, const data::variable_list& v, const state_formula&)
  {
    push(v);
    return true;
  }

  /// \brief Leave forall node
  void leave_forall()
  {
    pop();
  }

  /// \brief Visit exists node
  /// \param e A modal formula
  /// \param v A sequence of data variables
  /// \return The result of visiting the node
  bool visit_exists(const state_formula& e, const data::variable_list& v, const state_formula&)
  {
    push(v);
    return true;
  }

  /// \brief Leave exists node
  void leave_exists()
  {
    pop();
  }

  /// \brief Visit var node
  /// \param l A sequence of data expressions
  /// \return The result of visiting the node
  bool visit_var(const state_formula& /* e */, const core::identifier_string& /* n */, const data::data_expression_list& l)
  {
    std::set<data::variable> variables = find_all_variables(l);
    for (std::set<data::variable>::const_iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (!is_bound(*i))
      {
        result.insert(*i);
      }
    }
    return true;
  }

  /// \brief Visit data_expression node
  /// \param d A data expression
  /// \return The result of visiting the node
  bool visit_data_expression(const state_formula& /* e */, const data::data_expression& d)
  {
    std::set<data::variable> variables = find_all_variables(d);
    for (std::set<data::variable>::const_iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (!is_bound(*i))
      {
        result.insert(*i);
      }
    }
    return true;
  }
};

} // namespace state_frm

/// \brief Returns the free variables that occur in the state formula f.
/// \param f A modal formula
/// \return The free variables that occur in the state formula f.
inline
std::set<data::variable> compute_free_state_formula_variables(const state_formula& f)
{
  state_frm::free_variable_visitor visitor;
  visitor.visit(f);
  return visitor.result;
}

} // namespace modal

} // namespace mcrl2

#endif // MCRL2_MODAL_FREE_VARIABLES_H
