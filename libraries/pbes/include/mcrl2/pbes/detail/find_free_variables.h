// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/free_variable_visitor.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_FIND_FREE_VARIABLES_H
#define MCRL2_PBES_DETAIL_FIND_FREE_VARIABLES_H

#include "mcrl2/data/find.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2
{

namespace pbes_system
{

namespace detail
{

struct find_free_variables_traverser: public pbes_expression_traverser<find_free_variables_traverser>
{
  typedef pbes_expression_traverser<find_free_variables_traverser> super;
  using super::enter;
  using super::leave;
  using super::apply;

  data::variable_list bound_variables;
  std::vector<data::variable_list> quantifier_stack;
  std::set<data::variable> result;
  bool search_propositional_variables;

  find_free_variables_traverser(bool search_propositional_variables_ = true)
    : search_propositional_variables(search_propositional_variables_)
  {}

  find_free_variables_traverser(const data::variable_list& bound_variables_, bool search_propositional_variables_ = true)
    : bound_variables(bound_variables_), search_propositional_variables(search_propositional_variables_)
  {}

  /// \brief Returns true if v is an element of bound_variables or quantifier_stack
  /// \param v A data variable
  /// \return True if v is an element of bound_variables or quantifier_stack
  bool is_bound(const data::variable& v) const
  {
    if (std::find(bound_variables.begin(), bound_variables.end(), v) != bound_variables.end())
    {
      return true;
    }
    for (auto i = quantifier_stack.begin(); i != quantifier_stack.end(); ++i)
    {
      if (std::find(i->begin(), i->end(), v) != i->end())
      {
        return true;
      }
    }
    return false;
  }

  /// \brief Pushes v on the stack of quantifier variables
  /// \param v A sequence of data variables
  void push(const data::variable_list& v)
  {
    quantifier_stack.push_back(v);
  }

  /// \brief Pops the stack of quantifier variables
  void pop()
  {
    quantifier_stack.pop_back();
  }

  void enter(const forall& x)
  {
    push(x.variables());
  }

  void leave(const forall&)
  {
    pop();
  }

  void enter(const exists& x)
  {
    push(x.variables());
  }

  void leave(const exists&)
  {
    pop();
  }

  void enter(const propositional_variable& x)
  {
    if (search_propositional_variables)
    {
      std::set<data::variable> variables = data::find_free_variables(x.parameters());
      for (auto i = variables.begin(); i != variables.end(); ++i)
      {
        if (!is_bound(*i))
        {
          result.insert(*i);
        }
      }
    }
  }

  void enter(const data::data_expression& x)
  {
    std::set<data::variable> variables = data::find_free_variables(x);
    for (auto i = variables.begin(); i != variables.end(); ++i)
    {
      if (!is_bound(*i))
      {
        result.insert(*i);
      }
    }
  }
};

inline
std::set<data::variable> find_free_variables(const pbes_expression& x, const data::variable_list& bound_variables = data::variable_list(), bool search_propositional_variables = true)
{
  find_free_variables_traverser f(bound_variables, search_propositional_variables);
  f.apply(x);
  return f.result;
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_FIND_FREE_VARIABLES_H
