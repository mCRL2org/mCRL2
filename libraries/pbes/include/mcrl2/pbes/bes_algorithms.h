// Author(s): Wieger Wesselink
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/bes_algorithms.h
/// \brief Algorithms for boolean equation systems.

#ifndef MCRL2_PBES_ALGORITHMS_H
#define MCRL2_PBES_ALGORITHMS_H

#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/pbes2bes.h"
#include "mcrl2/pbes/gauss_elimination.h"

namespace mcrl2 {

namespace pbes_system {

/// Function to solve a bes equation.
template <typename PbesRewriter>
struct bes_equation_solver
{
  PbesRewriter& m_rewriter;
  
  bes_equation_solver(PbesRewriter& rewriter)
    : m_rewriter(rewriter)
  {}
   
  pbes_equation operator()(pbes_equation e)
  {
    pbes_equation result = gauss::substitute(e, e.variable(), gauss::sigma(e));
    pbes_expression t = m_rewriter(result.formula());
    result = pbes_equation(result.symbol(), result.variable(), t);
    return result;
  }
};

/// Solves a boolean equation system using Gauss elimination.
/// \precondition The pbes p is a bes.
/// \return 0 if false, 1 if true, 2 if unknown
template <typename Container>
int bes_gauss_elimination(pbes<Container>& p)
{
  typedef data::rewriter data_rewriter;
  typedef simplify_rewriter<data_rewriter> pbes_rewriter;
  typedef bes_equation_solver<pbes_rewriter> bes_solver;
    
  data_rewriter datar(p.data());
  pbes_rewriter pbesr(datar, p.data());
  bes_solver solver(pbesr);

  gauss_elimination_algorithm<pbes_rewriter, bes_solver> algorithm(pbesr, solver);
  algorithm.run(p.equations().begin(), p.equations().end());

  if (p.equations().front().formula() == pbes_expr::false_())
  {
    return 0;
  }
  else if (p.equations().front().formula() == pbes_expr::true_())
  {
    return 1;
  }
  else
  {
    return 2;
  } 
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ALGORITHMS_H
