// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/boolean_expression2pbes_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_BOOLEAN_EXPRESSION2PBES_EXPRESSION_VISITOR_H
#define MCRL2_PBES_DETAIL_BOOLEAN_EXPRESSION2PBES_EXPRESSION_VISITOR_H

#include <vector>
#include "mcrl2/pbes/boolean_expression_visitor.h"
#include "mcrl2/pbes/pbes_expression.h"

namespace mcrl2 {

namespace bes {

namespace detail {

  struct boolean_expression2pbes_expression_visitor: public boolean_expression_visitor<void>
  {
    typedef boolean_expression_visitor<void> super;
    typedef boolean_expression term_type;
    typedef core::term_traits<pbes_system::pbes_expression> tr;
    typedef core::term_traits<boolean_expression> br;
    typedef br::variable_type variable_type;

    /// \brief A stack containing PBES expressions.
    std::vector<pbes_system::pbes_expression> expression_stack;

    /// \brief Returns the top element of the expression stack, which is the result of the conversion.
    pbes_system::pbes_expression result() const
    {
      return expression_stack.back();
    }

    /// \brief Visit true node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_true(const term_type& /* e */)
    {
      expression_stack.push_back(tr::true_());
      return super::continue_recursion;
    }

    /// \brief Visit false node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_false(const term_type& /* e */)
    {
      expression_stack.push_back(tr::false_());
      return super::continue_recursion;
    }

    /// \brief Visit not node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_not(const term_type& /* e */, const term_type& /* arg */)
    {
      pbes_system::pbes_expression b = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(tr::not_(b));
      return super::continue_recursion;
    }

    /// \brief Leave and node
    void leave_and()
    {
      // join the two expressions on top of the stack
      pbes_system::pbes_expression right = expression_stack.back();
      expression_stack.pop_back();
      pbes_system::pbes_expression left  = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(tr::and_(left, right));
    }

    /// \brief Leave or node
    void leave_or()
    {
      // join the two expressions on top of the stack
      pbes_system::pbes_expression right = expression_stack.back();
      expression_stack.pop_back();
      pbes_system::pbes_expression left  = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(tr::or_(left, right));
    }

    /// \brief Visit imp node
    /// \param e A term
    /// \return The result of visiting the node
    void leave_imp()
    {
      // join the two expressions on top of the stack
      pbes_system::pbes_expression right = expression_stack.back();
      expression_stack.pop_back();
      pbes_system::pbes_expression left  = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(tr::imp(left, right));
    }

    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \param X A propositional variable
    /// \return The result of visiting the node
    bool visit_var(const term_type& /* e */, const variable_type& X)
    {
      expression_stack.push_back(pbes_system::propositional_variable_instantiation(X.name(), data::data_expression_list()));
      return super::continue_recursion;
    }
  };

} // namespace detail

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_BOOLEAN_EXPRESSION2PBES_EXPRESSION_VISITOR_H
