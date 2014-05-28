// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
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
#include "mcrl2/utilities/detail/optimized_logic_operators.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <template <class> class Builder, class Derived>
struct add_simplify_quantifiers: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();

  typedef core::term_traits<pbes_expression> tr;

  pbes_expression operator()(const forall& x)
  {
    pbes_expression result;
    pbes_expression body = super::operator()(x.body());
    auto const& variables = x.variables();

    if (variables.empty())
    {
      result = tr::true_();
    }
    else if (tr::is_not(body))
    {
      result = utilities::optimized_not(utilities::optimized_exists(variables, core::down_cast<not_>(body).operand(), true));
    }
    if (tr::is_and(body))
    {
      auto const& left = core::down_cast<and_>(body).left();
      auto const& right = core::down_cast<and_>(body).right();
      result = utilities::optimized_and(utilities::optimized_forall(variables, left, true), utilities::optimized_forall(variables, right, true));
    }
    else if (tr::is_or(body))
    {
      auto const& left = core::down_cast<or_>(body).left();
      auto const& right = core::down_cast<or_>(body).right();
      data::variable_list lv = tr::set_intersection(variables, tr::free_variables(left));
      data::variable_list rv = tr::set_intersection(variables, tr::free_variables(right));
      if (lv.empty())
      {
        result = utilities::optimized_or(left, utilities::optimized_forall_no_empty_domain(rv, right, true));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_or(right, utilities::optimized_forall_no_empty_domain(lv, left, true));
      }
      else
      {
        result = utilities::optimized_forall(variables, body, true);
      }
    }
    else
    {
      result = utilities::optimized_forall(variables, body, true);
    }
    return result;
  }

  pbes_expression operator()(const exists& x)
  {
    pbes_expression result;
    pbes_expression body = super::operator()(x.body());
    auto const& variables = x.variables();

    if (variables.empty())
    {
      result = tr::false_();
    }
    else if (tr::is_not(body))
    {
      result = utilities::optimized_not(utilities::optimized_forall(variables, core::down_cast<not_>(body).operand(), true));
    }
    if (tr::is_or(body))
    {
      auto const& left = core::down_cast<or_>(body).left();
      auto const& right = core::down_cast<or_>(body).right();
      result = utilities::optimized_and(utilities::optimized_exists(variables, left, true), utilities::optimized_forall(variables, right, true));
    }
    else if (tr::is_and(body))
    {
      auto const& left = core::down_cast<and_>(body).left();
      auto const& right = core::down_cast<and_>(body).right();
      data::variable_list lv = tr::set_intersection(variables, tr::free_variables(left));
      data::variable_list rv = tr::set_intersection(variables, tr::free_variables(right));
      if (lv.empty())
      {
        result = utilities::optimized_and(left, utilities::optimized_exists_no_empty_domain(rv, right, true));
      }
      else if (rv.empty())
      {
        result = utilities::optimized_and(right, utilities::optimized_exists_no_empty_domain(lv, left, true));
      }
      else
      {
        result = utilities::optimized_exists(variables, body, true);
      }
    }
    else
    {
      result = utilities::optimized_exists(variables, body, true);
    }
    return result;
  }
};

template <typename Derived>
struct simplify_quantifiers_builder: public add_simplify_quantifiers<pbes_system::detail::simplify_builder, Derived>
{
  typedef add_simplify_quantifiers<pbes_system::detail::simplify_builder, Derived> super;
  using super::enter;
  using super::leave;
  using super::operator();
};

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct simplify_quantifiers_data_rewriter_builder: public add_data_rewriter<pbes_system::detail::simplify_quantifiers_builder, Derived, DataRewriter, SubstitutionFunction>
{
  typedef add_data_rewriter<pbes_system::detail::simplify_quantifiers_builder, Derived, DataRewriter, SubstitutionFunction> super;
  using super::enter;
  using super::leave;
  using super::operator();

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
    return core::make_apply_builder<detail::simplify_quantifiers_builder>()(x);
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
    return detail::make_apply_rewriter_builder<detail::simplify_quantifiers_data_rewriter_builder>(R, sigma)(x);
  }

  template <typename SubstitutionFunction>
  pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
  {
    return detail::make_apply_rewriter_builder<detail::simplify_quantifiers_data_rewriter_builder>(R, sigma)(x);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_SIMPLIFY_QUANTIFIERS_REWRITER_H
