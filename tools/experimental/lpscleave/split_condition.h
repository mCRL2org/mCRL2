// Author(s): Maurice Laveaux
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef MCRL2_SPLIT_CONDITION_H_
#define MCRL2_SPLIT_CONDITION_H_

#include "mcrl2/data/data_expression.h"

using namespace mcrl2;

/// \brief Make a data expression consisting of the clauses in conjective normal form.
data::data_expression make_conjuntions(const std::list<data::data_expression>& clauses)
{
  data::data_expression result = data::sort_bool::true_();
  for (const data::data_expression& clause : clauses)
  {
    result = data::sort_bool::and_(result, clause);
  }

  return result;
}

/// \brief Stores the information about the conditions.
struct cleave_condition
{
  data::data_expression expression;
  data::data_expression_list implicit;
};

/// \brief Split the given condition based on a number of (simple) heuristics.
std::pair<cleave_condition, cleave_condition> split_condition(
  const data::data_expression& condition,
  data::variable_list left_parameters,
  data::variable_list right_parameters,
  const data::variable_list& summand_variables)
{
  assert(data::sort_bool::is_bool(condition.sort()));
  mCRL2log(log::debug) << "Condition " << condition << "\n";

  left_parameters = left_parameters + summand_variables;
  right_parameters = right_parameters + summand_variables;

  // First of all consider each clause in a conjunctive normal form separately.
  std::list<data::data_expression> clauses;
  data::data_expression lhs = condition;
  do
  {
    if (data::sort_bool::is_and_application(lhs))
    {
      const auto& application = static_cast<data::application>(lhs);
      clauses.push_front(application[1]);

      // Consider the next left hand side.
      lhs = application[0];
    }

    if (!data::sort_bool::is_and_application(lhs))
    {
      clauses.push_front(lhs);
    }
  }
  while (data::sort_bool::is_and_application(lhs));

  // Resulting conditions.
  cleave_condition left_condition;
  cleave_condition right_condition;

  // Separate the clauses.
  std::list<data::data_expression> left_clauses;
  std::list<data::data_expression> right_clauses;
  for (const data::data_expression& clause : clauses)
  {
    std::set<data::variable> dependencies = data::find_free_variables(clause);

    // Determine where its dependencies are located.
    // TODO: Consider other dependencies (action, and update) when throwing away conditions.
    bool split = false;
    if (is_subset(dependencies, left_parameters))
    {
      left_clauses.push_front(clause);
      split = true;
    }
    if (is_subset(dependencies, right_parameters))
    {
      right_clauses.push_back(clause);
      split = true;
    }

    if (!split)
    {
      if (data::is_equal_to_application(clause))
      {
        // For equality clauses we can move the operands around.
        const auto& application = static_cast<data::application>(clause);

        std::set<data::variable> left_dependencies = data::find_free_variables(application[0]);
        std::set<data::variable> right_dependencies = data::find_free_variables(application[1]);

        // These conditions are added to the synchronization.
        if (is_subset(left_dependencies, left_parameters) && is_subset(right_dependencies, right_parameters))
        {
          mCRL2log(log::debug) << "Made condition " << clause << " implicit\n";
          left_condition.implicit.push_front(application[0]);
          right_condition.implicit.push_front(application[1]);
        }
        else if (is_subset(left_dependencies, right_parameters) && is_subset(right_dependencies, left_parameters))
        {
          mCRL2log(log::debug) << "Made condition " << clause << " implicit\n";
          right_condition.implicit.push_front(application[0]);
          left_condition.implicit.push_front(application[1]);
        }
        else
        {
          mCRL2log(log::debug) << "Kept condition " << clause << "\n";
          left_clauses.push_front(clause);
          right_clauses.push_back(clause);
        }
      }
    }
  }

  left_condition.expression = make_conjuntions(left_clauses);
  right_condition.expression = make_conjuntions(right_clauses);

  mCRL2log(log::debug) << "Split condition into " << left_condition.expression << ", and " << right_condition.expression << "\n";
  return std::make_pair(left_condition, right_condition);
}

#endif // MCRL2_SPLIT_CONDITION_H_