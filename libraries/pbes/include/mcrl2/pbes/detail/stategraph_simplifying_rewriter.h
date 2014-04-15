// Author(s): Wieger Wesselink
// Copyright: see the accompanying file COPYING or copy at
// https://svn.win.tue.nl/trac/MCRL2/browser/trunk/COPYING
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
/// \file mcrl2/pbes/detail/stategraph_simplifying_rewriter.h
/// \brief add your file description here.

#ifndef MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFYING_REWRITER_H
#define MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFYING_REWRITER_H

#include "mcrl2/data/detail/one_point_rule_preprocessor.h"
#include "mcrl2/data/detail/simplify_rewrite_builder.h"
#include "mcrl2/pbes/rewriters/simplifying_quantifier_rewriter.h"

namespace mcrl2 {

namespace pbes_system {

namespace detail {

template <typename Derived, typename DataRewriter>
struct stategraph_simplify_quantifier_builder: public simplify_quantifier_builder<Derived, DataRewriter>
{
  typedef simplify_quantifier_builder<Derived, DataRewriter> super;
  using super::enter;
  using super::leave;
  using super::operator();

  typedef core::term_traits<pbes_expression> tr;

  /// \brief Constructor.
  /// \param rewr A data rewriter
  stategraph_simplify_quantifier_builder(const DataRewriter& rewr)
    : super(rewr)
  { }

  Derived& derived()
  {
    return static_cast<Derived&>(*this);
  }

  bool is_data_not(const pbes_expression& x) const
  {
    return data::is_data_expression(x) && data::sort_bool::is_not_application(x);
  }

  bool is_data_and(const pbes_expression& x) const
  {
    return data::is_data_expression(x) && data::sort_bool::is_and_application(x);
  }

  bool is_data_or(const pbes_expression& x) const
  {
    return data::is_data_expression(x) && data::sort_bool::is_or_application(x);
  }

  // returns the argument of a data not
  data::data_expression not_arg(const data::data_expression& x)
  {
    return *data::application(x).begin();
  }

  // replace !(y || z) by !y && !z
  // replace !(y && z) by !y || !z
  // replace !(y => z) by y || !z
  // replace y => z by !y || z
  pbes_expression post_process(const pbes_expression& x)
  {
    pbes_expression result = x;
    if (tr::is_not(x))
    {
      const pbes_expression& t = tr::not_arg(x);
      if (tr::is_or(t)) // x = !(y && z)
      {
        pbes_expression y = utilities::optimized_not(tr::left(t));
        pbes_expression z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_and(y, z);
      }
      else if (tr::is_and(t)) // x = !(y || z)
      {
        pbes_expression y = utilities::optimized_not(tr::left(t));
        pbes_expression z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
      else if (tr::is_imp(t)) // x = !(y => z)
      {
        pbes_expression y = tr::left(t);
        pbes_expression z = utilities::optimized_not(tr::right(t));
        result = utilities::optimized_or(y, z);
      }
      else if (is_data_not(t)) // x = !val(!y)
      {
        const data::data_expression& z = atermpp::aterm_cast<const data::data_expression>(t);
        data::data_expression y = not_arg(z);
        result = y;
      }
    }
    else if (tr::is_imp(x)) // x = y => z
    {
      pbes_expression y = utilities::optimized_not(tr::left(x));
      pbes_expression z = tr::right(x);
      result = utilities::optimized_or(y, z);
    }
    return result;
  }

  // replace the data expression y != z by !(y == z)
  pbes_expression operator()(const data::data_expression& x)
  {
    typedef core::term_traits<data::data_expression> tt;

    // step 1: simplify the data expression
    data::detail::simplify_rewriter R;
    data::data_expression x1 = R(x);

    // step 2: if it is a negation, apply the one point rule preprocessor
    if (tt::is_not(x1))
    {
      data::detail::one_point_rule_preprocessor R;
      x1 = R(x1);
    }

    // step 3: replace the data expression y != z by !(y == z)
    if (data::is_not_equal_to_application(x1)) // result = y != z
    {
      data::data_expression y = tt::left(x1);
      data::data_expression z = tt::right(x1);
      x1 = data::sort_bool::not_(data::equal_to(y, z));
    }
    return post_process(x1);
  }

  pbes_expression operator()(const true_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const false_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const not_& x)
  {
    typedef core::term_traits<data::data_expression> tt;

    if (is_data(x.operand())) // convert to data expression
    {
      pbes_expression y = data::sort_bool::not_(atermpp::aterm_cast<data::data_expression>(x.operand()));
      return derived()(y);
    }
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const and_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const or_& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const imp& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const forall& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const exists& x)
  {
    return post_process(super::operator()(x));
  }

  pbes_expression operator()(const propositional_variable_instantiation& x)
  {
    return post_process(super::operator()(x));
  }
};

template <typename Derived, typename DataRewriter, typename SubstitutionFunction>
struct stategraph_simplify_quantifier_with_substitution_builder: public stategraph_simplify_quantifier_builder<Derived, DataRewriter>
{
  typedef stategraph_simplify_quantifier_builder<Derived, DataRewriter> super;
  using super::enter;
  using super::leave;
  using super::operator();

  SubstitutionFunction& sigma;

  stategraph_simplify_quantifier_with_substitution_builder(const DataRewriter& R_, SubstitutionFunction& sigma_)
    : super(R_), sigma(sigma_)
  {}

  pbes_expression operator()(const data::data_expression& x)
  {
    return R(x, sigma);
  }
};

/// \brief A rewriter that simplifies expressions that simplifies quantifiers.
template <typename DataRewriter>
class stategraph_simplifying_rewriter
{
  protected:
    /// \brief The data rewriter
    const DataRewriter& m_rewriter;

  public:
    /// \brief The term type
    typedef pbes_expression term_type;

    /// \brief The variable type
    typedef data::variable variable_type;

    /// \brief Constructor
    /// \param rewriter A data rewriter
    stategraph_simplifying_rewriter(const DataRewriter& rewriter)
      : m_rewriter(rewriter)
    {}

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \return The rewrite result.
    pbes_expression operator()(const pbes_expression& x) const
    {
      return detail::make_apply_simplify_builder<stategraph_simplify_quantifier_builder>(m_rewriter)(x);
    }

    /// \brief Rewrites a pbes expression.
    /// \param x A term
    /// \param sigma A substitution function
    /// \return The rewrite result.
    template <typename SubstitutionFunction>
    pbes_expression operator()(const pbes_expression& x, SubstitutionFunction& sigma) const
    {
      return detail::make_apply_simplify_with_substitution_builder<stategraph_simplify_quantifier_with_substitution_builder>(m_rewriter, sigma)(x);
    }
};

} // namespace detail

} // namespace pbes_system

} // namespace mcrl2

#endif // MCRL2_PBES_DETAIL_STATEGRAPH_SIMPLIFYING_REWRITER_H
