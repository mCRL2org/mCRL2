// Author(s): Jan Friso Groote. Based on res_equation.h by Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/res/res_equation.h
/// \brief This file define res equations. 

#ifndef MCRL2_RES_BOOLEAN_EQUATION_H
#define MCRL2_RES_BOOLEAN_EQUATION_H

#include "mcrl2/res/res_expression.h"
#include "mcrl2/pbes/fixpoint_symbol.h"

namespace mcrl2
{

namespace bes
{

/// \brief The fixpoint symbol type
typedef pbes_system::fixpoint_symbol fixpoint_symbol;

/// \brief boolean equation.
// <RESEquation>   ::= RESEquation(<FixPoint>, <RESVariable>, <RESExpression>)
class res_equation
{
  protected:
    /// \brief The fixpoint symbol of the equation
    fixpoint_symbol m_symbol;

    /// \brief The predicate variable of the equation
    res_variable   m_variable;

    /// \brief The formula of the equation
    res_expression m_formula;

  public:
    /// \brief The expression type of the equation.
    typedef res_expression term_type;

    /// \brief The variable type of the equation.
    typedef res_variable variable_type;

    /// \brief The symbol type of the equation.
    typedef fixpoint_symbol symbol_type;

    /// \brief Constructor.
    res_equation()
    {}

    /// \brief Constructor.
    /// \brief Constructor.
    /// \param t A term
    res_equation(const atermpp::aterm_appl &t)
    {
      assert(core::detail::check_rule_RESEquation(t));
      atermpp::aterm_appl::iterator i = t.begin();
      m_symbol   = fixpoint_symbol(*i++);
      atermpp::aterm_appl var = atermpp::down_cast<atermpp::aterm_appl>(*i++);
      m_variable = res_variable(var);
      m_formula  = res_expression(*i);
    }

    /// \brief Constructor.
    /// \param symbol A fixpoint symbol
    /// \param variable A boolean variable
    /// \param expr A boolean expression
    res_equation(fixpoint_symbol symbol, res_variable variable, res_expression expr)
      : m_symbol(symbol),
        m_variable(variable),
        m_formula(expr)
    {
    }

    /// \brief Returns the fixpoint symbol of the equation.
    /// \return The fixpoint symbol of the equation.
    const fixpoint_symbol& symbol() const
    {
      return m_symbol;
    }

    /// \brief Returns the fixpoint symbol of the equation.
    /// \return The fixpoint symbol of the equation.
    fixpoint_symbol& symbol()
    {
      return m_symbol;
    }

    /// \brief Returns the res_equation_system variable of the equation.
    /// \return The res_equation_system variable of the equation.
    const res_variable& variable() const
    {
      return m_variable;
    }

    /// \brief Returns the res_equation_system variable of the equation.
    /// \return The res_equation_system variable of the equation.
    res_variable& variable()
    {
      return m_variable;
    }

    /// \brief Returns the predicate formula on the right hand side of the equation.
    /// \return The predicate formula on the right hand side of the equation.
    const res_expression& formula() const
    {
      return m_formula;
    }

    /// \brief Returns the predicate formula on the right hand side of the equation.
    /// \return The predicate formula on the right hand side of the equation.
    res_expression& formula()
    {
      return m_formula;
    }

    /// \brief Swaps the contents
    void swap(res_equation& other)
    {
      using std::swap;
      swap(m_symbol, other.m_symbol);
      swap(m_variable, other.m_variable);
      swap(m_formula, other.m_formula);
    }
};

//--- start generated class res_equation ---//
// prototype declaration
std::string pp(const res_equation& x);

/// \\brief Outputs the object to a stream
/// \\param out An output stream
/// \\param x Object x
/// \\return The output stream
inline
std::ostream& operator<<(std::ostream& out, const res_equation& x)
{
  return out << res::pp(x);
}

/// \\brief swap overload
inline void swap(res_equation& t1, res_equation& t2)
{
  t1.swap(t2);
}
//--- end generated class res_equation ---//

/// \brief equality operator
inline bool
operator==(const res_equation& x, const res_equation& y)
{
  return x.symbol() == y.symbol() &&
         x.variable() == y.variable() &&
         x.formula() == y.formula();
}

/// \brief inequality operator
inline bool
operator!=(const res_equation& x, const res_equation& y)
{
  return !(x == y);
}

/// \brief less operator
inline bool
operator<(const res_equation& x, const res_equation& y)
{
  return x.variable() < y.variable();
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_RES_BOOLEAN_EQUATION_H
