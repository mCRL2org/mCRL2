// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file confluence_checker.cpp

#include <cstdlib>
#include <cstring>
#include <string>
#include <algorithm>

#include "mcrl2/utilities/logger.h"
#include "mcrl2/core/detail/struct_core.h"
#include "mcrl2/data/detail/bdd_prover.h"
#include "mcrl2/lps/confluence_checker.h"
#include "mcrl2/utilities/exception.h"

using namespace mcrl2::log;

namespace mcrl2
{
namespace lps
{
namespace detail
{


using namespace mcrl2;
using namespace mcrl2::data;
using namespace mcrl2::data::detail;
using namespace mcrl2::core;
using namespace mcrl2::core::detail;

// Auxiliary functions ----------------------------------------------------------------------------

static
std::map < variable,data_expression> get_substitutions_from_assignments(const assignment_list a_assignments)
{
  std::map < variable,data_expression> v_substitutions;

  for (assignment_list::const_iterator i=a_assignments.begin(); i!=a_assignments.end(); ++i)
  {
    v_substitutions[i->lhs()]=i->rhs();
  }
  return v_substitutions;
}

// ----------------------------------------------------------------------------------------------
static
data_expression get_subst_equation_from_assignments(
  const variable_list a_variables,
  assignment_list a_assignments_1,
  assignment_list a_assignments_2,
  const std::map<variable,data_expression> &a_substitutions_1,
  const std::map<variable,data_expression> &a_substitutions_2)
{
  data_expression v_result = sort_bool::true_();

  const assignment_list v_assignment_1, v_assignment_2;
  variable v_variable_1, v_variable_2;
  data_expression v_expression_1, v_expression_2;
  bool v_next_1 = true, v_next_2 = true;

  for (variable_list::const_iterator i=a_variables.begin(); i!=a_variables.end();)
  {
    variable v_variable = *i;
    ++i;

    if (!a_assignments_1.empty() && v_next_1)
    {
      const assignment v_assignment_1 = a_assignments_1.front();
      a_assignments_1.pop_front();
      v_variable_1 = v_assignment_1.lhs();
      v_expression_1 = v_assignment_1.rhs();
      v_expression_1 = data::replace_free_variables(v_expression_1,
                       data::make_map_substitution(a_substitutions_2));
    }
    if (!a_assignments_2.empty() && v_next_2)
    {
      const assignment v_assignment_2 = a_assignments_2.front();
      a_assignments_2.pop_front();
      v_variable_2 = v_assignment_2.lhs();
      v_expression_2 = v_assignment_2.rhs();
      v_expression_2 = data::replace_free_variables(v_expression_2,
                       data::make_map_substitution(a_substitutions_1));
    }
    while (v_variable != v_variable_1 && v_variable != v_variable_2 && i!=a_variables.end())
    {
      v_variable = *i;
      ++i;
    }
    if (v_variable_1 == v_variable_2)
    {
      v_result = sort_bool::and_(v_result, equal_to(v_expression_1, v_expression_2));
      v_next_1 = true;
      v_next_2 = true;
    }
    else if (v_variable == v_variable_1)
    {
      data_expression expr = data::replace_free_variables(data_expression(v_variable_1),
                     data::make_map_substitution(a_substitutions_1));
      v_result = sort_bool::and_(data_expression(v_result), equal_to(v_expression_1, expr));
      v_next_1 = true;
      v_next_2 = false;
    }
    else if (v_variable == v_variable_2)
    {
      data_expression expr = data::replace_free_variables(data_expression(v_variable_2),
                     data::make_map_substitution(a_substitutions_2));
      v_result = sort_bool::and_(data_expression(v_result), equal_to(data_expression(v_expression_2), expr));
      v_next_1 = false;
      v_next_2 = true;
    }
  }
  return v_result;
}

// ----------------------------------------------------------------------------------------------

static
data_expression get_equation_from_assignments(
  const variable_list a_variables,
  assignment_list a_assignments_1,
  assignment_list a_assignments_2)
{
  data_expression v_result = sort_bool::true_();

  for (variable_list::const_iterator i=a_variables.begin(); i!=a_variables.end(); ++i)
  {
    const variable v_variable=*i;
    if (!a_assignments_1.empty() && v_variable == a_assignments_1.front().lhs())
    {
      if (!a_assignments_2.empty() && v_variable == a_assignments_2.front().lhs())
      {
        // Create a condition from the assigments from both lists.
        v_result = sort_bool::and_(v_result, equal_to(a_assignments_1.front().rhs(), a_assignments_2.front().rhs()));
        a_assignments_2.pop_front();
      }
      else
      {
        // Create a condition from first assigment only.
        v_result = sort_bool::and_(v_result, equal_to(a_assignments_1.front().rhs(), v_variable));
      }
      a_assignments_1.pop_front();
    }
    else
    {
      if (!a_assignments_2.empty() && v_variable == a_assignments_2.front().lhs())
      {
        // Create a condition from the second assigments only.
        v_result = sort_bool::and_(v_result, equal_to(v_variable, a_assignments_2.front().rhs()));
        a_assignments_2.pop_front();
      }
    }
  }

  assert(a_assignments_1.empty()); // If this is not the case, the assignments do not have the
  assert(a_assignments_2.empty()); // same order as the list of variables. This means that some equations
                                   // have not been generated.
  return v_result;
}

// ----------------------------------------------------------------------------------------------
static
data_expression get_subst_equation_from_actions(
  const action_list a_actions,
  const std::map<variable,data_expression> &a_substitutions)
{
  data_expression v_result = sort_bool::true_();

  for (action_list::const_iterator i=a_actions.begin(); i!=a_actions.end(); ++i)
  {
    const data_expression_list v_expressions = i->arguments();
    for (data_expression_list::const_iterator j=v_expressions.begin(); j!=v_expressions.end(); ++j)
    {
      const data_expression v_subst_expression = data::replace_free_variables(*j,
          data::make_map_substitution(a_substitutions));
      v_result = sort_bool::and_(data_expression(v_result), equal_to(*j, v_subst_expression));
    }
  }
  return v_result;
}

// ----------------------------------------------------------------------------------------------

static
data_expression get_confluence_condition(
  const data_expression a_invariant,
  const action_summand a_summand_1,
  const action_summand a_summand_2,
  const variable_list a_variables)
{
  assert(a_summand_1.is_tau());

  const data_expression v_condition_1 = a_summand_1.condition();
  const assignment_list v_assignments_1 = a_summand_1.assignments();

  std::map < variable,data_expression> v_substitutions_1 = get_substitutions_from_assignments(v_assignments_1);
  const data_expression v_condition_2 = a_summand_2.condition();
  const data_expression v_lhs = sort_bool::and_(sort_bool::and_(v_condition_1, v_condition_2), a_invariant);
  const assignment_list v_assignments_2 = a_summand_2.assignments();

  std::map < variable,data_expression> v_substitutions_2 = get_substitutions_from_assignments(v_assignments_2);
  const data_expression v_subst_condition_1 = data::replace_free_variables(v_condition_1,
      data::make_map_substitution(v_substitutions_2));
  const data_expression v_subst_condition_2 = data::replace_free_variables(v_condition_2,
      data::make_map_substitution(v_substitutions_1));

  const data_expression v_subst_equation = get_subst_equation_from_assignments(a_variables, v_assignments_1, v_assignments_2, v_substitutions_1, v_substitutions_2);

  const action_list v_actions =a_summand_2.multi_action().actions();
  data_expression v_rhs;

  if (v_actions.empty())
  {
    // tau-summand
    const data_expression  v_equation = get_equation_from_assignments(a_variables, v_assignments_1, v_assignments_2);
    v_rhs = sort_bool::and_(v_subst_condition_1, v_subst_condition_2);
    v_rhs = sort_bool::and_(v_rhs, v_subst_equation);
    v_rhs = sort_bool::or_(v_equation, v_rhs);
  }
  else
  {
    // non-tau-summand
    const data_expression v_actions_equation = get_subst_equation_from_actions(v_actions, v_substitutions_1);
    v_rhs = sort_bool::and_(v_subst_condition_1, v_subst_condition_2);
    v_rhs = sort_bool::and_(v_rhs, v_actions_equation);
    v_rhs = sort_bool::and_(v_rhs, data_expression(v_subst_equation));
  }
  return sort_bool::implies(v_lhs, v_rhs);
}

// --------------------------------------------------------------------------------------------

static
bool has_ctau_action(const specification& a_lps)
{
  const action_label_list v_action_specification = a_lps.action_labels();
  return std::find(v_action_specification.begin(),v_action_specification.end(),make_ctau_act_id())!=v_action_specification.end();
}


// Class Confluence_Checker -----------------------------------------------------------------------
// Class Confluence_Checker - Functions declared private ----------------------------------------

void Confluence_Checker::save_dot_file(size_t a_summand_number_1, size_t a_summand_number_2)
{
  if (!f_dot_file_name.empty())
  {
    std::ostringstream v_file_name(f_dot_file_name);

    v_file_name << "-" << a_summand_number_1 << "-" << a_summand_number_2 << ".dot";

    f_bdd2dot.output_bdd(f_bdd_prover.get_bdd(), v_file_name.str().c_str());
  }
}

// --------------------------------------------------------------------------------------------

void Confluence_Checker::print_counter_example()
{
  if (f_counter_example)
  {
    const data_expression v_counter_example(f_bdd_prover.get_counter_example());
    mCRL2log(info) << "  Counter example: " << data::pp(v_counter_example) << "\n";
  }
}

// --------------------------------------------------------------------------------------------

bool Confluence_Checker::check_summands(
  const data_expression a_invariant,
  const action_summand a_summand_1,
  const size_t a_summand_number_1,
  const action_summand a_summand_2,
  const size_t a_summand_number_2)
{
  assert(a_summand_1.is_tau());

  const variable_list v_variables = f_lps.process().process_parameters();
  bool v_is_confluent = true;

  if (f_disjointness_checker.disjoint(a_summand_number_1, a_summand_number_2))
  {
    mCRL2log(info) << ":";
  }
  else
  {
    const data_expression v_condition = get_confluence_condition(a_invariant, a_summand_1, a_summand_2, v_variables);
    f_bdd_prover.set_formula(v_condition);
    if (f_bdd_prover.is_tautology() == answer_yes)
    {
      mCRL2log(info) << "+";
    }
    else
    {
      if (f_generate_invariants)
      {
        const data_expression v_new_invariant(f_bdd_prover.get_bdd());
        mCRL2log(verbose) << "\nChecking invariant: " << data::pp(v_new_invariant) << "\n";
        if (f_invariant_checker.check_invariant(v_new_invariant))
        {
          mCRL2log(verbose) << "Invariant holds" << std::endl;
          mCRL2log(info) << "i";
        }
        else
        {
          mCRL2log(verbose) << "Invariant doesn't hold" << std::endl;
          v_is_confluent = false;
          if (f_check_all)
          {
            mCRL2log(info) << "-";
          }
          else
          {
            mCRL2log(info) << "Not confluent with summand " << a_summand_number_2 << ".";
          }
          print_counter_example();
          save_dot_file(a_summand_number_1, a_summand_number_2);
        }
      }
      else
      {
        v_is_confluent = false;
        if (f_check_all)
        {
          mCRL2log(info) << "-";
        }
        else
        {
          mCRL2log(info) << "Not confluent with summand " << a_summand_number_2 << ".";
        }
        print_counter_example();
        save_dot_file(a_summand_number_1, a_summand_number_2);
      }
    }

  }
  return v_is_confluent;
}

// --------------------------------------------------------------------------------------------

action_summand Confluence_Checker::check_confluence_and_mark_summand(
  const data_expression a_invariant,
  const action_summand a_summand,
  const size_t a_summand_number,
  bool& a_is_marked)
{
  assert(a_summand.is_tau());
  const action_summand_vector v_summands = f_lps.process().action_summands();
  size_t v_summand_number = 1;
  bool v_is_confluent = true;
  bool v_current_summands_are_confluent;

  // Add here that the sum variables of a_summand must be empty otherwise
  // the confluence of the summand must be checked with respect to itself,
  // which requires quantification. Otherwise tau.a+tau.b will be designated
  // tau-confluent, if linearised with summand clustering.

  const variable_list a_summand_sum_variables=a_summand.summation_variables();
  if (!a_summand_sum_variables.empty())
  {
    v_is_confluent = false;
    mCRL2log(info) << "Summand " << a_summand_number << " is not proven confluent because it contains a sum operator.";
  }

  for (action_summand_vector::const_iterator i=v_summands.begin();
       i!=v_summands.end() && (v_is_confluent || f_check_all); ++i)
  {
    const action_summand v_summand = *i;

    if (v_summand_number < a_summand_number)
    {
      if (f_intermediate[v_summand_number] > a_summand_number)
      {
        mCRL2log(info) << ".";
        v_summand_number++;
      }
      else
      {
        if (f_intermediate[v_summand_number] == a_summand_number)
        {
          if (f_check_all)
          {
            mCRL2log(info) << "-";
          }
          else
          {
            mCRL2log(info) << "Not confluent with summand " << v_summand_number << ".";
          }
          v_is_confluent = false;
        }
        else
        {
          v_current_summands_are_confluent = check_summands(a_invariant, a_summand, a_summand_number, v_summand, v_summand_number);
          if (v_current_summands_are_confluent)
          {
            v_summand_number++;
          }
          else
          {
            v_is_confluent = false;
          }
        }
      }
    }
    else
    {
      v_current_summands_are_confluent = check_summands(a_invariant, a_summand, a_summand_number, v_summand, v_summand_number);
      if (v_current_summands_are_confluent)
      {
        v_summand_number++;
      }
      else
      {
        v_is_confluent = false;
      }
    }
  }

  if (!f_check_all)
  {
    f_intermediate[a_summand_number] = v_summand_number;
  }

  if (v_is_confluent)
  {
    mCRL2log(info) << "Confluent with all summands.";
    a_is_marked = true;
    return action_summand(a_summand.summation_variables(),
                          a_summand.condition(),
                          multi_action(make_ctau_action()),
                          a_summand.assignments());
  }
  else
  {
    return a_summand;
  }
}

// Class Confluence_Checker - Functions declared public -----------------------------------------

Confluence_Checker::Confluence_Checker(
  mcrl2::lps::specification const& a_lps,
  mcrl2::data::rewriter::strategy a_rewrite_strategy,
  int a_time_limit,
  bool a_path_eliminator,
  smt_solver_type a_solver_type,
  bool a_apply_induction,
  bool a_no_marking,
  bool a_check_all,
  bool a_counter_example,
  bool a_generate_invariants,
  std::string const& a_dot_file_name):
  f_disjointness_checker(lps::linear_process_to_aterm(a_lps.process())),
  f_invariant_checker(a_lps, a_rewrite_strategy, a_time_limit, a_path_eliminator, a_solver_type, false, false, 0),
  f_bdd_prover(a_lps.data(), used_data_equation_selector(a_lps.data()), a_rewrite_strategy,
                     a_time_limit, a_path_eliminator, a_solver_type, a_apply_induction),
  f_lps(a_lps),
  f_no_marking(a_no_marking),
  f_check_all(a_check_all),
  f_counter_example(a_counter_example),
  f_dot_file_name(a_dot_file_name),
  f_generate_invariants(a_generate_invariants)
{
  if (has_ctau_action(a_lps))
  {
    throw mcrl2::runtime_error("An action named \'ctau\' already exists.\n");
  }
}

// --------------------------------------------------------------------------------------------

Confluence_Checker::~Confluence_Checker()
{}

// --------------------------------------------------------------------------------------------

specification Confluence_Checker::check_confluence_and_mark(const data_expression a_invariant, const size_t a_summand_number)
{
  const linear_process v_process_equation = f_lps.process();
  action_summand_vector v_summands = v_process_equation.action_summands();
  bool v_is_marked = false;
  size_t v_summand_number = 1;

  f_number_of_summands = v_summands.size();
  f_intermediate = std::vector<size_t>(f_number_of_summands + 2, 0);

  for (action_summand_vector::iterator i=v_summands.begin(); i!=v_summands.end(); ++i)
  {
    const action_summand v_summand = *i;
    if ((a_summand_number == v_summand_number) || (a_summand_number == 0))
    {
      if (v_summand.is_tau())
      {
        mCRL2log(info) << "tau-summand " << v_summand_number << ": ";
        *i = check_confluence_and_mark_summand(a_invariant, v_summand, v_summand_number, v_is_marked);
        mCRL2log(info) << std::endl;
      }
    }
    v_summand_number++;
  }
  const linear_process new_process_equation(
    v_process_equation.process_parameters(),
    v_process_equation.deadlock_summands(),
    v_summands);

  action_label_list v_act_decls=f_lps.action_labels();
  if (v_is_marked && !has_ctau_action(f_lps))
  {
    v_act_decls.push_front(make_ctau_act_id());
  }

  specification v_lps(f_lps.data(),v_act_decls,f_lps.global_variables(),new_process_equation,f_lps.initial_process());

  f_intermediate = std::vector<size_t>();

  return v_lps;
}

} // namespace detail
} // namespace lps
} // namespace mcrl2

