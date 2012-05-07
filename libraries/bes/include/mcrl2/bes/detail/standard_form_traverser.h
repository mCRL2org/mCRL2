// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/detail/standard_form_traverser.h
/// \brief add your file description here.

#ifndef MCRL2_BES_DETAIL_STANDARD_FORM_TRAVERSER_H
#define MCRL2_BES_DETAIL_STANDARD_FORM_TRAVERSER_H

#include <iterator>
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/traverser.h"
#include "mcrl2/utilities/number_postfix_generator.h"
#include "mcrl2/exception.h"

namespace mcrl2
{

namespace bes
{

namespace detail
{

enum standard_form_type
{
  standard_form_both,
  standard_form_and,
  standard_form_or
};

typedef std::pair<boolean_expression, standard_form_type> standard_form_pair;

} // namespace detail

} // namespace bes

} // namespace mcrl2

namespace atermpp
{

template<>
struct aterm_traits<mcrl2::bes::detail::standard_form_pair>
{
};

} // namespace atermpp

namespace mcrl2
{

namespace bes
{

namespace detail
{

/// \brief Traverser that implements the standard form normalization.
class standard_form_traverser: public bes::boolean_expression_traverser<standard_form_traverser>
{
  public:
    typedef bes::boolean_expression_traverser<standard_form_traverser> super;
    typedef core::term_traits<boolean_expression> tr;

    using super::operator();
    using super::enter;
    using super::leave;

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
    utilities::number_postfix_generator m_generator;

    /// \brief A stack containing sub-terms.
    std::vector<standard_form_pair> m_expression_stack;

    /// \brief A vector containing generated equations.
    std::vector<boolean_equation> m_equations;

    /// \brief A vector containing generated equations with new variables.
    std::vector<boolean_equation> m_equations2;

    /// \brief Maps right hand sides of equations to their corresponding left hand side.
    std::map<boolean_expression, boolean_variable> m_table;

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
      std::map<boolean_expression, boolean_variable>::iterator i = m_table.find(expr);
      if (i != m_table.end())
      {
        return i->second;
      }
      boolean_variable var = fresh_variable(hint);
      m_table[expr] = var;
      if (type == standard_form_and)
      {
        m_equations2.push_back(boolean_equation(m_symbol, var, expr));
      }
      else
      {
        m_equations2.push_back(boolean_equation(m_symbol, var, expr));
      }
      return var;
    }

    /// \brief Constructor.
    /// \param Determines whether or not the result will be in standard recursive normal form.
    standard_form_traverser(bool recursive_form = false)
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
    const std::vector<boolean_equation>& equations() const
    {
      return m_equations;
    }

    /// \brief Enter true node
    /// \param e A term
    void enter(const true_& /* x */)
    {
      m_has_true = true;
      push(m_true, standard_form_both);
    }

    /// \brief Enter false node
    /// \param e A term
    void enter(const false_& /* x */)
    {
      m_has_false = true;
      push(m_false, standard_form_both);
    }

    /// \brief Enter propositional_variable node
    /// \param e A term
    /// \param X A propositional variable
    void enter(const boolean_variable& x)
    {
      push(x, standard_form_both);
    }

    /// \brief Leave not node
    void leave(const not_& /* x */)
    {
      throw mcrl2::runtime_error("negation is not supported in standard recursive form algorithm");
    }

    /// \brief Leave and node
    void leave(const and_& /* x */)
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
    void leave(const or_& /* x */)
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
    void leave(const imp& /* x */)
    {
      throw mcrl2::runtime_error("implication is not supported in standard recursive form algorithm");
    }

    /// \brief Enter an equation
    void enter(const boolean_equation& eq)
    {
      m_symbol = eq.symbol();
      m_name = std::string(eq.variable().name()) + '_';
    }

    /// \brief Leave an equation
    void leave(const boolean_equation& eq)
    {
      standard_form_pair p = pop();
      m_equations.push_back(boolean_equation(eq.symbol(), eq.variable(), p.first));
      // m_table[p.first] = eq.variable();
    }

    /// \brief Enter a boolean equation system.
    void enter(const boolean_equation_system<>& eqn)
    {
      assert(!eqn.equations().empty());
      for (std::vector<boolean_equation>::const_iterator i = eqn.equations().begin(); i != eqn.equations().end(); ++i)
      {
        m_generator.add_identifier(std::string(i->variable().name()));
      }
    }

    /// \brief Leave a boolean equation system.
    void leave(const boolean_equation_system<>&)
    {
      // set the fixpoint symbol for the added equations m_equations2, and move them to m_equations
      assert(!m_equations.empty());
      fixpoint_symbol sigma = m_equations.back().symbol();
      for (std::vector<boolean_equation>::iterator i = m_equations2.begin(); i != m_equations2.end(); ++i)
      {
        i->symbol() = sigma;
      }
      std::copy(m_equations2.begin(), m_equations2.end(), std::back_inserter(m_equations));

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

#endif // MCRL2_BES_DETAIL_STANDARD_FORM_TRAVERSER_H
