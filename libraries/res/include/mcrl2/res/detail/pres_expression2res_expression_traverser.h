// Author(s): Jan Friso Groote. Based on pbes/detail/pbes_expression2boolean_expression_traverser.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pres/detail/pres_expression2res_expression_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_RES_DETAIL_PRES_EXPRESSION2RES_EXPRESSION_TRAVERSER_H
#define MCRL2_RES_DETAIL_PRES_EXPRESSION2RES_EXPRESSION_TRAVERSER_H

#include "mcrl2/res/res_expression.h"
#include "mcrl2/pres/traverser.h"

namespace mcrl2
{

namespace res
{

namespace detail
{

struct pres_expression2res_expression_traverser: public pres_system::pres_expression_traverser<pres_expression2res_expression_traverser>
{
  typedef pres_system::pres_expression_traverser<pres_expression2res_expression_traverser> super;
  typedef core::term_traits<pres_system::pres_expression> tr;
  typedef core::term_traits<res::res_expression> br;

  using super::apply;
  using super::enter;
  using super::leave;

  /// \brief A stack containing res expressions.
  std::vector<res::res_expression> expression_stack;

  void push(const res::res_expression& x)
  {
    expression_stack.push_back(x);
  }

  res::res_expression pop()
  {
    assert(!expression_stack.empty());
    auto result = expression_stack.back();
    expression_stack.pop_back();
    return result;
  }

  /// \brief Returns the top element of the expression stack, which is the result of the conversion.
  res::res_expression result() const
  {
    assert(!expression_stack.empty());
    return expression_stack.back();
  }

  void leave(const data::data_expression& x)
  {
    if (tr::is_true(x))
    {
      push(br::true_());
    }
    else if (tr::is_false(x))
    {
      push(br::false_());
    }
    else
    {
      throw mcrl2::runtime_error("data_expression encountered in pres_expression2res_expression_traverser");
    }
  }

  void leave(const pres_system::minus&)
  {
    res::res_expression b = pop();
    push(br::minus(b));
  }

  void leave(const pres_system::and_&)
  {
    // join the two expressions on top of the stack
    res::res_expression right = pop();
    res::res_expression left  = pop();
    push(br::and_(left, right));
  }

  void leave(const pres_system::or_&)
  {
    // join the two expressions on top of the stack
    res::res_expression right = pop();
    res::res_expression left  = pop();
    push(br::or_(left, right));
  }

  void leave(const pres_system::imp&)
  {
    // join the two expressions on top of the stack
    res::res_expression right = pop();
    res::res_expression left  = pop();
    push(br::imp(left, right));
  }

  void enter(const pres_system::minall&)
  {
    throw mcrl2::runtime_error("Minall encountered in pres_expression2res_expression_traverser.");
  }

  void enter(const pres_system::maxall&)
  {
    throw mcrl2::runtime_error("Maxall encountered in pres_expression2res_expression_traverser.");
  }

  void leave(const pres_system::propositional_variable_instantiation& x)
  {
    if (x.parameters().size() > 0)
    {
      throw mcrl2::runtime_error("Propositional variable with parameters encountered in pres_expression2res_expression_traverser.");
    }
    push(res::res_variable(x.name()));
  }
};

} // namespace detail

inline
res_expression pres_expression2res_expression(const pres_system::pres_expression& x)
{
  detail::pres_expression2res_expression_traverser f;
  f.apply(x);
  return f.result();
}

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_DETAIL_PRES_EXPRESSION2RES_EXPRESSION_TRAVERSER_H
