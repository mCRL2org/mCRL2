// Author(s): Ruud Koolen, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef MCRL2_SMT_UNFOLD_PATTERN_MATCHING_H
#define MCRL2_SMT_UNFOLD_PATTERN_MATCHING_H

#include "mcrl2/data/join.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/representative_generator.h"
#include "mcrl2/data/substitutions/map_substitution.h"
#include "mcrl2/data/substitutions/variable_substitution.h"
#include "mcrl2/smt/utilities.h"

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
  data::variable_list variables;

  rule(std::map<data::data_expression, data::data_expression> mc,
       data::data_expression r,
       data::data_expression c,
       data::variable_list v)
  : match_criteria(std::move(mc))
  , rhs(std::move(r))
  , condition(std::move(c))
  , variables(std::move(v))
  {
    assert(condition.sort() == data::sort_bool::bool_());
  }

};

inline
std::ostream& operator<<(std::ostream& out, const rule& r)
{
  return out << core::detail::print_list(r.variables) << ". " << r.condition << " -> " << core::detail::print_map(r.match_criteria) << " = " << r.rhs;
}

struct structured_sort_functions
{
  std::map< data::sort_expression, std::set<data::function_symbol> > constructors;
  std::map< data::function_symbol, data::function_symbol > recogniser_func;
  std::map< data::function_symbol, data::function_symbol_vector > projection_func;

  bool is_constructor(const data::function_symbol& f) const
  {
    const auto& cons_s = constructors.at(f.sort().target_sort());
    return cons_s.find(f) != cons_s.end();
  }
};

inline
data::data_expression lazyif(const data::data_expression& cond, const data::data_expression& then, const data::data_expression& else_)
{
  if (cond == data::sort_bool::true_() || then == else_)
  {
    return then;
  }
  else if (cond == data::sort_bool::false_())
  {
    return else_;
  }

  return data::if_(cond, then, else_);
}

/*
 * For a list of rules with equal left hand sides of match_criteria and only variables in
 * the right hand sides of match_criteria, construct a right hand side based on the
 * conditions and right hand sides of the rules.
 */
static data::data_expression construct_condition_rhs(const std::vector<rule>& rules, const data::data_expression& representative)
{
  data::data_expression_vector negated_conditions;
  for (const rule& r: rules)
  {
    negated_conditions.push_back(data::sort_bool::not_(r.condition));
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
    result = representative;
  }

  for (const rule& r: rules)
  {
    std::map<data::variable, data::data_expression> substitution_map;
    for (const auto& p: r.match_criteria)
    {
      assert(data::is_variable(p.second));
      substitution_map[data::variable(p.second)] = p.first;
    }
    data::map_substitution<std::map<data::variable, data::data_expression> > substitution(substitution_map);

    data::data_expression condition = data::replace_free_variables(r.condition, substitution);
    data::data_expression rhs = data::replace_free_variables(r.rhs, substitution);

    if (result == data::data_expression())
    {
      result = rhs;
    }
    else
    {
      result = lazyif(condition, rhs, result);
    }
  }

  return result;
}

/*
 * For a list of rules with equal left hand sides of match_criteria, construct a right hand side.
 */
