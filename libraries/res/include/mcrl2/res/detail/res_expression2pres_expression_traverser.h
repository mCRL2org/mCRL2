// Author(s): Jan Friso Groote. Based on bes/detail/boolean_expression2pbes_expression_traverser.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/detail/res_expression2pres_expression_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_RES_DETAIL_RES_EXPRESSION2PRES_EXPRESSION_TRAVERSER_H
#define MCRL2_RES_DETAIL_RES_EXPRESSION2PRES_EXPRESSION_TRAVERSER_H

#include "mcrl2/res/traverser.h"
#include "mcrl2/pres/pres_expression.h"

namespace mcrl2
{

namespace res
{

namespace detail
{

struct res_expression2pres_expression_traverser: public res::res_expression_traverser<res_expression2pres_expression_traverser>
{
  typedef res::res_expression_traverser<res_expression2pres_expression_traverser> super;
  typedef core::term_traits<pres_system::pres_expression> tr;

  using super::apply;
  using super::enter;
  using super::leave;

  /// \brief A stack containing PRES expressions.
  std::vector<pres_system::pres_expression> expression_stack;

  void push(const pres_system::pres_expression& x)
  {
    expression_stack.push_back(x);
  }

  pres_system::pres_expression pop()
  {
    assert(!expression_stack.empty());
    pres_system::pres_expression result = expression_stack.back();
    expression_stack.pop_back();
    return result;
  }

  /// \brief Returns the top element of the expression stack, which is the result of the conversion.
  pres_system::pres_expression result() const
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
  void leave(const minus& /* x */)
  {
    pres_system::pres_expression b = pop();
    push(tr::minus(b));
  }

  /// \brief Leave and node
  void leave(const and_& /* x */)
  {
    // join the two expressions on top of the stack
    pres_system::pres_expression right = pop();
    pres_system::pres_expression left  = pop();
    push(tr::and_(left, right));
  }

  /// \brief Leave or node
  void leave(const or_& /* x */)
  {
    // join the two expressions on top of the stack
    pres_system::pres_expression right = pop();
    pres_system::pres_expression left  = pop();
    push(tr::or_(left, right));
  }

  /// \brief Enter imp node
  /// \param e A term
  void leave(const imp& /* x */)
  {
    // join the two expressions on top of the stack
    pres_system::pres_expression right = pop();
    pres_system::pres_expression left  = pop();
    push(tr::imp(left, right));
  }

  /// \brief Enter propositional_variable node
  /// \param e A term
  /// \param X A propositional variable
  void enter(const res_variable& x)
  {
    push(pres_system::propositional_variable_instantiation(x.name(), data::data_expression_list()));
  }
};

} // namespace detail

} // namespace res

} // namespace mcrl2

#endif // MCRL2_RES_DETAIL_RES_EXPRESSION2PRES_EXPRESSION_TRAVERSER_H
