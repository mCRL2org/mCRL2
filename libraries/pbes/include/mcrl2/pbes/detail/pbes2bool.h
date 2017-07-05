// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/pbes2bool.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_PBES2BOOL_H
#define MCRL2_PBES_DETAIL_PBES2BOOL_H

#include "mcrl2/bes/boolean_equation_system.h"
#include "mcrl2/bes/local_fixpoints.h"
#include "mcrl2/bes/pbesinst_conversion.h"
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/data/selection.h"
#include "mcrl2/pbes/algorithms.h"
#include "mcrl2/pbes/pbesinst_alternative_lazy_algorithm.h"
#include "mcrl2/pbes/search_strategy.h"
#include "mcrl2/pbes/transformation_strategy.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

inline
bool pbes2bool(const pbes& p)
{
  pbes p1 = p;
  pbes_system::algorithms::instantiate_global_variables(p1);
  data::rewriter datar(p1.data(),
                       mcrl2::data::used_data_equation_selector(p1.data(), pbes_system::find_function_symbols(p1), p1.global_variables()),
                       data::jitty);
  pbesinst_alternative_lazy_algorithm algorithm(p1.data(), datar, breadth_first, lazy);
  algorithm.run(p1);
  bes::boolean_equation_system bes = bes::pbesinst_conversion(algorithm.get_result());
  std::vector<bool> full_solution;
  return local_fixpoints(bes, &full_solution);
}

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_PBES2BOOL_H