static data::data_expression construct_rhs(const structured_sort_functions& ssf, data::representative_generator& gen, const std::vector<rule>& rules, const data::sort_expression& sort)
{
  if (rules.empty())
  {
    return construct_condition_rhs(rules, gen(sort));
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
  for (const auto& p: rules[0].match_criteria)
  {
    bool variable_seen = false;
    std::set<data::function_symbol> constructors_seen;
    for (const rule& r: rules)
    {
      data::data_expression pattern = r.match_criteria.at(p.first);
      if (data::is_variable(pattern))
      {
        variable_seen = true;
      }
      else if (data::is_function_symbol(pattern))
      {
        assert(ssf.is_constructor(data::function_symbol(pattern)));
        constructors_seen.insert(data::function_symbol(pattern));
      }
      else if (data::is_application(pattern))
      {
        data::application application(pattern);
        assert(data::is_function_symbol(application.head()));
        assert(ssf.is_constructor(data::function_symbol(application.head())));
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
        matching_target = p.first;
        matching_type = MATCHING_PARTIAL;
      }
    }
    else if (constructors_seen.size() != ssf.constructors.at(p.first.sort()).size())
    {
      // There are constructors for which there are no rules.
      // Thus, we have an incomplete function definition, that needs to be completed artificially.
      // A partial matching would be a better choice.
      if (matching_type == MATCHING_NONE)
      {
        matching_target = p.first;
        matching_type = MATCHING_INCOMPLETE;
      }
    }
    else
    {
      // There is a matching rule for each constructor, and no rule with a plain variable.
      // This variable is a perfect pattern matching candidate.
      matching_target = p.first;
      matching_type = MATCHING_FULL;
      break;
    }
  }

  if (matching_type == MATCHING_NONE)
  {
    // No constructor-based matching needs to happen.
    // All that needs to happen is incorporating the rule conditions.
    return construct_condition_rhs(rules, gen(sort));
  }

  /*
   * For each constructor, find the set of rules that apply to it, rewritten to match the constructor.
   */
  std::set<data::function_symbol> match_constructors(ssf.constructors.at(matching_target.sort()));
  std::map<data::function_symbol, std::vector<rule> > constructor_rules;
  for (const rule& r: rules)
  {
    data::data_expression pattern = r.match_criteria.at(matching_target);
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
        assert(data::is_application(pattern));
        data::application application(pattern);
        assert(data::is_function_symbol(application.head()));
        constructor = data::function_symbol(application.head());
        parameters.insert(parameters.end(), application.begin(), application.end());
      }

      assert(match_constructors.count(constructor) != 0);
      rule rule = r;
      rule.match_criteria.erase(matching_target);
      for (std::size_t j = 0; j < parameters.size(); j++)
      {
        data::function_symbol projection_function = ssf.projection_func.at(constructor)[j];
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
      data::variable_substitution sigma(atermpp::down_cast<data::variable>(pattern), matching_target);
      data::data_expression condition = data::replace_free_variables(r.condition, sigma);
      data::data_expression rhs = data::replace_free_variables(r.rhs, sigma);

      const std::set<data::function_symbol>& constructors = match_constructors;
      for (const data::function_symbol& f: constructors)
      {
        rule rule(r.match_criteria, rhs, condition, r.variables);
        rule.match_criteria.erase(matching_target);

        data::set_identifier_generator generator;
        for (const data::variable& v: r.variables)
        {
          generator.add_identifier(v.name());
        }

        if (data::is_function_sort(f.sort()))
        {
          data::function_sort sort(f.sort());
          std::size_t index = 0;
          for (const data::sort_expression& s: sort.domain())
          {
            data::variable variable(generator("v"), s);
            data::function_symbol projection_function = ssf.projection_func.at(f)[index];
            data::data_expression lhs_expression = data::application(projection_function, matching_target);
            rule.match_criteria[lhs_expression] = variable;
            index++;
          }
        }

        constructor_rules[f].push_back(rule);
      }
    }
  }

  /*
   * Construct an rhs of the form if(is_cons1, rhs_cons1, if(is_cons2, rhs_cons2, ...))
   */
  std::set<data::function_symbol>::const_iterator i = match_constructors.begin();
  data::data_expression result = construct_rhs(ssf, gen, constructor_rules[*i], sort);
  for (i++; i != match_constructors.end(); i++)
  {
    data::data_expression term = construct_rhs(ssf, gen, constructor_rules[*i], sort);
    data::function_symbol recogniser_function = ssf.recogniser_func.at(*i);
    data::data_expression condition = data::application(recogniser_function, matching_target);
    result = lazyif(condition, term, result);
  }
  return result;
}

