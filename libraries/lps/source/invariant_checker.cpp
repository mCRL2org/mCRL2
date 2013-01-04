// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/invariant_checker.cpp
/// \brief Add your file description here.

#include <sstream>
#include <cstring>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/print.h"
#include "mcrl2/lps/invariant_checker.h"
#include "mcrl2/data/detail/prover/solver_type.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/utilities/exception.h"

namespace mcrl2
{
namespace lps
{
namespace detail
{

using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::core;
using namespace mcrl2::log;

// Class Invariant_Checker ------------------------------------------------------------------------
// Class Invariant_Checker - Functions declared private -----------------------------------------

void Invariant_Checker::print_counter_example()
{
  if (f_counter_example)
  {
    data_expression v_counter_example(f_bdd_prover.get_counter_example());
    assert(v_counter_example.defined());
    mCRL2log(info) << "  Counter example: " << data::pp(v_counter_example) << "\n";
  }
}

// --------------------------------------------------------------------------------------------

void Invariant_Checker::save_dot_file(size_t a_summand_number)
{
  if (! f_dot_file_name.empty())
  {
    std::ostringstream v_file_name;

    v_file_name << f_dot_file_name;

    if (a_summand_number == (size_t)-1) // Dangerous
    {
      v_file_name << "-init.dot";
    }
    else
    {
      v_file_name << "-" << a_summand_number << ".dot";
    }
    f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name.str().c_str());
  }
}

// --------------------------------------------------------------------------------------------

bool Invariant_Checker::check_init(const data_expression a_invariant)
{
  std::map < variable, data_expression> v_substitutions;
  const assignment_list l=f_init.assignments();
  for (assignment_list::const_iterator i=l.begin(); i!=l.end(); ++i)
  {
    v_substitutions[i->lhs()]=i->rhs();
  }

  data_expression b_invariant = data::replace_free_variables(a_invariant,
                                data::make_map_substitution(v_substitutions));
  f_bdd_prover.set_formula(b_invariant);
  if (f_bdd_prover.is_tautology() == answer_yes)
  {
    return true;
  }
  else
  {
    if (f_bdd_prover.is_contradiction() != answer_yes)
    {
      print_counter_example();
      save_dot_file((size_t)(-1));
    }
    return false;
  }
}

// --------------------------------------------------------------------------------------------

bool Invariant_Checker::check_summand(
  const data::data_expression a_invariant,
  const lps::action_summand a_summand,
  const size_t a_summand_number)
{
  using namespace mcrl2::data::sort_bool;
  const data_expression v_condition = a_summand.condition();
  const assignment_list v_assignments = a_summand.assignments();

  std::map < variable, data_expression> v_substitutions;

  for (assignment_list::const_iterator i=v_assignments.begin(); i!=v_assignments.end(); ++i)
  {
    v_substitutions[i->lhs()]=i->rhs();
  }

  const data_expression v_subst_invariant = data::replace_free_variables(a_invariant,
      data::make_map_substitution(v_substitutions));

  const data_expression v_formula = implies(and_(a_invariant, v_condition), v_subst_invariant);
  f_bdd_prover.set_formula(v_formula);
  if (f_bdd_prover.is_tautology() == answer_yes)
  {
    mCRL2log(verbose) << "The invariant holds for summand " << a_summand_number << "." << std::endl;
    return true;
  }
  else
  {
    mCRL2log(info) << "The invariant does not hold for summand " << a_summand_number << std::endl;
    if (f_bdd_prover.is_contradiction() != answer_yes)
    {
      print_counter_example();
      save_dot_file(a_summand_number);
    }
    return false;
  }
}

// --------------------------------------------------------------------------------------------

bool Invariant_Checker::check_summands(const data::data_expression a_invariant)
{
  bool v_result = true;
  size_t v_summand_number = 1;

  for (action_summand_vector::const_iterator i=f_summands.begin();
       i!=f_summands.end() && (f_all_violations || v_result); ++i)
  {
    v_result = check_summand(a_invariant, *i, v_summand_number) && v_result;
    v_summand_number++;
  }
  return v_result;
}

// Class Invariant_Checker - Functions declared public --------------------------------------------

Invariant_Checker::Invariant_Checker(
  mcrl2::lps::specification const& a_lps,
  mcrl2::data::rewriter::strategy a_rewrite_strategy, int a_time_limit, bool a_path_eliminator, smt_solver_type a_solver_type,
  bool a_apply_induction, bool a_counter_example, bool a_all_violations, std::string const& a_dot_file_name
):
  f_bdd_prover(a_lps.data(), used_data_equation_selector(a_lps.data()), a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction)
{
  f_init = a_lps.initial_process();
  f_summands = a_lps.process().action_summands();
  f_counter_example = a_counter_example;
  f_all_violations = a_all_violations;
  f_dot_file_name = a_dot_file_name;
}

// --------------------------------------------------------------------------------------------

Invariant_Checker::~Invariant_Checker()
{
  // Nothing to free.
}

// --------------------------------------------------------------------------------------------

bool Invariant_Checker::check_invariant(const data::data_expression a_invariant)
{
  bool v_result = true;

  if (check_init(a_invariant))
  {
    mCRL2log(verbose) << "The invariant holds for the initial state." << std::endl;
  }
  else
  {
    mCRL2log(info) << "The invariant does not hold for the initial state." << std::endl;
    v_result = false;
  }
  if ((f_all_violations || v_result))
  {
    if (check_summands(a_invariant))
    {
      mCRL2log(verbose) << "The invariant holds for all summands." << std::endl;
    }
    else
    {
      mCRL2log(info) << "The invariant does not hold for all summands." << std::endl;
      v_result = false;
    }
  }
  if (v_result)
  {
    mCRL2log(info) << "The invariant holds for this LPS." << std::endl;
  }
  else
  {
    mCRL2log(info) << "The invariant does not hold for this LPS." << std::endl;
  }

  return v_result;
}

} // namespace detail
} // namespace lps
} // namespace mcrl2

