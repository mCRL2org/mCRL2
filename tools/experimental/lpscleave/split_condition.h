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
// #include "mcrl2/data/standard_utility.h"
#include "mcrl2/data/join.h"
#include "lpscleave_utility.h"

#include <queue>

using namespace mcrl2;

/// \brief Make a data expression consisting of the clauses in conjective normal form.
inline data::data_expression make_conjuntions(const std::set<data::data_expression>& clauses)
{
  data::data_expression result = data::sort_bool::true_();
  for (const data::data_expression& clause : clauses)
  {
    result = data::sort_bool::and_(result, clause);
  }

  return result;
}

/// \brief Given a set of clauses and a set of variables computes a subset of clauses such that expressions over the given variables are not thrown away.
inline std::set<data::data_expression> compute_clauses(const std::set<data::data_expression>& clauses, std::set<data::variable> dependencies)
{
  std::set<data::data_expression> result;

  // Repeat until the set of dependencies stabilizes.
  std::size_t old_size = 0;
  do
  {
    old_size = dependencies.size();

    // Add all clauses over these dependencies
    for (const auto& clause : clauses)
    {
      std::set<data::variable> clause_deps = data::find_free_variables(clause);
      if (clause_deps.empty())
      {
        result.insert(clause);
      }
      else
      {
        std::set<data::variable> intersection;
        std::set_intersection(dependencies.begin(), dependencies.end(), clause_deps.begin(), clause_deps.end(), std::inserter(intersection, intersection.begin()));

        if (!intersection.empty())
        {
          result.insert(clause);
          dependencies.insert(clause_deps.begin(), clause_deps.end());
        }
      }
    }

  } while (old_size != dependencies.size());

  return result;
}

/// \brief Stores the information about the conditions.
struct cleave_condition
{
  data::data_expression expression;
  data::data_expression_vector implicit;
};

/// \brief Split the given condition based on a number of (simple) heuristics.
inline std::pair<cleave_condition, cleave_condition> split_condition(const data::data_expression& condition,
    const data::variable_list& left_parameters,
    const data::variable_list& right_parameters,
    const data::variable_list& summand_variables,
    const std::set<data::variable>& synchronized)
{
  assert(data::sort_bool::is_bool(condition.sort()));

  mCRL2log(log::debug) << "Splitting condition " << condition << "...\n";

  // First of all consider each clause in a conjunctive form separately.
  std::set<data::data_expression> clauses = split_and(condition);

  // The resulting conditions and dependencies.
  cleave_condition left_condition;
  cleave_condition right_condition;
  std::set<data::variable> left_dependencies;
  std::set<data::variable> right_dependencies;

  // Parameters including the summand variables.
  data::variable_list left_all_parameters = left_parameters + summand_variables;
  data::variable_list right_all_parameters = right_parameters + summand_variables;

  // Make equality clauses implicit if it is sensible.
  for (auto it = clauses.begin(); it != clauses.end(); )
  {
    const auto& clause = *it;

    bool remove_clause = false;
    if (data::is_equal_to_application(clause))
    {
      // Check if it makes sense to remove this clause.
      std::set<data::variable> dependencies = data::find_free_variables(clause);
      if (!is_subset(dependencies, left_all_parameters) && !is_subset(dependencies, right_all_parameters))
      {
        // For equality clauses we can move the operands around.
        const auto& application = static_cast<data::application>(clause);

        std::set<data::variable> left_variables = data::find_free_variables(application[0]);
        std::set<data::variable> right_variables = data::find_free_variables(application[1]);

        // If both sides are not constants, now find their optimal side.
        if (!left_variables.empty() && !right_variables.empty())
        {
          // The condition should be split.
          if (is_subset(left_variables, left_all_parameters) && is_subset(right_variables, right_all_parameters))
          {
            mCRL2log(log::debug) << "Made condition " << clause << " implicit\n";
            left_condition.implicit.push_back(application[0]);
            right_condition.implicit.push_back(application[1]);
            remove_clause = true;
          }
          else if (is_subset(left_variables, right_all_parameters) && is_subset(right_variables, left_all_parameters))
          {
            mCRL2log(log::debug) << "Made condition " << clause << " implicit\n";
            left_condition.implicit.push_back(application[1]);
            right_condition.implicit.push_back(application[0]);
            remove_clause = true;
          }
        }
      }
    }

    // Remove clause
    if (remove_clause)
    {
      it = clauses.erase(it);
    }
    else
    {
      ++it;
    }
  }

  // Extend the dependencies with the synchronisation, summand and original parameters.
  left_dependencies.insert(left_parameters.begin(), left_parameters.end());
  left_dependencies.insert(summand_variables.begin(), summand_variables.end());
  left_dependencies.insert(synchronized.begin(), synchronized.end());

  right_dependencies.insert(right_parameters.begin(), right_parameters.end());
  right_dependencies.insert(summand_variables.begin(), summand_variables.end());
  right_dependencies.insert(synchronized.begin(), synchronized.end());

  std::set<data::data_expression> left_clauses = compute_clauses(clauses, left_dependencies);
  std::set<data::data_expression> right_clauses = compute_clauses(clauses, right_dependencies);

  left_condition.expression = make_conjuntions(left_clauses);
  right_condition.expression = make_conjuntions(right_clauses);

  mCRL2log(log::debug) << "Split condition into " << left_condition.expression << ", and " << right_condition.expression << "\n";
  return std::make_pair(left_condition, right_condition);
}

#endif // MCRL2_SPLIT_CONDITION_H_