bool is_pattern_matching_rule(const structured_sort_functions& ssf, const data::data_equation& rewrite_rule)
{
  // For this rewrite rule to be usable in pattern matching, its lhs must only contain
  // constructor functions and variables that occur at most once.

  std::set<data::data_expression> subexpressions = data::find_data_expressions(rewrite_rule.lhs());
  subexpressions.erase(rewrite_rule.lhs());
  if (data::is_application(rewrite_rule.lhs()))
  {
    subexpressions.erase(data::application(rewrite_rule.lhs()).head());
  }

  for (const data::data_expression& expr: subexpressions)
  {
    if (data::is_variable(expr))
    {
      // Fine.
    }
    else if (data::is_function_symbol(expr))
    {
      if (!ssf.is_constructor(data::function_symbol(expr)))
      {
        return false;
      }
    }
    else if (data::is_application(expr))
    {
      data::application application(expr);
      if (!data::is_function_symbol(application.head()))
      {
        return false;
      }
      if (!ssf.is_constructor(data::function_symbol(application.head())))
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }

  // Check whether each variable occurs at most once
  std::set<data::variable> variable_set;
  std::multiset<data::variable> variable_multiset;
  data::find_all_variables(rewrite_rule.lhs(), std::inserter(variable_set, variable_set.end()));
  data::find_all_variables(rewrite_rule.lhs(), std::inserter(variable_multiset, variable_multiset.end()));
  return variable_set.size() == variable_multiset.size();
}

data::data_equation unfold_pattern_matching(const data::function_symbol& mapping, const data::data_equation_vector& rewrite_rules, const structured_sort_functions& ssf, data::representative_generator& gen, data::set_identifier_generator& id_gen)
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

  data::sort_expression codomain = mapping.sort().target_sort();
  data::variable_vector temp_par;
  if (data::is_function_sort(mapping.sort()))
  {
    const data::function_sort& sort = atermpp::down_cast<data::function_sort>(mapping.sort());
    for (data::sort_expression s: sort.domain())
    {
      temp_par.push_back(data::variable(id_gen("@par"), s));
    }
  }
  data::variable_list new_parameters(temp_par.begin(), temp_par.end());

  // Create a rule for each data_equation
  std::vector<rule> rules;
  for (const data::data_equation& eq: rewrite_rules)
  {
    assert(is_pattern_matching_rule(ssf, eq));

    std::map<data::data_expression, data::data_expression> match_criteria;
    if (data::is_application(eq.lhs()))
    {
      const data::application& lhs_appl = atermpp::down_cast<data::application>(eq.lhs());

      assert(lhs_appl.head() == mapping);
      assert(new_parameters.size() == lhs_appl.size());

      // Simultaneously iterate the parameters defined by the mapping and this
      // left-hand side to determine how matching occurs
      auto mappar_it = new_parameters.begin();
      auto lhspar_it = lhs_appl.begin();
      while(mappar_it != new_parameters.end())
      {
        match_criteria[*mappar_it] = *lhspar_it;
        ++mappar_it, ++lhspar_it;
      }

      assert(lhspar_it == lhs_appl.end());
    }

    rule rule(match_criteria, eq.rhs(), eq.condition(), eq.variables());
    rules.push_back(rule);
  }
  assert(rules.size() != 0);

  data::data_expression new_lhs(data::application(mapping, new_parameters));
  data::data_expression new_rhs(construct_rhs(ssf, gen, rules, codomain));
  return data::data_equation(new_parameters, new_lhs, new_rhs);
}

