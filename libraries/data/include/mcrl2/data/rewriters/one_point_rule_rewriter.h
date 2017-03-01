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

inline
data::data_expression one_point_rule_select_element(const std::set<data::data_expression>& V)
{
  if (V.size() == 1)
  {
    return *V.begin();
  }
  for (const data::data_expression& x: V)
  {
    if (data::is_constant(x))
    {
      return x;
    }
  }
  return *V.begin();
}

// creates a substitution from a set of (in-)equalities for a given list of quantifier variables
// returns the substitution, and the subset of quantifier variables that are not used in the substitution
std::pair<data::mutable_map_substitution<>, std::vector<data::variable> > make_one_point_rule_substitution(const std::map<data::variable, std::set<data::data_expression> >& equalities, const data::variable_list& quantifier_variables)
{
  using utilities::detail::contains;

  data::mutable_map_substitution<> sigma;
  std::vector<data::variable> remaining_variables;    // the quantifier variables that are not
  std::set<data::variable> forbidden_variables;       // variables that may not be used in the substitution
  for (const data::variable& v: quantifier_variables)
  {
    auto i = equalities.find(v);
    if (i != equalities.end() && !contains(forbidden_variables, v))
    {
      data::data_expression rhs = data::detail::one_point_rule_select_element(i->second);
      data::find_free_variables(rhs, std::inserter(forbidden_variables, forbidden_variables.end())); // N.B. free variables in rhs can no longer be used
      sigma[v] = rhs;
    }
    else
    {
      remaining_variables.push_back(v);
    }
  }
  return std::make_pair(sigma, remaining_variables);
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
