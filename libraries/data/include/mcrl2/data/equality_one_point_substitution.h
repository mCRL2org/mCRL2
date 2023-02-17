// Author(s): Wieger Wesselink, Thomas Neele
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/equality_one_point_substitution.h


#ifndef MCRL2_DATA_EQUALITY_ONE_POINT_SUBSTITUTION_H
#define MCRL2_DATA_EQUALITY_ONE_POINT_SUBSTITUTION_H


#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

namespace mcrl2::data
{
namespace detail
{

struct one_point_rule_substitution_algorithm
{
  std::map<data::variable, std::vector<data::data_expression> > equalities;
  data::mutable_map_substitution<> sigma;
  std::set<data::variable> sigma_lhs_variables;
  data::set_identifier_generator id_generator;

  // applies the substitution sigma to all right hand sides of equalities
  void apply_sigma()
  {
    for (auto& [_, exprs]: equalities)
    {
      for (data::data_expression& e: exprs)
      {
        e = data::replace_variables_capture_avoiding(e, sigma, id_generator);
      }
    }
  }

  // finds all assignments to a constant, and adds them to sigma
  // returns true if any assignment was found
  bool find_constant_assignments()
  {
    std::vector<data::variable> to_be_removed;
    for (const auto& [lhs, exprs]: equalities)
    {
      for (const data::data_expression& e: exprs)
      {
        if (data::is_constant(e))
        {
          sigma[lhs] = e;
          sigma_lhs_variables.insert(lhs);
          to_be_removed.push_back(lhs);
        }
      }
    }

    // remove entries for the assignments
    for (const data::variable& v: to_be_removed)
    {
      equalities.erase(v);
    }

    // apply sigma to the right hand sides
    apply_sigma();

    return !to_be_removed.empty();
  }

  // finds an arbitrary assignment and adds it to sigma
  // returns true if any assignment was found
  bool find_assignment()
  {
    std::set<data::variable> to_be_removed;
    for (const auto& [lhs,exprs]: equalities)
    {
      for (const data::data_expression& e: exprs)
      {
        if (e != lhs)
        {
          sigma[lhs] = e;
          sigma_lhs_variables.insert(lhs);
          std::set<data::variable> FV = data::find_free_variables(e);
          for (const data::variable& v: FV)
          {
            id_generator.add_identifier(v.name());
          }
          to_be_removed.insert(lhs);
          to_be_removed.insert(FV.begin(), FV.end());
          break;
        }
      }
      if (!to_be_removed.empty())
      {
        break;
      }
    }

    // remove entries for the assignments
    for (const data::variable& v: to_be_removed)
    {
      equalities.erase(v);
    }

    // apply sigma to the right hand sides
    apply_sigma();

    return !to_be_removed.empty();
  }

  void build_equality_map(const std::map<data::variable, std::set<data::data_expression> >& equalities_, bool check_vars, const data::variable_list& vars = data::variable_list())
  {
    using utilities::detail::contains;
    for (const auto& [lhs,exprs]: equalities_)
    {
      if (check_vars && !contains(vars, lhs))
      {
        continue;
      }
      std::vector<data::data_expression> E;
      for (const data::data_expression& e: exprs)
      {
        if (!contains(data::find_free_variables(e), lhs))
        {
          E.push_back(e);
        }
      }
      if (!E.empty())
      {
        equalities[lhs] = E;
      }
    }
  }

  one_point_rule_substitution_algorithm(const std::map<data::variable, std::set<data::data_expression> >& equalities_)
  {
    build_equality_map(equalities_, false);
  }

  one_point_rule_substitution_algorithm(const std::map<data::variable, std::set<data::data_expression> >& equalities_, const data::variable_list& quantifier_variables)
  {
    build_equality_map(equalities_, true, quantifier_variables);
  }

  data::mutable_map_substitution<> run(bool find_all_assignments = true)
  {
    find_constant_assignments();
    while (find_all_assignments)
    {
      if (!find_assignment())
      {
        break;
      }
    }
    return sigma;
  }

  // creates a substitution from a set of (in-)equalities for a given list of quantifier variables
  // returns the substitution, and the subset of quantifier variables that are not used in the substitution
  std::pair<data::mutable_map_substitution<>, std::vector<data::variable> > run(const data::variable_list& quantifier_variables, bool find_all_assignments = true)
  {
    using utilities::detail::contains;

    run(find_all_assignments);

    std::vector<data::variable> remaining_variables;
    for (const data::variable& v: quantifier_variables)
    {
      if (!contains(sigma_lhs_variables, v))
      {
        remaining_variables.push_back(v);
      }
    }

    return std::make_pair(sigma, remaining_variables);
  }
};

} // namespace detail


/// @brief creates a substitution from a set of (in-)equalities for a given list of quantifier variables
/// @param quantifier_variables Consider only these variables
/// @param find_all_assignments True to find all assignments, false to find only constant assignments
/// @return the substitution, and the subset of quantifier variables that are not used in the substitution
inline
std::pair<data::mutable_map_substitution<>, std::vector<data::variable> > make_one_point_rule_substitution(
  const std::map<data::variable, std::set<data::data_expression> >& equalities,
  const data::variable_list& quantifier_variables,
  bool find_all_assignments = true)
{
  detail::one_point_rule_substitution_algorithm algorithm(equalities, quantifier_variables);
  return algorithm.run(quantifier_variables, find_all_assignments);
}

/// @brief creates a substitution from a set of (in-)equalities
/// @param find_all_assignments True to find all assignments, false to find only constant assignments
/// @return the substitution
inline
data::mutable_map_substitution<> make_one_point_rule_substitution(
  const std::map<data::variable, std::set<data::data_expression> >& equalities,
  bool find_all_assignments = true)
{
  detail::one_point_rule_substitution_algorithm algorithm(equalities);
  return algorithm.run(find_all_assignments);
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_EQUALITY_ONE_POINT_SUBSTITUTION_H
