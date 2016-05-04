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
    if (core::term_traits<data::data_expression>::is_constant(x))
    {
      return x;
    }
  }
  return *V.begin();
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
      data_expression body = derived().apply(forall(x).body());
      std::vector<variable> variables;

      std::map<variable, std::set<data_expression> > inequalities = find_inequalities(body);
      if (!inequalities.empty())
      {
        mutable_map_substitution<> sigma;
        for (const variable& v: x.variables())
        {
          auto i = inequalities.find(v);
          if (i != inequalities.end())
          {
            sigma[v] = one_point_rule_select_element(i->second);
          }
          else
          {
            variables.push_back(v);
          }
        }
        if (variables.size() != x.variables().size()) // one or more substitutions were found
        {
          body = data::replace_variables_capture_avoiding(body, sigma, substitution_variables(sigma));
          if (variables.empty())
          {
            return body;
          }
          variable_list v(variables.begin(), variables.end());
          return forall(v, body);
        }
      }
      return forall(x.variables(), body);
    }

    data_expression apply(const exists& x)
    {
      data_expression body = derived().apply(exists(x).body());
      std::vector<variable> variables;

      std::map<variable, std::set<data_expression> > equalities = find_equalities(body);
      if (!equalities.empty())
      {
        mutable_map_substitution<> sigma;
        for (const variable& v: x.variables())
        {
          auto i = equalities.find(v);
          if (i != equalities.end())
          {
            sigma[v] = one_point_rule_select_element(i->second);
          }
          else
          {
            variables.push_back(v);
          }
        }
        if (variables.size() != x.variables().size()) // one or more substitutions were found
        {
          body = data::replace_variables_capture_avoiding(body, sigma, substitution_variables(sigma));
          if (variables.empty())
          {
            return body;
          }
          variable_list v(variables.begin(), variables.end());
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
