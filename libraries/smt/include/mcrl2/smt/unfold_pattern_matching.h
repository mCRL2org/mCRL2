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
#include "mcrl2/data/unfold_pattern_matching.h"
#include "mcrl2/data/substitutions/map_substitution.h"
#include "mcrl2/data/substitutions/variable_substitution.h"
#include "mcrl2/smt/utilities.h"

namespace mcrl2::smt
{

/**
 * \brief Contains information on sorts that behave similar to a structured sort in a data specification.
 * \details That is, there is a number of constructors, and for some constructors we have a recogniser function
 * and several projection functions.
 */
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

  const std::set<data::function_symbol>& get_constructors(const data::sort_expression& sort) const
  {
    return constructors.at(sort);
  }

  data::data_expression create_recogniser_expr(const data::function_symbol& f, const data::data_expression& expr) const
  {
    return data::application(recogniser_func.at(f), expr);
  }

  data::data_expression create_cases(const data::data_expression& target, const data::data_expression_vector& rhss)
  {
    std::set<data::function_symbol> constr = get_constructors(target.sort());
    auto const_it = constr.begin();
    auto rhs_it = rhss.begin();
    data::data_expression result = *rhs_it++;
    for (const_it++; const_it != constr.end(); ++const_it, ++rhs_it)
    {
      data::data_expression term = *rhs_it;
      data::data_expression condition = create_recogniser_expr(*const_it, target);
      result = data::lazy::if_(condition, term, result);
    }
    return result;
  }

  const data::function_symbol_vector& get_projection_funcs(const data::function_symbol& f) const
  {
    return projection_func.at(f);
  }
};

template <typename T>
struct always_false
{
  bool operator()(const T&)
  {
    return false;
  }
};

/// @brief Find sorts that behave like a structured sort and the associated rewrite rules
/// @tparam Skip Unary Boolean function type.
/// @param dataspec The data specification to consider
/// @param skip If skip(f) is true then function symbol f will not be considered
/// @return A pair containing: (1) recogniser and projection function symbols for each structured sort and
///         (2) a map that gives a list of equations for each function symbol.
template <typename Skip = always_false<data::function_symbol>>
std::pair<structured_sort_functions, std::map< data::function_symbol, data::data_equation_vector >>
find_structured_sort_functions(const data::data_specification& dataspec, Skip skip = Skip())
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
    data::function_symbol function = data::detail::get_top_fs(lhs);
    if (function == data::function_symbol())
    {
      continue;
    }
    if (skip(function))
    {
      continue;
    }
    //TODO equations of the shape x < x or x <= x are simply removed, so the remaining equations
    // form a valid pattern matching. How can this problem be adressed in a proper way?
    if(eqn.variables().size() == 1 && (core::pp(function.name()) == "<" || core::pp(function.name()) == "<="))
    {
      continue;
    }

    rewrite_rules[function].push_back(eqn);
  }

  // For each mapping, find out whether it is a recogniser or projection function.
  for (const auto& [mapping, equations]: rewrite_rules)
  {
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
        data::function_symbol constructor = data::detail::get_top_fs(argument);
        if (constructor == data::function_symbol())
        {
          invalid_equations_seen = true;
          break;
        }
        if (data::is_application(argument))
        {
          const data::application& constructor_application = atermpp::down_cast<data::application>(argument);
          bool all_args_are_vars = std::all_of(constructor_application.begin(), constructor_application.end(), &data::is_variable);
          bool all_vars_are_unique = data::find_all_variables(constructor_application).size() == constructor_application.size();
          if(!all_args_are_vars || !all_vars_are_unique)
          {
            invalid_equations_seen = true;
            break;
          }
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
inline std::set<data::function_symbol> complete_recognisers_projections(const data::data_specification& dataspec,
    native_translations& nt,
    structured_sort_functions& ssf)
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
  std::set<core::identifier_string> used_ids = data::find_identifiers(dataspec);
  auto p = find_structured_sort_functions(dataspec, [&nt](const data::function_symbol& f){ return nt.has_native_definition(f); });
  structured_sort_functions& ssf = p.first;
  std::map<data::function_symbol, data::data_equation_vector>& rewrite_rules = p.second;

  std::set<data::function_symbol> recog_and_proj = complete_recognisers_projections(dataspec, nt, ssf);

  data::representative_generator rep_gen(dataspec);
  for(const auto& [function, rewr_equations]: rewrite_rules)
  {
    // Only unfold equations with parameters
    // Do not unfold recognisers and projection functions
    // Only unfold equations that satisfy the function 'is_pattern_matching_rule'
    if (data::is_function_sort(function.sort()) &&
        recog_and_proj.find(function) == recog_and_proj.end() &&
        std::all_of(rewr_equations.begin(),
                    rewr_equations.end(),
                    [&ssf](const data::data_equation& eqn){ return data::is_pattern_matching_rule(ssf, eqn); }))
    {
      data::set_identifier_generator id_gen;
      id_gen.add_identifiers(used_ids);
      data::data_equation unfolded_eqn = data::unfold_pattern_matching(function, rewr_equations, ssf, rep_gen, id_gen);
      nt.set_native_definition(function, unfolded_eqn);
    }
  }
}

} // namespace mcrl2::smt

#endif
