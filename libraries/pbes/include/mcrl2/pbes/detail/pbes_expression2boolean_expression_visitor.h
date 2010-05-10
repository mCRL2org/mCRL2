// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes_expression2boolean_expression_visitor.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES_EXPRESSION2BOOLEAN_EXPRESSION_VISITOR_H
#define MCRL2_PBES_DETAIL_PBES_EXPRESSION2BOOLEAN_EXPRESSION_VISITOR_H

#include "mcrl2/pbes/bes.h"
#include "mcrl2/pbes/pbes_expression_visitor.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

  template <typename Term>
  struct pbes_expression2boolean_expression_visitor: public pbes_expression_visitor<Term>
  {
    typedef pbes_expression_visitor<Term> super;
    typedef Term term_type;
    typedef core::term_traits<Term> tr;
    typedef typename tr::variable_type variable_type;
    typedef typename tr::variable_sequence_type variable_sequence_type;
    typedef typename tr::data_term_type data_term_type;
    typedef typename tr::propositional_variable_type propositional_variable_type;

    typedef core::term_traits<bes::boolean_expression> br;

    /// \brief A stack containing boolean expressions.
    std::vector<bes::boolean_expression> expression_stack;

    /// \brief Returns the top element of the expression stack, which is the result of the conversion.
    bes::boolean_expression result() const
    {
      return expression_stack.back();
    }

    /// \brief Visit data_expression node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_data_expression(const term_type& e, const data_term_type& /* d */)
    {
      throw mcrl2::runtime_error("data_expression encountered in pbes_expression2boolean_expression_visitor");
      return super::continue_recursion;
    }

    /// \brief Visit true node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_true(const term_type& e)
    {
      expression_stack.push_back(br::true_());
      return super::continue_recursion;
    }

    /// \brief Visit false node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_false(const term_type& e)
    {
      expression_stack.push_back(br::false_());
      return super::continue_recursion;
    }

    /// \brief Visit not node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_not(const term_type& e, const term_type& /* arg */)
    {
      bes::boolean_expression b = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(br::not_(b));
      return super::continue_recursion;
    }

    /// \brief Leave and node
    void leave_and()
    {
      // join the two expressions on top of the stack
      bes::boolean_expression right = expression_stack.back();
      expression_stack.pop_back();
      bes::boolean_expression left  = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(br::and_(left, right));
    }

    /// \brief Leave or node
    void leave_or()
    {
      // join the two expressions on top of the stack
      bes::boolean_expression right = expression_stack.back();
      expression_stack.pop_back();
      bes::boolean_expression left  = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(br::or_(left, right));
    }

    /// \brief Visit imp node
    /// \param e A term
    /// \return The result of visiting the node
    void leave_imp()
    {
      // join the two expressions on top of the stack
      bes::boolean_expression right = expression_stack.back();
      expression_stack.pop_back();
      bes::boolean_expression left  = expression_stack.back();
      expression_stack.pop_back();
      expression_stack.push_back(br::imp(left, right));
    }

    /// \brief Visit forall node
    /// \param e A term
    /// \param variables A sequence of variables
    /// \return The result of visiting the node
    bool visit_forall(const term_type& e, const variable_sequence_type& variables, const term_type& /* expression */)
    {
      throw mcrl2::runtime_error("forall encountered in pbes_expression2boolean_expression_visitor");
      return super::continue_recursion;
    }

    /// \brief Visit exists node
    /// \param e A term
    /// \param variables A sequence of variables
    /// \return The result of visiting the node
    bool visit_exists(const term_type& e, const variable_sequence_type& variables, const term_type& /* expression */)
    {
      throw mcrl2::runtime_error("exists encountered in pbes_expression2boolean_expression_visitor");
      return super::continue_recursion;
    }

    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \param X A propositional variable
    /// \return The result of visiting the node
    bool visit_propositional_variable(const term_type& e, const propositional_variable_type& X)
    {
      if (X.parameters().size() > 0)
      {
        throw mcrl2::runtime_error("propositional variable with parameters encountered in pbes_expression2boolean_expression_visitor");
      }     
      expression_stack.push_back(bes::boolean_variable(X.name()));
      return super::continue_recursion;
    }
  };

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES_EXPRESSION2BOOLEAN_EXPRESSION_VISITOR_H
