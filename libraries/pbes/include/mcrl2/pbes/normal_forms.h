// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/normal_forms.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_NORMAL_FORMS_H
#define MCRL2_PBES_NORMAL_FORMS_H

#include "mcrl2/pbes/traverser.h"

namespace mcrl2::pbes_system
{

namespace detail
{

enum standard_form_type
{
  standard_form_both,
  standard_form_and,
  standard_form_or
};

using standard_form_pair = std::pair<pbes_expression, standard_form_type>;

/// \brief Traverser that implements the standard form normalization.
class standard_form_traverser: public pbes_system::pbes_expression_traverser<standard_form_traverser>
{
  public:
    using super = pbes_system::pbes_expression_traverser<standard_form_traverser>;

    using super::apply;
    using super::enter;
    using super::leave;

    /// \brief If true, the result will be in standard recursive normal form, otherwise in standard form.
    bool m_recursive_form;

    /// \brief The fixpoint symbol of the current equation.
    fixpoint_symbol m_symbol;

    /// \brief The name of the variable of the current equation, with a trailing underscore added.
    std::string m_name;

    /// \brief Is set to true if the value true is encountered in the PBES.
    bool m_has_true = false;

    /// \brief Is set to true if the value false is encountered in the PBES.
    bool m_has_false = false;

    /// \brief For generating fresh variables.
    utilities::number_postfix_generator m_generator;

    /// \brief A stack containing sub-terms.
    std::vector<standard_form_pair> m_expression_stack;

    /// \brief A vector containing generated equations.
    std::vector<pbes_equation> m_equations;

    /// \brief A vector containing generated equations with new variables.
    std::vector<pbes_equation> m_equations2;

    /// \brief Maps right hand sides of equations to their corresponding left hand side.
    std::map<pbes_expression, propositional_variable_instantiation> m_table;

    /// \brief The expression corresponding to true.
    pbes_expression m_true;

    /// \brief The expression corresponding to false.
    pbes_expression m_false;

    /// \brief Pops the stack and returns the popped element
    standard_form_pair pop()
    {
      standard_form_pair result = m_expression_stack.back();
      m_expression_stack.pop_back();
      return result;
    }

    /// \brief Pushes (first, second) on the stack.
    void push(const pbes_expression& first, standard_form_type second)
    {
      m_expression_stack.emplace_back(first, second);
    }

    /// \brief Generates a fresh pbes variable.
    propositional_variable fresh_variable(const std::string& hint)
    {
      core::identifier_string s = m_generator(hint);
      return propositional_variable(s, data::variable_list());
    }

    /// \brief Generates an equation var=expr for the expression expr (if it does not exist).
    /// \return The variable var.
    propositional_variable_instantiation create_variable(const pbes_expression& expr, standard_form_type type, const std::string& hint)
    {
      std::map<pbes_expression, propositional_variable_instantiation>::iterator i = m_table.find(expr);
      if (i != m_table.end())
      {
        return i->second;
      }
      propositional_variable var=fresh_variable(hint);
      propositional_variable_instantiation varinst(var.name(), data::data_expression_list());;
      m_table[expr] = varinst;
      if (type == standard_form_and)
      {
        m_equations2.emplace_back(m_symbol, var, expr);
      }
      else
      {
        m_equations2.emplace_back(m_symbol, var, expr);
      }
      return varinst;
    }

    /// \brief Constructor.
    /// \param recursive_form Determines whether or not the result will be in standard recursive normal form.
    standard_form_traverser(bool recursive_form = false)
      : m_recursive_form(recursive_form)
    {
      if (m_recursive_form)
      {
        m_true = propositional_variable_instantiation(fresh_variable("True").name(), data::data_expression_list());
        m_false = propositional_variable_instantiation(fresh_variable("False").name(), data::data_expression_list());
      }
      else
      {
        m_true = true_();
        m_false = false_();
      }
    }

    /// \brief Returns the top element of the expression stack, which is the result of the normalization.
    pbes_expression result() const
    {
      return m_expression_stack.back().first;
    }

    /// \brief Returns the generated equations.
    const std::vector<pbes_equation>& equations() const
    {
      return m_equations;
    }

    /// \brief Enter a data_expression that must be either true or false.
    /// \param x A term
    void enter(const data::data_expression& x)
    {
      if (is_false(x))
      {
        m_has_false = true;
        push(m_false, standard_form_both);
      }
      else
      {
        assert(is_true(x));
        m_has_true = true;
        push(m_true, standard_form_both);
      }
    }

    /// \brief Enter propositional_variable node.
    /// \param x A pbes variable.
    void enter(const propositional_variable_instantiation& x)
    {
      push(x, standard_form_both);
    }

    /// \brief Leave not node.
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
      push(and_(left.first, right.first), standard_form_and);
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
      push(or_(left.first, right.first), standard_form_or);
    }

    /// \brief Leave imp node
    void leave(const imp& /* x */)
    {
      throw mcrl2::runtime_error("implication is not supported in standard recursive form algorithm");
    }

    /// \brief Enter an equation
    void enter(const pbes_equation& eq)
    {
      m_symbol = eq.symbol();
      m_name = std::string(eq.variable().name()) + '_';
    }

    /// \brief Leave an equation
    void leave(const pbes_equation& eq)
    {
      standard_form_pair p = pop();
      m_equations.emplace_back(eq.symbol(), eq.variable(), p.first);
      // m_table[p.first] = eq.variable();
    }

    /// \brief Enter a pbes equation system.
    void enter(const pbes& x)
    {
      assert(!x.equations().empty());
      for (const pbes_equation& eqn: x.equations())
      {
        m_generator.add_identifier(std::string(eqn.variable().name()));
      }
    }

    /// \brief Leave a pbes equation system.
    void leave(const pbes&)
    {
      // set the fixpoint symbol for the added equations m_equations2, and move them to m_equations
      assert(!m_equations.empty());
      fixpoint_symbol sigma = m_equations.back().symbol();
      for (pbes_equation& eqn: m_equations2)
      {
        eqn.symbol() = sigma;
      }
      std::copy(m_equations2.begin(), m_equations2.end(), std::back_inserter(m_equations));

      // add equations for true and false if needed
      if (m_recursive_form)
      {
        if (m_has_true)
        {
          m_equations.emplace_back(fixpoint_symbol::nu(),
              propositional_variable(atermpp::down_cast<propositional_variable_instantiation>(m_true).name()),
              m_true);
        }
        if (m_has_false)
        {
          m_equations.emplace_back(fixpoint_symbol::mu(),
              propositional_variable(atermpp::down_cast<propositional_variable_instantiation>(m_false).name()),
              m_false);
        }
      }
    }

};

} // namespace detail

/// \brief Transforms a PBES into standard form.
/// \param eqn A pbes equation system
/// \param recursive_form Determines whether or not the result will be in standard recursive normal form
inline
void make_standard_form(pbes& eqn, bool recursive_form = false)
{
  detail::standard_form_traverser t(recursive_form);
  t.apply(eqn);
  assert(!is_propositional_variable(eqn.initial_state()) || eqn.equations().begin()->variable() == propositional_variable(eqn.initial_state()));
  assert(!is_propositional_variable(eqn.initial_state()) || t.m_equations.begin()->variable() == propositional_variable(eqn.initial_state()));
  eqn.equations() = t.m_equations;
}

} // namespace mcrl2::pbes_system

#endif // MCRL2_PBES_NORMAL_FORMS_H
