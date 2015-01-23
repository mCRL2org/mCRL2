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

#ifndef MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H
#define MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H

#include "mcrl2/pbes/rewriters/data_rewriter.h"
#include "mcrl2/utilities/detail/optimized_logic_operators.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <template <class> class Builder, class Derived>
struct add_simplify: public Builder<Derived>
{
  typedef Builder<Derived> super;
  using super::apply;

  typedef core::term_traits<pbes_expression> tr;

  pbes_expression apply(const not_& x)
  {
    return utilities::optimized_not(apply(x.operand()));
  }

  pbes_expression apply(const and_& x)
  {
    auto left = apply(x.left());
    if (tr::is_false(left))
    {
      return tr::false_();
    }
    auto right = apply(x.right());
    return utilities::optimized_and(left, right);
  }

  pbes_expression apply(const or_& x)
  {
    auto left = apply(x.left());
    if (tr::is_true(left))
    {
      return tr::true_();
    }
    auto right = apply(x.right());
    return utilities::optimized_or(left, right);
  }

  pbes_expression apply(const imp& x)
  {
    auto left = apply(x.left());
    if (tr::is_false(left))
    {
      return tr::true_();
    }
    auto right = apply(x.right());
    return utilities::optimized_imp(left, right);
  }

  pbes_expression apply(const forall& x)
  {
    auto body = apply(x.body());
    return utilities::optimized_forall(x.variables(), body, true);
  }

  pbes_expression apply(const exists& x)
  {
    auto body = apply(x.body());
    return utilities::optimized_exists(x.variables(), body, true);
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
    return core::make_apply_builder<detail::simplify_builder>().apply(x);
  }
};

/// \brief A rewriter that simplifies boolean expressions in a term, and rewrites data expressions.
template <typename DataRewriter>
struct simplify_data_rewriter
{
  typedef pbes_expression term_type;
  typedef data::variable variable_type;

  const DataRewriter& R;

  simplify_data_rewriter(const DataRewriter& R_)
    : R(R_)
  {}

  pbes_expression operator()(const pbes_expression& x) const
  {
    data::no_substitution sigma;
    return detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(x);
  }

  template <typename SubstitutionFunction>
  pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
  {
    return detail::make_apply_rewriter_builder<pbes_system::detail::simplify_data_rewriter_builder>(R, sigma).apply(x);
  }
};

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_REWRITERS_SIMPLIFY_REWRITER_H
