// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/boolean_equation.h
/// \brief add your file description here.

#ifndef MCRL2_BES_BOOLEAN_EQUATION_H
#define MCRL2_BES_BOOLEAN_EQUATION_H

#include <cassert>
#include <string>
#include "mcrl2/atermpp/aterm_appl.h"
#include "mcrl2/core/detail/constructors.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/core/detail/soundness_checks.h"
#include "mcrl2/core/print.h"
#include "mcrl2/pbes/fixpoint_symbol.h"
#include "mcrl2/bes/boolean_expression.h"

namespace mcrl2 {

namespace bes {

  /// \brief The fixpoint symbol type
  typedef pbes_system::fixpoint_symbol fixpoint_symbol;

  /// \brief boolean equation.
  // <BooleanEquation>   ::= BooleanEquation(<FixPoint>, <BooleanVariable>, <BooleanExpression>)
  class boolean_equation: public atermpp::aterm_appl
  {
    protected:
      /// \brief The fixpoint symbol of the equation
      fixpoint_symbol m_symbol;
  
      /// \brief The predicate variable of the equation
      boolean_variable   m_variable;
  
      /// \brief The formula of the equation
      boolean_expression m_formula;
  
    public:
      /// \brief The expression type of the equation.
      typedef boolean_expression term_type;
  
      /// \brief The variable type of the equation.
      typedef boolean_variable variable_type;
  
      /// \brief The symbol type of the equation.
      typedef fixpoint_symbol symbol_type;
  
      /// \brief Constructor.
      boolean_equation()
        : atermpp::aterm_appl(core::detail::constructBooleanEquation())
      {}
  
      /// \brief Constructor.
      /// \param t A term
      boolean_equation(atermpp::aterm_appl t)
        : atermpp::aterm_appl(t)
      {
        assert(core::detail::check_rule_BooleanEquation(m_term));
        iterator i = t.begin();
        m_symbol   = fixpoint_symbol(*i++);
        atermpp::aterm_appl var(*i++);
        m_variable = boolean_variable(var);
        m_formula  = boolean_expression(*i);
      }
  
      /// \brief Constructor.
      /// \param symbol A fixpoint symbol
      /// \param variable A boolean variable
      /// \param expr A boolean expression
      boolean_equation(fixpoint_symbol symbol, boolean_variable variable, boolean_expression expr)
        : atermpp::aterm_appl(core::detail::gsMakeBooleanEquation(symbol, variable, expr)),
          m_symbol(symbol),
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
  
      /// \brief Returns the boolean_equation_system variable of the equation.
      /// \return The boolean_equation_system variable of the equation.
      const boolean_variable& variable() const
      {
        return m_variable;
      }
  
      /// \brief Returns the boolean_equation_system variable of the equation.
      /// \return The boolean_equation_system variable of the equation.
      boolean_variable& variable()
      {
        return m_variable;
      }
  
      /// \brief Returns the predicate formula on the right hand side of the equation.
      /// \return The predicate formula on the right hand side of the equation.
      const boolean_expression& formula() const
      {
        return m_formula;
      }
  
      /// \brief Returns the predicate formula on the right hand side of the equation.
      /// \return The predicate formula on the right hand side of the equation.
      boolean_expression& formula()
      {
        return m_formula;
      }
  };
  
  /// \brief Read-only singly linked list of boolean equations
  typedef atermpp::term_list<boolean_equation> boolean_equation_list;

  /// \brief Pretty print function
  /// \param eq A boolean equation
  /// \return A pretty printed representation of the boolean equation
  inline
  std::string pp(const boolean_equation& eq)
  {
    return core::pp(eq.symbol()) + " " + bes::pp(eq.variable()) + " = " + bes::pp(eq.formula());
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_BOOLEAN_EQUATION_H
