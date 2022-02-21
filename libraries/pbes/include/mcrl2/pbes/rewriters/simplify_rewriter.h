// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://github.com/mCRL2org/mCRL2/blob/master/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/simplify_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H
#define MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H

#include "mcrl2/pbes/rewriters/data_rewriter.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <template <class> class Builder, class Derived>
struct add_simplify: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  template <class T>
  void apply(T& result, const not_& x)
  {
    apply(result, x.operand());
    result = data::optimized_not(result);
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    apply(result, x.left());
    if (is_false(result))
    {
      result = false_();
      return;
    }
    if (is_true(result))
    {
      apply(result, x.right());
      return;
    }
    pbes_expression right;
    apply(right, x.right());
    result = data::optimized_and(result, right);
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    apply(result, x.left());
    if (is_true(result))
    {
      result = true_();
      return;
    }
    if (is_false(result))
    {
      apply(result, x.right());
      return;
    }
    pbes_expression right;
    apply(right, x.right());
    result = data::optimized_or(result, right);
  }

  template <class T>
  void apply(T& result, const imp& x)
  {
    apply(result, x.left());
    if (is_false(result))
    {
      result = true_();
      return;
    }
    if (is_true(result))
    {
      apply(result, x.right());
      return;
    }
    pbes_expression right;
    apply(right, x.right());
    result = data::optimized_imp(result, right);
  }

  template <class T>
  void apply(T& result, const forall& x)
  {
    pbes_expression body;
    apply(body, x.body());
    result = data::optimized_forall(x.variables(), body, true);
  }

  template <class T>
  void apply(T& result, const exists& x)
  {
    pbes_expression body;
    apply(body, x.body());
    result = data::optimized_exists(x.variables(), body, true);
  }
};

template <typename Derived>
struct simplify_builder: public add_simplify<pbes_system::pbes_expression_builder, Derived>
{ };

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct simplify_data_rewriter_builder : public add_data_rewriter < pbes_system::detail::simplify_builder, Derived, DataRewriter, SubstitutionFunction >
{
  typedef add_data_rewriter < pbes_system::detail::simplify_builder, Derived, DataRewriter, SubstitutionFunction > super;
  simplify_data_rewriter_builder(const DataRewriter& R, SubstitutionFunction& sigma)
    : super(R, sigma)
  {}
};

} // namespace detail

/// \brief A rewriter that simplifies boolean expressions in a term.
struct simplify_rewriter
{
  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  pbes_expression operator()(const pbes_expression& x) const
  {
    pbes_expression result;
    core::make_apply_builder<detail::simplify_builder>().apply(result, x);
    return result;
  }

  void operator()(pbes_expression& result, const pbes_expression& x) const
  {
    core::make_apply_builder<detail::simplify_builder>().apply(result, x);
  }
};

/// \brief A rewriter that simplifies boolean expressions in a term, and rewrites data expressions.
template <typename DataRewriter>
struct simplify_data_rewriter
{
  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  const DataRewriter& R;

  explicit simplify_data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    data::no_substitution sigma;
    pbes_expression result;
    detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result,x);
    return result;
  }

  template <typename SubstitutionFunction>
  pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
  {
    pbes_expression result;
    detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result, x);
    return result;
  }

  template <typename SubstitutionFunction>
  void operator()(pbes_expression& result, const pbes_expression& x, SubstitutionFunction& sigma) const
  {
    detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(result, x);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H
