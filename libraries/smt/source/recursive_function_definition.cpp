// Author(s): Ruud Koolen
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <map>
#include <set>

#include "mcrl2/data/find.h"
#include "mcrl2/data/join.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/set_identifier_generator.h"
#include "mcrl2/data/standard.h"
#include "mcrl2/data/substitutions/map_substitution.h"
#include "mcrl2/data/substitutions/variable_assignment.h"
#include "mcrl2/smt/recursive_function_definition.h"

namespace mcrl2
{

namespace smt
{

/*
 * A rule describes a partially pattern-matched rewrite rule.
 * match_criteria is a set of data_expression pairs (A, B)
 * where A is a data_expression over variables in the left hand
 * side of a function definition, and B is a pattern consisting of
 * constructor applications over free variables.
 *
 * The rule match_criteria = { (A, B), (C, D) }, condition = E, rhs = R
 * describes the following rewrite proposition:
 *
 *  "if the data expression A pattern-matches to pattern B, and the
 *   data expression C pattern-matches to pattern D, and the condition
 *   E is true after substituting the proper pattern-matching variables,
 *   then the right hand side R applies (again with substitution of
 *   pattern-matched variables."
 *
 * Pattern matching can then be performed by deconstructing the patterns
 * in the right hand sides of match_criteria, and rewriting rules accordingly.
 * As an example, the following rewrite rule:
 *
 *   is_even(n) -> sign_of_list_sum(n |> l) = sign_of_list_sum(l)
 *
 * Can be represented as the following rule:
 *
 *   match_criteria = { v1 -> n |> l }, condition = is_even(n), rhs = sign_of_list_sum(l)
 *
 * Which, after one step of pattern matching, gets simplified to the following rule:
 *
 *   match_criteria = { head(v1) -> n, tail(v1) -> l }, condition = is_even(n), rhs = sign_of_list_sum(l)
 */
struct rule
{
  std::map<data::data_expression, data::data_expression> match_criteria;
  data::data_expression rhs;
  data::data_expression condition;
  std::set<data::variable> variables;
};

/*
 * For a list of rules with equal left hand sides of match_criteria and only variables in
 * the right hand sides of match_criteria, construct a right hand side based on the
 * conditions and right hand sides of the rules.
 */
static data::data_expression construct_condition_rhs(data_specification *data_specification, const std::vector<rule>& rules, const data::sort_expression& sort)
{
  data::data_expression_vector negated_conditions;
  for (std::vector<rule>::const_iterator i = rules.begin(); i != rules.end(); i++)
  {
    negated_conditions.push_back(data::sort_bool::not_(i->condition));
  }
  data::data_expression else_clause = data::join_and(negated_conditions.begin(), negated_conditions.end());

  // TODO: Check whether else_clause is equivalent to false. Can we use the enumerator for this?
  bool conditions_are_complete = false;
  if (rules.size() == 1 && rules[0].condition == data::sort_bool::true_())
  {
    conditions_are_complete = true;
  }
  data::data_expression result;
  if (!conditions_are_complete)
  {
    result = data_specification->representative(sort);
  }

  for (std::vector<rule>::const_iterator i = rules.begin(); i != rules.end(); i++)
  {
    std::map<data::variable, data::data_expression> substitution_map;
    for (std::map<data::data_expression, data::data_expression>::const_iterator j = i->match_criteria.begin(); j != i->match_criteria.end(); j++)
    {
      assert(data::is_variable(j->second));
      substitution_map[data::variable(j->second)] = j->first;
    }
    data::map_substitution<std::map<data::variable, data::data_expression> > substitution(substitution_map);

    data::data_expression condition = data::replace_free_variables(i->condition, substitution);
    data::data_expression rhs = data::replace_free_variables(i->rhs, substitution);

    if (result == data::data_expression())
    {
      result = rhs;
    }
    else
    {
      result = data::if_(condition, rhs, result);
    }
  }

  return result;
}

/*
 * For a list of rules with equal left hand sides of match_criteria, construct a right hand side.
 */
static data::data_expression construct_rhs(data_specification *data_specification, const std::vector<rule>& rules, const data::sort_expression& sort)
{
  if (rules.empty())
  {
    return construct_condition_rhs(data_specification, rules, sort);
  }

  /*
   * For each matching rule LHS, check whether pattern matching needs to happen on that LHS.
   *
   * We prefer to match on an expression such that *all* rules perform matching on that expression.
   * For example, for a ruleset representing the following rewrite rules:
   *
   *   optionally_remove_first_element(false, l) = l
   *   optionally_remove_first_element(true, []) = []
   *   optionally_remove_first_element(true, x |> l) = l
   *
   * We prefer to pattern match on the first parameter, not the second. Pattern matching on the 
   * second parameter will (recursively) happen only on the true-branch, not the false-branch.
   * Pattern matching on the second parameter is still possible if there is no other option, but
   * it requires splitting the first rewrite rule into different cases for different 
   * constructors -- suboptimal.
   */
  data::data_expression matching_target;
  enum matching_type_t { MATCHING_NONE, MATCHING_INCOMPLETE, MATCHING_PARTIAL, MATCHING_FULL };
  matching_type_t matching_type = MATCHING_NONE;
  for (std::map<data::data_expression, data::data_expression>::const_iterator i = rules[0].match_criteria.begin(); i != rules[0].match_criteria.end(); i++)
  {
    bool variable_seen = false;
    std::set<data::function_symbol> constructors_seen;
    for (std::vector<rule>::const_iterator j = rules.begin(); j != rules.end(); j++)
    {
      data::data_expression pattern = j->match_criteria.at(i->first);
      if (data::is_variable(pattern))
      {
        variable_seen = true;
      }
      else if (data::is_function_symbol(pattern))
      {
        assert(data_specification->is_constructor(data::function_symbol(pattern)));
        constructors_seen.insert(data::function_symbol(pattern));
      }
      else if (data::is_application(pattern))
      {
        data::application application(pattern);
        assert(data::is_function_symbol(application.head()));
        assert(data_specification->is_constructor(data::function_symbol(application.head())));
        constructors_seen.insert(data::function_symbol(application.head()));
      }
      else
      {
        assert(false);
      }
    }

    if (constructors_seen.empty())
    {
      // No pattern matching is possible on this variable.
      continue;
    }
    else if (variable_seen)
    {
      // There are both rules that match on this variable and rules that do not.
      // That's better than an incomplete matching but worse than a full matching.
      if (matching_type == MATCHING_NONE || matching_type == MATCHING_INCOMPLETE)
      {
        matching_target = i->first;
        matching_type = MATCHING_PARTIAL;
      }
    }
    else if (constructors_seen.size() != data_specification->constructors(i->first.sort()).size())
    {
      // There are constructors for which there are no rules.
      // Thus, we have an incomplete function definition, that needs to be completed artificially.
      // A partial matching would be a better choice.
      if (matching_type == MATCHING_NONE)
      {
        matching_target = i->first;
        matching_type = MATCHING_INCOMPLETE;
      }
    }
    else
    {
      // There is a matching rule for each constructor, and no rule with a plain variable.
      // This variable is a perfect pattern matching candidate.
      matching_target = i->first;
      matching_type = MATCHING_FULL;
      break;
    }
  }

  if (matching_type == MATCHING_NONE)
  {
    // No constructor-based matching needs to happen. 
    // All that needs to happen is incorporating the rule conditions.
    return construct_condition_rhs(data_specification, rules, sort);
  }

  /*
   * For each constructor, find the set of rules that apply to it, rewritten to match the constructor.
   */
  std::map<data::function_symbol, std::vector<rule> > constructor_rules;
  for (std::vector<rule>::const_iterator i = rules.begin(); i != rules.end(); i++)
  {
    data::data_expression pattern = i->match_criteria.at(matching_target);
    if (data::is_function_symbol(pattern) || data::is_application(pattern))
    {
      /*
       * For a rule with a constructor pattern, strip the constructor and
       * introduce patterns for the constructor parameters.
       */
      data::function_symbol constructor;
      data::data_expression_vector parameters;

      if (data::is_function_symbol(pattern))
      {
        constructor = data::function_symbol(pattern);
      }
      else
      {
        data::application application(pattern);
        assert(data::is_function_symbol(application.head()));
        constructor = data::function_symbol(application.head());
        parameters.insert(parameters.end(), application.begin(), application.end());
      }

      assert(data_specification->constructors(matching_target.sort()).count(constructor) != 0);
      rule rule = *i;
      rule.match_criteria.erase(matching_target);
      for (size_t j = 0; j < parameters.size(); j++)
      {
        data::function_symbol projection_function = data_specification->projection_function(constructor, j);
        data::data_expression lhs_expression = data::application(projection_function, matching_target);
        rule.match_criteria[lhs_expression] = parameters[j];
      }
      constructor_rules[constructor].push_back(rule);
    }
    else
    {
      /*
       * For a rule with a variable pattern that nonetheless needs to pattern match
       * against the possible constructors for that variable, copy the rule for each
       * possible constructor. Substitute the original un-matched term for the pattern
       * variable that disappears.
       */
      assert(data::is_variable(pattern));
      data::data_expression condition = data::replace_free_variables(i->condition, data::variable_assignment(data::variable(pattern), matching_target));
      data::data_expression rhs = data::replace_free_variables(i->rhs, data::variable_assignment(data::variable(pattern), matching_target));

      const std::set<data::function_symbol>& constructors = data_specification->constructors(matching_target.sort());
      for (std::set<data::function_symbol>::const_iterator j = constructors.begin(); j != constructors.end(); j++)
      {
        rule rule;
        rule.condition = condition;
        rule.rhs = rhs;
        rule.variables = i->variables;
        rule.match_criteria = i->match_criteria;
        rule.match_criteria.erase(matching_target);

        data::set_identifier_generator generator;
        for (std::set<data::variable>::const_iterator k = i->variables.begin(); k != i->variables.end(); k++)
        {
          generator.add_identifier(k->name());
        }

        if (data::is_function_sort(j->sort()))
        {
          data::function_sort sort(j->sort());
          size_t index = 0;
          for (data::sort_expression_list::const_iterator k = sort.domain().begin(); k != sort.domain().end(); k++)
          {
            data::variable variable(generator("v"), *k);
            data::function_symbol projection_function = data_specification->projection_function(*j, index);
            data::data_expression lhs_expression = data::application(projection_function, matching_target);
            rule.match_criteria[lhs_expression] = variable;
            index++;
          }
        }

        constructor_rules[*j].push_back(rule);
      }
    }
  }

  /*
   * Construct an rhs of the form if(is_cons1, rhs_cons1, if(is_cons2, rhs_cons2, ...))
   */
  std::set<data::function_symbol>::const_iterator i = data_specification->constructors(matching_target.sort()).begin();
  data::data_expression result = construct_rhs(data_specification, constructor_rules[*i], sort);
  for (i++; i != data_specification->constructors(matching_target.sort()).end(); i++)
  {
    data::data_expression term = construct_rhs(data_specification, constructor_rules[*i], sort);
    data::function_symbol recogniser_function = data_specification->recogniser_function(*i);
    data::data_expression condition = data::application(recogniser_function, matching_target);
    result = data::if_(condition, term, result);
  }
  return result;
}

bool recursive_function_definition::is_pattern_matching_rule(data_specification *data_specification, const data::data_equation& rewrite_rule)
{
  /*
   * For this rewrite rule to be usable in pattern matching, its lhs must only contain
   * constructor functions and *fresh* variables.
   */
  std::set<data::data_expression> subexpressions = data::find_data_expressions(rewrite_rule.lhs());
  subexpressions.erase(rewrite_rule.lhs());
  if (data::is_application(rewrite_rule.lhs()))
  {
    subexpressions.erase(data::application(rewrite_rule.lhs()).head());
  }

  for (std::set<data::data_expression>::const_iterator i = subexpressions.begin(); i != subexpressions.end(); i++)
  {
    if (data::is_variable(*i))
    {
      // Fine.
    }
    else if (data::is_function_symbol(*i))
    {
      if (!data_specification->is_constructor(data::function_symbol(*i)))
      {
        return false;
      }
    }
    else if (data::is_application(*i))
    {
      data::application application(*i);
      if (!data::is_function_symbol(application.head()))
      {
        return false;
      }
      if (!data_specification->is_constructor(data::function_symbol(application.head())))
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  std::set<data::variable> variable_set;
  std::multiset<data::variable> variable_multiset;
  data::find_all_variables(rewrite_rule.lhs(), std::inserter(variable_set, variable_set.end()));
  data::find_all_variables(rewrite_rule.lhs(), std::inserter(variable_multiset, variable_multiset.end()));
  return variable_set.size() == variable_multiset.size();
}

recursive_function_definition::recursive_function_definition(data_specification *data_specification, data::function_symbol function, const data::data_equation_vector& rewrite_rules):
  named_function_definition(data_specification, function)
{
  /*
   * This converts a collection of rewrite rules for a give function symbol into a
   * one-rule specification of the function, using recogniser and projection functions
   * to implement pattern matching.
   *
   * For example, the collection of rewrite rules below:
   *
   *   sign_of_list_sum([]) = false;
   *   is_even(n) -> sign_of_list_sum(n |> l) = sign_of_list_sum(l);
   *   !is_even(n) -> sign_of_list_sum(n |> l) = !sign_of_list_sum(l);
   *
   * gets translated into the following function specification:
   *
   *   sign_of_list_sum(l) = if(is_emptylist(l), false,
   *                            if(is_even(head(l)), sign_of_list_sum(tail(l)),
   *                               !sign_of_list_sum(tail(l))))
   *
   * Two complications can arise. The rewrite rule set may contain rules that do not
   * pattern-match on the function parameters, such as 'not(not(b)) = b`; rules like
   * these are discarded.
   * More problematically, the set of rewrite rules may not be complete, or may not
   * easily be proven complete; in the example above, if the rewriter cannot rewrite
   * 'is_even(n) || !is_even(n)' to 'true', the translation cannot tell that the
   * rewrite rule set is complete.
   * In cases where a (non-constructor )function invocation can genuinely not be
   * rewritten any further, the MCRL2 semantics are unspecified (TODO check this);
   * the translation resolves this situation by returning an arbitrary value in this
   * case. Thus, in practice, the function definion above might well be translated
   * into the following:
   *
   *   sign_of_list_sum(l) = if(is_emptylist(l), false,
   *                            if(is_even(head(l)), sign_of_list_sum(tail(l)),
   *                               if(!is_even(head(l)), !sign_of_list_sum(tail(l)),
   *                                  false)))
   *
   * Where 'false' is a representative of sort_bool.
   */

  data::sort_expression codomain;
  if (data::is_function_sort(function.sort()))
  {
    data::function_sort sort(function.sort());
    codomain = sort.codomain();
    for (data::sort_expression_list::const_iterator i = sort.domain().begin(); i != sort.domain().end(); i++)
    {
      m_parameters.push_back(data::variable(data_specification->identifier_generator()("parameter"), *i));
    }
  }
  else
  {
    codomain = function.sort();
  }

  std::vector<rule> rules;
  for (data::data_equation_vector::const_iterator i = rewrite_rules.begin(); i != rewrite_rules.end(); i++)
  {
    assert(is_pattern_matching_rule(data_specification, *i));

    data::data_expression_vector arguments;
    if (data::is_application(i->lhs()))
    {
      data::application function_application(i->lhs());
      assert(function_application.head() == function);
      arguments.insert(arguments.end(), function_application.begin(), function_application.end());
    }
    assert(m_parameters.size() == arguments.size());

    rule rule;
    rule.rhs = i->rhs();
    rule.condition = i->condition();
    rule.variables.insert(i->variables().begin(), i->variables().end());
    for (size_t j = 0; j < arguments.size(); j++)
    {
      rule.match_criteria[m_parameters[j]] = arguments[j];
    }
    rules.push_back(rule);
  }

  assert(rules.size() != 0);
  m_rhs = construct_rhs(data_specification, rules, codomain);
  m_function_dependencies = data::find_function_symbols(m_rhs);
  m_function_dependencies.erase(function);
}

} // namespace smt
} // namespace mcrl2
