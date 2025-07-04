// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/significant_variables.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_SIGNIFICANT_VARIABLES_H
#define MCRL2_PBES_SIGNIFICANT_VARIABLES_H

#include "mcrl2/pbes/traverser.h"

namespace mcrl2::pbes_system
{

namespace detail {

struct significant_variables_traverser: public pbes_expression_traverser<significant_variables_traverser>
{
  using super = pbes_expression_traverser<significant_variables_traverser>;
  using super::enter;
  using super::leave;
  using super::apply;

  std::vector<std::set<data::variable> > result_stack;

  // Push a sig set to result_stack
  void push(const std::set<data::variable>& v)
  {
    result_stack.push_back(v);
  }

  // Pop the top element of result_stack and return it
  std::set<data::variable> pop()
  {
    std::set<data::variable> result = std::move(result_stack.back());
    result_stack.pop_back();
    return result;
  }

  // Return the top element of result_stack
  std::set<data::variable>& top()
  {
    return result_stack.back();
  }

  // Return the top element of result_stack
  const std::set<data::variable>& top() const
  {
    return result_stack.back();
  }

  // Merges the two elements A1 and A2 at the top of the stack into a single
  // stack element representing union(A1, A2)
  void join()
  {
    std::set<data::variable> right = pop();
    // left == top(), the (previously) second element on the stack.
    top().insert(std::make_move_iterator(right.begin()), std::make_move_iterator(right.end()));
  }

  void leave(const and_& /* x */)
  {
    join();
  }

  void leave(const or_& /* x */)
  {
    join();
  }

  void leave(const imp& /* x */)
  {
    join();
  }

  void leave(const exists& x)
  {
    for (const data::variable& v: x.variables())
    {
      top().erase(v);
    }
  }

  void leave(const forall& x)
  {
    for (const data::variable& v: x.variables())
    {
      top().erase(v);
    }
  }

  void leave(const propositional_variable_instantiation&)
  {
    push(std::set<data::variable>());
  }

  void leave(const data::data_expression& x)
  {
    push(data::find_free_variables(x));
  }
};

} // namespace detail

inline
std::set<data::variable> significant_variables(const pbes_expression& x)
{
  detail::significant_variables_traverser f;
  f.apply(x);
  return f.pop();
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_SIGNIFICANT_VARIABLES_H
