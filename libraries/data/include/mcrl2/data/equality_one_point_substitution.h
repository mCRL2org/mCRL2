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


#include "mcrl2/atermpp/aterm_structural_order.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

namespace mcrl2::data
{
namespace detail
{

/// \brief Computes a substitution from (in)equalities.
/// \details All choices are deterministic: variables are considered in the order of the quantifier variables (or in
///          structural order if no quantifier variables are given), and right hand sides in structural order. In
///          particular for an equality x == y between two quantifier variables, the first of them is eliminated.
struct one_point_rule_substitution_algorithm
{
  std::vector<std::pair<data::variable, std::vector<data::data_expression> > > equalities;
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

  // removes the entries for the variables in V
  void remove_equalities(const std::set<data::variable>& V)
  {
    using utilities::detail::contains;
    std::erase_if(equalities, [&](const auto& p) { return contains(V, p.first); });
  }

  // finds all assignments to a constant, and adds them to sigma; if a variable is equal to several constants,
  // the first one in structural order is chosen
  // returns true if any assignment was found
  bool find_constant_assignments()
  {
    std::set<data::variable> to_be_removed;
    for (const auto& [lhs, exprs]: equalities)
    {
      auto i = std::find_if(exprs.begin(), exprs.end(), [](const data::data_expression& e) { return data::is_constant(e); });
      if (i != exprs.end())
      {
        sigma[lhs] = *i;
        sigma_lhs_variables.insert(lhs);
        to_be_removed.insert(lhs);
      }
    }

    // remove entries for the assignments
    remove_equalities(to_be_removed);

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
    remove_equalities(to_be_removed);

    // apply sigma to the right hand sides
    apply_sigma();

    return !to_be_removed.empty();
  }

  // adds the equalities lhs == e for the e in exprs in which lhs does not occur, in structural order
  void add_equalities(const data::variable& lhs, const std::set<data::data_expression>& exprs)
  {
    using utilities::detail::contains;
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
      std::sort(E.begin(), E.end(), atermpp::structural_less());
      equalities.emplace_back(lhs, E);
    }
  }

  one_point_rule_substitution_algorithm(const std::map<data::variable, std::set<data::data_expression> >& equalities_)
  {
    for (const auto& [lhs, exprs]: equalities_)
    {
      add_equalities(lhs, exprs);
    }
    std::sort(equalities.begin(), equalities.end(), [](const auto& p, const auto& q) { return atermpp::structural_less()(p.first, q.first); });
  }

  // only equalities for the quantifier variables are considered, in the order of the quantifier variables
  one_point_rule_substitution_algorithm(const std::map<data::variable, std::set<data::data_expression> >& equalities_, const data::variable_list& quantifier_variables)
  {
    for (const data::variable& v: quantifier_variables)
    {
      auto i = equalities_.find(v);
      if (i != equalities_.end())
      {
        add_equalities(v, i->second);
      }
    }
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
