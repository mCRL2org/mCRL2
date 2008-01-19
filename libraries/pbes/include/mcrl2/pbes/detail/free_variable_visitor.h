// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file free_variable_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_FREE_VARIABLE_VISITOR_H
#define MCRL2_PBES_DETAIL_FREE_VARIABLE_VISITOR_H

#include "mcrl2/data/utility.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace lps {

namespace detail {

using namespace mcrl2::data;

struct free_variable_visitor: public pbes_expression_visitor
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

  bool visit_forall(const pbes_expression& e, const data_variable_list& v, const pbes_expression&)
  {
    push(v);
    return true;
  }

  void leave_forall()
  {
    pop();
  }

  bool visit_exists(const pbes_expression& e, const data_variable_list& v, const pbes_expression&)
  {
    push(v);
    return true;
  }

  void leave_exists()
  {
    pop();
  }

  bool visit_propositional_variable(const pbes_expression& e, const propositional_variable_instantiation& v)
  {
    std::set<data_variable> variables = mcrl2::data::find_variables(v.parameters());
    for (std::set<data_variable>::iterator i = variables.begin(); i != variables.end(); ++i)
    {
      if (!is_bound(*i))
      {
        result.insert(*i);
      }
    }
    return true;
  }

  bool visit_data_expression(const pbes_expression& e, const data_expression& d)
  {
    std::set<data_variable> variables = mcrl2::data::find_variables(d);
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

} // namespace detail

} // namespace lps

#endif // MCRL2_PBES_DETAIL_FREE_VARIABLE_VISITOR_H

