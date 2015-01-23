// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_expression2boolean_expression_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_BES_DETAIL_PBES_EXPRESSION2BOOLEAN_EXPRESSION_TRAVERSER_H
#define MCRL2_BES_DETAIL_PBES_EXPRESSION2BOOLEAN_EXPRESSION_TRAVERSER_H

#include "mcrl2/bes/boolean_expression.h"
#include "mcrl2/pbes/traverser.h"

namespace mcrl2
{

namespace bes
{

namespace detail
{

struct pbes_expression2boolean_expression_traverser: public pbes_system::pbes_expression_traverser<pbes_expression2boolean_expression_traverser>
{
  typedef pbes_system::pbes_expression_traverser<pbes_expression2boolean_expression_traverser> super;
  typedef core::term_traits<pbes_system::pbes_expression> tr;
  typedef core::term_traits<bes::boolean_expression> br;

  using super::apply;
  using super::enter;
  using super::leave;

  /// \brief A stack containing boolean expressions.
  std::vector<bes::boolean_expression> expression_stack;

  void push(const bes::boolean_expression& x)
  {
    expression_stack.push_back(x);
  }

  bes::boolean_expression pop()
  {
    assert(!expression_stack.empty());
    auto result = expression_stack.back();
    expression_stack.pop_back();
    return result;
  }

  /// \brief Returns the top element of the expression stack, which is the result of the conversion.
  bes::boolean_expression result() const
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
      throw mcrl2::runtime_error("data_expression encountered in pbes_expression2boolean_expression_traverser");
    }
  }

  void leave(const pbes_system::not_&)
  {
    bes::boolean_expression b = pop();
    push(br::not_(b));
  }

  void leave(const pbes_system::and_&)
  {
    // join the two expressions on top of the stack
    bes::boolean_expression right = pop();
    bes::boolean_expression left  = pop();
    push(br::and_(left, right));
  }

  void leave(const pbes_system::or_&)
  {
    // join the two expressions on top of the stack
    bes::boolean_expression right = pop();
    bes::boolean_expression left  = pop();
    push(br::or_(left, right));
  }

  void leave(const pbes_system::imp&)
  {
    // join the two expressions on top of the stack
    bes::boolean_expression right = pop();
    bes::boolean_expression left  = pop();
    push(br::imp(left, right));
  }

  void enter(const pbes_system::forall&)
  {
    throw mcrl2::runtime_error("forall encountered in pbes_expression2boolean_expression_traverser");
  }

  void enter(const pbes_system::exists&)
  {
    throw mcrl2::runtime_error("exists encountered in pbes_expression2boolean_expression_traverser");
  }

  void leave(const pbes_system::propositional_variable_instantiation& x)
  {
    if (x.parameters().size() > 0)
    {
      throw mcrl2::runtime_error("propositional variable with parameters encountered in pbes_expression2boolean_expression_traverser");
    }
    push(bes::boolean_variable(x.name()));
  }
};

} // namespace detail

inline
boolean_expression pbes_expression2boolean_expression(const pbes_system::pbes_expression& x)
{
  detail::pbes_expression2boolean_expression_traverser f;
  f.apply(x);
  return f.result();
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_DETAIL_PBES_EXPRESSION2BOOLEAN_EXPRESSION_TRAVERSER_H
