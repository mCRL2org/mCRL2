// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/rewriters/simplify_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_BES_SIMPLIFY_REWRITER_H
#define MCRL2_BES_SIMPLIFY_REWRITER_H

#include "mcrl2/bes/builder.h"
#include "mcrl2/data/optimized_boolean_operators.h"

namespace mcrl2 {

namespace bes {

namespace detail {

template <template <class> class Builder, class Derived>
struct add_simplify: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  boolean_expression apply(const not_& x)
  {
    return data::optimized_not(apply(x.operand()));
  }

  boolean_expression apply(const and_& x)
  {
    auto left = apply(x.left());
    if (is_false(left))
    {
      return false_();
    }
    auto right = apply(x.right());
    return data::optimized_and(left, right);
  }

  boolean_expression apply(const or_& x)
  {
    auto left = apply(x.left());
    if (is_true(left))
    {
      return true_();
    }
    auto right = apply(x.right());
    return data::optimized_or(left, right);
  }

  boolean_expression apply(const imp& x)
  {
    auto left = apply(x.left());
    if (is_false(left))
    {
      return true_();
    }
    auto right = apply(x.right());
    return data::optimized_imp(left, right);
  }
};

template <typename Derived>
struct simplify_builder: public add_simplify<bes::boolean_expression_builder, Derived>
{ };

} // namespace detail

/// \brief A rewriter that simplifies boolean expressions in a term.
struct simplify_rewriter
{
  typedef boolean_expression term_type;
  typedef boolean_variable variable_type;

  boolean_expression operator()(const boolean_expression& x) const
  {
    return core::make_apply_builder<detail::simplify_builder>().apply(x);
  }
};

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_SIMPLIFY_REWRITER_H
