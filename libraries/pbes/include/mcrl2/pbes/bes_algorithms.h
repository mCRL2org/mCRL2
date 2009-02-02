// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/bes_algorithms.h
/// \brief Algorithms for boolean equation systems.

#ifndef MCRL2_PBES_ALGORITHMS_H
#define MCRL2_PBES_ALGORITHMS_H

#include "mcrl2/pbes/pbes_expression_with_variables.h"
#include "mcrl2/data/identifier_generator.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/gauss_elimination.h"
#include "mcrl2/pbes/bes.h"

namespace mcrl2 {

namespace pbes_system {

/// \brief Function object for solving a pbes equation.
template <typename PbesRewriter>
struct pbes_equation_solver
{
  /// \brief A pbes rewriter
  PbesRewriter& m_rewriter;

  /// \brief Constructor
  /// \param rewriter A PBES rewriter
  pbes_equation_solver(PbesRewriter& rewriter)
    : m_rewriter(rewriter)
  {}

  /// \brief Returns true if e.symbol() == nu(), else false.
  /// \param e A PBES equation
  /// \return True if e.symbol() == nu(), else false.
  pbes_expression sigma(const pbes_equation& e) const
  {
    using namespace pbes_expr;
    return e.symbol().is_nu() ? true_() : false_();
  }

  /// \brief Applies the substitution X := phi to the pbes equation eq.
  /// \param eq A PBES equation
  /// \param X A propositional variable
  /// \param phi A PBES expression
  /// \return The substition result
  pbes_equation substitute(pbes_equation eq, propositional_variable X, pbes_expression phi) const
  {
    pbes_expression formula = substitute_propositional_variable(eq.formula(), X, phi);
    return pbes_equation(eq.symbol(), eq.variable(), formula);
  }

  /// \brief Applies the substitution from a solved pbes equation e2 to the pbes equation e1.
  /// \param e1 A PBES equation
  /// \param e2 A PBES equation
  void substitute(pbes_equation& e1, const pbes_equation& e2) const
  {
    e1 = substitute(e1, e2.variable(), e2.formula());
  }

  /// \brief Solves an equation
  /// \param e A PBES equation
  void solve(pbes_equation& e) const
  {
    pbes_equation result = substitute(e, e.variable(), sigma(e));
    pbes_expression t = m_rewriter(result.formula());
    e = pbes_equation(result.symbol(), result.variable(), t);
  }
};

/// \brief Solves a boolean equation system using Gauss elimination.
/// \pre The pbes \p p is a bes.
/// \param p A pbes
/// \return 0 if the solution is false, 1 if the solution is true, 2 if the solution is unknown
template <typename Container>
int pbes_gauss_elimination(pbes<Container>& p)
{
  typedef data::data_enumerator<number_postfix_generator> my_enumerator;
  typedef enumerate_quantifiers_rewriter<pbes_expression_with_variables, data::rewriter, my_enumerator> my_rewriter;
  typedef typename core::term_traits<pbes_expression> tr;

  data::rewriter datar(p.data());
  number_postfix_generator name_generator;
  my_enumerator datae(p.data(), datar, name_generator);
  my_rewriter pbesr(datar, datae);

  gauss_elimination_algorithm algorithm;
  algorithm.run(p.equations().begin(), p.equations().end(), pbes_equation_solver<my_rewriter>(pbesr));

  if (tr::is_false(p.equations().front().formula()))
  {
    return 0;
  }
  else if (tr::is_true(p.equations().front().formula()))
  {
    return 1;
  }
  else
  {
    return 2;
  }
}

/// \brief Instantiates a pbes.
/// \param p A PBES
/// \param lazy If true, the lazy instantiation algorithm is used, otherwise the finite instantiation algorithm.
/// \return A bes.
pbes<> pbes2bes(const pbes<>& p, bool lazy = false)
{
  typedef data::data_enumerator<number_postfix_generator> my_enumerator;
  typedef enumerate_quantifiers_rewriter<pbes_expression_with_variables, data::rewriter, my_enumerator> my_rewriter;
  data::rewriter datar(p.data());
  number_postfix_generator name_generator;
  my_enumerator datae(p.data(), datar, name_generator);
  my_rewriter pbesr(datar, datae);
  if (lazy)
  {
    return do_lazy_algorithm(p, pbesr);
  }
  else
  {
    return do_finite_algorithm(p, pbesr);
  }
}

} // namespace pbes_system

namespace bes {

  /// \brief Solver for boolean equations.
  template <typename BesRewriter>
  struct bes_equation_solver
  {
    /// \brief A bes rewriter
    BesRewriter& m_rewriter;
  
    /// \brief Constructor
    /// \param rewriter A bes rewriter
    bes_equation_solver(BesRewriter& rewriter)
      : m_rewriter(rewriter)
    {}
  
    /// \brief Returns true if e.symbol() == nu(), else false.
    /// \param e A pbes equation
    /// \return True if e.symbol() == nu(), else false.
    inline
    boolean_expression sigma(const boolean_equation& e)
    {
      typedef typename core::term_traits<boolean_expression> tr;
      return e.symbol().is_nu() ? tr::true_() : tr::false_();
    }
  
    /// \brief Applies the substitution X := phi to the bes equation eq.
    /// \param eq A bes equation
    /// \param X A boolean variable
    /// \param phi A bes expression
    /// \return The substition result
    boolean_equation substitute(boolean_equation eq, boolean_variable X, boolean_expression phi)
    {
      boolean_expression formula = atermpp::replace(eq.formula(), X, phi);
      return boolean_equation(eq.symbol(), eq.variable(), formula);
    }
  
    /// \brief Applies the substitution from a solved bes equation e2 to the bes equation e1.
    /// \param e1 A bes equation
    /// \param e2 A solved bes equation
    void substitute(boolean_equation& e1, const boolean_equation& e2)
    {
      e1 = substitute(e1, e2.variable(), e2.formula());
    }
  
    /// \brief Solves an equation
    /// \param e A bes equation
    void solve(boolean_equation& e)
    {
      boolean_equation result = substitute(e, e.variable(), sigma(e));
      boolean_expression t = m_rewriter(result.formula());
      e = boolean_equation(result.symbol(), result.variable(), t);
    }
  };

  /// \brief Solves a boolean equation system using Gauss elimination.
  /// \param p A bes
  /// \return The solution of the system
  template <typename Container>
  bool gauss_elimination(boolean_equation_system<Container>& p)
  {
    typedef typename core::term_traits<boolean_expression> tr;
    typedef boolean_expression_rewriter<boolean_expression> bes_rewriter;

    gauss_elimination_algorithm algorithm;
    bes_rewriter besr;
    algorithm.run(p.equations().begin(), p.equations().end(), bes_equation_solver<bes_rewriter>(besr));
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
      throw std::runtime_error("fatal error in gauss_elimination");
    }
    return false;
  }

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_PBES_ALGORITHMS_H
