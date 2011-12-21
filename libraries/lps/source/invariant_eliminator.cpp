// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/invariant_eliminator.cpp
/// \brief Add your file description here.

#include "mcrl2/utilities/logger.h"
#include "mcrl2/aterm/aterm_ext.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/lps/invariant_eliminator.h"


namespace mcrl2
{
namespace lps
{
namespace detail
{

using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::lps;
using namespace mcrl2::log;

// Class Invariant_Eliminator ---------------------------------------------------------------------
// Class Invariant_Eliminator - Functions declared private --------------------------------------

mcrl2::lps::deprecated::summand Invariant_Eliminator::simplify_summand(
  const deprecated::summand a_summand,
  const data_expression a_invariant,
  const bool a_no_elimination,
  const size_t a_summand_number)
{
  const data_expression v_condition = a_summand.condition();
  const data_expression v_formula = lazy::and_(a_invariant, v_condition);

  if (a_no_elimination)
  {
    return deprecated::summand(a_summand.summation_variables(),
                   v_formula,
                   a_summand.is_delta(),
                   a_summand.actions(),
                   a_summand.time(),
                   a_summand.assignments());
  }

  f_bdd_prover.set_formula(v_formula);
  if (f_bdd_prover.is_contradiction() == answer_yes)
  {
    return deprecated::summand(variable_list(),sort_bool::false_(),deadlock(sort_real::real_(0)));
  }
  else
  {
    if (f_simplify_all)
    {
      mCRL2log(info) << "Summand number " << a_summand_number << " is simplified." << std::endl;
      return deprecated::summand(a_summand.summation_variables(),
                     data_expression(f_bdd_prover.get_bdd()),
                     a_summand.is_delta(),
                     a_summand.actions(),
                     a_summand.time(),
                     a_summand.assignments());
    }
    return a_summand;
  }
}

// Class Invariant_Eliminator - Functions declared public ---------------------------------------

Invariant_Eliminator::Invariant_Eliminator(
  const mcrl2::lps::specification& a_lps,
  const mcrl2::data::rewriter::strategy a_rewrite_strategy /* = mcrl2::data::rewriter::jitty */,
  const int a_time_limit /* = 0 */,
  const bool a_path_eliminator /* = false */,
  const mcrl2::data::detail::smt_solver_type a_solver_type /* = mcrl2::data::detail::solver_type_ario */,
  const bool a_apply_induction /* = false */,
  const bool a_simplify_all /* = false */):
  f_bdd_prover(a_lps.data(), used_data_equation_selector(a_lps.data()),a_rewrite_strategy,
    a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction),
    f_lps(a_lps), f_simplify_all(a_simplify_all)
{
}

// --------------------------------------------------------------------------------------------

Invariant_Eliminator::~Invariant_Eliminator()
{}

// --------------------------------------------------------------------------------------------

specification Invariant_Eliminator::simplify(
  const mcrl2::data::data_expression a_invariant,
  const bool a_no_elimination,
  const size_t a_summand_number)
{
  const deprecated::summand_list v_summands = deprecated::linear_process_summands(f_lps.process());
  lps::deprecated::summand_list v_simplified_summands;
  size_t v_summand_number = 1;

  for (deprecated::summand_list::const_iterator i=v_summands.begin();
       i!=v_summands.end(); ++i)
  {
    lps::deprecated::summand v_summand = *i;
    if ((a_summand_number == v_summand_number) || (a_summand_number == 0))
    {
      v_summand = simplify_summand(v_summand, a_invariant, a_no_elimination, v_summand_number);
      if ((v_summand.condition() != sort_bool::false_())||a_no_elimination)
      {
        v_simplified_summands = push_front(v_simplified_summands, v_summand);
        if (!a_no_elimination)
        {
          mCRL2log(verbose) << "Summand number " << v_summand_number << " could not be eliminated." << std::endl;
        }
      }
      else
      {
        mCRL2log(info) << "Summand number " << v_summand_number << " is eliminated." << std::endl;
      }
    }
    v_summand_number++;
  }
  v_simplified_summands = reverse(v_simplified_summands);
  linear_process v_process = f_lps.process();
  lps::deprecated::set_linear_process_summands(v_process, v_simplified_summands);
  return specification(f_lps.data(),f_lps.action_labels(),f_lps.global_variables(),v_process, f_lps.initial_process());
}


} // namespace detail
} // namespace lps
} // namespace mcrl2

