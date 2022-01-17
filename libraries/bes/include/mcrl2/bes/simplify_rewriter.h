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

  template <class T>
  void apply(T& result, const not_& x)
  {
    boolean_expression op;
    apply(op, x.operand());
    result = data::optimized_not(op);
  }

  template <class T>
  void apply(T& result, const and_& x)
  {
    boolean_expression left; 
    apply(left, x.left());
    if (is_false(left))
    {
      result = false_();
      return;
    }
    boolean_expression right;
    apply(right, x.right());
    result = data::optimized_and(left, right);
  }

  template <class T>
  void apply(T& result, const or_& x)
  {
    boolean_expression left;
    apply(left, x.left());
    if (is_true(left))
    {
      result = true_();
      return;
    }
    boolean_expression right;
    apply(right, x.right());
    result = data::optimized_or(left, right);
  }

  template <class T>
  void apply(T& result, const imp& x)
  {
    boolean_expression left;
    apply(left, x.left());
    if (is_false(left))
    {
      result = true_();
      return;
    }
    boolean_expression right;
    apply(right, x.right());
    result = data::optimized_imp(left, right);
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
    boolean_expression result;
    core::make_apply_builder<detail::simplify_builder>().apply(result, x);
    return result;
  }
};

} // namespace bes

} // namespace mcrl2

#endif // MCRL2_BES_SIMPLIFY_REWRITER_H
