// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriters/one_point_rule_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REWRITERS_ONE_POINT_RULE_REWRITER_H
#define MCRL2_DATA_REWRITERS_ONE_POINT_RULE_REWRITER_H

#include "mcrl2/data/builder.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/find_equalities.h"
#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

namespace mcrl2 {

namespace data {

namespace detail {

struct one_point_rule_subtitution_algorithm
{
  const data::variable_list& quantifier_variables;
  std::map<data::variable, std::vector<data::data_expression> > equalities;
  data::mutable_map_substitution<> sigma;
  std::set<data::variable> sigma_lhs_variables;
  std::set<data::variable> sigma_rhs_variables; // variables appearing in the right hand side of the substitution

  // applies the substitution sigma to all right hand sides of equalities
  void apply_sigma()
  {
    for (auto& p: equalities)
    {
      for (data::data_expression& e: p.second)
      {
        e = data::replace_variables_capture_avoiding(e, sigma, sigma_rhs_variables);
      }
    }
  }

  // finds all assignments to a constant, and adds them to sigma
  // returns true if any assignment was found
  bool find_constant_assignments()
  {
    std::vector<data::variable> to_be_removed;
    for (const auto& p: equalities)
    {
      const data::variable& v = p.first;
      for (const data::data_expression& e: p.second)
      {
        if (data::is_constant(e))
        {
          sigma[v] = e;
          sigma_lhs_variables.insert(v);
          to_be_removed.push_back(v);
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
    for (const auto& p: equalities)
    {
      const data::variable& v = p.first;
      for (const data::data_expression& e: p.second)
      {
        if (e != v)
        {
          sigma[v] = e;
          sigma_lhs_variables.insert(v);
          std::set<data::variable> FV = data::find_free_variables(e);
          sigma_rhs_variables.insert(FV.begin(), FV.end());
          to_be_removed.insert(v);
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

  one_point_rule_subtitution_algorithm(const std::map<data::variable, std::set<data::data_expression> >& equalities_, const data::variable_list& quantifier_variables_)
    : quantifier_variables(quantifier_variables_)
  {
    using utilities::detail::contains;
    for (const auto& p: equalities_)
    {
      const data::variable& v = p.first;
      if (!contains(quantifier_variables, v))
      {
        continue;
      }
      std::vector<data::data_expression> E;
      for (const data::data_expression& e: p.second)
      {
        if (!contains(data::find_free_variables(e), v))
        {
          E.push_back(e);
        }
      }
      if (!E.empty())
      {
        equalities[v] = E;
      }
    }
  }

  // creates a substitution from a set of (in-)equalities for a given list of quantifier variables
  // returns the substitution, and the subset of quantifier variables that are not used in the substitution
  std::pair<data::mutable_map_substitution<>, std::vector<data::variable> > run()
  {
    using utilities::detail::contains;
    find_constant_assignments();
    for (;;)
    {
      if (!find_assignment())
      {
        break;
      }
    }

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

// creates a substitution from a set of (in-)equalities for a given list of quantifier variables
// returns the substitution, and the subset of quantifier variables that are not used in the substitution
std::pair<data::mutable_map_substitution<>, std::vector<data::variable> > make_one_point_rule_substitution(const std::map<data::variable, std::set<data::data_expression> >& equalities, const data::variable_list& quantifier_variables)
{
  one_point_rule_subtitution_algorithm algorithm(equalities, quantifier_variables);
  return algorithm.run();
}

template <typename Derived>
class one_point_rule_rewrite_builder: public data_expression_builder<Derived>
{
  public:
    typedef data_expression_builder<Derived> super;

    using super::apply;

    Derived& derived()
    {
      return static_cast<Derived&>(*this);
    }

    data_expression apply(const forall& x)
    {
      data_expression body = derived().apply(x.body());
      std::vector<variable> variables;

      std::map<variable, std::set<data_expression> > inequalities = find_inequalities(body);
      if (!inequalities.empty())
      {
        auto p = make_one_point_rule_substitution(inequalities, x.variables());
        data::mutable_map_substitution<>& sigma = p.first;
        const std::vector<data::variable>& remaining_variables = p.second;
        if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
        {
          mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
          body = data::replace_variables_capture_avoiding(body, sigma, substitution_variables(sigma));
          mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
          if (remaining_variables.empty())
          {
            mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
            return body;
          }
          data::variable_list v(remaining_variables.begin(), remaining_variables.end());
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << forall(v, body) << std::endl;
          return forall(v, body);
        }
      }
      return forall(x.variables(), body);
    }

    data_expression apply(const exists& x)
    {
      data_expression body = derived().apply(x.body());
      std::vector<variable> variables;

      std::map<variable, std::set<data_expression> > equalities = find_equalities(body);
      if (!equalities.empty())
      {
        auto p = make_one_point_rule_substitution(equalities, x.variables());
        data::mutable_map_substitution<>& sigma = p.first;
        const std::vector<data::variable>& remaining_variables = p.second;
        if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
        {
          mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
          body = data::replace_variables_capture_avoiding(body, sigma, substitution_variables(sigma));
          mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
          if (remaining_variables.empty())
          {
            mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
            return body;
          }
          data::variable_list v(remaining_variables.begin(), remaining_variables.end());
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << exists(v, body) << std::endl;
          return exists(v, body);
        }
      }
      return exists(x.variables(), body);
    }
};

} // namespace detail

struct one_point_rule_rewriter: public std::unary_function<data_expression, data_expression>
{
  data_expression operator()(const data_expression& x) const
  {
    return core::make_apply_builder<detail::one_point_rule_rewrite_builder>().apply(x);
  }
};

template <typename T>
void one_point_rule_rewrite(T& x, typename std::enable_if< !std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  core::make_update_apply_builder<data::data_expression_builder>(one_point_rule_rewriter()).update(x);
}

template <typename T>
T one_point_rule_rewrite(const T& x, typename std::enable_if< std::is_base_of< atermpp::aterm, T >::value>::type* = 0)
{
  T result = core::make_update_apply_builder<data::data_expression_builder>(one_point_rule_rewriter()).apply(x);
  return result;
}

} // namespace data

} // namespace mcrl2

#endif // MCRL2_DATA_REWRITERS_ONE_POINT_RULE_REWRITER_H
