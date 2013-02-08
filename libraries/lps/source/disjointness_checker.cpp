// Author(s): Luc Engelen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file source/disjointness_checker.cpp
/// \brief Add your file description here.

#include "mcrl2/lps/disjointness_checker.h"


namespace mcrl2
{
namespace lps
{
namespace detail
{

using namespace mcrl2::core;
using namespace mcrl2::core::detail;
using namespace mcrl2::data;

// Auxiliary functions ----------------------------------------------------------------------------

static bool disjoint_sets(
  std::set<variable> &a_set_1,
  std::set<variable> &a_set_2)
{
  for (std::set<variable>::const_iterator i = a_set_1.begin(); i!=a_set_1.end(); ++i)
  {
    if (a_set_2.count(*i)>0)
    {
      return false;
    }
  }
  return true;
}

// Class Disjointness_Checker ---------------------------------------------------------------------
// Class Disjointness_Checker - Functions declared private --------------------------------------

void Disjointness_Checker::process_data_expression(const size_t a_summand_number, const data_expression a_expression)
{
  // This should probably once be replaced by a visitor.
  if (is_variable(a_expression))
  {
    f_used_parameters_per_summand[a_summand_number].insert(variable(a_expression));
  }
  else if (is_where_clause(a_expression))
  {
    const where_clause t(a_expression);
    process_data_expression(a_summand_number,t.body());
    const assignment_list assignments=t.assignments();
    for (assignment_list::const_iterator i=assignments.begin(); i!=assignments.end(); ++i)
    {
      process_data_expression(a_summand_number,i->rhs());
    }
  }
  else if (is_function_symbol(a_expression))
  {
    // do nothing
  }
  else if (is_application(a_expression))
  {
    const application &t=atermpp::aterm_cast<application>(a_expression);
    process_data_expression(a_summand_number,t.head());
    for (data_expression_list::const_iterator i=t.begin(); i!=t.end(); ++i)
    {
      process_data_expression(a_summand_number,*i);
    }
  }
  else if (is_abstraction(a_expression))
  {
    process_data_expression(a_summand_number,abstraction(a_expression).body());
  }
  else
  {
    assert(0);
  }
}

// --------------------------------------------------------------------------------------------

void Disjointness_Checker::process_multi_action(size_t a_summand_number, const multi_action v_multi_action)
{
  if (v_multi_action.has_time())
  {
    process_data_expression(a_summand_number, v_multi_action.time());
  }

  const action_list v_actions = v_multi_action.actions();
  for (action_list::const_iterator i=v_actions.begin(); i!=v_actions.end(); ++i)
  {
    const data_expression_list v_expressions=i->arguments();

    for (data_expression_list::const_iterator j=v_expressions.begin(); j!=v_expressions.end(); ++j)
    {
      process_data_expression(a_summand_number, *j);
    }
  }
}

// --------------------------------------------------------------------------------------------

void Disjointness_Checker::process_summand(size_t a_summand_number, const action_summand a_summand)
{
  // variables used in condition
  process_data_expression(a_summand_number, a_summand.condition());

  // variables used in multiaction
  process_multi_action(a_summand_number, a_summand.multi_action());

  // variables used and changed in assignments
  const assignment_list v_assignments=a_summand.assignments();
  for (assignment_list::const_iterator i=v_assignments.begin(); i!=v_assignments.end(); ++i)
  {
    // variables changed in the assignment
    f_changed_parameters_per_summand[a_summand_number].insert(i->lhs());

    // variables used in assignment
    process_data_expression(a_summand_number, i->rhs());
  }
}

// Class Disjointness_Checker - Functions declared public ---------------------------------------

Disjointness_Checker::Disjointness_Checker(const linear_process& a_process_equation)
{
  const action_summand_vector v_summands = a_process_equation.action_summands();
  size_t v_summand_number = 1;

  f_number_of_summands = v_summands.size();
  f_used_parameters_per_summand =
    std::vector< std::set < variable > >(f_number_of_summands + 1,std::set < variable >());
  f_changed_parameters_per_summand =
    std::vector< std::set < variable > >(f_number_of_summands + 1,std::set < variable >());

  for (action_summand_vector::const_iterator i=v_summands.begin(); i!=v_summands.end(); ++i)
  {
    process_summand(v_summand_number, *i);
    v_summand_number++;
  }
}

// --------------------------------------------------------------------------------------------

Disjointness_Checker::~Disjointness_Checker()
{
}

// --------------------------------------------------------------------------------------------

bool Disjointness_Checker::disjoint(size_t a_summand_number_1, size_t a_summand_number_2)
{
  assert((a_summand_number_1 <= f_number_of_summands) && (a_summand_number_2 <= f_number_of_summands));

  bool v_used_1_changed_2 = disjoint_sets(
                              f_used_parameters_per_summand[a_summand_number_1],
                              f_changed_parameters_per_summand[a_summand_number_2]);
  bool v_used_2_changed_1 = disjoint_sets(
                              f_used_parameters_per_summand[a_summand_number_2],
                              f_changed_parameters_per_summand[a_summand_number_1]);
  bool v_changed_1_changed_2 = disjoint_sets(
                                 f_changed_parameters_per_summand[a_summand_number_1],
                                 f_changed_parameters_per_summand[a_summand_number_2]);
  return v_used_1_changed_2 && v_used_2_changed_1 && v_changed_1_changed_2;
}

} // namespace detail
} // namespace lps
} // namespace mcrl2

