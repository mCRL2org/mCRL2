// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/detail/standard_form_visitor.h
/// \brief Visitor for BES transformation into standard (recursive) normal form.

#ifndef MCRL2_BES_DETAIL_STANDARD_FORM_VISITOR_H
#define MCRL2_BES_DETAIL_STANDARD_FORM_VISITOR_H

#include "mcrl2/bes/boolean_expression_visitor.h"
#include "mcrl2/bes/boolean_equation.h"
#include "mcrl2/core/identifier_generator.h"
#include "mcrl2/exception.h"
#include "mcrl2/atermpp/map.h"

namespace mcrl2 {

namespace bes {

namespace detail {

  enum standard_form_type
  {
    standard_form_both,
    standard_form_and,
    standard_form_or
  };

  typedef std::pair<boolean_expression, standard_form_type> standard_form_pair;

  struct standard_form_visitor: public boolean_expression_visitor<void>
  {
    typedef boolean_expression_visitor<void> super;
    typedef core::term_traits<boolean_expression> tr;

    /// \brief If true, the result will be in standard recursive normal form, otherwise in standard form.
    bool m_recursive_form;

    /// \brief The fixpoint symbol of the current equation.
    fixpoint_symbol m_symbol;

    /// \brief The name of the variable of the current equation, with a trailing underscore added.
    std::string m_name;

    /// \brief Is set to true if the value true is encountered in the BES.
    bool m_has_true;

    /// \brief Is set to true if the value false is encountered in the BES.
    bool m_has_false;

    /// \brief For generating fresh variables.
    core::number_postfix_generator m_generator;

    /// \brief A stack containing sub-terms.
    std::vector<standard_form_pair> m_expression_stack;

    /// \brief A vector containing generated equations.
    atermpp::vector<boolean_equation> m_equations;

    /// \brief Maps right hand sides of equations to their corresponding left hand side.
    atermpp::map<boolean_expression, boolean_variable> m_table;

    /// \brief The expression corresponding to true.
    boolean_expression m_true;

    /// \brief The expression corresponding to false.
    boolean_expression m_false;

    /// \brief Pops the stack and returns the popped element
    standard_form_pair pop()
    {
      standard_form_pair result = m_expression_stack.back();
      m_expression_stack.pop_back();
      return result;
    }

    /// \brief Pushes (first, second) on the stack.
    void push(const boolean_expression& first, standard_form_type second)
    {     
      m_expression_stack.push_back(standard_form_pair(first, second));
    }

    /// \brief Generates a fresh boolean variable.
    boolean_variable fresh_variable(const std::string& hint)
    {
      core::identifier_string s = m_generator(hint);
      return boolean_variable(s);
    }

    /// \brief Generates an equation var=expr for the expression expr (if it does not exist).
    /// \return The variable var.
    boolean_variable create_variable(const boolean_expression& expr, standard_form_type type, const std::string& hint)
    {
      atermpp::map<boolean_expression, boolean_variable>::iterator i = m_table.find(expr);
      if (i != m_table.end())
      {
        return i->second;
      }
      boolean_variable var = fresh_variable(hint);
      m_table[expr] = var;
      if (type == standard_form_and)
      {
        m_equations.push_back(boolean_equation(m_symbol, var, expr));
      }
      else
      {
        m_equations.push_back(boolean_equation(m_symbol, var, expr));
      }
      return var;
    }

    /// \brief Constructor.
    /// \param Determines whether or not the result will be in standard recursive normal form.
    standard_form_visitor(bool recursive_form = false)
    : m_recursive_form(recursive_form),
      m_has_true(false),
      m_has_false(false)
    {
      if (m_recursive_form)
      {
        m_true = fresh_variable("True");
        m_false = fresh_variable("False");
      }
      else
      {
        m_true = tr::true_();
        m_false = tr::false_();
      }
    }

    /// \brief Returns the top element of the expression stack, which is the result of the normalization.
    boolean_expression result() const
    {
      return m_expression_stack.back().first;
    }

    /// \brief Returns the generated equations.
    const atermpp::vector<boolean_equation>& equations() const
    {
      return m_equations;
    }

    /// \brief Visit true node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_true(const boolean_expression& /* e */)
    {
      m_has_true = true;
      push(m_true, standard_form_both);
      return super::continue_recursion;
    }

    /// \brief Visit false node
    /// \param e A term
    /// \return The result of visiting the node
    bool visit_false(const boolean_expression& /* e */)
    {
      m_has_false = true;
      push(m_false, standard_form_both);
      return super::continue_recursion;
    }

    /// \brief Visit propositional_variable node
    /// \param e A term
    /// \param X A propositional variable
    /// \return The result of visiting the node
    bool visit_var(const boolean_expression& /* e */, const boolean_variable& X)
    {
      push(X, standard_form_both);
      return super::continue_recursion;
    }

    /// \brief Leave not node
    void leave_not()
    {
      throw mcrl2::runtime_error("negation is not supported in standard recursive form algorithm");
    }

    /// \brief Leave and node
    void leave_and()
    {
      standard_form_pair right = pop();
      standard_form_pair left = pop();
      if (left.second == standard_form_or)
      {
        left.first = create_variable(left.first, standard_form_or, m_name);
      }
      if (right.second == standard_form_or)
      {
        right.first = create_variable(right.first, standard_form_or, m_name);
      }
      push(tr::and_(left.first, right.first), standard_form_and);
    }

    /// \brief Leave or node
    void leave_or()
    {
      standard_form_pair right = pop();
      standard_form_pair left = pop();
      if (left.second == standard_form_and)
      {
        left.first = create_variable(left.first, standard_form_and, m_name);
      }
      if (right.second == standard_form_and)
      {
        right.first = create_variable(right.first, standard_form_and, m_name);
      }
      push(tr::or_(left.first, right.first), standard_form_or);
    }

    /// \brief Leave imp node
    void leave_imp()
    {
      throw mcrl2::runtime_error("implication is not supported in standard recursive form algorithm");
    }
    
    /// \brief Visit an equation
    void visit_equation(const boolean_equation& eq)
    {
      m_symbol = eq.symbol();
      m_name = std::string(eq.variable().name()) + '_';
      super::visit(eq.formula());
      standard_form_pair p = pop();
      m_equations.push_back(boolean_equation(eq.symbol(), eq.variable(), p.first));
      m_table[p.first] = eq.variable();
    }

    /// \brief Visit a boolean equation system.
    void visit_boolean_equation_system(const boolean_equation_system<>& eqn)
    {
      for (atermpp::vector<boolean_equation>::const_iterator i = eqn.equations().begin(); i != eqn.equations().end(); ++i)
      {
        m_generator.add_to_context(std::string(i->variable().name()));
      }

      for (atermpp::vector<boolean_equation>::const_iterator i = eqn.equations().begin(); i != eqn.equations().end(); ++i)
      {
        visit_equation(*i);
      }
      
      // add equations for true and false if needed
      if (m_recursive_form)
      {
        if (m_has_true)
        {
          m_equations.push_back(boolean_equation(fixpoint_symbol::nu(), m_true, m_true));
        }
        if (m_has_false)
        {
          m_equations.push_back(boolean_equation(fixpoint_symbol::mu(), m_false, m_false));
        }
      }
    }
  };

} // namespace detail

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_DETAIL_STANDARD_FORM_VISITOR_H
