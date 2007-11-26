// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_variables.h
/// \brief Add your file description here.

#ifndef MCRL2_BASIC_FREE_VARIABLES_H
#define MCRL2_BASIC_FREE_VARIABLES_H

#include "mcrl2/modal_formula/state_formula_visitor.h"

namespace lps {

namespace state_frm {

struct free_variable_visitor: public state_formula_visitor
{
  data_variable_list bound_variables;
  std::vector<data_variable_list> quantifier_stack;
  std::set<data_variable> result;

  free_variable_visitor()
  {}

  free_variable_visitor(data_variable_list bound_variables_)
    : bound_variables(bound_variables_)
  {}

  // returns true if v is an element of bound_variables or quantifier_stack
  bool is_bound(const data_variable& v) const
  {
    if (std::find(bound_variables.begin(), bound_variables.end(), v) != bound_variables.end())
    {
      return true;
    }
    for (std::vector<data_variable_list>::const_iterator i = quantifier_stack.begin(); i != quantifier_stack.end(); ++i)
    {
      if (std::find(i->begin(), i->end(), v) != i->end())
      {
        return true;
      }
    }
    return false;
  }

  void push(const data_variable_list& v)
  {
    quantifier_stack.push_back(v);
  }

  void pop()
  {
    quantifier_stack.pop_back();
  }

  bool visit_forall(const state_formula& e, const data_variable_list& v, const state_formula&)
  {
    push(v);
    return true;
  }

  void leave_forall()
  {
    pop();
  }

  bool visit_exists(const state_formula& e, const data_variable_list& v, const state_formula&)
  {
    push(v);
    return true;
  }

  void leave_exists()
  {
    pop();
  }

  bool visit_var(const state_formula& /* e */, const identifier_string& /* n */, const data_expression_list& l)
  {
    std::set<data_variable> variables = find_variables(l);
    for (std::set<data_variable>::iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (!is_bound(*i))
      {
        result.insert(*i);
      }
    }
    return true;
  }

  bool visit_data_expression(const state_formula& /* e */, const data_expression& d)
  {
    std::set<data_variable> variables = find_variables(d);
    for (std::set<data_variable>::iterator i = variables.begin(); i != variables.end(); ++i)
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

/// Computes the free variables that occur in the state formula f.
inline
std::set<data_variable> compute_free_state_formula_variables(const state_formula& f)
{
  state_frm::free_variable_visitor visitor;
  visitor.visit(f);
  return visitor.result;
}

} // namespace lps

#endif // MCRL2_BASIC_FREE_VARIABLES_H
