// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/data/rewriters/one_point_rule_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_DATA_REWRITERS_ONE_POINT_RULE_REWRITER_H
#define MCRL2_DATA_REWRITERS_ONE_POINT_RULE_REWRITER_H

#include "mcrl2/data/equality_one_point_substitution.h"
#include "mcrl2/data/expression_traits.h"
#include "mcrl2/data/find_equalities.h"
#include "mcrl2/data/optimized_boolean_operators.h"
#include "mcrl2/data/replace.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"

namespace mcrl2::data
{

namespace detail
{

template <typename Derived>
class one_point_rule_rewrite_builder: public data_expression_builder<Derived>
{
  public:
    using super = data_expression_builder<Derived>;

    using super::apply;

    Derived& derived()
    {
      return static_cast<Derived&>(*this);
    }

    template <class T>
    void apply(T& result, const forall& x)
    {
      data_expression body; 
      derived().apply(body, x.body());
      std::vector<variable> variables;

      std::map<variable, std::set<data_expression> > inequalities = find_inequalities(body);
      if (!inequalities.empty())
      {
        auto [sigma, remaining_variables] = make_one_point_rule_substitution(inequalities, x.variables());
        if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
        {
          mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
          body = data::replace_variables_capture_avoiding(body, sigma);
          mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
          if (remaining_variables.empty())
          {
            mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
            result = body;
            return;
          }
          data::variable_list v(remaining_variables.begin(), remaining_variables.end());
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << forall(v, body) << std::endl;
          make_forall(result, v, body);
          return;
        }
      }
      make_forall(result, x.variables(), body);
    }

    template <class T>
    void apply(T& result, const exists& x)
    {
      data_expression body;
      derived().apply(body, x.body());
      std::vector<variable> variables;

      std::map<variable, std::set<data_expression> > equalities = find_equalities(body);
      if (!equalities.empty())
      {
        auto [sigma, remaining_variables] = make_one_point_rule_substitution(equalities, x.variables());
        if (remaining_variables.size() != x.variables().size()) // one or more substitutions were found
        {
          mCRL2log(log::debug) << "Apply substitution sigma = " << sigma << " to x = " << body << std::endl;
          body = data::replace_variables_capture_avoiding(body, sigma);
          mCRL2log(log::debug) << "sigma(x) = " << body << std::endl;
          if (remaining_variables.empty())
          {
            mCRL2log(log::debug) << "Replaced " << x << "\nwith " << body << std::endl;
            result = body;
            return;
          }
          data::variable_list v(remaining_variables.begin(), remaining_variables.end());
          mCRL2log(log::debug) << "Replaced " << x << "\nwith " << exists(v, body) << std::endl;
          make_exists(result, v, body);
          return;
        }
      }
      make_exists(result, x.variables(), body);
    }
};

} // namespace detail

struct one_point_rule_rewriter
{
  using argument_type = data_expression;
  using result_type = data_expression;

  data_expression operator()(const data_expression& x) const
  {
    data_expression result;
    core::make_apply_builder<detail::one_point_rule_rewrite_builder>().apply(result, x);
    return result;
  }
};

template <typename T>
void one_point_rule_rewrite(T& x, std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = 0)
{
  core::make_update_apply_builder<data::data_expression_builder>(one_point_rule_rewriter()).update(x);
}

template <typename T>
T one_point_rule_rewrite(const T& x, std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  core::make_update_apply_builder<data::data_expression_builder>(one_point_rule_rewriter()).apply(result, x);
  return result;
}

} // namespace mcrl2::data

#endif // MCRL2_DATA_REWRITERS_ONE_POINT_RULE_REWRITER_H
