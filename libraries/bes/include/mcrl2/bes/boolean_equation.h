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

namespace mcrl2
{

namespace bes
{

/// \brief The fixpoint symbol type
typedef pbes_system::fixpoint_symbol fixpoint_symbol;

/// \brief boolean equation.
// <BooleanEquation>   ::= BooleanEquation(<FixPoint>, <BooleanVariable>, <BooleanExpression>)
class boolean_equation
{
  friend struct atermpp::aterm_traits<boolean_equation>;

  protected:
    /// \brief The fixpoint symbol of the equation
    fixpoint_symbol m_symbol;

    /// \brief The predicate variable of the equation
    boolean_variable   m_variable;

    /// \brief The formula of the equation
    boolean_expression m_formula;

    /// \brief Protects the term from being freed during garbage collection.
    void protect() const
    {
      m_symbol.protect();
      m_variable.protect();
      m_formula.protect();
    }

    /// \brief Unprotect the term.
    /// Releases protection of the term which has previously been protected through a
    /// call to protect.
    void unprotect() const
    {
      m_symbol.unprotect();
      m_variable.unprotect();
      m_formula.unprotect();
    }

    /// \brief Mark the term for not being garbage collected.
    void mark() const
    {
      m_symbol.mark();
      m_variable.mark();
      m_formula.mark();
    }

  public:
    /// \brief The expression type of the equation.
    typedef boolean_expression term_type;

    /// \brief The variable type of the equation.
    typedef boolean_variable variable_type;

    /// \brief The symbol type of the equation.
    typedef fixpoint_symbol symbol_type;

    /// \brief Constructor.
    boolean_equation()
    {}

    /// \brief Constructor.
    /// \param t A term
    boolean_equation(atermpp::aterm_appl t)
    {
      assert(core::detail::check_rule_BooleanEquation(t));
      atermpp::aterm_appl::iterator i = t.begin();
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

inline bool
operator==(const boolean_equation& x, const boolean_equation& y)
{
  return x.symbol() == y.symbol() &&
         x.variable() == y.variable() &&
         x.formula() == y.formula();
}

inline bool
operator!=(const boolean_equation& x, const boolean_equation& y)
{
  return !(x == y);
}

inline bool
operator<(const boolean_equation& x, const boolean_equation& y)
{
  return ATermAppl(x.variable()) < ATermAppl(y.variable());
}

/// \brief Conversion to ATermAppl.
/// \return The boolean equation converted to ATerm format.
inline
atermpp::aterm_appl boolean_equation_to_aterm(const boolean_equation& eqn)
{
  return core::detail::gsMakeBooleanEquation(eqn.symbol(), eqn.variable(), eqn.formula());
}

} // namespace bes

} // namespace mcrl2

/// \cond INTERNAL_DOCS
namespace atermpp
{

template<>
struct aterm_traits<mcrl2::bes::boolean_equation>
{
  static void protect(const mcrl2::bes::boolean_equation& t)
  {
    t.protect();
  }
  static void unprotect(const mcrl2::bes::boolean_equation& t)
  {
    t.unprotect();
  }
  static void mark(const mcrl2::bes::boolean_equation& t)
  {
    t.mark();
  }
};

} // namespace atermpp
/// \endcond

#endif // MCRL2_BES_BOOLEAN_EQUATION_H
