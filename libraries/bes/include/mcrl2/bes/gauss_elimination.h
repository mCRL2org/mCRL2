// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/bes/gauss_elimination.h
/// \brief add your file description here.

#ifndef MCRL2_BES_GAUSS_ELIMINATION_H
#define MCRL2_BES_GAUSS_ELIMINATION_H

#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/gauss_elimination.h"
#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/print.h"
#include "mcrl2/bes/replace.h"

namespace mcrl2
{

namespace bes
{

/// \brief Traits class for pbes expressions
struct bes_traits
{
  public:
    typedef boolean_expression expression_type;
    typedef boolean_variable variable_type;
    typedef boolean_equation equation_type;
    typedef bes::fixpoint_symbol symbol_type;

    /// \brief Applies the substitution X := phi to the boolean expression t.
    /// \param t A boolean expression
    /// \param X A boolean variable
    /// \param phi A boolean expression
    /// \return The substition result
    static inline
    expression_type substitute(const expression_type& t, const variable_type& X, const expression_type& phi)
    {
      expression_type result=bes::replace_boolean_variables(t, boolean_variable_substitution(X, phi));
      return result;
    }

    /// \brief Returns the value true
    static inline
    expression_type true_()
    {
      return core::term_traits<expression_type>::true_();
    }

    /// \brief Returns the value false
    static inline
    expression_type false_()
    {
      return core::term_traits<expression_type>::false_();
    }

    /// \brief Returns the fixpoint symbol mu
    static inline
    symbol_type mu()
    {
      return fixpoint_symbol::mu();
    }

    /// \brief Returns the fixpoint symbol nu
    static inline
    symbol_type nu()
    {
      return fixpoint_symbol::nu();
    }

    /// \brief Pretty print an equation without generating a newline after the equal sign
    /// \param eq An equation
    /// \return A pretty printed string
    static inline
    std::string print(const equation_type& eq)
    {
      return bes::pp(eq);
    }
};

/// \brief Solves an equation
/// \param e A bes equation
template <typename Rewriter>
struct boolean_equation_solver
{
  const Rewriter& m_rewriter;

  boolean_equation_solver(const Rewriter& rewriter)
    : m_rewriter(rewriter)
  {}

  /// \brief Returns true if e.symbol() == nu(), else false.
  /// \param e A pbes equation
  /// \return True if e.symbol() == nu(), else false.
  boolean_expression sigma(const boolean_equation& e)
  {
    typedef typename core::term_traits<boolean_expression> tr;
    return e.symbol().is_nu() ? tr::true_() : tr::false_();
  }

  /// \brief Solves the equation e
  void operator()(boolean_equation& e)
  {
    e.formula() = m_rewriter(bes_traits::substitute(e.formula(), e.variable(), sigma(e)));
  }
};

/// \brief Utility function for creating a boolean_equation_solver
template <typename Rewriter>
boolean_equation_solver<Rewriter> make_boolean_equation_solver(const Rewriter& rewriter)
{
  return boolean_equation_solver<Rewriter>(rewriter);
}

/// \brief Solves a boolean equation system using Gauss elimination.
/// \param p A bes
/// \return The solution of the system
template <typename Container>
bool gauss_elimination(boolean_equation_system<Container>& p)
{
  typedef typename core::term_traits<boolean_expression> tr;
  typedef pbes_system::boolean_expression_rewriter<boolean_expression> bes_rewriter;

  pbes_system::gauss_elimination_algorithm<bes_traits> algorithm;
  bes_rewriter besr;
  algorithm.run(p.equations().begin(), p.equations().end(), boolean_equation_solver<bes_rewriter>(besr));
  if (tr::is_false(p.equations().front().formula()))
  {
    return false;
  }
  else if (tr::is_true(p.equations().front().formula()))
  {
    return true;
  }
  else
  {
    throw std::runtime_error("fatal error in bes::gauss_elimination");
  }
  return false;
}

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_GAUSS_ELIMINATION_H
