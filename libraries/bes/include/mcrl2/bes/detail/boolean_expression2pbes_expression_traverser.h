// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/detail/boolean_expression2pbes_expression_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_BES_DETAIL_BOOLEAN_EXPRESSION2PBES_EXPRESSION_TRAVERSER_H
#define MCRL2_BES_DETAIL_BOOLEAN_EXPRESSION2PBES_EXPRESSION_TRAVERSER_H

#include <cassert>
#include <vector>
#include "mcrl2/bes/traverser.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2
{

namespace bes
{

namespace detail
{

struct boolean_expression2pbes_expression_traverser: public bes::boolean_expression_traverser<boolean_expression2pbes_expression_traverser>
{
  typedef bes::boolean_expression_traverser<boolean_expression2pbes_expression_traverser> super;
  typedef core::term_traits<pbes_system::pbes_expression> tr;

  using super::operator();
  using super::enter;
  using super::leave;

  /// \brief A stack containing PBES expressions.
  std::vector<pbes_system::pbes_expression> expression_stack;

  void push(const pbes_system::pbes_expression& x)
  {
    expression_stack.push_back(x);
  }

  pbes_system::pbes_expression pop()
  {
    assert(!expression_stack.empty());
    pbes_system::pbes_expression result = expression_stack.back();
    expression_stack.pop_back();
    return result;
  }

  /// \brief Returns the top element of the expression stack, which is the result of the conversion.
  pbes_system::pbes_expression result() const
  {
    assert(!expression_stack.empty());
    return expression_stack.back();
  }

  /// \brief Enter true node
  void enter(const true_& /* x */)
  {
    push(tr::true_());
  }

  /// \brief Enter false node
  void enter(const false_& /* x */)
  {
    push(tr::false_());
  }

  /// \brief Leave not node
  void leave(const not_& /* x */)
  {
    pbes_system::pbes_expression b = pop();
    push(tr::not_(b));
  }

  /// \brief Leave and node
  void leave(const and_& /* x */)
  {
    // join the two expressions on top of the stack
    pbes_system::pbes_expression right = pop();
    pbes_system::pbes_expression left  = pop();
    push(tr::and_(left, right));
  }

  /// \brief Leave or node
  void leave(const or_& /* x */)
  {
    // join the two expressions on top of the stack
    pbes_system::pbes_expression right = pop();
    pbes_system::pbes_expression left  = pop();
    push(tr::or_(left, right));
  }

  /// \brief Enter imp node
  /// \param e A term
  void leave(const imp& /* x */)
  {
    // join the two expressions on top of the stack
    pbes_system::pbes_expression right = pop();
    pbes_system::pbes_expression left  = pop();
    push(tr::imp(left, right));
  }

  /// \brief Enter propositional_variable node
  /// \param e A term
  /// \param X A propositional variable
  void enter(const boolean_variable& x)
  {
    push(pbes_system::propositional_variable_instantiation(x.name(), data::data_expression_list()));
  }
};

} // namespace detail

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_DETAIL_BOOLEAN_EXPRESSION2PBES_EXPRESSION_TRAVERSER_H
