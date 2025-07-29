// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/simplify_quantifiers_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_SIMPLIFY_QUANTIFIERS_REWRITER_H
#define MCRL2_PBES_REWRITERS_SIMPLIFY_QUANTIFIERS_REWRITER_H

#include "mcrl2/pbes/rewriters/simplify_rewriter.h"

namespace mcrl2::pbes_system {

namespace detail {

template <template <class> class Builder, class Derived>
struct add_simplify_quantifiers: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  template <class T>
  void apply(T& result, const forall& x)
  {
    pbes_expression body;
    super::apply(body, x.body());
    const data::variable_list& variables = x.variables();

    if (variables.empty())
    {
      result = true_();
    }
    else if (is_not(body))
    {
      data::optimized_exists(result, variables, atermpp::down_cast<not_>(body).operand(), true);
      data::optimized_not(result, result);
    }
    if (is_and(body))
    {
      auto const& left = atermpp::down_cast<and_>(body).left();
      auto const& right = atermpp::down_cast<and_>(body).right();
      data::optimized_forall(result, variables, left, true); 
      pbes_expression result_right;
      data::optimized_forall(result_right, variables, right, true);
      data::optimized_and(result, result, result_right);
    }
    else if (is_or(body))
    {
      auto const& left = atermpp::down_cast<or_>(body).left();
      auto const& right = atermpp::down_cast<or_>(body).right();
      data::variable_list lv = data::detail::set_intersection(variables, free_variables(left));
      data::variable_list rv = data::detail::set_intersection(variables, free_variables(right));
      if (lv.empty())
      {
        data::optimized_forall_no_empty_domain(result, rv, right, true);
        data::optimized_or(result, left, result);
      }
      else if (rv.empty())
      {
        data::optimized_forall_no_empty_domain(result, lv, left, true);
        data::optimized_or(result, result, right);
      }
      else
      {
        data::optimized_forall(result, variables, body, true);
      }
    }
    else
    {
      data::optimized_forall(result, variables, body, true);
    }
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    pbes_expression body;
    super::apply(body, x.body());
    const data::variable_list& variables = x.variables();

    if (variables.empty())
    {
      result = false_();
    }
    else if (is_not(body))
    {
      data::optimized_forall(result, variables, atermpp::down_cast<not_>(body).operand(), true);
      data::optimized_not(result, result);
    }
    if (is_or(body))
    {
      auto const& left = atermpp::down_cast<or_>(body).left();
      auto const& right = atermpp::down_cast<or_>(body).right();
      data::optimized_exists(result, variables, left, true);
      pbes_expression result_right;
      data::optimized_exists(result_right, variables, right, true);
      data::optimized_or(result, result, result_right);
    }
    else if (is_and(body))
    {
      auto const& left = atermpp::down_cast<and_>(body).left();
      auto const& right = atermpp::down_cast<and_>(body).right();
      data::variable_list lv = data::detail::set_intersection(variables, free_variables(left));
      data::variable_list rv = data::detail::set_intersection(variables, free_variables(right));
      if (lv.empty())
      {
        data::optimized_exists_no_empty_domain(result, rv, right, true);
        data::optimized_and(result, left, result);
      }
      else if (rv.empty())
      {
        data::optimized_exists_no_empty_domain(result, lv, left, true);
        data::optimized_and(result, right, result);
      }
      else
      {
        data::optimized_exists(result, variables, body, true);
      }
    }
    else
    {
      data::optimized_exists(result, variables, body, true);
    }
  }
};

template <typename Derived>
struct simplify_quantifiers_builder: public add_simplify_quantifiers<pbes_system::detail::simplify_builder, Derived>
{ };

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct simplify_quantifiers_data_rewriter_builder: public add_data_rewriter<pbes_system::detail::simplify_quantifiers_builder, Derived, DataRewriter, SubstitutionFunction>
{
  typedef add_data_rewriter<pbes_system::detail::simplify_quantifiers_builder, Derived, DataRewriter, SubstitutionFunction> super;
  using super::enter;
  using super::leave;

  simplify_quantifiers_data_rewriter_builder(const DataRewriter& R, SubstitutionFunction& sigma)
    : super(R, sigma)
  {}
};

} // namespace detail

/// \brief A rewriter that simplifies boolean expressions and quantifiers.
struct simplify_quantifiers_rewriter
{
  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  pbes_expression operator()(const pbes_expression& x) const
  {
    pbes_expression result;
    core::make_apply_builder<detail::simplify_quantifiers_builder>().apply(result, x);
    return result;
  }
};

/// \brief A rewriter that simplifies boolean expressions and quantifiers, and rewrites data expressions.
template <typename DataRewriter>
struct simplify_quantifiers_data_rewriter
{
  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  const DataRewriter& R;

  simplify_quantifiers_data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    data::no_substitution sigma;
    pbes_expression result;
    detail::make_apply_rewriter_builder<detail::simplify_quantifiers_data_rewriter_builder>(R, sigma).apply(result, x);
    return result;
  }

  template <typename SubstitutionFunction>
  pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
  {
    pbes_expression result;
    detail::make_apply_rewriter_builder<detail::simplify_quantifiers_data_rewriter_builder>(R, sigma).apply(result, x);
    return result;
  }
};

} // namespace mcrl2::pbes_system



#endif // MCRL2_PBES_REWRITERS_SIMPLIFY_QUANTIFIERS_REWRITER_H
