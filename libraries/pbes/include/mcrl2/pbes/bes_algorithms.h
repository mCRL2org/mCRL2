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

namespace mcrl2 {

namespace pbes_system {

/// \brief Function object for solving a bes equation.
template <typename PbesRewriter>
struct bes_equation_solver
{
  /// \brief A pbes rewriter
  PbesRewriter& m_rewriter;

  /// \brief Constructor
  /// \param rewriter A pbes rewriter
  bes_equation_solver(PbesRewriter& rewriter)
    : m_rewriter(rewriter)
  {}

  /// \brief Solves the equation
  /// \param e A pbes equation
  /// \return The solved equation
  pbes_equation operator()(pbes_equation e)
  {
    pbes_equation result = gauss::substitute(e, e.variable(), gauss::sigma(e));
    pbes_expression t = m_rewriter(result.formula());
    result = pbes_equation(result.symbol(), result.variable(), t);
    return result;
  }
};

/// \brief Solves a boolean equation system using Gauss elimination.
/// \pre The pbes \p p is a bes.
/// \param p A pbes
/// \return 0 if the solution is false, 1 if the solution is true, 2 if the solution is unknown
template <typename Container>
int bes_gauss_elimination(pbes<Container>& p)
{
  typedef data::data_enumerator<number_postfix_generator> my_enumerator;
  typedef enumerate_quantifiers_rewriter<pbes_expression_with_variables, data::rewriter, my_enumerator> my_rewriter;
  typedef bes_equation_solver<my_rewriter> bes_solver;
  typedef typename core::term_traits<pbes_expression> tr;

  data::rewriter datar(p.data());
  number_postfix_generator name_generator;
  my_enumerator datae(p.data(), datar, name_generator);
  my_rewriter pbesr(datar, datae);
  bes_solver solver(pbesr);

  gauss_elimination_algorithm<my_rewriter, bes_solver> algorithm(pbesr, solver);
  algorithm.run(p.equations().begin(), p.equations().end());

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
/// \param p A pbes
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

} // namespace mcrl2

#endif // MCRL2_PBES_ALGORITHMS_H