std::pair<structured_sort_functions, std::map< data::function_symbol, data::data_equation_vector >> find_structured_sort_functions(const data::data_specification& dataspec, const native_translations& nt)
{
  structured_sort_functions ssf;
  for(const data::sort_expression& s: dataspec.sorts())
  {
    ssf.constructors[s] = std::set<data::function_symbol>(dataspec.constructors(s).begin(), dataspec.constructors(s).end());
  }

  std::map< data::function_symbol, data::data_equation_vector > rewrite_rules;
  for (const data::data_equation& eqn: dataspec.equations())
  {
    data::data_expression lhs = eqn.lhs();
    data::function_symbol function;
    if (data::is_application(lhs))
    {
      data::application application(lhs);
      if (data::is_function_symbol(application.head()))
      {
        function = data::function_symbol(application.head());
      }
      else
      {
        continue;
      }
    }
    else if (data::is_function_symbol(lhs))
    {
      function = data::function_symbol(lhs);
    }
    else
    {
      continue;
    }
    if(nt.has_native_definition(function))
    {
      continue;
    }
    //TODO equations of the shape x < x or x <= x are simply removed, so the remaining equations
    // for a valid pattern matching. How can this problem be adressed in a proper way?
    if(eqn.variables().size() == 1 && (core::pp(function.name()) == "<" || core::pp(function.name()) == "<="))
    {
      continue;
    }

    rewrite_rules[function].push_back(eqn);
  }

  // For each mapping, find out whether it is a recogniser or projection function.
  for (const auto& p: rewrite_rules)
  {
    const data::function_symbol& mapping = p.first;
    const data::data_equation_vector& equations = p.second;

    if (!data::is_function_sort(mapping.sort()))
    {
      continue;
    }
    data::function_sort sort(mapping.sort());
    if (sort.domain().size() != 1)
    {
      continue;
    }
    data::sort_expression domain = sort.domain().front();
    if (ssf.constructors[domain].empty())
    {
      continue;
    }

    // TODO implement this using a rewriter, which is a much easier way the find
    // the same patterns that are implemented manually below.
    // Check for recognisers.
    if (data::sort_bool::is_bool(sort.codomain()))
    {
      std::set<data::function_symbol> positive_recogniser_equation_seen;
      std::set<data::function_symbol> negative_recogniser_equation_seen;
      bool invalid_equations_seen = false;
      for (const data::data_equation& eqn: equations)
      {
        if (eqn.condition() != data::sort_bool::true_() ||
            !data::is_application(eqn.lhs()))
        {
          invalid_equations_seen = true;
          break;
        }

        data::application application(eqn.lhs());
        assert(application.head() == mapping);
        assert(application.size() == 1);
        data::data_expression argument(application[0]);
        data::function_symbol constructor;
        if (data::is_application(argument))
        {
          data::application constructor_application(argument);
          if (data::is_function_symbol(constructor_application.head()))
          {
            constructor = data::function_symbol(constructor_application.head());
          }
          else
          {
            invalid_equations_seen = true;
            break;
          }
          bool all_args_are_vars = std::all_of(constructor_application.begin(), constructor_application.end(), &data::is_variable);
          bool all_vars_are_unique = data::find_all_variables(constructor_application).size() == constructor_application.size();
          if(!all_args_are_vars || !all_vars_are_unique)
          {
            invalid_equations_seen = true;
            break;
          }
        }
        else if (data::is_function_symbol(argument))
        {
          constructor = data::function_symbol(argument);
        }
        else
        {
          invalid_equations_seen = true;
          break;
        }
        // Check if the function symbol we found is really a constructor
        if (ssf.constructors[domain].count(constructor) == 0)
        {
          invalid_equations_seen = true;
          break;
        }

        if (eqn.rhs() == data::sort_bool::true_())
        {
          positive_recogniser_equation_seen.insert(constructor);
          if (negative_recogniser_equation_seen.count(constructor) != 0)
          {
            invalid_equations_seen = true;
            break;
          }
        }
        else if (eqn.rhs() == data::sort_bool::false_())
        {
          negative_recogniser_equation_seen.insert(constructor);
          if (positive_recogniser_equation_seen.count(constructor) != 0)
          {
            invalid_equations_seen = true;
            break;
          }
        }
        else
        {
          invalid_equations_seen = true;
          break;
        }
      }
      if (!invalid_equations_seen &&
          positive_recogniser_equation_seen.size() == 1 &&
          positive_recogniser_equation_seen.size() + negative_recogniser_equation_seen.size() == ssf.constructors[domain].size())
      {
        data::function_symbol constructor = *positive_recogniser_equation_seen.begin();
        ssf.recogniser_func[constructor] = mapping;
      }
    }

    // Check for projections.
    if (equations.size() == 1)
    {
      data::data_equation equation = equations[0];
      if (equation.condition() == data::sort_bool::true_() &&
          data::is_variable(equation.rhs()) &&
          data::is_application(equation.lhs()))
      {
        data::application application(equation.lhs());
        assert(application.head() == mapping);
        assert(application.size() == 1);
        data::data_expression argument(application[0]);
        if (data::is_application(argument) &&
            data::is_function_symbol(data::application(argument).head()) &&
            ssf.constructors[domain].count(data::function_symbol(data::application(argument).head())) == 1)
        {
          data::application constructor_application(argument);
          data::function_symbol constructor(constructor_application.head());

          bool all_args_are_vars = std::all_of(constructor_application.begin(), constructor_application.end(), &data::is_variable);
          bool all_vars_are_unique = data::find_all_variables(constructor_application).size() == constructor_application.size();
          auto find_result = std::find(constructor_application.begin(), constructor_application.end(), equation.rhs());

          if (find_result != constructor_application.end() && all_args_are_vars && all_vars_are_unique)
          {
            data::application::const_iterator::difference_type index = find_result - constructor_application.begin();
            assert(index >= 0 && index < static_cast<data::application::const_iterator::difference_type>(constructor_application.size()));
            ssf.projection_func[constructor].resize(constructor_application.size());
            ssf.projection_func[constructor][index] = mapping;
          }
        }
      }
    }
  }

  return std::make_pair(ssf, rewrite_rules);
}

