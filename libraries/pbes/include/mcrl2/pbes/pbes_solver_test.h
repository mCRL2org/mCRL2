// Author(s): Jan Friso Groote
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file pbes_solver_test.h
/// \brief This file contains a simple solver for pbesses.
///        This solver is mainly intended for testing purposes.

#ifndef MCRL2_PBES_PBES_SOLVER_TEST_H
#define MCRL2_PBES_PBES_SOLVER_TEST_H

//General includes
#include "mcrl2/core/detail/print_utility.h"

//Rewriters
#include "mcrl2/data/rewriter.h"
#include "mcrl2/pbes/algorithms.h"

//Data framework
#include "mcrl2/data/enumerator.h"
#include "mcrl2/data/selection.h"

//Boolean equation systems
#include "mcrl2/pbes/pbesinst_alternative_lazy_algorithm.h"
#include "mcrl2/pbes/search_strategy.h"
#include "mcrl2/pbes/transformation_strategy.h"
#include "mcrl2/bes/pbesinst_conversion.h"
#include "mcrl2/bes/local_fixpoints.h"
#include "mcrl2/bes/gauss_elimination.h"
#include "mcrl2/bes/small_progress_measures.h"


namespace mcrl2
{

namespace pbes_system
{



inline
bool pbes_instantiation_test_algorithm_test(pbes& pbes_spec, const bool expected_outcome, const data::rewriter::strategy rewrite_strategy)
{
  pbes_system::algorithms::instantiate_global_variables(pbes_spec);
  pbesinst_algorithm algorithm(pbes_spec.data());
  algorithm.run(pbes_spec);
  bes::boolean_equation_system bes = bes::pbesinst_conversion(algorithm.get_result());
  std::vector<bool> full_solution;
  const bool outcome_local_fixed=local_fixpoints(bes, &full_solution);
  const bool outcome_smp=small_progress_measures(bes);
  const bool outcome_gauss=bes::gauss_elimination(bes);
  if ((outcome_local_fixed==expected_outcome) &&
      (outcome_smp==expected_outcome) && 
      (outcome_gauss==expected_outcome))
  {
    return true;
  }
  std::cerr << "----------------------------------------------------------------\n";
  std::cerr << "Solving pbes failed using plain instantiation of a PBES\n";
  std::cerr << "Expected outcome: " << expected_outcome << "\n";
  std::cerr << "Actual outcome local fixed points: " << outcome_local_fixed << "\n";
  std::cerr << "Actual outcome small progress measures: " << outcome_smp << "\n";
  std::cerr << "Actual outcome gauss elimination: " << outcome_gauss << "\n";
  std::cerr << "PBES " << pbes_spec << "\n";
  std::cerr << "----------------------------------------------------------------\n";
  return false;
}

inline
bool alternative_lazy_algorithm_test(pbes& pbes_spec, 
                                     const bool expected_outcome, 
                                     const transformation_strategy trans_strat, 
                                     const search_strategy search_strat,
                                     const data::rewriter::strategy rewrite_strategy)
{
  pbes_system::algorithms::instantiate_global_variables(pbes_spec);

  // Generate an enumerator, a data rewriter and a pbes rewriter.
  const data::rewriter datar(pbes_spec.data(),
                       mcrl2::data::used_data_equation_selector(pbes_spec.data(), pbes_system::find_function_symbols(pbes_spec), pbes_spec.global_variables()),
                       rewrite_strategy);

  pbesinst_alternative_lazy_algorithm algorithm(pbes_spec.data(), datar, search_strat, trans_strat);
  algorithm.run(pbes_spec);
  bes::boolean_equation_system bes = bes::pbesinst_conversion(algorithm.get_result());
  std::vector<bool> full_solution;
  const bool outcome_local_fixed=local_fixpoints(bes, &full_solution);
  const bool outcome_smp=small_progress_measures(bes);
  const bool outcome_gauss=gauss_elimination(bes);
  if ((outcome_local_fixed==expected_outcome) &&
      (outcome_smp==expected_outcome) && 
      (outcome_gauss==expected_outcome))
  {
    return true;
  }
  std::cerr << "----------------------------------------------------------------\n";
  std::cerr << "Solving pbes failed using alternative_lazy instantiation of a PBES\n";
  std::cerr << "Expected outcome: " << expected_outcome << "\n";
  std::cerr << "Actual outcome local fixed points: " << outcome_local_fixed << "\n";
  std::cerr << "Actual outcome small progress measures: " << outcome_smp << "\n";
  std::cerr << "Actual outcome gauss elimination: " << outcome_gauss << "\n";
  std::cerr << "Used transformation strategy " << trans_strat << "\n";
  std::cerr << "Used search strategy " << search_strat << "\n";
  std::cerr << "PBES " << pbes_spec << "\n";
  std::cerr << "----------------------------------------------------------------\n";
  return false;
}

bool pbes2_bool_test(pbes& pbes_spec, const bool expected_outcome, data::rewriter::strategy rewrite_strategy = data::jitty)
{
  bool result=true;
  result &= pbes_instantiation_test_algorithm_test(pbes_spec, expected_outcome, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, lazy, breadth_first, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, optimize, breadth_first, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, on_the_fly, breadth_first, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, on_the_fly_with_fixed_points, breadth_first, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, lazy, depth_first, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, optimize, depth_first, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, on_the_fly, depth_first, rewrite_strategy);
  result &= alternative_lazy_algorithm_test(pbes_spec, expected_outcome, on_the_fly_with_fixed_points, depth_first, rewrite_strategy);
  return result;
}


} // namespace pbes_system
} // namespace mcrl2

#endif // MCRL2_PBES_PBES_SOLVER_TEST_H
