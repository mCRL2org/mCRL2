// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/find_free_variables.h
/// \brief Add your file description here.

#ifndef MCRL2_PBES_DETAIL_FIND_FREE_VARIABLES_H
#define MCRL2_PBES_DETAIL_FIND_FREE_VARIABLES_H

#include "mcrl2/pbes/traverser.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <vector>

namespace mcrl2::pbes_system::detail
{

template<typename OutputIterator>
struct find_free_variables_traverser : public pbes_expression_traverser<find_free_variables_traverser<OutputIterator>>
{
  using super = pbes_expression_traverser<find_free_variables_traverser<OutputIterator>>;
  using super::apply;
  using super::enter;
  using super::leave;

  data::variable_list bound_variables;
  std::vector<data::variable_list> quantifier_stack;
  OutputIterator out;
  bool search_propositional_variables;

  find_free_variables_traverser(OutputIterator out_, bool search_propositional_variables_ = true)
    : out(out_),
      search_propositional_variables(search_propositional_variables_)
  {}

  find_free_variables_traverser(OutputIterator out_,
    const data::variable_list& bound_variables_,
    bool search_propositional_variables_ = true)
    : bound_variables(bound_variables_),
      out(out_),
      search_propositional_variables(search_propositional_variables_)
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
    for (const data::variable_list& vars: quantifier_stack)
    {
      if (std::find(vars.begin(), vars.end(), v) != vars.end())
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

  void apply(const propositional_variable_instantiation& x)
  {
    if (search_propositional_variables)
    {
      std::vector<data::variable> variables;
      data::find_free_variables(x.parameters(), std::back_inserter(variables));
      for (const data::variable& v: variables)
      {
        if (!is_bound(v))
        {
          *out = v;
        }
      }
    }
  }

  void enter(const data::data_expression& x)
  {
    std::vector<data::variable> variables;
    data::find_free_variables(x, std::back_inserter(variables));
    for (const data::variable& v: variables)
    {
      if (!is_bound(v))
      {
        *out = v;
      }
    }
  }
};

template<typename OutputIterator>
void find_free_variables(const pbes_expression& x,
  OutputIterator out,
  const data::variable_list& bound_variables,
  bool search_propositional_variables = true)
{
  find_free_variables_traverser<OutputIterator> f(out, bound_variables, search_propositional_variables);
  f.apply(x);
}

inline std::set<data::variable> find_free_variables(const pbes_expression& x,
  const data::variable_list& bound_variables,
  bool search_propositional_variables = true)
{
  std::set<data::variable> result;
  find_free_variables(x, std::inserter(result, result.end()), bound_variables, search_propositional_variables);
  return result;
}

inline std::vector<data::variable> find_free_variables_in_order(const pbes_expression& x,
  const data::variable_list& bound_variables,
  bool search_propositional_variables = true)
{
  std::vector<data::variable> occurrences;
  find_free_variables(x, std::back_inserter(occurrences), bound_variables, search_propositional_variables);

  std::vector<data::variable> result;
  for (const data::variable& variable: occurrences)
  {
    if (std::find(result.begin(), result.end(), variable) == result.end())
    {
      result.push_back(variable);
    }
  }
  return result;
}

} // namespace mcrl2::pbes_system::detail

#endif // MCRL2_PBES_DETAIL_FIND_FREE_VARIABLES_H