/**
 * \brief Complete the containers with recognisers and projections in ssf
 * \details Also sets native translations and build a set of all recognisers and
 * projections in dataspec.
 */
std::set<data::function_symbol> complete_recognisers_projections(const data::data_specification& dataspec, native_translations& nt, structured_sort_functions& ssf)
{
  std::set<data::function_symbol> recog_and_proj;

  for(const data::function_symbol& cons: dataspec.constructors())
  {
    auto find_result = ssf.recogniser_func.find(cons);
    if(find_result != ssf.recogniser_func.end())
    {
      nt.set_native_definition(find_result->second, make_recogniser_name(cons, nt));
      recog_and_proj.insert(find_result->second);
    }
    else
    {
      ssf.recogniser_func[cons] = make_recogniser_func(cons, nt);
    }

    if(data::is_function_sort(cons.sort()))
    {
      std::size_t index = 0;
      const data::sort_expression_list& arg_list = atermpp::down_cast<data::function_sort>(cons.sort()).domain();
      ssf.projection_func[cons].resize(arg_list.size());
      for(const data::sort_expression& arg: arg_list)
      {
        data::function_symbol& projection = ssf.projection_func[cons][index];
        if(projection != data::function_symbol())
        {
          nt.set_native_definition(projection, make_projection_name(cons, index, nt));
          recog_and_proj.insert(projection);
        }
        else
        {
          projection = make_projection_func(cons, arg, index, nt);
        }
        index++;
      }
    }
  }

  return recog_and_proj;
}

inline
void unfold_pattern_matching(const data::data_specification& dataspec, native_translations& nt)
{
  auto p = find_structured_sort_functions(dataspec, nt);
  structured_sort_functions& ssf = p.first;
  std::map<data::function_symbol, data::data_equation_vector>& rewrite_rules = p.second;

  std::set<data::function_symbol> recog_and_proj = complete_recognisers_projections(dataspec, nt, ssf);

  data::representative_generator rep_gen(dataspec);
  for(const auto& map_eqn: rewrite_rules)
  {
    // Only unfold equations with parameters
    // Do not unfold recognisers and projection functions
    // Only unfold equations that satisfy the function 'is_pattern_matching_rule'
    if (data::is_function_sort(map_eqn.first.sort()) &&
        recog_and_proj.find(map_eqn.first) == recog_and_proj.end() &&
        std::all_of(map_eqn.second.begin(), 
                    map_eqn.second.end(), 
                    [&ssf](const data::data_equation& eqn){ return is_pattern_matching_rule(ssf, eqn); }))
    {
      data::set_identifier_generator id_gen;
      data::data_equation unfolded_eqn = unfold_pattern_matching(map_eqn.first, map_eqn.second, ssf, rep_gen, id_gen);
      nt.set_native_definition(map_eqn.first, unfolded_eqn);
    }
  }
}

} // namespace smt
} // namespace mcrl2

#endif
