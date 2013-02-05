// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/guard_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_GUARD_TRAVERSER_H
#define MCRL2_PBES_DETAIL_GUARD_TRAVERSER_H

#include "mcrl2/pbes/traverser.h"
#include "mcrl2/utilities/logger.h"
#include "mcrl2/utilities/optimized_boolean_operators.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

struct guard_expression
{
  atermpp::vector<std::pair<propositional_variable_instantiation, pbes_expression> > guards;
  pbes_expression condition;

  bool is_simple() const
  {
    return guards.empty();
  }

  void add_guard(const pbes_expression& guard)
  {
    if (is_simple())
    {
      condition = utilities::optimized_and(guard, condition);
    }
    else
    {
      for (atermpp::vector<std::pair<propositional_variable_instantiation, pbes_expression> >::iterator i = guards.begin(); i != guards.end(); ++i)
      {
        i->second = utilities::optimized_and(guard, i->second);
      }
    }
  }

  void negate()
  {
    if (is_simple())
    {
      condition = utilities::optimized_not(condition);
    }
    else
    {
      for (atermpp::vector<std::pair<propositional_variable_instantiation, pbes_expression> >::iterator i = guards.begin(); i != guards.end(); ++i)
      {
        i->second = utilities::optimized_not(i->second);
      }
    }
  }

  guard_expression(const pbes_expression& condition_ = pbes_system::true_())
    : condition(condition_)
  {}
};

inline
std::ostream& operator<<(std::ostream& out, const guard_expression& x)
{
  if (x.is_simple())
  {
    out << "condition = " << pbes_system::pp(x.condition) << std::endl;
  }
  else
  {
    for (atermpp::vector<std::pair<propositional_variable_instantiation, pbes_expression> >::const_iterator i = x.guards.begin(); i != x.guards.end(); ++i)
    {
      out << pbes_system::pp(i->first) << " guard = " << pbes_system::pp(i->second) << std::endl;
    }
  }
  return out;
}

/// \brief Computes a multimap of propositional variable instantiations and the corresponding guards from a PBES expression
struct guard_traverser: public pbes_expression_traverser<guard_traverser>
{
  typedef pbes_expression_traverser<guard_traverser> super;
  using super::enter;
  using super::leave;
  using super::operator();

  atermpp::vector<guard_expression> expression_stack;

  void push(const guard_expression& x)
  {
    mCRL2log(log::debug1) << "<push>" << "\n" << x << std::endl;
    expression_stack.push_back(x);
  }

  guard_expression& top()
  {
    return expression_stack.back();
  }

  const guard_expression& top() const
  {
    return expression_stack.back();
  }

  guard_expression pop()
  {
    guard_expression result = top();
    expression_stack.pop_back();
    return result;
  }

  void leave(const data::data_expression& x)
  {
    push(guard_expression(x));
  }

  void leave(const pbes_system::propositional_variable_instantiation& x)
  {
    guard_expression node;
    node.guards.push_back(std::make_pair(x, pbes_system::true_()));
    push(node);
  }

  void leave(const pbes_system::true_& x)
  {
    push(guard_expression(x));
  }

  void leave(const pbes_system::false_& x)
  {
    push(guard_expression(x));
  }

  void leave(const pbes_system::not_& x)
  {
    top().negate();
  }

  void leave(const pbes_system::and_& x)
  {
    guard_expression right = pop();
    guard_expression left = pop();
    pbes_expression new_condition = utilities::optimized_and(left.condition, right.condition);
    if (left.is_simple() && right.is_simple())
    {
      left.condition = new_condition;
      push(left);
    }
    else if (left.is_simple())
    {
      right.add_guard(left.condition);
      right.condition = new_condition;
      push(right);
    }
    else if (right.is_simple())
    {
      left.add_guard(right.condition);
      left.condition = new_condition;
      push(left);
    }
    else
    {
      left.add_guard(right.condition);
      right.add_guard(left.condition);
      left.guards.insert(left.guards.end(), right.guards.begin(), right.guards.end());
      left.condition = new_condition;
      push(left);
    }
  }

  void leave(const pbes_system::or_& x)
  {
    guard_expression right = pop();
    guard_expression left = pop();
    pbes_expression new_condition = utilities::optimized_or(left.condition, right.condition);
    if (left.is_simple() && right.is_simple())
    {
      left.condition = new_condition;
      push(left);
    }
    else if (left.is_simple())
    {
      right.add_guard(utilities::optimized_not(left.condition));
      right.condition = new_condition;
      push(right);
    }
    else if (right.is_simple())
    {
      left.add_guard(utilities::optimized_not(right.condition));
      left.condition = new_condition;
      push(left);
    }
    else
    {
      left.guards.insert(left.guards.end(), right.guards.begin(), right.guards.end());
      left.condition = new_condition;
      push(left);
    }
  }

  void leave(const pbes_system::imp& x)
  {
    guard_expression right = pop();
    guard_expression left = pop();
    left.negate();
    pbes_expression new_condition = utilities::optimized_or(left.condition, right.condition);
    if (left.is_simple() && right.is_simple())
    {
      left.condition = new_condition;
      push(left);
    }
    else if (left.is_simple())
    {
      right.add_guard(utilities::optimized_not(left.condition));
      right.condition = new_condition;
      push(right);
    }
    else if (right.is_simple())
    {
      left.add_guard(utilities::optimized_not(right.condition));
      left.condition = new_condition;
      push(left);
    }
    else
    {
      left.guards.insert(left.guards.end(), right.guards.begin(), right.guards.end());
      left.condition = new_condition;
      push(left);
    }
  }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

namespace atermpp
{

template<>
struct aterm_traits<std::pair<mcrl2::pbes_system::propositional_variable_instantiation, mcrl2::pbes_system::pbes_expression> >
{
  static void protect(const std::pair<mcrl2::pbes_system::propositional_variable_instantiation, mcrl2::pbes_system::pbes_expression>& t)
  {
    t.first.protect();
    t.second.protect();
  }

  static void unprotect(const std::pair<mcrl2::pbes_system::propositional_variable_instantiation, mcrl2::pbes_system::pbes_expression>& t)
  {
    t.first.unprotect();
    t.second.unprotect();
  }

  static void mark(const std::pair<mcrl2::pbes_system::propositional_variable_instantiation, mcrl2::pbes_system::pbes_expression>& t)
  {
    t.first.mark();
    t.second.mark();
  }
};

} // namespace atermpp

#endif // MCRL2_PBES_DETAIL_GUARD_TRAVERSER_H
