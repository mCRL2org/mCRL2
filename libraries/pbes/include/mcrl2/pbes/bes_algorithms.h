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
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/rewriter.h"
#include "mcrl2/pbes/pbesinst.h"
#include "mcrl2/pbes/gauss_elimination.h"
#include "mcrl2/utilities/number_postfix_generator.h"

namespace mcrl2
{

namespace pbes_system
{

/// \brief Instantiates a pbes.
/// \param p A PBES
/// \param lazy If true, the lazy instantiation algorithm is used, otherwise the finite instantiation algorithm.
/// \return A bes.
pbes<> pbesinst(const pbes<>& p, bool lazy = false)
{
  typedef data::data_enumerator<utilities::number_postfix_generator> my_enumerator;
  typedef enumerate_quantifiers_rewriter<pbes_expression_with_variables, data::rewriter, my_enumerator> my_rewriter;
  data::rewriter datar(p.data());
  utilities::number_postfix_generator name_generator("x");
  my_enumerator datae(p.data(), datar, name_generator);
  my_rewriter pbesr(datar, datae);
  if (lazy)
  {
    return do_lazy_algorithm(p, pbesr);
  }
  else
  {
    return do_finite_algorithm(p, pbesr,datar);
  }
}

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_ALGORITHMS_H
