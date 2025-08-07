// Author(s): Jeroen Keiren
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/lps/rewriters/one_point_condition_rewrite.h
/// \brief Rewriter for LPSs that takes equalities of the form p_i == c, where
///        p_i is a process parameter, and c is a constant, into account when
///        simplifying the remainder of a summand.

#ifndef MCRL2_LPS_REWRITERS_ONE_POINT_CONDITION_REWRITE_H
#define MCRL2_LPS_REWRITERS_ONE_POINT_CONDITION_REWRITE_H

#include "mcrl2/data/join.h"
#include "mcrl2/data/substitutions/mutable_map_substitution.h"
#include "mcrl2/data/rewriter.h"
#include "mcrl2/lps/rewriters/data_rewriter.h"
#include "mcrl2/lps/builder.h"

namespace mcrl2::lps
{

namespace detail {

// Extracts all conjuncts d == e from the data expression x, for variables d, and with e a constant.
inline void find_equality_conjuncts(const data::data_expression& x,
    std::map<data::variable, data::data_expression>& result)
{
  std::set<data::data_expression> conjuncts = data::split_and(x);
  for (const data::data_expression& expr: conjuncts)
  {
    const auto& v_i = expr;
    if (data::is_equal_to_application(v_i))
    {
      const data::data_expression& left = data::binary_left1(v_i);
      const data::data_expression& right = data::binary_right1(v_i);
      if (data::is_variable(left) && data::is_constant(right))
      {
        const auto& vleft = atermpp::down_cast<data::variable>(left);
        result[vleft] = right;
      }
      else if (data::is_variable(right) && data::is_constant(left))
      {
        const auto& vright = atermpp::down_cast<data::variable>(right);
        result[vright] = left;
      }
    }
    // handle conjuncts b and !b, with b a variable with sort Bool
    else if (data::is_variable(v_i) && data::sort_bool::is_bool(v_i.sort()))
    {
      const auto& v = atermpp::down_cast<data::variable>(v_i);
      result[v] = data::sort_bool::true_();
    }
    else if (data::sort_bool::is_not_application(v_i))
    {
      const data::data_expression& narg = data::sort_bool::arg(v_i);
      if (data::is_variable(narg) && data::sort_bool::is_bool(v_i.sort()))
      {
        const auto& v = atermpp::down_cast<data::variable>(narg);
        result[v] = data::sort_bool::false_();
      }
    }
  }
  mCRL2log(log::trace) << "  computing one point variables: expression = " << x << ", result = " << core::detail::print_map(result) << std::endl;
}

template <typename DataRewriter>
struct one_point_condition_rewrite_builder: public lps::data_expression_builder<one_point_condition_rewrite_builder<DataRewriter> >
{
  using super = lps::data_expression_builder<one_point_condition_rewrite_builder<DataRewriter>>;
  using super::enter;
  using super::leave;
  using super::apply;
  using super::update;

  const DataRewriter& R;
  data::mutable_map_substitution<> sigma; // substitution is only used for variables that appear condition of summand, and is cleared after.

  one_point_condition_rewrite_builder(const DataRewriter& R_)
      : R(R_)
  {}

  void calculate_substitutions(const data::data_expression& x)
  {
    // calculate substitutions
    std::map<data::variable, data::data_expression> assignments; // TODO: adapt to substitution
    detail::find_equality_conjuncts(x, assignments);
    for (const auto& [k,v]: assignments)
    {
      sigma[k] = v;
    }
  }

  void reset_substitutions()
  {
    sigma.clear();
  }

  void update(lps::action_summand& x)
  {
    enter(x);
    calculate_substitutions(x.condition());

    // rewrite summand
    lps::multi_action result_multi_action;
    apply(result_multi_action, x.multi_action());
    x.multi_action() = result_multi_action;
    data::assignment_list result_assignments;
    apply(result_assignments, x.assignments());
    x.assignments() = result_assignments;

    reset_substitutions();
    leave(x);
  }

  void update(lps::deadlock_summand& x)
  {
    enter(x);
    calculate_substitutions(x.condition());

    // rewrite summand
    update(x.deadlock());

    // reset substitution
    reset_substitutions();
    leave(x);
  }

  // TODO: should the stochastic distribution be rewritten or not?
  void update(lps::stochastic_action_summand& x)
  {
    enter(x);
    calculate_substitutions(x.condition());

    // rewrite summand
    lps::multi_action result_multi_action;
    apply(result_multi_action, x.multi_action());
    x.multi_action() = result_multi_action;
    data::assignment_list result_assignments;
    apply(result_assignments, x.assignments());
    x.assignments() = result_assignments;
    stochastic_distribution result_distribution;
    apply(result_distribution, x.distribution());
    x.distribution() = result_distribution;

    reset_substitutions();
    leave(x);
  }

  template <class T>
  void apply(T& result, const data::data_expression& x)
  {
    result = R(x, sigma);
  }
};

} // namespace detail

/// \brief Applies the one point condition rewriter to all embedded data expressions in an object x
/// \param x an object containing data expressions
template <typename T, typename DataRewriter>
void one_point_condition_rewrite(T& x,
    const DataRewriter& R,
    std::enable_if_t<!std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  detail::one_point_condition_rewrite_builder<DataRewriter>  f(R);
  f.update(x);
}

/// \brief Applies the one point condition rewriter to all embedded data expressions in an object x
/// \param x an object containing data expressions
/// \return the rewrite result
template <typename T, typename DataRewriter>
T one_point_condition_rewrite(const T& x,
    const DataRewriter& R,
    std::enable_if_t<std::is_base_of_v<atermpp::aterm, T>>* = nullptr)
{
  T result;
  detail::one_point_condition_rewrite_builder<DataRewriter> f(R);
  f.apply(result, x);
  return result;
}

} // namespace mcrl2::lps

#endif // MCRL2_LPS_DETAIL_ONE_POINT_CONDITION_REWRITE_H


